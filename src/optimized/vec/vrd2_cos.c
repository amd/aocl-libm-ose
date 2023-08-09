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
 *   v_f64x2_t vrd2_cos(v_f64x2_t x)
 *
 * Spec:
 *  cos(0)    = 1
 *  cos(-0)   = 1
 *  cos(inf)  = NaN
 *  cos(-inf) = NaN
 *
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 * To compute cos(v_f64x2_t x)
 *
 * 1. Argument Reduction
 *      Let input  x be represented as,
 *          |x| = N * pi + f        (1) | N is an integer,
 *                                        -pi/2 <= f <= pi/2
 *
 *      From (2), N = int(x / pi + 1/2) - 1/2
 *                f = |x| - (N * pi)
 *
 * 2. Polynomial Evaluation
 *       From (1) and (2),cos(f) can be calculated using a polynomial
 *       cos(f) = f + C1 * f^3 + C2 * f^5 + C3 * f^7 + C4 * f^9 +
 *                C5 * f^11 + C6 * f^13 + C7 * f^15 + C8 * f^17
 *
 * 3. Reconstruction
 *      Hence, cos(x) = sin(x + pi/2) = sign(x) * cos(f)
 *
 * MAX ULP of current implementation : 2
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

static struct {
        v_f64x2_t poly_cos[8];
        v_f64x2_t pi, halfpi, invpi;
        v_f64x2_t pi1, pi2, pi3;
        v_f64x2_t half, alm_huge;
        v_u64x2_t sign_mask, max;
        } v2_cos_data = {
                        .pi        = _MM_SET1_PD2(0x1.921fb54442d188p1),
                        .halfpi    = _MM_SET1_PD2(0x1.921fb54442d18p0),
                        .invpi     = _MM_SET1_PD2(0x1.45f306dc9c883p-2),
                        .pi1       = _MM_SET1_PD2(-0x1.921fb54442d18p+1),
                        .pi2       = _MM_SET1_PD2(-0x1.1a62633145c07p-53),
                        .pi3       = _MM_SET1_PD2(0x1.f1976b7ed8fbcp-109),
                        .half      = _MM_SET1_PD2(0x1p-1),
                        .alm_huge  = _MM_SET1_PD2(0x1.8p+52),
                        .sign_mask = _MM_SET1_I64x2(0x7FFFFFFFFFFFFFFF),
                        .max       = _MM_SET1_I64x2(0x4160000000000000), /* 0x1p23 */
                        .poly_cos  = {
                                        _MM_SET1_PD2(-0x1.5555555555555p-3),
                                        _MM_SET1_PD2(0x1.11111111110bp-7),
                                        _MM_SET1_PD2(-0x1.a01a01a013e1ap-13),
                                        _MM_SET1_PD2(0x1.71de3a524f063p-19),
                                        _MM_SET1_PD2(-0x1.ae6454b5dc0b5p-26),
                                        _MM_SET1_PD2(0x1.6123c686ad6b4p-33),
                                        _MM_SET1_PD2(-0x1.ae420dc08fd52p-41),
                                        _MM_SET1_PD2(0x1.880ff69a83bbep-49),
                                    },
        };

#define V2_COS_PI      v2_cos_data.pi
#define V2_COS_HALF_PI v2_cos_data.halfpi
#define V2_COS_INV_PI  v2_cos_data.invpi
#define V2_COS_PI1     v2_cos_data.pi1
#define V2_COS_PI2     v2_cos_data.pi2
#define V2_COS_PI3     v2_cos_data.pi3
#define V2_COS_HALF    v2_cos_data.half


#define C1 v2_cos_data.poly_cos[0]
#define C2 v2_cos_data.poly_cos[1]
#define C3 v2_cos_data.poly_cos[2]
#define C4 v2_cos_data.poly_cos[3]
#define C5 v2_cos_data.poly_cos[4]
#define C6 v2_cos_data.poly_cos[5]
#define C7 v2_cos_data.poly_cos[6]
#define C8 v2_cos_data.poly_cos[7]

#define V2_COS_SIGN_MASK v2_cos_data.sign_mask
#define V2_COS_MAX       v2_cos_data.max
#define V2_ALM_HUGE      v2_cos_data.alm_huge


static inline v_f64x2_t
cos_specialcase(v_f64x2_t _x, v_f64x2_t result,
                v_u64x2_t cond)
{
    return (v_f64x2_t) {
            (cond[0]) ? ALM_PROTO(cos)(_x[0]):result[0],
            (cond[1]) ? ALM_PROTO(cos)(_x[1]):result[1],
        };
}

v_f64x2_t
ALM_PROTO_OPT(vrd2_cos)(v_f64x2_t x)
{

    v_f64x2_t dinput, dn, frac, poly, result;

    v_u64x2_t n, ixd, odd;

    ixd = as_v2_u64_f64(x);

    /* Check for special cases */
    v_u64x2_t cond = (ixd & V2_COS_SIGN_MASK) > (V2_COS_MAX);

    /* Remove sign from the input */
    ixd = ixd & V2_COS_SIGN_MASK;
    dinput = as_v2_f64_u64(ixd);

    /* Get n = int(x / pi + 1/2) - 1/2 */
    dn = ((dinput * V2_COS_INV_PI) + V2_COS_HALF) + V2_ALM_HUGE;
    n = as_v2_u64_f64(dn);
    dn = dn - V2_ALM_HUGE;
    dn = dn - V2_COS_HALF;

    /* frac = x - (n*pi) */
    frac = dinput + dn * V2_COS_PI1;
    frac = frac + dn * V2_COS_PI2;
    frac = frac + dn * V2_COS_PI3;

    /* Check if n is odd or not */
    odd = n << 63;

    /* Compute cos(f) using the polynomial
     * f + C1 * f^3 + C2 * f^5 + C3 * f^7 + C4 * f^9 +
     * C5 * f^11 + C6 * f^13 + C7 * f^15 + C8 * f^17
     */
    poly = frac + POLY_EVAL_ODD_17(frac, C1, C2, C3, C4, C5, C6, C7, C8);

    /* If n is odd, result is negative */
    result = as_v2_f64_u64( as_v2_u64_f64(poly) ^ odd);

    if(unlikely(any_v2_u64_loop(cond))) {
        return cos_specialcase(x, result,cond);
    }

    return result;

}

