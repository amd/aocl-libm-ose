/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc. All rights reserved.
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
 * Fast version of double precision cos function
 * This rouine does not handle special cases like NaNs or INFs
 * Maximum ULP: 2.34
 * 
 * Signature:
 *   double cos(double x)
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 * 1. If |x| < pi/4, then calculate cos(x) as
 * 
 *      cos(x) = 1.0 + x*x * (-0.5 + (C1*x*x + (C2*x*x + (C3*x*x
 *                              + (C4*x*x + (C5*x*x + C6*x*x))))))
 * 
 * 2. For the rest of inputs, cos(x) is calculated as follows:
 *
 *      2.1. Argument Reduction
 *              Let input  x be represented as,
 *                  |x| = N * pi + f        (1) | N is an integer,
 *                                            -pi/2 <= f <= pi/2
 *
 *              From (2), N = int(x / pi + 1/2) - 1/2
 *                    f = |x| - (N * pi)
 *
 *      2.2. Polynomial Evaluation
 *              From (1) and (2),cos(f) can be calculated using a polynomial
 *                  cos(f) = f + C1 * f^3 + C2 * f^5 + C3 * f^7 + C4 * f^9 +
 *                           C5 * f^11 + C6 * f^13 + C7 * f^15 + C8 * f^17
 *
 *      2.3. Reconstruction
 *              Hence, cos(x) = sin(x + pi/2) = sign(x) * cos(f)
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

static struct {
        double poly_cos[8];
        double poly_cos_small[6];
        double pi, halfpi, invpi;
        double pi1, pi2, pi3;
        double half, alm_huge;
        uint64_t sign_mask, max;
        } fast_cos_data = {
                        .pi        = 0x1.921fb54442d188p1,
                        .halfpi    = 0x1.921fb54442d18p0,
                        .invpi     = 0x1.45f306dc9c883p-2,
                        .pi1       = -0x1.921fb54442d18p+1,
                        .pi2       = -0x1.1a62633145c07p-53,
                        .pi3       = 0x1.f1976b7ed8fbcp-109,
                        .half      = 0x1p-1,
                        .alm_huge  = 0x1.8p+52,
                        .sign_mask = 0x7FFFFFFFFFFFFFFF,
                        .max       = 0x4160000000000000, /* 0x1p23 */
                        .poly_cos  = {
                                        -0x1.5555555555555p-3,
                                        0x1.11111111110bp-7,
                                        -0x1.a01a01a013e1ap-13,
                                        0x1.71de3a524f063p-19,
                                        -0x1.ae6454b5dc0b5p-26,
                                        0x1.6123c686ad6b4p-33,
                                        -0x1.ae420dc08fd52p-41,
                                        0x1.880ff69a83bbep-49,
                                    },
                        .poly_cos_small = {
                                            0x1.5555555555555p-5,   /* 0.0416667 */
                                            -0x1.6c16c16c16967p-10, /* -0.00138889 */
                                            0x1.A01A019F4EC91p-16,  /* 2.48016e-005 */
                                            -0x1.27E4FA17F667Bp-22, /* -2.75573e-007 */
                                            0x1.1EEB690382EECp-29,  /* 2.08761e-009 */
                                            -0x1.907DB47258AA7p-37  /* -1.13826e-011 */
                        }
        };

#define FAST_COS_PI      fast_cos_data.pi
#define FAST_COS_HALF_PI fast_cos_data.halfpi
#define FAST_COS_INV_PI  fast_cos_data.invpi
#define FAST_COS_PI1     fast_cos_data.pi1
#define FAST_COS_PI2     fast_cos_data.pi2
#define FAST_COS_PI3     fast_cos_data.pi3
#define FAST_COS_HALF    fast_cos_data.half


#define C1 fast_cos_data.poly_cos[0]
#define C2 fast_cos_data.poly_cos[1]
#define C3 fast_cos_data.poly_cos[2]
#define C4 fast_cos_data.poly_cos[3]
#define C5 fast_cos_data.poly_cos[4]
#define C6 fast_cos_data.poly_cos[5]
#define C7 fast_cos_data.poly_cos[6]
#define C8 fast_cos_data.poly_cos[7]

#define S1 fast_cos_data.poly_cos_small[0]
#define S2 fast_cos_data.poly_cos_small[1]
#define S3 fast_cos_data.poly_cos_small[2]
#define S4 fast_cos_data.poly_cos_small[3]
#define S5 fast_cos_data.poly_cos_small[4]
#define S6 fast_cos_data.poly_cos_small[5]
#define S7 fast_cos_data.poly_cos_small[6]

#define FAST_COS_SIGN_MASK fast_cos_data.sign_mask
#define FAST_COS_MAX       fast_cos_data.max
#define FAST_ALM_HUGE      fast_cos_data.alm_huge

#define PIby4       0x3fe921fb54442d18

double
ALM_PROTO_FAST(cos)(double x)
{

    double dinput, dn, frac, poly, result;

    uint64_t n, ixd, odd;

    ixd = asuint64(x);

    /* Remove sign from the input */
    ixd = ixd & FAST_COS_SIGN_MASK;

    /* Check if input value is greater than pi/4 */
    if(ixd > PIby4)
    {
        dinput = asdouble(ixd);

        /* Get n = int(x / pi + 1/2) - 1/2 */
        dn = ((dinput * FAST_COS_INV_PI) + FAST_COS_HALF) + FAST_ALM_HUGE;
        n = asuint64(dn);
        dn = dn - FAST_ALM_HUGE;
        dn = dn - FAST_COS_HALF;

        /* frac = x - (n*pi) */
        frac = dinput + dn * FAST_COS_PI1;
        frac = frac + dn * FAST_COS_PI2;
        frac = frac + dn * FAST_COS_PI3;

        /* Check if n is odd or not */
        odd = n << 63;

        /* Compute cos(f) using the polynomial
         * f + C1 * f^3 + C2 * f^5 + C3 * f^7 + C4 * f^9 +
         * C5 * f^11 + C6 * f^13 + C7 * f^15 + C8 * f^17
        */
        poly = frac + POLY_EVAL_ODD_17(frac, C1, C2, C3, C4, C5, C6, C7, C8);

        /* If n is odd, result is negative */
        result = asdouble( asuint64(poly) ^ odd);
    }
    /* Code path for values less than pi/4 */
    else
    {
        double x2 = x*x;

        result = 1.0 + x2 * (-0.5 + (x2 * POLY_EVAL_6(x2, S1, S2, S3, S4, S5, S6)));
    }

    return result;

}

strong_alias (__cos_finite, ALM_PROTO_FAST(cos))
weak_alias (amd_cos, ALM_PROTO_FAST(cos))
weak_alias (cos, ALM_PROTO_FAST(cos))