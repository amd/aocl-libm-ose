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
 * Signature:
 *   v_f32x8_t cosf(v_f32x8_t x)
 *
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 * To compute cosf(float x)
 * Using the identity,
 * cos(x) = sin(x + pi/2)           (1)
 *
 * 1. Argument Reduction
 *      Now, let x be represented as,
 *          |x| = N * pi + f        (2) | N is an integer,
 *                                        -pi/2 <= f <= pi/2
 *
 *      From (2), N = int( (x + pi/2) / pi) - 0.5
 *                f = |x| - (N * pi)
 *
 * 2. Polynomial Evaluation
 *       From (1) and (2),sin(f) can be calculated using a polynomial
 *       sin(f) = f*(1 + C1*f^2 + C2*f^4 + C3*f^6 + c4*f^8)
 *
 * 3. Reconstruction
 *      Hence, cos(x) = sin(x + pi/2) = (-1)^N * sin(f)
 *
 * MAX ULP of current implementation : 1
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

            v_f32x8_t poly_cosf[5];
            v_f32x8_t half;
            v_f32x8_t half_pi, inv_pi, pi_head, pi_tail1, pi_tail2;
            v_f32x8_t alm_huge;
            v_u32x8_t mask_32, arg_max;
            } v8_cosf_data = {
                            .half     = _MM256_SET1_PS8(0x1p-1f),
                            .half_pi   = _MM256_SET1_PS8(0x1.921fb6p0f),
                            .inv_pi    = _MM256_SET1_PS8(0x1.45f306p-2f),
                            .pi_head   = _MM256_SET1_PS8(-0x1.921fb6p1f),
                            .pi_tail1  = _MM256_SET1_PS8(0x1.777a5cp-24f),
                            .pi_tail2  = _MM256_SET1_PS8(0x1.ee59dap-49f),
                            .alm_huge  = _MM256_SET1_PS8(0x1.8p23),
                            .mask_32   = _MM256_SET1_I32(0x7FFFFFFF),
                            .arg_max   = _MM256_SET1_I32(0x4A989680),
                            .poly_cosf = {
                                            _MM256_SET1_PS8(0x1.p0),
                                            _MM256_SET1_PS8(-0x1.555548p-3f),
                                            _MM256_SET1_PS8(0x1.110df4p-7f),
                                            _MM256_SET1_PS8(-0x1.9f42eap-13f),
                                            _MM256_SET1_PS8(0x1.5b2e76p-19f),
                                         },
            };

#define V8_COSF_HALF       v8_cosf_data.half
#define V8_COSF_HALF_PI    v8_cosf_data.half_pi
#define V8_COSF_INV_PI     v8_cosf_data.inv_pi
#define V8_COSF_PI_HEAD    v8_cosf_data.pi_head
#define V8_COSF_PI_TAIL1   v8_cosf_data.pi_tail1
#define V8_COSF_PI_TAIL2   v8_cosf_data.pi_tail2
#define V8_COSF_MASK_32    v8_cosf_data.mask_32
#define V8_COSF_ARG_MAX    v8_cosf_data.arg_max
#define ALM_HUGE           v8_cosf_data.alm_huge

#define C0 v8_cosf_data.poly_cosf[0]
#define C1 v8_cosf_data.poly_cosf[1]
#define C2 v8_cosf_data.poly_cosf[2]
#define C3 v8_cosf_data.poly_cosf[3]
#define C4 v8_cosf_data.poly_cosf[4]


static inline v_f32x8_t
cosf_specialcase(v_f32x8_t _x, v_f32x8_t result, v_u32x8_t cond)
{
    return call_v8_f32(ALM_PROTO(cosf), _x, result, cond);
}


v_f32x8_t
ALM_PROTO_OPT(vrs8_cosf)(v_f32x8_t x)
{

    v_f32x8_t dinput, frac, poly, result;

    v_u32x8_t ux = as_v8_u32_f32(x);

    /* Check for special cases */
    v_u32x8_t cond = (ux & V8_COSF_MASK_32) > V8_COSF_ARG_MAX;

    /* Remove sign from input */
    dinput = as_v8_f32_u32(ux & V8_COSF_MASK_32);

    /* Get n = int ((x + pi/2) /pi) - 0.5 */
    v_f32x8_t dn = ((dinput + V8_COSF_HALF_PI) * V8_COSF_INV_PI) + ALM_HUGE;
    v_u32x8_t n = as_v8_u32_f32(dn);
    dn = dn - ALM_HUGE;
    dn = dn - V8_COSF_HALF;

    /* frac = x - (n*pi) */
    frac = dinput + (dn * V8_COSF_PI_HEAD);
    frac = frac + (dn * V8_COSF_PI_TAIL1);
    frac = frac + (dn * V8_COSF_PI_TAIL2);

    /* Check if n is odd or not */
    v_u32x8_t odd = n << 31;

    /* Compute sin(f) using the polynomial
     * x*(1+C1*x^2+C2*x^4+C3*x^6+C4*x^8)
     */
    poly = POLY_EVAL_ODD_9(frac, C0, C1, C2, C3, C4);

    /* If n is odd, result is negative */
    result = as_v8_f32_u32(as_v8_u32_f32(poly) ^ odd);

    /* If any of the input values are greater than ARG_MAX,
     * call scalar cosf
     */
    if(unlikely(any_v8_u32_loop(cond)))
        return cosf_specialcase(x, result, cond);

    return result;
}

