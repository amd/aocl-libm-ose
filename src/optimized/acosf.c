/*
 * Copyright (C) 2021-2023 Advanced Micro Devices, Inc. All rights reserved.
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
 *   float acosf(float x)
 *
 * Spec:
 *   acosf(0)    = pi/2
 *   acosf(1)    = 0
 *   acosf(-1)   = pi
 *   acosf(+inf) = NaN
 *   acosf(-inf) = NaN
 *
 *
 ********************************************
 * Implementation Notes
 * ---------------------
 * To compute acosf(float x)
 *
 * Based on the value of x, acosf(x) is calculated as,
 *
 * 1. If x > 0.5
 *      acosf(x) = 2 * asinf(sqrt((1 - x) / 2))
 *
 * 2. If x < -0.5
 *      acosf(x) = pi - 2asinf(sqrt((1 + x) / 2))
 *
 * 3. If x <= |0.5|
 *      acosf(x) = pi / 2 - asinf(x)
 *
 * 4. 	acosf(-x) = 1 / 2 * pi + asinf(x)
 *
 * acosf(x) is calculated using the polynomial,
 *      x + C1*x^3 + C2*x^5 + C3*x^7 + C4*x^9 + C5*x^11
 *
 */


#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>
#include <libm/alm_special.h>


static struct {
                float const piby2, pi;
                double half;
                double a[2], b[2], poly_asinf[5];
              } asinf_data = {
                                .piby2      = 1.5707963705e+00F,
                                .pi         = 3.1415926535897933e+00F,
                                .half       = 0x1p-1,
                                .a          = {
                                                0,
                                                0x1.921fb6p-1,
                                              },
                                .b          = {
                                                0x1.921fb6p0,
                                                0x1.921fb6p-1,
                                              },
                                /* Polynomial coefficients*/
                                .poly_asinf = {
                                                0x1.5555fcp-3,
                                                0x1.32f8d8p-4,
                                                0x1.7525aap-5,
                                                0x1.86e46ap-6,
                                                0x1.5d456cp-5,
                                              },
                             };

#define ALM_ACOSF_PIBY2 asinf_data.piby2
#define ALM_ACOSF_PI asinf_data.pi
#define ALM_ACOSF_HALF asinf_data.half

#define A asinf_data.a
#define B asinf_data.b

#define C1 asinf_data.poly_asinf[0]
#define C2 asinf_data.poly_asinf[1]
#define C3 asinf_data.poly_asinf[2]
#define C4 asinf_data.poly_asinf[3]
#define C5 asinf_data.poly_asinf[4]

float
ALM_PROTO_OPT(acosf)(float x)
{
    double y, z, poly, result;
    unsigned int ux, aux, xneg;
    int xexp, xnan;
    int32_t n = 0;

    ux   = asuint32 (x);

    /* Get absolute value of input */
    aux  = (ux & ~SIGNBIT_SP32);

    /* Get sign of input */
    xneg = (ux & SIGNBIT_SP32);

    /* check for inf */
    xnan = (aux > PINFBITPATT_SP32);

    /* Get the exponent part */
    xexp = (int)((ux & EXPBITS_SP32) >> EXPSHIFTBITS_SP32) - EXPBIAS_SP32;

    /* Special cases */
    if (xnan)
    {
        return alm_acosf_special(x, ALM_E_IN_X_NAN);
    }
    else if (xexp < -26)
    {
        /* Input small enough that arccos(x) = pi/2 */
        return ALM_ACOSF_PIBY2;
    }
    else if (xexp >= 0)
    {   /* abs(x) >= 1.0 */
        if (x == 1.0F)
            return 0.0F;
        else if (x == -1.0F)
            return ALM_ACOSF_PI;
        else
            return alm_acosf_special(x, ALM_E_OUT_NAN);
    }

    x = asfloat(aux);

    /* Convert input to double precision value */
    y = (double) x;

    if (y > ALM_ACOSF_HALF)
    {
        z = ALM_ACOSF_HALF * (1.0 - y);
        y = -2.0 * sqrt(z);
    }
    else
    {
        n = 1;
        z = y * y;
    }
    /*
     *  Compute acosf(x) using the polynomial
     *  x + C1*x^3 + C2*x^5 + C3*x^7 + C4*x^9 + C5*x^11
     */
    poly = POLY_EVAL_5(z, C1, C2, C3, C4, C5);
    poly = y + y * z * poly;

    if (xneg)
        result = (B[n] + poly) + B[n];
    else
        result = (A[n] - poly) + A[n];

    return (float)(result);

}
