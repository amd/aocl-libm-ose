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
 *   v_f32x16_t amd_vrs16_exp10f(v_f32x16_t x)
 *
 * ---------------------------------------------------------------------------
 * Math (what we compute)
 * ---------------------------------------------------------------------------
 *   10^x = 2^(x * log2(10)) = 2^n * 2^f,   n = round(x*log2_10),
 *                                          f = x*log2_10 - n,  |f| <= 1/2
 *
 * Fast path (see vrs16_exp10f_fastpath below):
 *   1. Split x*log2(10) into integer n and small fraction f (|f| <= 1/2).
 *   2. Approximate 2^f with a degree-7 polynomial in f.
 *   3. Fold 2^n into the polynomial result's biased exponent.
 *
 * ---------------------------------------------------------------------------
 * Control flow
 * ---------------------------------------------------------------------------
 *   1. Check |x| for each lane (one unsigned compare on the float bits).
 *   2. If |x| is too large, or the lane is inf/NaN, mark it for scalar exp10f.
 *   3. Always run the vector fast path on all 16 lanes.
 *   4. If any lane was marked, replace only those lanes with scalar exp10f.
 */

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>

#define AMD_LIBM_FMA_USABLE 1
#include <libm/poly-vec.h>

static const struct {
    v_u32x16_t abs_mask;
    v_f32x16_t arg_max;        /* |x| above this -> scalar (also catches inf/nan) */
    v_f32x16_t log2_10_hi;     /* high part of log2(10) */
    v_f32x16_t log2_10_lo;     /* tail   part of log2(10) */
    v_f32x16_t huge;
    v_f32x16_t poly[8];        /* 2^f coefficients, E0..E7  (Dk = ln2^k / k!) */
} v16_exp10f_data = {
    .abs_mask    = _MM512_SET1_U32x16((uint32_t)POS_BITSET_F32),
    /*
     * Fast-path limit: largest |x| with a normal 10^x result.
     * |x| <= arg_max -> vector; |x| > arg_max -> scalar
     * (subnormal, overflow, inf, nan).  The first float beyond arg_max
     * already makes 10^x subnormal, where the error jumps significantly.
     */
    .arg_max     = _MM512_SET1_PS16(0x1.2f703p+5f),   /* = 37.9297791, the largest |x| whose 10^x is still a normal float.*/
    .log2_10_hi  = _MM512_SET1_PS16(0x1.a934fp+1f),    /* 3.321928024  */
    .log2_10_lo  = _MM512_SET1_PS16(0x1.2f346ep-24f),  /* 7.05954e-08  */
    .huge        = _MM512_SET1_PS16(0x1.8p+23f),
    /*
     * 2^f Remez/Taylor coefficients evaluated directly in f (no separate
     * r = f*ln2 multiply): Dk = ln2^k / k!, so the ln2 scaling is folded
     * into the constants and incurs no extra runtime rounding.
     */
    .poly = {
        _MM512_SET1_PS16(0x1p+0f),           /* 1            */
        _MM512_SET1_PS16(0x1.62e43p-1f),     /* ln2          */
        _MM512_SET1_PS16(0x1.ebfbep-3f),     /* ln2^2 / 2    */
        _MM512_SET1_PS16(0x1.c6b08ep-5f),    /* ln2^3 / 6    */
        _MM512_SET1_PS16(0x1.3b2ab6p-7f),    /* ln2^4 / 24   */
        _MM512_SET1_PS16(0x1.5d87fep-10f),   /* ln2^5 / 120  */
        _MM512_SET1_PS16(0x1.430912p-13f),   /* ln2^6 / 720  */
        _MM512_SET1_PS16(0x1.ffcbfcp-17f),   /* ln2^7 / 5040 */
    },
};

#define V16_EXP10F_ABS_MASK     v16_exp10f_data.abs_mask
#define V16_EXP10F_ARG_MAX      v16_exp10f_data.arg_max
#define V16_EXP10F_LOG2_10_HI   v16_exp10f_data.log2_10_hi
#define V16_EXP10F_LOG2_10_LO   v16_exp10f_data.log2_10_lo
#define V16_EXP10F_HUGE         v16_exp10f_data.huge
#define V16_EXP10F_E0           v16_exp10f_data.poly[0]
#define V16_EXP10F_E1           v16_exp10f_data.poly[1]
#define V16_EXP10F_E2           v16_exp10f_data.poly[2]
#define V16_EXP10F_E3           v16_exp10f_data.poly[3]
#define V16_EXP10F_E4           v16_exp10f_data.poly[4]
#define V16_EXP10F_E5           v16_exp10f_data.poly[5]
#define V16_EXP10F_E6           v16_exp10f_data.poly[6]
#define V16_EXP10F_E7           v16_exp10f_data.poly[7]

/*
 * Lanes needing scalar fallback: |x| larger than the normal-output threshold,
 * +/-inf or NaN.  A single unsigned compare on the abs bit pattern catches all
 * three (inf/nan bit patterns are numerically larger than any finite value).
 */
static inline ALM_ALWAYS_INLINE v_u32x16_t
vrs16_exp10f_special_mask(v_f32x16_t x)
{
    v_u32x16_t ax = as_v16_u32_f32(x) & V16_EXP10F_ABS_MASK;
    return (ax > as_v16_u32_f32(V16_EXP10F_ARG_MAX));
}

/*
 * Branch-free fast path, no domain checks.  Valid only when 10^x is a normal
 * float (|x| <= ARG_MAX); the caller must route subnormal/overflow/inf/nan
 * lanes to scalar.
 *
 * Identity:
 * 10^x = 2^(x*log2(10)) = 2^n * 2^f
 * where n = round(x*log2(10)) is an integer and f = x*log2(10) - n, |f| <= 1/2.
 * n is applied by the biased exponent of the polynomial result; 2^f is handled by a
 * small polynomial in f.
 */
static inline ALM_ALWAYS_INLINE v_f32x16_t
vrs16_exp10f_fastpath(v_f32x16_t x)
{
    /*
     * Step 1: base-2 exponent estimate and its integer part.
     * phi ~= x*log2(10), using only the HI half of the split constant
     * (the dropped low bits are recovered in step 2).
     */
    v_f32x16_t phi = x * V16_EXP10F_LOG2_10_HI;

    /*
     * Round phi to integer n via (phi + HUGE) - HUGE; n becomes 2^n exponent.
     */
    v_f32x16_t dn = phi + V16_EXP10F_HUGE;
    v_u32x16_t n  = as_v16_u32_f32(dn);
    dn = dn - V16_EXP10F_HUGE;                  /* dn = round(phi) */

    /*
     * Step 2: Compute f = x*log2(10) - n, with |f| <= 1/2.
     * One multiply by log2(10) is not accurate enough, so we use
     * HI and LO parts: recover the rounding error in e, then
     * f = (phi - dn) + x*LO + e.
     */
    /* mul_add == fma(x, y, z) when AMD_LIBM_FMA_USABLE (see poly-vec.h). */
    v_f32x16_t e = mul_add(x, V16_EXP10F_LOG2_10_HI, -phi);
    v_f32x16_t f = (phi - dn) + mul_add(x, V16_EXP10F_LOG2_10_LO, e);

    /* Step 3: Approximate 2^f with a degree-7 polynomial (|f| <= 1/2). */
    v_f32x16_t poly = POLY_EVAL_7(f,
        V16_EXP10F_E0, V16_EXP10F_E1, V16_EXP10F_E2, V16_EXP10F_E3,
        V16_EXP10F_E4, V16_EXP10F_E5, V16_EXP10F_E6, V16_EXP10F_E7);

    /*
     * Step 4: result = poly * 2^n.
     */
    return as_v16_f32_u32(as_v16_u32_f32(poly) + (n << 23));
}

#define SCALAR_EXP10F ALM_PROTO_OPT(exp10f)

static inline v_f32x16_t
exp10f_specialcase(v_f32x16_t x, v_f32x16_t result, v_u32x16_t cond)
{
    return call_v16_f32(SCALAR_EXP10F, x, result, cond);
}

v_f32x16_t
ALM_PROTO_OPT(vrs16_exp10f)(v_f32x16_t x)
{
    v_u32x16_t need_scalar = vrs16_exp10f_special_mask(x);

    /* Fast path: 10^x = 2^(x * log2(10)) */
    v_f32x16_t result = vrs16_exp10f_fastpath(x);

    if (unlikely(any_v16_u32_avx512(need_scalar))) {
        return exp10f_specialcase(x, result, need_scalar);
    }

    return result;
}
