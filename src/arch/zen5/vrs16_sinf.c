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
#include <libm/arch/zen5.h>

/*
 * Signature:
 *    v_f32x16_t amd_vrs16_sinf(v_f32x16_t x)
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
 * sin(x) = sin(N * pi + f) = sin(N * pi)*cos(f) + cos(N*pi)*sin(f) using the formula
 * sin(A + B) = sin(A)*cos(B) + cos(A)*sin(B)
 * sin(x) = sign(x)*sin(f)*(-1)^N as
 * cos(N*pi) = 1 if n is even
 * cos(N*pi) = -1 if n is odd
 * sin(N*pi) = 0
 *
 * The term sin(f) can be approximated by using a polynomial approximated by the mimimax polynomial.
 * The coeffiecients of the polynomial are obtained using the sollya tool.
 *
 ******************************************
*/

static struct {
    v_f32x16_t invpi, pi1, pi2, pi3;
    v_f32x16_t shift;
    v_u32x16_t mask32;
    v_u32x16_t max_arg;
    v_f32x16_t poly_sinf[5];
 } v16_sinf_data = {
     .max_arg = _MM512_SET1_U32x16(0x49800000U), /* 0x1p20f */
     .mask32    = _MM512_SET1_U32x16(0x7fffffffU),
     .pi1    = _MM512_SET1_PS16(-0x1.921fb6p1f),
     .pi2   = _MM512_SET1_PS16(0x1.777a5cp-24f),
     .pi3   = _MM512_SET1_PS16(0x1.ee59dap-49f),
     .invpi = _MM512_SET1_PS16(0x1.45f306p-2f),
     .shift = _MM512_SET1_PS16(0x1.8p23f),
     /*
      * Polynomial coefficients obtained using Remez algorithm from Sollya
      */
     .poly_sinf = {
         _MM512_SET1_PS16(0x1.p0f),
         _MM512_SET1_PS16(-0x1.555548p-3f),
         _MM512_SET1_PS16(0x1.110e7cp-7f),
         _MM512_SET1_PS16(-0x1.9f6446p-13f),
         _MM512_SET1_PS16(0x1.5d38b6p-19f)
     },
};


#define V16_SINF_PI1       v16_sinf_data.pi1
#define V16_SINF_PI2       v16_sinf_data.pi2
#define V16_SINF_PI3       v16_sinf_data.pi3
#define V16_SINF_INVPI     v16_sinf_data.invpi
#define ALM_SHIFT_SP       v16_sinf_data.shift
#define V16_SINF_ARG_MAX   v16_sinf_data.max_arg
#define ALM_SIGN_MASK32    v16_sinf_data.mask32

#define C1  v16_sinf_data.poly_sinf[0]
#define C3  v16_sinf_data.poly_sinf[1]
#define C5  v16_sinf_data.poly_sinf[2]
#define C7  v16_sinf_data.poly_sinf[3]
#define C9  v16_sinf_data.poly_sinf[4]


static inline v_f32x16_t
sinf_specialcase(v_f32x16_t _x,
                 v_f32x16_t result,
                 v_u32x16_t cond)
{

    return call_v16_f32(ALM_PROTO(sinf), _x, result, cond);

}

v_f32x16_t
ALM_PROTO_ARCH_ZN5(vrs16_sinf)(v_f32x16_t x)
{

    v_f32x16_t r, F, poly, result;

    v_u32x16_t n;

    v_u32x16_t sign;

    v_u32x16_t ux = as_v16_u32_f32(x);

    sign = ux & ~ALM_SIGN_MASK32;

    v_u32x16_t cmp = (ux & ALM_SIGN_MASK32) > (V16_SINF_ARG_MAX);

    r  = as_v16_f32_u32(ux & ALM_SIGN_MASK32);

    v_f32x16_t dn =  r * V16_SINF_INVPI + ALM_SHIFT_SP;

    n = as_v16_u32_f32(dn);

    dn -= ALM_SHIFT_SP;

    F = r + dn * V16_SINF_PI1;

    F = F + dn * V16_SINF_PI2;

    F = F + dn * V16_SINF_PI3;

    v_u32x16_t odd =  n << 31;

    /*
     *  Calculate the polynomial approximation x*(1+C1*x^2+C2*x^4+C3*x^6+C4*x^8)
     *
     */

    poly = POLY_EVAL_ODD_9(F, C1, C3, C5, C7, C9);

    result = as_v16_f32_u32(as_v16_u32_f32(poly) ^ sign ^ odd);

    if(unlikely(any_v16_u32_loop(cmp))) {

        return sinf_specialcase(x, result, cmp);

    }

    return result;
}
