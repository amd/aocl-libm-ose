/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
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
 *   v_f32x16_t vrs16_coshf(v_f32x16_t x)
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 * See vrs4_coshf.c / vrs8_coshf.c. This is the 16-wide (AVX-512) variant.
 *
 * Derivation
 * ----------
 * Starting from the definition cosh(x) = (e^|x| + e^-|x|) / 2, factor a scale
 * V out of each exponential argument by adding and subtracting log(V):
 *
 *   e^|x|  = V * e^(|x| - log(V))
 *   e^-|x| = (1/V) * e^-(|x| - log(V)) = (1/V) / e^(|x| - log(V))
 *
 * Substituting and letting w = e^(|x| - log(V)) gives
 *
 *   coshf(x) = V/2 * (w + (1/V^2) / w)
 *            = V/2 * (exp(|x| - log(V)) + (1/V^2) / exp(|x| - log(V)))
 *
 * Choice of V
 * -----------
 * V shifts the exp argument down by log(V), so the inner exp(|x| - log(V))
 * stays below the expf overflow limit for a wider range of |x|, extending the
 * vector fast path before the scalar coshf fallback is needed. V just above 2
 * (V/2 = 0x1.0000e8p0f, approx 1.0000138, so V approx 2.0000276) is used here;
 * the exact value is the float nearest to the constant that keeps the fast path
 * bit-for-bit with vrs16_expf across the handoff. (A value of V = 2 exactly
 * would make V/2 = 1 and drop the final multiply, at the cost of a marginally
 * smaller fast-path interval - see the arg_max note below.) The halfV constant
 * below holds this V/2 value.
 *
 * exp(|x| - log(V)) is produced by the vrs16_expf fast path (no domain checks).
 * That fast path matches the public vrs16_expf bit-for-bit only while its
 * argument stays within the scalar-expf handoff threshold, so any lane whose
 * |x| exceeds arg_max (which also covers overflow / Inf / NaN) is handled by
 * the scalar coshf() instead.
 */

#include <stdint.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>

#include "vrs16_expf.h"

static const struct {
    v_u32x16_t arg_max;
    v_u32x16_t sign_mask;
    v_f32x16_t logV;
    v_f32x16_t invV2;
    v_f32x16_t halfV;
} v16_coshf_data = {
    .sign_mask = _MM512_SET1_U32x16(0x7FFFFFFFu),
    /*
     * |x| above this takes the scalar coshf path. The vrs16_expf fast path is
     * bit-for-bit valid only while its argument (|x| - log(V)) stays within the
     * scalar-expf handoff threshold (expf ARG_MAX = 0x42AE0000, i.e. 87.0f);
     * 0x42AF62E6 (87.6931610f) is the largest |x| for which |x| - log(V) <= 87.0f.
     * This threshold sits below the coshf overflow limit (0x42B2D4FC), so the
     * scalar coshf fallback also covers the overflow / Inf / NaN lanes.
     */
    .arg_max   = _MM512_SET1_U32x16(0x42AF62E6u),
    .logV      = _MM512_SET1_PS16(0x1.62e6p-1f),
    .invV2     = _MM512_SET1_PS16(0x1.fffc6p-3f),
    .halfV     = _MM512_SET1_PS16(0x1.0000e8p0f),
};

#define V16_SIGN_MASK v16_coshf_data.sign_mask
#define V16_LOGV      v16_coshf_data.logV
#define V16_INVV2     v16_coshf_data.invV2
#define V16_HALFV     v16_coshf_data.halfV
#define V16_ARG_MAX   v16_coshf_data.arg_max

static inline v_f32x16_t
coshf_specialcase(v_f32x16_t _x, v_f32x16_t result, v_u32x16_t cond)
{
    return call_v16_f32(ALM_PROTO_OPT(coshf), _x, result, cond);
}

v_f32x16_t
ALM_PROTO_OPT(vrs16_coshf)(v_f32x16_t x)
{
    v_u32x16_t ux = as_v16_u32_f32(x) & V16_SIGN_MASK;

    v_f32x16_t y = as_v16_f32_u32(ux);

    v_u32x16_t cond = ux > V16_ARG_MAX;

    /*
     * Inline the vrs16_expf fast path directly: lanes with |x| > ARG_MAX are
     * already routed to scalar coshf below, so the expf out-of-range check is
     * redundant here and is skipped for throughput.
     */
    v_f32x16_t z = vrs16_expf_fastpath(y - V16_LOGV);

    v_f32x16_t result = V16_HALFV * (z + V16_INVV2 / z);

    if (unlikely(any_v16_u32_loop(cond))) {
        return coshf_specialcase(x, result, cond);
    }

    return result;
}
