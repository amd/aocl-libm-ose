/*
 * Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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
 *   v_f32x4_t vrs4_tanhf(v_f32x4_t x)
 *
 ******************************************
 * Implementation Notes
 * ----------------------
 * To compute vrs4_tanhf(v_f32x4_t x)
 * Let y = |x|
 * If 0 <= y < 0x1.154246p3
 *    Let z = e^(-2.0 * y) - 1      -(1)
 *
 *    Using (1), tanhf(y) can be calculated as,
 *    tanhf(y) = -z / (z + 2.0)
 *
 * For other cases, call scalar tanhf()
 *
 * If x < 0, then we use the identity
 *       tanhf(-x) = -tanhf(x)
 *
 * Max ULP of current implementation: 1
 *
 */

#include <stdint.h>
#include <emmintrin.h>

#include <libm_util_amd.h>
#include <libm_special.h>
#include <libm_macros.h>

#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/poly.h>

static struct {
                v_u32x4_t arg_max, sign_mask;
                v_f32x4_t one, two;
              } v4_tanhf_data = {
                                    .arg_max   = _MM_SET1_I32(0x410AA123),
                                    .sign_mask = _MM_SET1_I32(0x7FFFFFFF),
                                    .two       = _MM_SET1_PS4(0x1p1f),
                                    .one       = _MM_SET1_PS4(0x1p0f),
                                };

#define V4_TANHF_ARG_MAX    v4_tanhf_data.arg_max
#define V4_TANHF_SIGN_MASK  v4_tanhf_data.sign_mask
#define V4_TANHF_ONE        v4_tanhf_data.one
#define V4_TANHF_TWO        v4_tanhf_data.two

float ALM_PROTO(tanhf)(float);
v_f32x4_t ALM_PROTO(vrs4_expf)(v_f32x4_t);

static inline v_f32x4_t
tanhf_specialcase(v_f32x4_t _x, v_f32x4_t result, v_u32x4_t cond, v_u32x4_t sign)
{
    return call_v4_f32(ALM_PROTO(tanhf), _x, result, cond);
}

v_f32x4_t
ALM_PROTO_OPT(vrs4_tanhf)(v_f32x4_t x)
{

    /* Get sign of input argument */
    v_u32x4_t ux = as_v4_u32_f32(x);
    v_u32x4_t sign = ux & (~V4_TANHF_SIGN_MASK);

    /* Get absolute value of input argument */
    ux = ux & V4_TANHF_SIGN_MASK;
    v_f32x4_t y = as_v4_f32_u32(ux);

    /* Check for special cases */
    v_u32x4_t cond = ux  <=  V4_TANHF_ARG_MAX;

    /* z = e^(-2.0 * y) + 1 */
    v_f32x4_t z = ALM_PROTO(vrs4_expf)( -V4_TANHF_TWO * y);
    z = z - V4_TANHF_ONE;

    /* Calculate -z / (z + 2.0) */
    v_f32x4_t result = -z / (z + V4_TANHF_TWO);

    /* Result is -ve if input argument is -ve */
    result = as_v4_f32_u32(sign ^ as_v4_u32_f32(result));

    /* If any of the input values are greater than ARG_MAX,
     * call scalar tanhf
     */
    if(unlikely(any_v4_u32_loop(cond)))
        return tanhf_specialcase(x, result,cond, sign);

    return result;

}

