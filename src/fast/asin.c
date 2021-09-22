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
 *   float asinf(float x)
 *

 * Contains implementation of float asinf(float x)
 *
 * The input domain should be in the [-1, +1] else a domain error is displayed
 *
 * asin(-x) = -asin(x)
 * asin(x) = pi/2-2*asin(sqrt(1/2*(1-x)))  when x > 1/2
 *
 * y = abs(x)
 * asinf(y) = asinf(g)  when y <= 0.5,  where g = y*y
 *		    = pi/2-asinf(g)  when y > 0.5, where g = 1/2*(1-y), y = -2*sqrt(g)
 * The term asin(f) is approximated by using a polynomial where the inputs lie in the interval [0 1/2]
 */

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm_special.h>
#include <libm_macros.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>
#include "kern/sqrt_pos.c"
#include <libm/alm_special.h>

static struct {
    double half, piby2;
    double a[2], b[2], poly_asin[12];
} asin_data = {
    .half = 0x1p-1,
    .piby2      = 1.5707963267948965580e+00,
    // Values of factors of pi required to calculate asin
    .a = {
        0,
        0x1.921fb54442d18p0,
    },
    .b = {
        0x1.921fb54442d18p0,
        0x1.921fb54442d18p-1,
    },
    // Polynomial coefficients obtained using fpminimax algorithm from Sollya
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

#define HALF asin_data.half

#define ALM_ASIN_PIBY2 asin_data.piby2
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
ALM_PROTO_FAST(asin)(double x)
{
    double y, g, poly, result ,sign = 1.0 ;
    uint64_t ux;
    int64_t n = 0, xexp;

    // Include check for inf, -inf, nan here
    // asin(NaN) = NaN

    if (x < 0)
        sign = -1.0;

    ux   = asuint64 (x);

    /* Get the exponent part */
    xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;

    /* Special cases */
    if (x != x) {   /* if x is a nan */
        return x;
    }
    else if (xexp < -56) {
        /* Input small enough that arcsin(x) = x */
        return x;
    }
    else if (xexp >= 0) {
        /* abs(x) >= 1.0 */
        if (x == 1.0)
            return ALM_ASIN_PIBY2;
        else if (x == -1.0)
            return -ALM_ASIN_PIBY2;
        else
            return x;
    }


    y = sign*x;            // make x positive, if it is negative

    if (y > HALF)
    {
        n = 1 ;
        g = HALF*(1.0d-y);
        y = -2.0*ALM_PROTO_KERN(sqrt)(g);
        //Y = -2.0d*sqrt(G) ;
    }
    else
    {
        g = y*y;
    }

    // Calculate the polynomial approximation x+C1*x^3+C2*x^5+C3*x^7+C4*x^9+C5*x^11+C6*x^13+C7*x^15+C8*x^17+C9*x^19+C10*x^21+C11*x^23+C12*x^25
    //                                       = x + x*(C1*x^2+C2*x^4+C3*x^6+C4*x^8+C5*x^10+C6*x^12+C7*x^14+C8*x^16+C9*x^18+C10*x^20+C11*x^22+C12*x^24)
    //                                       = x + x*(C1*G+C2*G^2+C3*G^3+C4*G^4+C5*G^5+C6*G^6+C7*G^7+C8*G^8+C9*G^9+C10*G^10+C11*G^11+C12*G^12)
    //                                       = x + x*G*(C1+G*(C2+G*(C3+G*(C4+G*(C5+G*(C6+G*(C7+G*(C8+G*(C9+G*(C10+G*(C11+C12*G)))))))))))

    poly = POLY_EVAL_12(g, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12);
    poly = y + y * g * poly;
    result =  poly+A[n] ;

    // if (sign == -1)  result = -result ;
    return (sign*result) ;
}

strong_alias (__asin_finite, ALM_PROTO_FAST(asin))
weak_alias (amd_asin, ALM_PROTO_FAST(asin))
weak_alias (asin, ALM_PROTO_FAST(asin))
