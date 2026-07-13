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
 *   v_f64x8_t amd_vrd8_exp10(v_f64x8_t x)   [AVX-512, 8 doubles]
 *
 * ---------------------------------------------------------------------------
 * Math (what we compute)
 * ---------------------------------------------------------------------------
 *   10^x = 2^(x * log2(10)) = 2^n * 2^f,   n = round(x * log2(10)),
 *                                            f = x*log2(10) - n,  |f| <= 1/2
 *
 * Fast path (see vrd8_exp10_fastpath below):
 *   1. Split x*log2(10) into integer n and small fraction f (|f| <= 1/2).
 *   2. Approximate 2^f with a degree-11 minimax poly in f (ln2 folded into
 *      the coefficients) evaluates C0 + f * POLY_EVAL_HORNER_10(f, C1..C11).
 *   3. Fold 2^n into the polynomial result's biased exponent (n << 52).
 *
 * ---------------------------------------------------------------------------
 * Control flow
 * ---------------------------------------------------------------------------
 *   1. Check |x| for each lane (one unsigned compare on the double bits).
 *   2. If |x| is too large, or the lane is inf/NaN, mark it for scalar exp10.
 *   3. Always run the vector fast path on all 8 lanes.
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
    v_u64x8_t abs_mask;
    v_u64x8_t arg_max;        /* |x| bits above this -> scalar (overflow/underflow/inf/nan) */
    v_f64x8_t log2_10_hi;
    v_f64x8_t log2_10_lo;
    v_f64x8_t huge;
    v_f64x8_t poly[12];       /* 2^f minimax coeffs C0..C11, degree-11 in f */
} v8_exp10_data = {
    .abs_mask   = _MM512_SET1_U64x8(0x7fffffffffffffffUL),
    .arg_max    = _MM512_SET1_U64x8(0x4073300000000000UL),  /* 307.0 */
    .log2_10_hi = _MM512_SET1_PD8(0x1.a934f0979a370p+1),   /* 3.321928095  */
    .log2_10_lo = _MM512_SET1_PD8(0x1.5fc9257edfe9bp-51),  /* 6.10251e-16  */
    .huge       = _MM512_SET1_PD8(0x1.8p+52),
    /* Degree-11 minimax coeffs for 2^f, |f| <= 1/2. */
    .poly = {
        _MM512_SET1_PD8(0x1p0),                /* C0:  1.0           */
        _MM512_SET1_PD8(0x1.62e42fefa39efp-1), /* C1:  0.693147181  */
        _MM512_SET1_PD8(0x1.ebfbdff82c5adp-3), /* C2:  0.240226507  */
        _MM512_SET1_PD8(0x1.c6b08d7049fd1p-5), /* C3:  0.055504109  */
        _MM512_SET1_PD8(0x1.3b2ab6fb9f413p-7), /* C4:  0.009618129  */
        _MM512_SET1_PD8(0x1.5d87fe78cf26ep-10),/* C5:  0.001333356  */
        _MM512_SET1_PD8(0x1.43091310bf6c4p-13),/* C6:  0.000154035  */
        _MM512_SET1_PD8(0x1.ffcbfba7b847p-17),/* C7:  1.525273e-05  */
        _MM512_SET1_PD8(0x1.62bfc3c1c57ddp-20),/* C8:  1.321543e-06  */
        _MM512_SET1_PD8(0x1.b526788bf2853p-24),/* C9:  1.017820e-07  */
        _MM512_SET1_PD8(0x1.e620fb7baec71p-28),/* C10: 7.074106e-09  */
        _MM512_SET1_PD8(0x1.e7aa0e43a8875p-32),/* C11: 4.435281e-10  */
    },
};

#define V8_EXP10_ABS_MASK     v8_exp10_data.abs_mask
#define V8_EXP10_ARG_MAX      v8_exp10_data.arg_max
#define V8_EXP10_LOG2_10_HI   v8_exp10_data.log2_10_hi
#define V8_EXP10_LOG2_10_LO   v8_exp10_data.log2_10_lo
#define V8_EXP10_HUGE         v8_exp10_data.huge
#define V8_EXP10_C0           v8_exp10_data.poly[0]
#define V8_EXP10_C1           v8_exp10_data.poly[1]
#define V8_EXP10_C2           v8_exp10_data.poly[2]
#define V8_EXP10_C3           v8_exp10_data.poly[3]
#define V8_EXP10_C4           v8_exp10_data.poly[4]
#define V8_EXP10_C5           v8_exp10_data.poly[5]
#define V8_EXP10_C6           v8_exp10_data.poly[6]
#define V8_EXP10_C7           v8_exp10_data.poly[7]
#define V8_EXP10_C8           v8_exp10_data.poly[8]
#define V8_EXP10_C9           v8_exp10_data.poly[9]
#define V8_EXP10_C10          v8_exp10_data.poly[10]
#define V8_EXP10_C11          v8_exp10_data.poly[11]

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
 *   10^x = 2^(x*log2(10)) = 2^n * 2^f
 * where n = round(x*log2(10)) is an integer and f = x*log2(10) - n, |f| <= 1/2.
 * n is applied by the biased exponent of the polynomial result; 2^f is handled
 * by a degree-11 minimax polynomial in f.
 */
static inline ALM_ALWAYS_INLINE v_f64x8_t
vrd8_exp10_fastpath(v_f64x8_t x)
{
    /*
     * Step 1: base-2 exponent estimate x*log2(10), split into HI and LO parts
     * for accuracy.  phi uses the HI half; e recovers the rounding error of
     * phi, and tail = x*LO + e holds the low-order bits dropped from phi.
     */
    v_f64x8_t phi  = x * V8_EXP10_LOG2_10_HI;
    v_f64x8_t e    = mul_add(x, V8_EXP10_LOG2_10_HI, -phi);  /* rounding error of phi */
    v_f64x8_t tail = mul_add(x, V8_EXP10_LOG2_10_LO, e);     /* x*lo + error */

    /*
     * Step 2: round to integer n = nearest(x*log2(10)) via (s + HUGE) - HUGE.
     * Use the full value s = phi+tail, not phi alone, or n can be wrong by 1
     * near half-integers.  n becomes the 2^n exponent.
     */
    v_f64x8_t s  = phi + tail;
    v_f64x8_t t  = s + V8_EXP10_HUGE;
    v_u64x8_t n  = as_v8_u64_f64(t);
    v_f64x8_t dn = t - V8_EXP10_HUGE;   /* n as a double */

    /*
     * Step 3: f = x*log2(10) - n,  |f| <= 1/2.
     * Compute as (phi-dn)+tail to avoid losing the tail in cancellation.
     */
    v_f64x8_t f = (phi - dn) + tail;

    /*
     * Step 4: approximate 2^f with a degree-11 Horner minimax polynomial in f.
     * C0 = 1, so factor out f: 1 + f * POLY_EVAL_HORNER_10(f, C1..C11).
     */
    v_f64x8_t poly = V8_EXP10_C0 + f * POLY_EVAL_HORNER_10(f,
            V8_EXP10_C1, V8_EXP10_C2, V8_EXP10_C3,
            V8_EXP10_C4, V8_EXP10_C5, V8_EXP10_C6, V8_EXP10_C7,
            V8_EXP10_C8, V8_EXP10_C9, V8_EXP10_C10, V8_EXP10_C11);
    /*
     * Step 5: result = poly * 2^n.
     */
    return as_v8_f64_u64(as_v8_u64_f64(poly) + (n << 52));
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

    /* Fast path: 10^x = 2^(x * log2(10)) */
    v_f64x8_t result = vrd8_exp10_fastpath(x);

    if (unlikely(any_need_scalar)) {
        return exp10_specialcase(x, result, need_scalar);
    }

    return result;
}
