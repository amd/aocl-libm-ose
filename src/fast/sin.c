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
 * Fast version of double precision sin function
 * This routine does not take care of special cases like NaNs and INFs
 * Maximum ULP: 2.319
 * 
 * Signature:
 *    double sin(double x)
 *
 ***********************************************************************************************
 * Implementation Notes
 * ---------------------
 * 1. If |x| < pi/4, then calculate sin(x) as
 * 
 *     sin(x) = x + (x * (r2 * (S1 + r2 * (S2 + r2 * (S3 + r2 * (S4 + r2 * (S5 + r2 * S6)))))))
 * 
 * 2. For other inputs, sin(x) is calculated as follows:
 * 
 *     Convert given x into the form
 *     |x| = N * pi + f where N is an integer and f lies in [-pi/2,pi/2]
 *     N is obtained by : N = round(x/pi)
 *     f is obtained by : f = abs(x)-N*pi
 *     sin(x) = sin(N * pi + f) = sin(N * pi)*cos(f) + cos(N*pi)*sin(f)
 *     sin(x) = sign(x)*sin(f)*(-1)**N
 *
 *     The term sin(f) can be approximated by using a polynomial
 *
 ************************************************************************************************
*/

static struct {
    double invpi, pi1, pi2, pi3;
    double shift;
    double one_by_six;
    uint64_t sign_mask;
    uint64_t max_arg;
    double poly_sin[8];
    double poly_sin_small[7];
 } fast_sin_data = {
     .max_arg    = 0x4160000000000000,
     .sign_mask  = 0x7fffffffffffffff,
     .pi1        = 0x1.921fb54442d18p+1,
     .pi2        = 0x1.1a62633145c06p-53,
     .pi3        = 0x1.c1cd129024e09p-106,
     .one_by_six = 0.1666666666666666666,
     .invpi      = 0x1.45f306dc9c883p-2,
     .shift      = 0x1.8p+52,
     /*
      * Polynomial coefficients obtained using Remez algorithm from Sollya
      */
     .poly_sin = {
         -0x1.5555555555555p-3,
         0x1.11111111110bp-7,
         -0x1.a01a01a013e1ap-13,
         0x1.71de3a524f063p-19,
         -0x1.ae6454b5dc0abp-26,
         0x1.6123c686ad43p-33,
         -0x1.ae420dc08499cp-41,
         0x1.880ff6993df95p-49

     },
     .poly_sin_small = {
        -0x1.5555555555555p-3,
         0x1.1111111110bb3p-7,
         -0x1.a01a019e83e5cp-13,
         0x1.71de3796cde01p-19,
         -0x1.ae600b42fdfa7p-26,
         0x1.5e0b2f9a43bb8p-33

     },
};


#define PI1         fast_sin_data.pi1
#define PI2         fast_sin_data.pi2
#define PI3         fast_sin_data.pi3
#define INVPI       fast_sin_data.invpi
#define ALM_SHIFT   fast_sin_data.shift
#define ARG_MAX     fast_sin_data.max_arg
#define SIGN_MASK64 fast_sin_data.sign_mask
#define ONE_BY_SIX  fast_sin_data.one_by_six

#define C0  fast_sin_data.poly_sin[0]
#define C2  fast_sin_data.poly_sin[1]
#define C4  fast_sin_data.poly_sin[2]
#define C6  fast_sin_data.poly_sin[3]
#define C8  fast_sin_data.poly_sin[4]
#define C10 fast_sin_data.poly_sin[5]
#define C12 fast_sin_data.poly_sin[6]
#define C14 fast_sin_data.poly_sin[7]

#define S1 fast_sin_data.poly_sin_small[0]
#define S2 fast_sin_data.poly_sin_small[1]
#define S3 fast_sin_data.poly_sin_small[2]
#define S4 fast_sin_data.poly_sin_small[3]
#define S5 fast_sin_data.poly_sin_small[4]
#define S6 fast_sin_data.poly_sin_small[5]

#define PIby4       0x3fe921fb54442d18

double
ALM_PROTO_FAST(sin)(double x)
{

    double r, F, poly, result;

    uint64_t ux = asuint64(x);

    /* Get the sign of input value */
    uint64_t sign = ux & ~SIGN_MASK64;

    /* Get the absolute value of input */
    ux = ux & SIGN_MASK64;

    /* Check if input value is greater than pi/4 */
    if(ux > PIby4)
    {

        uint64_t n;

        r  = asdouble(ux);

        double dn =  (r * INVPI) + ALM_SHIFT;

        n = asuint64(dn);

        dn -= ALM_SHIFT;

        F = r - dn * PI1;

        F = F - dn * PI2;

        F = F - dn * PI3;

        uint64_t odd =  n << 63;

        /*
         *  Calculate the polynomial approximation
         *
         * poly = x + C1*x^3 + C2*x^5 + C3*x^7 + C4*x^9 + C5*x^11 +
         *        C6*x^13 + C7*x^15 + C8*x^17
         *
         */

        poly = F + POLY_EVAL_ODD_17(F, C0, C2, C4, C6, C8, C10, C12, C14);

        result = asdouble(asuint64(poly) ^ sign ^ odd);

    }
    /* Code path for values less than pi/4 */
    else {

        double x2 = x * x;
        
        /*
         * Calculate the polynomial approximation
         *
         * poly = x + (x * (r2 * (S1 + r2 * (S2 + r2 * (S3 + r2 * (S4 + 
         *             r2 * (S5 + r2 * S6)))))))
         * 
         */
        result = x + (x * (x2 * POLY_EVAL_6(x2, S1, S2, S3, S4, S5, S6)));
    }

    return result;
}

strong_alias (__sin_finite, ALM_PROTO_FAST(sin))
weak_alias (amd_sin, ALM_PROTO_FAST(sin))
weak_alias (sin, ALM_PROTO_FAST(sin))