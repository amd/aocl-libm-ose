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
 *   v_f32x8_t amd_vrs8_cbrtf(v_f32x8_t x)
 *
 * ---------------------------------------------------------------------------
 * Math (what we compute)
 * ---------------------------------------------------------------------------
 *   cbrt(x) = sign(x) * |x|^(1/3)
 *           = sign(x) * 2^(log2(|x|) / 3)
 *
 * Fast path uses:  |x|^(1/3) ≈ exp2( log2(|x|) * (1/3) )  on |x|, then OR sign.
 *
 * ---------------------------------------------------------------------------
 * Control flow —
 * ---------------------------------------------------------------------------
 *   Strip the sign bit to get ux_abs, then extract the biased exponent:
 *
 *       exp_biased = ux_abs >> 23    (values 0x00 .. 0xFF for float32)
 *
 *   All special cases (zero, subnormal, huge finite, inf, NaN) are detected
 *   with one unsigned compare by subtracting 1 and checking >= 0xDF:
 *
 *       need_scalar = (exp_biased - 1) >= 0xDF
 *
 *   This works because unsigned subtraction wraps around for exp_biased == 0:
 *
 *       exp_biased   case              (exp_biased - 1) unsigned   >= 0xDF?
 *       ──────────   ────              ────────────────────────    ────────
 *       0x00         ±0 / subnormal    0xFFFFFFFF (wrap-around)    YES → scalar
 *       0x01..0xDF   normal (safe)     0x00 .. 0xDE                 no → fast path
 *       0xE0..0xFE   huge finite       0xDF .. 0xFD                YES → scalar
 *       0xFF         ±inf / NaN        0xFE                        YES → scalar
 *
 *   The vector fast path (log2f, ×⅓, exp2f, OR sign) always runs.
 *   One unlikely branch at the end blends scalar cbrtf results into
 *   lanes that need special handling (zeros, subnormals, huge, inf, NaN).
 *
 * ---------------------------------------------------------------------------
 * Inlined log2f / exp2f kernels
 * ---------------------------------------------------------------------------
 *   This implementation uses the inline vector log2f / exp2f kernels
 *   defined in vrs8_log2f_data.h and vrs8_exp2f_data.h respectively,
 *   so the cbrtf fast path can reuse that math with full inlining here.
 */
#include <stdint.h>
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

#include "../vrs8_log2f_data.h"
#include "../vrs8_exp2f_data.h"

static const struct {
    v_u32x8_t sign_mask;
    v_u32x8_t abs_mask;
    v_u32x8_t ones;
    v_u32x8_t scalar_threshold;   /* 0xDF: (exp_biased - 1) >= this => scalar */
    v_f32x8_t one_third;
} v8_cbrtf_data = {
    .sign_mask        = _MM256_SET1_I32(SIGNBIT_SP32),
    .abs_mask         = _MM256_SET1_I32(POS_BITSET_F32),
    .ones             = _MM256_SET1_I32(1),
    .scalar_threshold = _MM256_SET1_I32(0xDF),
    .one_third        = _MM256_SET1_PS8(0x1.555556p-2f),  /* 1/3 */
};

#define V8_CBRTF_SIGN_MASK        v8_cbrtf_data.sign_mask
#define V8_CBRTF_ABS_MASK         v8_cbrtf_data.abs_mask
#define V8_CBRTF_ONES             v8_cbrtf_data.ones
#define V8_CBRTF_SCALAR_THRESHOLD v8_cbrtf_data.scalar_threshold
#define V8_CBRTF_ONE_THIRD        v8_cbrtf_data.one_third

#define SCALAR_CBRTF ALM_PROTO_OPT(cbrtf)

static inline v_f32x8_t
cbrtf_specialcase(v_f32x8_t _x,
                  v_f32x8_t result,
                  v_u32x8_t cond)
{
    return call_v8_f32(SCALAR_CBRTF, _x, result, cond);
}

v_f32x8_t
ALM_PROTO_OPT(vrs8_cbrtf)(v_f32x8_t x)
{
    v_u32x8_t ux = as_v8_u32_f32(x);
    v_u32x8_t sign = ux & V8_CBRTF_SIGN_MASK;
    v_u32x8_t ux_abs = ux & V8_CBRTF_ABS_MASK;

    v_u32x8_t exp_biased = ux_abs >> 23;
    v_u32x8_t need_scalar =
        ((exp_biased - V8_CBRTF_ONES) >= V8_CBRTF_SCALAR_THRESHOLD);

    int any_need_scalar = any_v8_u32_loop(need_scalar);

    /* Vector fast path: log2(|x|) * (1/3) -> exp2 -> OR sign bits */
    v_f32x8_t x_abs = as_v8_f32_u32(ux_abs);
    v_f32x8_t log2_val = vrs8_log2f_fastpath(x_abs);
    v_f32x8_t log2_third = log2_val * V8_CBRTF_ONE_THIRD;
    v_f32x8_t computed = vrs8_exp2f_fastpath(log2_third);

    /* Newton-Raphson refinement: y = (2y + x/y²) / 3 */
    v_f32x8_t y2 = computed * computed;
    computed = (computed + computed + x_abs / y2) * V8_CBRTF_ONE_THIRD;

    v_u32x8_t result_u32 = as_v8_u32_f32(computed) | sign;
    v_f32x8_t result = as_v8_f32_u32(result_u32);

    if (unlikely(any_need_scalar)) {
        return cbrtf_specialcase(x, result, need_scalar);
    }

    return result;
}
