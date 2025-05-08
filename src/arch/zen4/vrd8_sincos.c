/*
 * Copyright (C) 2008-2025 Advanced Micro Devices, Inc. All rights reserved.
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
#include <libm/arch/zen4.h>


/*
 * Signature:
 *    void amd_vrd8_sincos(v_f64x8_t x, v_f64x8_t *result_sin, v_f64x8_t *result_cos)
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
    v_f64x8_t invpi, pi1, pi2, pi3, pi3c, half, shift;
    v_u64x8_t sign_mask, max_arg;
    v_f64x8_t poly_sincos[8];
 } v4_sincos_data = {
     .max_arg = _MM512_SET1_U64x8(0x4160000000000000UL),
     .sign_mask  = _MM512_SET1_U64x8(0x7fffffffffffffffUL),
     .pi1   = _MM512_SET1_PD8(0x1.921fb54442d18p+1),
     .pi2   = _MM512_SET1_PD8(0x1.1a62633145c06p-53),
     .pi3   = _MM512_SET1_PD8(0x1.c1cd129024e09p-106),
     .pi3c  = _MM512_SET1_PD8(-0x1.f1976b7ed8fbcp-109),
     .half  = _MM512_SET1_PD8(0x1p-1),
     .invpi = _MM512_SET1_PD8(0x1.45f306dc9c883p-2),
     .shift = _MM512_SET1_PD8(0x1.8p+52),
     /*
      * Polynomial coefficients obtained using Remez algorithm from Sollya
      */
     .poly_sincos = {
         _MM512_SET1_PD8(-0x1.5555555555555p-3),
         _MM512_SET1_PD8(0x1.11111111110bp-7),
         _MM512_SET1_PD8(-0x1.a01a01a013e1ap-13),
         _MM512_SET1_PD8(0x1.71de3a524f063p-19),
         _MM512_SET1_PD8(-0x1.ae6454b5dc0abp-26),
         _MM512_SET1_PD8(0x1.6123c686ad43p-33),
         _MM512_SET1_PD8(-0x1.ae420dc08499cp-41),
         _MM512_SET1_PD8(0x1.880ff6993df95p-49)

     },
};


#define pi1     v4_sincos_data.pi1
#define pi2     v4_sincos_data.pi2
#define pi3     v4_sincos_data.pi3
#define pi3c    v4_sincos_data.pi3c
#define invpi   v4_sincos_data.invpi
#define HALF    v4_sincos_data.half
#define ALM_SHIFT   v4_sincos_data.shift
#define ARG_MAX     v4_sincos_data.max_arg
#define SIGN_MASK64 v4_sincos_data.sign_mask

#define C0  v4_sincos_data.poly_sincos[0]
#define C2  v4_sincos_data.poly_sincos[1]
#define C4  v4_sincos_data.poly_sincos[2]
#define C6  v4_sincos_data.poly_sincos[3]
#define C8  v4_sincos_data.poly_sincos[4]
#define C10 v4_sincos_data.poly_sincos[5]
#define C12 v4_sincos_data.poly_sincos[6]
#define C14 v4_sincos_data.poly_sincos[7]


static inline v_f64x8_t
sin_specialcase(v_f64x8_t _x,
                v_f64x8_t result,
                v_u64x8_t cond)
{
    return (v_f64x8_t) {
            (cond[0]) ? ALM_PROTO(sin)(_x[0]):result[0],
            (cond[1]) ? ALM_PROTO(sin)(_x[1]):result[1],
            (cond[2]) ? ALM_PROTO(sin)(_x[2]):result[2],
            (cond[3]) ? ALM_PROTO(sin)(_x[3]):result[3],
            (cond[4]) ? ALM_PROTO(sin)(_x[4]):result[4],
            (cond[5]) ? ALM_PROTO(sin)(_x[5]):result[5],
            (cond[6]) ? ALM_PROTO(sin)(_x[6]):result[6],
            (cond[7]) ? ALM_PROTO(sin)(_x[7]):result[7],
        };
}

static inline v_f64x8_t
cos_specialcase(v_f64x8_t _x,
	        v_f64x8_t result,
                v_u64x8_t cond)
{
    return (v_f64x8_t) {
            (cond[0]) ? ALM_PROTO(cos)(_x[0]):result[0],
            (cond[1]) ? ALM_PROTO(cos)(_x[1]):result[1],
            (cond[2]) ? ALM_PROTO(cos)(_x[2]):result[2],
            (cond[3]) ? ALM_PROTO(cos)(_x[3]):result[3],
            (cond[4]) ? ALM_PROTO(cos)(_x[4]):result[4],
            (cond[5]) ? ALM_PROTO(cos)(_x[5]):result[5],
            (cond[6]) ? ALM_PROTO(cos)(_x[6]):result[6],
            (cond[7]) ? ALM_PROTO(cos)(_x[7]):result[7],
	};
}

void
ALM_PROTO_ARCH_ZN4(vrd8_sincos)(v_f64x8_t x, v_f64x8_t *result_sin, v_f64x8_t *result_cos)
{

    v_f64x8_t r, F_sin, F_cos, poly_sin, poly_cos;
    v_u64x8_t n_sin, n_cos, sign;

    v_u64x8_t ux = as_v8_u64_f64(x);

    sign = ux & ~SIGN_MASK64;

    /* Check for special cases */
    v_u64x8_t cmp = (ux & SIGN_MASK64) > (ARG_MAX);

    /* Remove sign from the input */
    r  = as_v8_f64_u64(ux & SIGN_MASK64);

    v_f64x8_t dn_sin =  (r * invpi) + ALM_SHIFT;
    n_sin = as_v8_u64_f64(dn_sin);
    dn_sin -= ALM_SHIFT;

    F_sin = r - dn_sin * pi1;
    F_sin = F_sin - dn_sin * pi2;
    F_sin = F_sin - dn_sin * pi3;

    v_f64x8_t dn_cos =  (r * invpi + HALF) + ALM_SHIFT;
    n_cos = as_v8_u64_f64(dn_cos);
    dn_cos -= ALM_SHIFT;
    dn_cos -= HALF;

    F_cos = r - dn_cos * pi1;
    F_cos = F_cos - dn_cos * pi2;
    F_cos = F_cos - dn_cos * pi3c;

    /* Check whether n is odd or not */
    v_u64x8_t odd_sin =  n_sin << 63;
    v_u64x8_t odd_cos =  n_cos << 63;

    /*
     *  Calculate the polynomial approximations
     *
     *   poly = x + C0*x^3 + C2*x^5 + C4*x^7 + C6*x^9 + C8*x^11 +
     *          C10*x^13 + C12*x^15 + C14*x^17
     *
     */
    poly_sin = F_sin + POLY_EVAL_ODD_17(F_sin, C0, C2, C4, C6, C8, C10, C12, C14);
    poly_cos = F_cos + POLY_EVAL_ODD_17(F_cos, C0, C2, C4, C6, C8, C10, C12, C14);

    *result_sin = as_v8_f64_u64(as_v8_u64_f64(poly_sin) ^ sign ^ odd_sin);
    *result_cos = as_v8_f64_u64(as_v8_u64_f64(poly_cos) ^ odd_cos);

    if(unlikely(any_v8_u64_loop(cmp))) {
      *result_sin = sin_specialcase(x, *result_sin, cmp);
      *result_cos = cos_specialcase(x, *result_cos, cmp);
    }

    return;
}
