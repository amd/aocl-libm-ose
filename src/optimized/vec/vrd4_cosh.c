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
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   v_f64x4_t vrd4_cosh(v_f64x4_t x)
 *
 * Spec:
 *   cosh(0)    = 1
 *   cosh(+Inf) = +Inf
 *   cosh(NaN)  = NaN
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 *
 * cosh is an even function: cosh(-x) = cosh(x).
 *
 * For |x| <= COSH_ARG_MAX the value is computed from the double-precision
 * vector exponential:
 *
 *      z        = exp(|x|)
 *      cosh(x)  = 0.5 * z + 0.5 / z   = 0.5 * (exp(|x|) + exp(-|x|))
 *
 * cosh(x) >= 1 for all x, so this reconstruction never suffers cancellation.
 * COSH_ARG_MAX is chosen at 705.0 (bit pattern 0x4086080000000000): below it
 * exp(|x|) stays finite and the 0.5/z term never underflows, so the vector
 * path raises no spurious overflow/underflow. For |x| > COSH_ARG_MAX (this also
 * covers the narrow finite band close to the cosh overflow threshold and
 * +/-Inf) the scalar cosh() is used so the correct finite result, overflow, and
 * Inf handling are preserved. The exp input is additionally zeroed on those
 * fallback lanes so exp() itself raises no spurious flags. The COSH_ARG_MAX
 * test is an unsigned compare on the |x| bit pattern, so it is true for NaN as
 * well (a NaN bit pattern exceeds that of COSH_ARG_MAX); NaN therefore takes
 * the scalar cosh() fallback, which returns NaN.
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

#include "../vrd4_exp_kernel.h"

static const struct {
    v_u64x4_t arg_max;
    v_f64x4_t half;
} v4_cosh_data = {
    .arg_max = _MM_SET1_I64x4(0x4086080000000000), /* 705.0 */
    .half    = _MM_SET1_PD4(0.5),
};

#define V4_COSH_ARG_MAX v4_cosh_data.arg_max
#define V4_COSH_HALF    v4_cosh_data.half

v_f64x4_t
ALM_PROTO_OPT(vrd4_cosh)(v_f64x4_t x)
{
    v_u64x4_t ux = as_v4_u64_f64(x);

    /* aux = |x| (as bits) */
    v_u64x4_t aux = ux & ~SIGNBIT_DP64;

    /* Elements needing the scalar path (large magnitude / Inf). */
    v_u64x4_t cond = aux > V4_COSH_ARG_MAX;

    /* Zero the exp input on fallback lanes so exp() cannot raise a spurious
     * overflow/underflow on values the scalar cosh() will overwrite. */
    v_f64x4_t ax = as_v4_f64_u64(aux & ~cond);

    /* Out-of-range lanes are already zeroed above and overwritten by the
     * scalar cosh() fallback below, so the domain-check-free exp fastpath is
     * safe here and skips vrd4_exp's redundant range test. */
    v_f64x4_t z = vrd4_exp_fastpath(ax);

    v_f64x4_t result = V4_COSH_HALF * z + V4_COSH_HALF / z;

    if (unlikely(any_v4_u64_loop(cond))) {
        result = call_v4_f64(ALM_PROTO_OPT(cosh), x, result, cond);
    }

    return result;
}
