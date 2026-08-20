/*
 * Copyright (C) 2026, Advanced Micro Devices, Inc. All rights reserved.
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
 */

/*
 * Signature:
 *   v_f64x4_t amd_vrd4_exp10(v_f64x4_t x)   [AVX2, 4 doubles]
 *
 * ---------------------------------------------------------------------------
 * Math (what we compute)
 * ---------------------------------------------------------------------------
 *   10^x = 2^(x * log2(10)) = 2^n * 2^f,   n = round(x * log2(10)),
 *                                            f = x*log2(10) - n,  |f| <= 1/2
 *
 * Fast path (see vrd4_exp10_fastpath below):
 *   1. Split x*log2(10) into integer n and small fraction f (|f| <= 1/2).
 *   2. Approximate 2^f with a degree-11 minimax poly in f; evaluates the
 *      leading (C0 + C1*f) fused and the remainder f^2*Q(f) via Estrin.
 *   3. Fold 2^n into the polynomial result's biased exponent (n << 52).
 *
 * ---------------------------------------------------------------------------
 * Control flow
 * ---------------------------------------------------------------------------
 *   1. Check |x| for each lane (one unsigned compare on the double bits).
 *   2. If |x| is too large, or the lane is inf/NaN, mark it for scalar exp10.
 *   3. Always run the vector fast path on all 4 lanes.
 *   4. If any lane was marked, replace only those lanes with scalar exp10.
 */

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
    v_u64x4_t abs_mask;
    v_u64x4_t arg_max;        /* |x| bits above this -> scalar (overflow/underflow/inf/nan) */
    v_f64x4_t log2_10_hi;
    v_f64x4_t log2_10_lo;
    v_f64x4_t huge;
    v_f64x4_t poly[12];       /* 2^f minimax coeffs C0..C11, degree-11 in f */
} v4_exp10_data = {
    .abs_mask   = _MM_SET1_I64(0x7FFFFFFFFFFFFFFFULL),
    .arg_max    = _MM_SET1_I64(0x4073300000000000ULL),  /* 307.0 */
    .log2_10_hi = _MM_SET1_PD4(0x1.a934f0979a370p+1),  /* 3.321928095  */
    .log2_10_lo = _MM_SET1_PD4(0x1.5fc9257edfe9bp-51), /* 6.10251e-16  */
    .huge       = _MM_SET1_PD4(0x1.8p+52),
    /* Degree-11 minimax coeffs for 2^f, |f| <= 1/2. */
    .poly = {
        _MM_SET1_PD4(0x1p0),                /* C0:  1.0           */
        _MM_SET1_PD4(0x1.62e42fefa39efp-1), /* C1:  0.693147181  */
        _MM_SET1_PD4(0x1.ebfbdff82c5adp-3), /* C2:  0.240226507  */
        _MM_SET1_PD4(0x1.c6b08d7049fd1p-5), /* C3:  0.055504109  */
        _MM_SET1_PD4(0x1.3b2ab6fb9f413p-7), /* C4:  0.009618129  */
        _MM_SET1_PD4(0x1.5d87fe78cf26ep-10),/* C5:  0.001333356  */
        _MM_SET1_PD4(0x1.43091310bf6c4p-13),/* C6:  0.000154035  */
        _MM_SET1_PD4(0x1.ffcbfba7b847p-17), /* C7:  1.525273e-05  */
        _MM_SET1_PD4(0x1.62bfc3c1c57ddp-20),/* C8:  1.321543e-06  */
        _MM_SET1_PD4(0x1.b526788bf2853p-24),/* C9:  1.017820e-07  */
        _MM_SET1_PD4(0x1.e620fb7baec71p-28),/* C10: 7.074106e-09  */
        _MM_SET1_PD4(0x1.e7aa0e43a8875p-32),/* C11: 4.435281e-10  */
    },
};

#define V4_EXP10_ABS_MASK     v4_exp10_data.abs_mask
#define V4_EXP10_ARG_MAX      v4_exp10_data.arg_max
#define V4_EXP10_LOG2_10_HI   v4_exp10_data.log2_10_hi
#define V4_EXP10_LOG2_10_LO   v4_exp10_data.log2_10_lo
#define V4_EXP10_HUGE         v4_exp10_data.huge
#define V4_EXP10_C0           v4_exp10_data.poly[0]
#define V4_EXP10_C1           v4_exp10_data.poly[1]
#define V4_EXP10_C2           v4_exp10_data.poly[2]
#define V4_EXP10_C3           v4_exp10_data.poly[3]
#define V4_EXP10_C4           v4_exp10_data.poly[4]
#define V4_EXP10_C5           v4_exp10_data.poly[5]
#define V4_EXP10_C6           v4_exp10_data.poly[6]
#define V4_EXP10_C7           v4_exp10_data.poly[7]
#define V4_EXP10_C8           v4_exp10_data.poly[8]
#define V4_EXP10_C9           v4_exp10_data.poly[9]
#define V4_EXP10_C10          v4_exp10_data.poly[10]
#define V4_EXP10_C11          v4_exp10_data.poly[11]

/*
 * Lanes needing scalar fallback: |x| larger than the normal-output threshold,
 * +/-inf or NaN.  A single unsigned compare on the abs bit pattern catches all
 * three (inf/nan bit patterns are numerically larger than any finite value).
 */
static inline ALM_ALWAYS_INLINE v_u64x4_t
vrd4_exp10_special_mask(v_f64x4_t x)
{
    v_u64x4_t ax = as_v4_u64_f64(x) & V4_EXP10_ABS_MASK;
    return (ax > V4_EXP10_ARG_MAX);
}

/*
 * Branch-free fast path, no domain checks.  Valid only when 10^x is a normal
 * double (|x| <= ARG_MAX); the caller must route subnormal/overflow/inf/nan
 * lanes to scalar.
 *
 * Identity:
 *   10^x = 2^(x*log2(10)) = 2^n * 2^f
 * where n = round(x*log2(10)) is an integer and f = x*log2(10) - n, |f| <= 1/2.
 * n is applied by the biased exponent of the polynomial result; 2^f is handled
 * by a degree-11 minimax polynomial in f.
 */
static inline ALM_ALWAYS_INLINE v_f64x4_t
vrd4_exp10_fastpath(v_f64x4_t x)
{
    /*
     * Step 1: base-2 exponent estimate x*log2(10), split into HI and LO parts
     * for accuracy.  phi uses the HI half; e recovers the rounding error of
     * phi, and tail = x*LO + e holds the low-order bits dropped from phi.
     */
    v_f64x4_t phi  = x * V4_EXP10_LOG2_10_HI;
    v_f64x4_t e    = mul_add(x, V4_EXP10_LOG2_10_HI, -phi);  /* rounding error of phi */
    v_f64x4_t tail = mul_add(x, V4_EXP10_LOG2_10_LO, e);     /* x*lo + error */

    /*
     * Step 2: round to integer n = nearest(x*log2(10)) via (s + HUGE) - HUGE.
     * Use the full value s = phi+tail, not phi alone, or n can be wrong by 1
     * near half-integers.  n becomes the 2^n exponent.
     */
    v_f64x4_t s  = phi + tail;
    v_f64x4_t t  = s + V4_EXP10_HUGE;
    v_u64x4_t n  = as_v4_u64_f64(t);
    v_f64x4_t dn = t - V4_EXP10_HUGE;   /* n as a double */

    /*
     * Step 3: f = x*log2(10) - n,  |f| <= 1/2.
     * Compute as (phi-dn)+tail to avoid losing the tail in cancellation.
     */
    v_f64x4_t f = (phi - dn) + tail;

    /*
     * Step 4: approximate 2^f with a degree-11 minimax polynomial in f.
     *
     *   2^f = (C0 + C1*f) + f^2 * (C2 + C3*f + ... + C11*f^9)
     *
     * The dominant (C0 + C1*f) is kept as one fused op for accuracy; the small
     * remainder Q(f) uses Estrin (POLY_EVAL_9) for a short FMA chain (fast).
     */
    v_f64x4_t f2   = f * f;
    v_f64x4_t lead = mul_add(V4_EXP10_C1, f, V4_EXP10_C0);   /* C0 + C1*f */
    v_f64x4_t corr = POLY_EVAL_9(f,
            V4_EXP10_C2, V4_EXP10_C3, V4_EXP10_C4, V4_EXP10_C5,
            V4_EXP10_C6, V4_EXP10_C7, V4_EXP10_C8, V4_EXP10_C9,
            V4_EXP10_C10, V4_EXP10_C11);
    v_f64x4_t poly = mul_add(f2, corr, lead);
    /*
     * Step 5: result = poly * 2^n.
     */
    return as_v4_f64_u64(as_v4_u64_f64(poly) + (n << 52));
}

#define SCALAR_EXP10 ALM_PROTO_OPT(exp10)

static inline ALM_ALWAYS_INLINE v_f64x4_t
exp10_specialcase(v_f64x4_t x, v_f64x4_t result, v_u64x4_t cond)
{
    return call_v4_f64(SCALAR_EXP10, x, result, cond);
}

/*
 * amd_vrd4_exp10 (__m256d): AVX2, four IEEE-754 doubles per register.
 * Runs vrd4_exp10_fastpath on all lanes; lanes outside the finite primary
 * range (see V4_EXP10_ARG_MAX) are fixed up via scalar exp10.
 */
v_f64x4_t
ALM_PROTO_OPT(vrd4_exp10)(v_f64x4_t x)
{
    v_u64x4_t need_scalar = vrd4_exp10_special_mask(x);
    int any_need_scalar = any_v4_u64_loop(need_scalar);

    /* Fast path: 10^x = 2^(x * log2(10)) */
    v_f64x4_t result = vrd4_exp10_fastpath(x);

    if (unlikely(any_need_scalar)) {
        return exp10_specialcase(x, result, need_scalar);
    }

    return result;
}
