/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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
 *   v_f32x4_t coshf(v_f32x4_t x)
 *
 * Spec:
 *   coshf(|x| > 89.415985107421875) = Infinity
 *   coshf(Infinity)  = infinity
 *   coshf(-Infinity) = infinity
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 *
 * cosh(x) = (exp(x) + exp(-x))/2
 * cosh(-x) = +cosh(x)
 *
 * checks for special cases
 * if ( asint(x) > infinity) return x with overflow exception and
 * return x.
 * if x is NaN then raise invalid FP operation exception and return x.
 *
 *  coshf = v/2 * exp(x - log(v)) where v = 0x1.0000e8p-1
 *
 */

#include <stdint.h>
#include <emmintrin.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>

static struct {
    v_u32x4_t arg_max, sign_mask;
    v_f32x4_t logV, invV2, halfV;
    v_f32x4_t one;
 } v4_coshf_data = {
        .sign_mask = _MM_SET1_I32(0x7FFFFFFF),
        .arg_max = _MM_SET1_I32(0x42B2D4FC),
        .logV = _MM_SET1_PS4(0x1.62e6p-1f),
        .invV2 = _MM_SET1_PS4(0x1.fffc6p-3f),
        .halfV = _MM_SET1_PS4(0x1.0000e8p0f),
        .one = _MM_SET1_PS4(1.0f),
};

#define SIGN_MASK v4_coshf_data.sign_mask
#define LOGV      v4_coshf_data.logV
#define INVV2     v4_coshf_data.invV2
#define ONE       v4_coshf_data.one
#define HALFVM1   v4_coshf_data.halfVm1
#define HALFV     v4_coshf_data.halfV
#define ARG_MAX   v4_coshf_data.arg_max


static inline v_f32x4_t
coshf_specialcase(v_f32x4_t _x, v_f32x4_t result, v_u32x4_t cond)
{
    return call_v4_f32(ALM_PROTO(coshf), _x, result, cond);
}


v_f32x4_t ALM_PROTO_OPT(vrs4_coshf)(v_f32x4_t x)
{

    v_f32x4_t z, y, result;

    v_u32x4_t ux = as_v4_u32_f32(x) & SIGN_MASK;

    y = as_v4_f32_u32(ux);

    v_u32x4_t cond = ux > ARG_MAX;

    z = ALM_PROTO(vrs4_expf)(y - LOGV);

    result = HALFV * (z + INVV2 * ONE / z);

    if(unlikely(any_v4_u32_loop(cond))) {

        return coshf_specialcase(x, result, cond);

    }

    return result;

}




