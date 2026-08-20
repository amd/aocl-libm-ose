/*
 * Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * Signature:
 *   v_f64x8_t amd_vrd8_exp10(v_f64x8_t x)   [AVX-512, 8 doubles]
 *
 * ---------------------------------------------------------------------------
 * Math (what we compute)
 * ---------------------------------------------------------------------------
 *   10^x = 2^m * 2^(j/8) * 10^r
 *
 *   n = round(8 * x * log2(10)),  j = n & 7,  m = n >> 3,
 *   r = x - (n/8) * log10(2),     |r| <= log10(2)/16.
 *
 * Fast path (see vrd8_exp10_fastpath below):
 *   1. n = round(8 * x * log2_10_hi) via (x * 8log2_10_hi + HUGE) - HUGE
 *      (lo omitted; equivalent to round(8*x*log2(10)) for |x| <= ARG_MAX);
 *      then j = n & 7 and m = n >> 3.
 *   2. Form r = x - (n/8)*log10(2) (|r| <= log10(2)/16) using a hi/lo
 *      split of log10(2) for accurate cancellation; dn8 = dn * 1/8 is exact.
 *   3. Look up 2^(j/8) (one vpermpd) before the poly so shuffle can overlap
 *      the Horner FMA chain.
 *   4. Approximate 10^r with a degree-7 minimax poly (10^r = 1 + r*poly6(r)),
 *      Horner via POLY_EVAL_HORNER_7_0; then H*(1+q) = fma(H,q,H) and 2^m
 *      via exponent bump.
 *
 * ---------------------------------------------------------------------------
 * Control flow
 * ---------------------------------------------------------------------------
 *   1. Check |x| for each lane (one unsigned compare on the double bits).
 *   2. If |x| is too large, or the lane is inf/NaN, mark it for scalar exp10.
 *   3. Always run the vector fast path on all 8 lanes.
 *   4. If any lane was marked, replace only those lanes with scalar exp10.
 */

#include <immintrin.h>

#include <libm_util_amd.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>

#define AMD_LIBM_FMA_USABLE 1
#include <libm/poly-vec.h>

static const struct {
    v_u64x8_t abs_mask;
    v_u64x8_t arg_max;        /* |x| above 307 -> scalar (overflow band / inf / nan) */
    v_u64x8_t huge_bits;      /* bit pattern of HUGE, to recover integer n */
    v_i64x8_t seven_mask;     /* 7, for n & 7 */
    v_u64x8_t head;           /* 2^(j/8), j=0..7, for vpermpd */
    v_f64x8_t eight_log2_10;  /* 8 * log2_10_hi */
    v_f64x8_t neg_log10_2_hi; /* -log10(2)_hi */
    v_f64x8_t neg_log10_2_lo; /* -(log10(2)_lo); positive word (log10(2)_lo < 0) */
    v_f64x8_t eighth;
    v_f64x8_t huge;
    v_f64x8_t poly[7];        /* 10^r minimax T1..T7: 10^r = 1 + r*poly6(r) */
} v8_exp10_data = {
    .abs_mask      = _MM512_SET1_U64x8((uint64_t)0x7FFFFFFFFFFFFFFFULL),
    .arg_max       = _MM512_SET1_U64x8((uint64_t)0x4073300000000000ULL),  /* 307.0 */
    .huge_bits     = _MM512_SET1_U64x8((uint64_t)0x4338000000000000ULL),
    .seven_mask    = _MM512_SET1_I64x8((int64_t)7),
    .head          = { 0x3ff0000000000000ULL, 0x3ff172b83c7d517bULL,
                       0x3ff306fe0a31b715ULL, 0x3ff4bfdad5362a27ULL,
                       0x3ff6a09e667f3bcdULL, 0x3ff8ace5422aa0dbULL,
                       0x3ffae89f995ad3adULL, 0x3ffd5818dcfba487ULL },
    .eight_log2_10 = _MM512_SET1_PD8(0x1.a934f0979a371p+4),   /* 8*log2_10_hi  */
    .neg_log10_2_hi = _MM512_SET1_PD8(-0x1.34413509f79ffp-2),
    .neg_log10_2_lo = _MM512_SET1_PD8(0x1.9dc1da994fd21p-59),
    .eighth        = _MM512_SET1_PD8(0.125),
    .huge          = _MM512_SET1_PD8(0x1.8p+52),
    /*
     * Degree-7 minimax for 10^r, |r| <= log10(2)/16:  10^r = 1 + r*poly6(r).
     * poly6(r) for 10^r = e^(r*ln(10)); T1 = ln(10) exactly; T2..T7 fpminimax
     * (near Taylor ln(10)^k/k!).
     */
    .poly = {
        _MM512_SET1_PD8(0x1.26bb1bbb55516p1),  /* T1 = ln(10) */
        _MM512_SET1_PD8(0x1.53524c73cebd7p1), /* T2 ≈ ln(10)^2/2 */
        _MM512_SET1_PD8(0x1.0470591de0a62p1), /* T3 ≈ ln(10)^3/6 */
        _MM512_SET1_PD8(0x1.2bd76093fe2fdp0), /* T4 ≈ ln(10)^4/24 */
        _MM512_SET1_PD8(0x1.142a0076a696bp-1),/* T5 ≈ ln(10)^5/120 */
        _MM512_SET1_PD8(0x1.a7f473b908176p-3),/* T6 ≈ ln(10)^6/720 */
        _MM512_SET1_PD8(0x1.16c8fec759c72p-4),/* T7 ≈ ln(10)^7/5040 */
    },
};

#define V8_EXP10_ABS_MASK     v8_exp10_data.abs_mask
#define V8_EXP10_ARG_MAX      v8_exp10_data.arg_max
#define V8_EXP10_HUGE_BITS    v8_exp10_data.huge_bits
#define V8_EXP10_SEVEN_MASK   v8_exp10_data.seven_mask
#define V8_EXP10_HEAD         v8_exp10_data.head
#define V8_EXP10_8LOG2_10     v8_exp10_data.eight_log2_10
#define V8_EXP10_NEG_LOG10_2_HI   v8_exp10_data.neg_log10_2_hi
#define V8_EXP10_NEG_LOG10_2_LO   v8_exp10_data.neg_log10_2_lo
#define V8_EXP10_EIGHTH       v8_exp10_data.eighth
#define V8_EXP10_HUGE         v8_exp10_data.huge
#define V8_EXP10_T1           v8_exp10_data.poly[0]
#define V8_EXP10_T2           v8_exp10_data.poly[1]
#define V8_EXP10_T3           v8_exp10_data.poly[2]
#define V8_EXP10_T4           v8_exp10_data.poly[3]
#define V8_EXP10_T5           v8_exp10_data.poly[4]
#define V8_EXP10_T6           v8_exp10_data.poly[5]
#define V8_EXP10_T7           v8_exp10_data.poly[6]

/*
 * Lanes needing scalar fallback: |x| larger than the normal-output threshold,
 * +/-inf or NaN.  A single unsigned compare on the abs bit pattern catches all
 * three (inf/nan bit patterns are numerically larger than any finite value).
 */
static inline ALM_ALWAYS_INLINE v_u64x8_t
vrd8_exp10_special_mask(v_f64x8_t x)
{
    v_u64x8_t ax = as_v8_u64_f64(x) & V8_EXP10_ABS_MASK;
    return (ax > V8_EXP10_ARG_MAX);
}

/*
 * Branch-free fast path, no domain checks.  Valid only when 10^x is a normal
 * double (|x| <= ARG_MAX); the caller must route subnormal/overflow/inf/nan
 * lanes to scalar.
 *
 * Identity:
 *   10^x = 2^m * 2^(j/8) * 10^r,  n = round(8*x*log2_10_hi), j = n&7, m = n>>3,
 *   r = x - (n/8)*log10(2), |r| <= log10(2)/16.  Step 1 uses 8*log2_10_hi
 *   only; step 2 uses log10(2) hi+lo; table lookup (vpermpd) before Horner;
 *   step 3 is POLY_EVAL_HORNER_7_0; 2^m via exponent bump.
 */
static inline ALM_ALWAYS_INLINE v_f64x8_t
vrd8_exp10_fastpath(v_f64x8_t x)
{
    /*
     * Step 1: n = round(8 * x * log2_10_hi) via (x * 8log2_10_hi + HUGE) - HUGE.
     * Recover signed n from the low bits.
     */
    v_f64x8_t qd = mul_add(x, V8_EXP10_8LOG2_10, V8_EXP10_HUGE);
    v_i64x8_t n  = as_v8_i64_f64(qd) - (v_i64x8_t)V8_EXP10_HUGE_BITS;
    v_f64x8_t dn = qd - V8_EXP10_HUGE;          /* n as a double */

    /*
     * Table lookup depends only on n; issue before r/poly so vpermpd can
     * overlap the Horner FMA chain.
     */
    v_i64x8_t j = n & V8_EXP10_SEVEN_MASK;
    v_f64x8_t H = _mm512_permutexvar_pd((__m512i)j, as_v8_f64_u64(V8_EXP10_HEAD));

    /*
     * Step 2: r = x - (n/8)*log10(2), |r| <= log10(2)/16.  Form with -log10(2)
     * hi/lo via mul_add (dn8*(-log10(2)) + x); dn8 = dn * 1/8 is exact.
     */
    v_f64x8_t dn8 = dn * V8_EXP10_EIGHTH;
    v_f64x8_t r   = mul_add(dn8, V8_EXP10_NEG_LOG10_2_HI, x);
    r             = mul_add(dn8, V8_EXP10_NEG_LOG10_2_LO, r);

    /*
     * Step 3: 10^r = 1 + q with q = r * poly6(r).  Horner (POLY_EVAL_HORNER_7_0):
     *   q = r * (T1 + r * (T2 + r * (... + r * T7)))
     */
    v_f64x8_t q = POLY_EVAL_HORNER_7_0(r,
            V8_EXP10_T1, V8_EXP10_T2, V8_EXP10_T3, V8_EXP10_T4,
            V8_EXP10_T5, V8_EXP10_T6, V8_EXP10_T7);

    v_f64x8_t Hq = mul_add(H, q, H);

    /*
     * Step 4: result = Hq * 2^(n>>3).  bits(double(n>>3) + HUGE) equals
     * HUGE_BITS + (n>>3); m = (HUGE_BITS + (n>>3)) << 52, same as vrd8_exp2.
     */
    v_u64x8_t m = ((v_u64x8_t)V8_EXP10_HUGE_BITS + (v_u64x8_t)(n >> 3)) << 52;

    /* result = Hq * 2^(n>>3) */
    v_f64x8_t result = as_v8_f64_u64(as_v8_u64_f64(Hq) + m);

    return result;
}

#define SCALAR_EXP10 ALM_PROTO_OPT(exp10)

static inline ALM_ALWAYS_INLINE v_f64x8_t
exp10_specialcase(v_f64x8_t x, v_f64x8_t result, v_u64x8_t cond)
{
    return call_v8_f64(SCALAR_EXP10, x, result, cond);
}

/*
 * amd_vrd8_exp10 (__m512d): AVX-512, eight IEEE-754 doubles per register.
 * Runs vrd8_exp10_fastpath on all lanes; lanes outside the finite primary
 * range (see V8_EXP10_ARG_MAX) are fixed up via scalar exp10.
 */
v_f64x8_t
ALM_PROTO_OPT(vrd8_exp10)(v_f64x8_t x)
{
    v_u64x8_t need_scalar = vrd8_exp10_special_mask(x);
    int any_need_scalar = any_v8_u64_avx512(need_scalar);

    /* Fast path: 10^x = 2^m * 2^(j/8) * 10^r */
    v_f64x8_t result = vrd8_exp10_fastpath(x);

    if (unlikely(any_need_scalar)) {
        return exp10_specialcase(x, result, need_scalar);
    }

    return result;
}
