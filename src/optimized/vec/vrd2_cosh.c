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
 *   v_f64x2_t vrd2_cosh(v_f64x2_t x)
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 * See vrd4_cosh.c. This is the 2-wide (128-bit) variant:
 *   z       = exp(|x|)                            (via vrd2_exp)
 *   cosh(x) = 0.5 * z + 0.5 / z
 * For |x| > 705.0 (large magnitude / Inf), the scalar cosh() is used to
 * preserve the correct finite results near overflow and the overflow/Inf
 * behavior. The ARG_MAX test is an unsigned compare on the |x| bit pattern, so
 * it is also true for NaN; NaN takes the scalar cosh() fallback and returns NaN.
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

#include "../vrd2_exp_kernel.h"

static const struct {
    v_u64x2_t arg_max;
    v_f64x2_t half;
} v2_cosh_data = {
    .arg_max = _MM_SET1_I64x2(0x4086080000000000), /* 705.0 */
    .half    = _MM_SET1_PD2(0.5),
};

#define V2_COSH_ARG_MAX v2_cosh_data.arg_max
#define V2_COSH_HALF    v2_cosh_data.half

v_f64x2_t
ALM_PROTO_OPT(vrd2_cosh)(v_f64x2_t x)
{
    v_u64x2_t ux = as_v2_u64_f64(x);
    v_u64x2_t aux = ux & ~SIGNBIT_DP64;

    v_u64x2_t cond = aux > V2_COSH_ARG_MAX;

    /* Zero the exp input on fallback lanes so exp() cannot raise a spurious
     * overflow/underflow on values the scalar cosh() will overwrite. */
    v_f64x2_t ax = as_v2_f64_u64(aux & ~cond);

    /* Out-of-range lanes are already zeroed above and overwritten by the
     * scalar cosh() fallback below, so the domain-check-free exp fastpath is
     * safe here and skips vrd2_exp's redundant range test. */
    v_f64x2_t z = vrd2_exp_fastpath(ax);

    v_f64x2_t result = V2_COSH_HALF * z + V2_COSH_HALF / z;

    if (unlikely(any_v2_u64_loop(cond))) {
        result = call_v2_f64(ALM_PROTO_OPT(cosh), x, result, cond);
    }

    return result;
}
