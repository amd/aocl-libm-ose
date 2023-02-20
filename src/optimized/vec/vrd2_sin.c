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
 *    v_f64x2_t amd_vrd2_sin(v_f64x2_t x)
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
    v_f64x2_t invpi, pi1, pi2, pi3;
    v_f64x2_t shift;
    v_u64x2_t sign_mask;
    v_u64x2_t max_arg;
    v_f64x2_t poly_sin[8];
 } v2_sin_data = {
     .max_arg = _MM_SET1_I64x2(0x4160000000000000),
     .sign_mask  = _MM_SET1_I64x2(0x7fffffffffffffff),
     .pi1   = _MM_SET1_PD2(0x1.921fb54442d18p+1),
     .pi2   = _MM_SET1_PD2(0x1.1a62633145c06p-53),
     .pi3   = _MM_SET1_PD2(0x1.c1cd129024e09p-106),
     .invpi = _MM_SET1_PD2(0x1.45f306dc9c883p-2),
     .shift = _MM_SET1_PD2(0x1.8p+52),
     /*
      * Polynomial coefficients obtained using Remez algorithm from Sollya
      */
     .poly_sin = {
         _MM_SET1_PD2(-0x1.5555555555555p-3),
         _MM_SET1_PD2(0x1.11111111110bp-7),
         _MM_SET1_PD2(-0x1.a01a01a013e1ap-13),
         _MM_SET1_PD2(0x1.71de3a524f063p-19),
         _MM_SET1_PD2(-0x1.ae6454b5dc0abp-26),
         _MM_SET1_PD2(0x1.6123c686ad43p-33),
         _MM_SET1_PD2(-0x1.ae420dc08499cp-41),
         _MM_SET1_PD2(0x1.880ff6993df95p-49)

     },
};


#define pi1     v2_sin_data.pi1
#define pi2     v2_sin_data.pi2
#define pi3     v2_sin_data.pi3
#define invpi   v2_sin_data.invpi
#define ALM_SHIFT   v2_sin_data.shift
#define ARG_MAX     v2_sin_data.max_arg
#define SIGN_MASK64 v2_sin_data.sign_mask

#define C0  v2_sin_data.poly_sin[0]
#define C2  v2_sin_data.poly_sin[1]
#define C4  v2_sin_data.poly_sin[2]
#define C6  v2_sin_data.poly_sin[3]
#define C8  v2_sin_data.poly_sin[4]
#define C10 v2_sin_data.poly_sin[5]
#define C12 v2_sin_data.poly_sin[6]
#define C14 v2_sin_data.poly_sin[7]


static inline v_f64x2_t
sin_specialcase(v_f64x2_t _x,
                 v_f64x2_t result,
                 v_u64x2_t cond)
{

    return (v_f64x2_t) {
                    (cond[0]) ? ALM_PROTO(sin)(_x[0]):result[0],
                    (cond[1]) ? ALM_PROTO(sin)(_x[1]):result[1],
                };

}

v_f64x2_t
ALM_PROTO_OPT(vrd2_sin)(v_f64x2_t x)
{

    v_f64x2_t r, F, poly, result;

    v_u64x2_t n;

    v_u64x2_t sign;

    v_u64x2_t ux = as_v2_u64_f64(x);

    sign = ux & ~SIGN_MASK64;

    v_u64x2_t cmp = (ux & SIGN_MASK64) > (ARG_MAX);

    r  = as_v2_f64_u64(ux & SIGN_MASK64);

    v_f64x2_t dn =  (r * invpi) + ALM_SHIFT;

    n = as_v2_u64_f64(dn);

    dn -= ALM_SHIFT;

    F = r - dn * pi1;

    F = F - dn * pi2;

    F = F - dn * pi3;

    v_u64x2_t odd =  n << 63;

    /*
     *  Calculate the polynomial approximation
     *
     * poly = x + C1*x^3 + C2*x^5 + C3*x^7 + C4*x^9 + C5*x^11 +
     *        C6*x^13 + C7*x^15 + C8*x^17
     *
     */

    poly = F + POLY_EVAL_ODD_17(F, C0, C2, C4, C6, C8, C10, C12, C14);

    result = as_v2_f64_u64(as_v2_u64_f64(poly) ^ sign ^ odd);

    if(unlikely(any_v2_u64_loop(cmp))) {

        return sin_specialcase(x, result, cmp);

    }

    return result;
}
