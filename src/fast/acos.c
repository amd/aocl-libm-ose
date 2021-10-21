/*
 * Copyright (C) 2021 Advanced Micro Devices, Inc. All rights reserved.
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
 *   double acos(double x)
 *
 * Spec:
 *   acos(0)    = pi/2
 *   acos(1)    = 0
 *   acos(-1)   = pi
 *   acos(+inf) = NaN
 *   acos(-inf) = NaN
 *
 *
 ********************************************
 * Implementation Notes
 * ---------------------
 * To compute acos(double x)
 *
 * Based on the value of x, acos(x) is calculated as,
 *
 * 1. If x > 0.5
 *      acos(x) = 2 * asin(sqrt((1 - x) / 2))
 *
 * 2. If x < -0.5
 *      acos(x) = pi - 2asin(sqrt((1 + x) / 2))
 *
 * 3. If x <= |0.5|
 *      acos(x) = pi / 2 - asin(x)
 *
 * 4.   acos(-x) = 1 / 2 * pi + asin(x)
 *
 * asin(x) is calculated using the polynomial,
 *      x+C1*x^3+C2*x^5+C3*x^7+C4*x^9+C5*x^11+C6*x^13+C7*x^15+C8*x^17
 *       +C9*x^19+C10*x^21+C11*x^23+C12*x^25
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
                double const piby2, pi;
                double half;
                double a[2], b[2], poly_asin[12];
} asin_data = {
               .piby2      = 1.5707963267948965580e+00,
               .pi         = 3.1415926535897933e+00,
               .half       = 0x1p-1,
               .a          = {
                              0,
                              0x1.921fb54442d18p-1,
                             },
               .b          = {
                              0x1.921fb54442d18p0,
                              0x1.921fb54442d18p-1,
                             },
               /* Polynomial coefficients*/
               .poly_asin = {
                             0x1.55555555552aap-3,
                             0x1.333333337cbaep-4,
                             0x1.6db6db3c0984p-5,
                             0x1.f1c72dd86cbafp-6,
                             0x1.6e89d3ff33aa4p-6,
                             0x1.1c6d83ae664b6p-6,
                             0x1.c6e1568b90518p-7,
                             0x1.8f6a58977fe49p-7,
                             0x1.a6ab10b3321bp-8,
                             0x1.43305ebb2428fp-6,
                            -0x1.0e874ec5e3157p-6,
                             0x1.06eec35b3b142p-5,
                            },
};

#define ALM_ACOS_PIBY2 asin_data.piby2
#define ALM_ACOS_PI    asin_data.pi
#define ALM_ACOS_HALF  asin_data.half

#define A asin_data.a
#define B asin_data.b

#define C1 asin_data.poly_asin[0]
#define C2 asin_data.poly_asin[1]
#define C3 asin_data.poly_asin[2]
#define C4 asin_data.poly_asin[3]
#define C5 asin_data.poly_asin[4]
#define C6 asin_data.poly_asin[5]
#define C7 asin_data.poly_asin[6]
#define C8 asin_data.poly_asin[7]
#define C9 asin_data.poly_asin[8]
#define C10 asin_data.poly_asin[9]
#define C11 asin_data.poly_asin[10]
#define C12 asin_data.poly_asin[11]

double
ALM_PROTO_FAST(acos)(double x)
{
    double    y, z, poly, result;
    uint64_t  ux, aux, xneg;
    int       xexp;
    int32_t   n = 0;

    ux   = asuint64 (x);

    /* Get absolute value of input */
    aux  = (ux & ~SIGNBIT_DP64);

    /* Get sign of input */
    xneg = (ux & SIGNBIT_DP64);

    /* Get the exponent part */
    xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;

    /* Special cases */
    if (xexp < -56) {
        /* Input small enough that arccos(x) = pi/2 */
        return ALM_ACOS_PIBY2;
    }
    else if (xexp >= 0)
    {   /* abs(x) >= 1.0 */
        if (x == 1.0)
            return 0.0;
        else if (x == -1.0)
            return ALM_ACOS_PI;
    }

    y = asdouble(aux);

    if (y > ALM_ACOS_HALF) {
        z = ALM_ACOS_HALF * (1.0 - y);
        y = -2.0 * sqrt(z);
    }
    else {
        n = 1;
        z = y * y;
    }
    /*
     *  Compute asin(x) using the polynomial
     *  x+C1*x^3+C2*x^5+C3*x^7+C4*x^9+C5*x^11+C6*x^13+C7*x^15+C8*x^17+C9*x^19+C10*x^21+C11*x^23+C12*x^25
     *  = x + x*G*(C1+G*(C2+G*(C3+G*(C4+G*(C5+G*(C6+G*(C7+G*(C8+G*(C9+G*(C10+G*(C11+C12*G)))))))))))
     *  polynomial is approximated as x+x*P(G)  where G = x^2
     */
    poly = POLY_EVAL_12(z, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12);
    poly = y + y * z * poly;

    if (xneg)
        result = (B[n] + poly) + B[n];
    else
        result = (A[n] - poly) + A[n];

    return (result);
}

strong_alias (__acos_finite, ALM_PROTO_FAST(acos))
weak_alias (amd_acos, ALM_PROTO_FAST(acos))
weak_alias (acos, ALM_PROTO_FAST(acos))