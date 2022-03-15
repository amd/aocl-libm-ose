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
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   double atan(double x)
 *
 * Spec:
 *   atan(1)    = pi/4
 *   atan(0)    = 0
 *   atan(-0)   = -0
 *   atan(+inf) = pi/2
 *   atan(-inf) = -pi/2
 *   atan(nan)  = nan
 *
 *
 ********************************************
 * Implementation Notes
 * ---------------------
 * To compute atan(double x)
 * Let y = x
 * Let x = |x|
 *
 * Based on the value of x, atan(x) is calculated as,
 *
 * 1. If x > 1,
 *      atan(x) = pi/2 - atan(1/x)
 *
 * 2. Otherwise,
 *      Compute, f = (sqrt(3) * (x-1))/(x + sqrt(3))
 *
 *      Then, atan(x) = pi/6 + atan(f),
 *      where -(2 - sqrt(3)) < f < (2 - sqrt(3))
 *
 *      atan(f) is calculated using the polynomial,
 *      f + C1*x^3 + C2*x^5 + C3*x^7 + C4*x^9 + C5*x^11+
 *      C6*x^13 + C7*x^15 + C8*x^17 + C9*x^19
 *
 * If y < 0, then atan(y) = -atan(x)
 */


#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>


static struct {
                double theeps, sqrt3, range, piby_2;
                double pi[4], poly_atan[9];
              } atan_data = {
                                .theeps    = 0x1.6a09e667f3bcdp-27,
                                .sqrt3     = 0x1.bb67ae8584caap0,
                                .range     = 0x1.126145e9ecd56p-2,
                                .piby_2    = 0x1.921fb54442d18p0,
                                .pi        = {
                                                0,
                                                0x1.0c152382d7366p-1,
                                                0x1.921fb54442d18p0,
                                                0x1.0c152382d7366p0,
                                             },
                                .poly_atan = {
                                                -0x1.5555555555549p-2,
                                                0x1.9999999996eccp-3,
                                                -0x1.24924922b2972p-3,
                                                0x1.c71c707163579p-4,
                                                -0x1.745cd1358b0f1p-4,
                                                0x1.3b0aea74b0a51p-4,
                                                -0x1.1061c5f6997a6p-4,
                                                0x1.d1242ae875135p-5,
                                                -0x1.3a3c92f7949aep-5,
                                             },
                            };

#define THEEPS atan_data.theeps
#define SQRT3  atan_data.sqrt3
#define RANGE  atan_data.range
#define PI     atan_data.pi
#define PIBY_2 atan_data.piby_2

#define C0 atan_data.poly_atan[0]
#define C1 atan_data.poly_atan[1]
#define C2 atan_data.poly_atan[2]
#define C3 atan_data.poly_atan[3]
#define C4 atan_data.poly_atan[4]
#define C5 atan_data.poly_atan[5]
#define C6 atan_data.poly_atan[6]
#define C7 atan_data.poly_atan[7]
#define C8 atan_data.poly_atan[8]

#define SIGN_MASK   0x7FFFFFFFFFFFFFFFUL

double
ALM_PROTO_OPT(atan)(double x)
{
    double poly, result;
    int64_t n = 0;
    uint64_t sign;

    uint64_t ux = asuint64(x);

    /* Get sign of input value */
    sign = ux & (~SIGN_MASK);

    /* Get absolute value of input */
    ux = ux & SIGN_MASK;
    x = asdouble(ux);

    /* Check for special cases */
    if(unlikely(ux > PINFBITPATT_DP64)) {
        /* x is inf */
        if((ux & MANTBITS_DP64) == 0x0) {
            return asdouble(sign ^ asuint64(PIBY_2));
        }
        else {
            /* x is nan */
            return alm_atan_special(asdouble(sign ^ ux));
        }
    }

    /* If x > 1, then we use the identity
     * atan(x) = pi/2 - atan(1/x)
     */
    if (x > 1.0) {
            x = 1.0/x;
            n = 2;
    }

    /* Calculate f = (sqrt(3) * (x-1))/(x + sqrt(3)) */
    if (x > RANGE) {
            x = (x * SQRT3 - 1.0)/(SQRT3 + x);
            n ++;
    }

    /* Compute atan(f) using the polynomial
     * (c9*x^19 + c8*x^17 + c7*x^15 + c6*x^13 + c5*x^11 + c4*x^9 +
     *  c3*x^7 + c2*x^5 + c1*x^3) + x
     */
    poly = POLY_EVAL_ODD_19(x, C0, C1, C2, C3, C4, C5, C6, C7, C8);

    if (n > 1)
        poly = - poly;

    /* atan(x) = pi/6 + atan(f) */
    result = PI[n] + poly;

    /* atan(-x) = -atan(x) */
    return asdouble(asuint64(result) ^ sign);
}

