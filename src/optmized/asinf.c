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
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>
#include <libm/alm_special.h>
#include "kern/sqrt_pos.c"

static struct {
    double THEEPS, HALF ;
    double A[2], B[2], poly_asinf[12];
} asinf_data = {
    .THEEPS = 0x1.6a09e667f3bcdp-27,
    .HALF = 0x1p-1,
    // Values of factors of pi required to calculate asin
    .A = {
        0,
        0x1.921fb54442d18p0,
    },
    .B = {
        0x1.921fb54442d18p0,
        0x1.921fb54442d18p-1,
    },
    // Polynomial coefficients obtained using fpminimax algorithm from Sollya
    .poly_asinf = {
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
#define HALF asinf_data.HALF

#define A asinf_data.A
#define B asinf_data.B

#define C1 asinf_data.poly_asinf[0]
#define C2 asinf_data.poly_asinf[1]
#define C3 asinf_data.poly_asinf[2]
#define C4 asinf_data.poly_asinf[3]
#define C5 asinf_data.poly_asinf[4]
#define C6 asinf_data.poly_asinf[5]
#define C7 asinf_data.poly_asinf[6]
#define C8 asinf_data.poly_asinf[7]
#define C9 asinf_data.poly_asinf[8]
#define C10 asinf_data.poly_asinf[9]
#define C11 asinf_data.poly_asinf[10]
#define C12 asinf_data.poly_asinf[12]


float
ALM_PROTO_OPT(asinf)(float x)
{
    double Y, G, poly, result, sign =1;
    int32_t n = 0;

    // Include check for inf, -inf, nan here
    //  asin(NaN) = NaN

    if (x < 0)
        sign = -1;
    Y = ((double)(x))*sign;			// Make x positive, if it is negative

    if(Y>1.0)
            return alm_asinf_special(x, ALM_E_OUT_NAN);

    if (Y > HALF)
    {
        n = 1;
        G = HALF*(1.0-Y);
        Y = -2.0*ALM_PROTO_KERN(sqrt)(G);

        poly = Y + Y*G *POLY_EVAL_9_0(G,C1,C2,C3,C4,C5,C6,C7,C8,C9);

        result =  (A[n]+poly);

        return (float)(sign*result);
    }
    G = Y*Y;
    result = Y + Y*G *POLY_EVAL_9_0(G,C1,C2,C3,C4,C5,C6,C7,C8,C9);

    return (float)(sign*result);
}
