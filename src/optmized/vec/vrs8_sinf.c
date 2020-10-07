/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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
#include <libm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/poly.h>


/*
 * Signature:
 *    v_f32x8_t amd_vrs8_sinf(v_f32x8_t x)
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
    v_f32x8_t invpi, pi1, pi2, pi3;
    v_f32x8_t shift;
    v_u32x8_t mask32;
    v_u32x8_t max_arg;
    v_f32x8_t poly_sinf[5];
 } v8_sinf_data = {
     .max_arg = _MM256_SET1_I32(0x4A989680),
     .mask32    = _MM256_SET1_I32(0x7fffffff),
     .pi1    = _MM256_SET1_PS8(-0x1.921fb6p1),
     .pi2   = _MM256_SET1_PS8(0x1.777a5cp-24),
     .pi3   = _MM256_SET1_PS8(0x1.ee59dap-49),
     .invpi = _MM256_SET1_PS8(0x1.45f306p-2),
     .shift = _MM256_SET1_PS8(0x1.8p23),
     /*
      * Polynomial coefficients obtained using Remez algorithm from Sollya
      */
     .poly_sinf = {
         _MM256_SET1_PS8(0x1.p0),
         _MM256_SET1_PS8(-0x1.555548p-3),
         _MM256_SET1_PS8(0x1.110e7cp-7),
         _MM256_SET1_PS8(-0x1.9f6446p-13),
         _MM256_SET1_PS8(0x1.5d38b6p-19)
     },
};


#define V8_SINF_PI1       v8_sinf_data.pi1
#define V8_SINF_PI2       v8_sinf_data.pi2
#define V8_SINF_PI3       v8_sinf_data.pi3
#define V8_SINF_INVPI     v8_sinf_data.invpi
#define ALM_SHIFT_SP      v8_sinf_data.shift
#define V8_SINF_ARG_MAX   v8_sinf_data.max_arg
#define ALM_SIGN_MASK32   v8_sinf_data.mask32

#define C1  v8_sinf_data.poly_sinf[0]
#define C3  v8_sinf_data.poly_sinf[1]
#define C5  v8_sinf_data.poly_sinf[2]
#define C7  v8_sinf_data.poly_sinf[3]
#define C9  v8_sinf_data.poly_sinf[4]


float ALM_PROTO(sinf)(float);


static inline v_f32x8_t
sinf_specialcase(v_f32x8_t _x,
                 v_f32x8_t result,
                 v_i32x8_t cond)
{
    return call_v8_f32(ALM_PROTO(sinf), _x, result, cond);
}

v_f32x8_t
ALM_PROTO_OPT(vrs8_sinf)(v_f32x8_t x)
{

    v_f32x8_t r, F, poly, result;

    v_u32x8_t n;

    v_u32x8_t sign;

    v_u32x8_t ux = as_v8_u32_f32(x);

    sign = ux & ~ALM_SIGN_MASK32;

    v_u32x8_t cmp = (ux & ~ALM_SIGN_MASK32) > (V8_SINF_ARG_MAX);

    r  = as_v8_f32_u32(ux & ALM_SIGN_MASK32);

    v_f32x8_t dn =  r * V8_SINF_INVPI + ALM_SHIFT_SP;

    n = as_v8_u32_f32(dn);

    dn -= ALM_SHIFT_SP;

    F = r + dn * V8_SINF_PI1;

    F = F + dn * V8_SINF_PI2;

    F = F + dn * V8_SINF_PI3;

    v_u32x8_t odd =  n << 31;

    /*
     *  Calculate the polynomial approximation x*(1+C1*x^2+C2*x^4+C3*x^6+C4*x^8)
     *
     */

    poly = POLY_EVAL_ODD_9(F, C1, C3, C5, C7, C9);

    result = as_v8_f32_u32(as_v8_u32_f32(poly) ^ sign ^ odd);

    if(unlikely(any_v8_u32_loop(cmp))) {

        return sinf_specialcase(x, result, cmp);

    }

    return result;
}
