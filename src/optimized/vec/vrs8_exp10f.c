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
 *   v_f32x8_t amd_vrs8_exp10f(v_f32x8_t x)
 *
 * ---------------------------------------------------------------------------
 * Math (what we compute)
 * ---------------------------------------------------------------------------
 *   10^x = 2^(x * log2(10)) = 2^n * 2^f,   n = round(x*log2_10),
 *                                          f = x*log2_10 - n,  |f| <= 1/2
 *
 * Fast path (see vrs8_exp10f_fastpath below):
 *   1. Split x*log2(10) into integer n and small fraction f (|f| <= 1/2).
 *   2. Approximate 2^f with a degree-7 polynomial in f.
 *   3. Fold 2^n into the polynomial result's biased exponent.
 *
 * ---------------------------------------------------------------------------
 * Control flow
 * ---------------------------------------------------------------------------
 *   1. Check |x| for each lane (one unsigned compare on the float bits).
 *   2. If |x| is too large, or the lane is inf/NaN, mark it for scalar exp10f.
 *   3. Always run the vector fast path on all 8 lanes.
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

#include "../vrs8_exp10f_data.h"

/*
 * Lanes needing scalar fallback: |x| larger than the normal-output threshold,
 * +/-inf or NaN.  A single unsigned compare on the abs bit pattern catches all
 * three (inf/nan bit patterns are numerically larger than any finite value).
 */
static inline ALM_ALWAYS_INLINE v_u32x8_t
vrs8_exp10f_special_mask(v_f32x8_t x)
{
    v_u32x8_t ax = as_v8_u32_f32(x) & V8_EXP10F_ABS_MASK;
    return (ax > as_v8_u32_f32(V8_EXP10F_ARG_MAX));
}

/*
 * Branch-free fast path, no domain checks.  Valid only when 10^x is a normal
 * float (|x| <= ARG_MAX); the caller must route subnormal/overflow/inf/nan
 * lanes to scalar.
 *
 * Identity:
 *   10^x = 2^(x*log2(10)) = 2^n * 2^f
 * where n = round(x*log2(10)) is an integer and f = x*log2(10) - n, |f| <= 1/2.
 * n is applied by the biased exponent of the polynomial result; 2^f is handled by a
 * small polynomial in f.
 */
static inline ALM_ALWAYS_INLINE v_f32x8_t
vrs8_exp10f_fastpath(v_f32x8_t x)
{
    /*
     * Step 1: base-2 exponent estimate and its integer part.
     * phi ~= x*log2(10), using only the HI half of the split constant
     * (the dropped low bits are recovered in step 2).
     */
    v_f32x8_t phi = x * V8_EXP10F_LOG2_10_HI;

    /*
     * Round phi to integer n via (phi + HUGE) - HUGE; n becomes 2^n exponent.
     */
    v_f32x8_t dn = phi + V8_EXP10F_HUGE;
    v_u32x8_t n  = as_v8_u32_f32(dn);
    dn = dn - V8_EXP10F_HUGE;                  /* dn = round(phi) */

    /*
     * Step 2: Compute f = x*log2(10) - n, with |f| <= 1/2.
     * One multiply by log2(10) is not accurate enough, so we use
     * HI and LO parts: recover the rounding error in e, then
     * f = (phi - dn) + x*LO + e.
     */
    /* mul_add == fma(x, y, z) when AMD_LIBM_FMA_USABLE (see poly-vec.h). */
    v_f32x8_t e = mul_add(x, V8_EXP10F_LOG2_10_HI, -phi);
    v_f32x8_t f = (phi - dn) + mul_add(x, V8_EXP10F_LOG2_10_LO, e);

    /* Step 3: Approximate 2^f with a degree-7 polynomial (|f| <= 1/2). */
    v_f32x8_t poly = POLY_EVAL_7(f,
        V8_EXP10F_E0, V8_EXP10F_E1, V8_EXP10F_E2, V8_EXP10F_E3,
        V8_EXP10F_E4, V8_EXP10F_E5, V8_EXP10F_E6, V8_EXP10F_E7);

    /*
     * Step 4: result = poly * 2^n.
     */
    return as_v8_f32_u32(as_v8_u32_f32(poly) + (n << 23));
}

#define SCALAR_EXP10F ALM_PROTO_OPT(exp10f)

static inline v_f32x8_t
exp10f_specialcase(v_f32x8_t x, v_f32x8_t result, v_u32x8_t cond)
{
    return call_v8_f32(SCALAR_EXP10F, x, result, cond);
}

v_f32x8_t
ALM_PROTO_OPT(vrs8_exp10f)(v_f32x8_t x)
{
    v_u32x8_t need_scalar = vrs8_exp10f_special_mask(x);
    int any_need_scalar = any_v8_u32_loop(need_scalar);

    /* Fast path: 10^x = 2^(x * log2(10)) */
    v_f32x8_t result = vrs8_exp10f_fastpath(x);

    if (unlikely(any_need_scalar)) {
        return exp10f_specialcase(x, result, need_scalar);
    }

    return result;
}
