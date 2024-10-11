/*
 * Copyright (C) 2024 Advanced Micro Devices, Inc. All rights reserved.
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
#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>


/*
 * Signature:
 *    void amd_vrs8_sincosf(v_f32x8_t x, v_f32x8_t *result_sin, v_f32x8_t *result_cos)
 *
 *
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 *
 * Convert given x into the form
 * |x| = N * pi + f where N is an integer and f lies in [-pi/2,pi/2]
 * N is obtained by : N = round(x/pi)
 * f is obtained by : f = abs(x)-N*pi
 * sin(x) = sin(N * pi + f) = sin(N * pi)*cos(f) + cos(N*pi)*sin(f)
 * sin(x) = sign(x)*sin(f)*(-1)**N
 *
 * The term sin(f) can be approximated by using a polynomial
 *
 ******************************************
*/

static struct {
    v_f32x8_t invpi, pi1, pi2, pi3, half, shift;
    v_u32x8_t sign_mask, max_arg;
    v_f32x8_t poly_sincosf[8];
 } v8_sincosf_data = {
     .max_arg = _MM256_SET1_I32(0x49800000),
     .sign_mask  = _MM256_SET1_I32(0x7fffffff),
     .pi1   = _MM256_SET1_PS8(0x1.921fb6p1f),
     .pi2   = _MM256_SET1_PS8(0x1.777a5cp-24f),
     .pi3   = _MM256_SET1_PS8(0x1.ee59dap-49f),
     .half  = _MM256_SET1_PS8(0x1p-1f),
     .invpi = _MM256_SET1_PS8(0x1.45f306p-2f),
     .shift = _MM256_SET1_PS8(0x1.8p23f),
     /*
      * Polynomial coefficients obtained using Remez algorithm from Sollya
      */
     .poly_sincosf = {
         _MM256_SET1_PS8(0x1.p0f),
         _MM256_SET1_PS8(-0x1.555548p-3f),
         _MM256_SET1_PS8(0x1.110e7cp-7f),
         _MM256_SET1_PS8(-0x1.9f6446p-13f),
         _MM256_SET1_PS8(0x1.5d38b6p-19f),

     },
};


#define pi1     v8_sincosf_data.pi1
#define pi2     v8_sincosf_data.pi2
#define pi3     v8_sincosf_data.pi3
#define invpi   v8_sincosf_data.invpi
#define HALF    v8_sincosf_data.half
#define ALM_SHIFT   v8_sincosf_data.shift
#define ARG_MAX     v8_sincosf_data.max_arg
#define SIGN_MASK32 v8_sincosf_data.sign_mask

#define C1  v8_sincosf_data.poly_sincosf[0]
#define C3  v8_sincosf_data.poly_sincosf[1]
#define C5  v8_sincosf_data.poly_sincosf[2]
#define C7  v8_sincosf_data.poly_sincosf[3]
#define C9  v8_sincosf_data.poly_sincosf[4]

static inline v_f32x8_t
sin_specialcase(v_f32x8_t _x,
                v_f32x8_t result,
                v_u32x8_t cond)
{
    return (v_f32x8_t) {
            (cond[0]) ? ALM_PROTO(sinf)(_x[0]):result[0],
            (cond[1]) ? ALM_PROTO(sinf)(_x[1]):result[1],
            (cond[2]) ? ALM_PROTO(sinf)(_x[2]):result[2],
            (cond[3]) ? ALM_PROTO(sinf)(_x[3]):result[3],
            (cond[4]) ? ALM_PROTO(sinf)(_x[4]):result[4],
            (cond[5]) ? ALM_PROTO(sinf)(_x[5]):result[5],
            (cond[6]) ? ALM_PROTO(sinf)(_x[6]):result[6],
            (cond[7]) ? ALM_PROTO(sinf)(_x[7]):result[7],
        };
}

static inline v_f32x8_t
cos_specialcase(v_f32x8_t _x,
            v_f32x8_t result,
                v_u32x8_t cond)
{
    return (v_f32x8_t) {
            (cond[0]) ? ALM_PROTO(cosf)(_x[0]):result[0],
            (cond[1]) ? ALM_PROTO(cosf)(_x[1]):result[1],
            (cond[2]) ? ALM_PROTO(cosf)(_x[2]):result[2],
            (cond[3]) ? ALM_PROTO(cosf)(_x[3]):result[3],
            (cond[4]) ? ALM_PROTO(cosf)(_x[4]):result[4],
            (cond[5]) ? ALM_PROTO(cosf)(_x[5]):result[5],
            (cond[6]) ? ALM_PROTO(cosf)(_x[6]):result[6],
            (cond[7]) ? ALM_PROTO(cosf)(_x[7]):result[7],
        };
}

void
ALM_PROTO_OPT(vrs8_sincosf)(v_f32x8_t x, v_f32x8_t *result_sin, v_f32x8_t *result_cos)
{

    v_f32x8_t r, F_sin, F_cos, poly_sin, poly_cos;
    v_u32x8_t n_sin, n_cos, sign;

    v_u32x8_t ux = as_v8_u32_f32(x);

    sign = ux & ~SIGN_MASK32;

    /* Check for special cases */
    v_u32x8_t cmp = (ux & SIGN_MASK32) > (ARG_MAX);

    /* Remove sign from the input */
    r  = as_v8_f32_u32(ux & SIGN_MASK32);

    v_f32x8_t dn_sin =  (r * invpi) + ALM_SHIFT;
    n_sin = as_v8_u32_f32(dn_sin);
    dn_sin -= ALM_SHIFT;

    F_sin = r - dn_sin * pi1;
    F_sin = F_sin - dn_sin * pi2;
    F_sin = F_sin - dn_sin * pi3;

    v_f32x8_t dn_cos =  (r * invpi + HALF) + ALM_SHIFT;
    n_cos = as_v8_u32_f32(dn_cos);
    dn_cos -= ALM_SHIFT;
    dn_cos -= HALF;

    F_cos = r - dn_cos * pi1;
    F_cos = F_cos - dn_cos * pi2;
    F_cos = F_cos - dn_cos * pi3;

    /* Check whether n is odd or not */
    v_u32x8_t odd_sin =  n_sin << 31;
    v_u32x8_t odd_cos =  n_cos << 31;

    /*
     *  Calculate the polynomial approximations
     *
     *   poly = x + C1*x^3 + C3*x^5 + C5*x^7 + C7*x^9 + C9*x^11
     *
     */
    poly_sin = F_sin + POLY_EVAL_ODD_9(F_sin, C1, C3, C5, C7, C9);
    poly_cos = F_cos + POLY_EVAL_ODD_9(F_cos, C1, C3, C5, C7, C9);

    *result_sin = as_v8_f32_u32(as_v8_u32_f32(poly_sin) ^ sign ^ odd_sin);
    *result_cos = as_v8_f32_u32(as_v8_u32_f32(poly_cos) ^ odd_cos);

    if(unlikely(any_v8_u32_loop(cmp))) {
      *result_sin = sin_specialcase(x, *result_sin, cmp);
      *result_cos = cos_specialcase(x, *result_cos, cmp);
    }

    return;
}
