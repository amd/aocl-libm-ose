
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
/* Contains implementation of float atanf(float x)
 *
 * sign = sign(x)
 * x = abs(x)
 *
 * Reduce the given x into f, so that it falls into the interval [-(2-sqrt(3)),+(2-sqrt(3))]
 * Use the following identities to evealute atan and reduce the range of x
 * atan(x) = pi/2 - atan(1/x)				when x > 1
 * 		   = pi/6 + atan(f) 				when f > (2-sqrt(3))
 * where f = (sqrt(3)*x-1)/(x+sqrt(3))
 *
 * The term atan(f) is approximated by using a polynomial
 */
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <libm_util_amd.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>

/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   float atan(float x)
 *
 * Spec:
 *   atan(1)    = pi/4
 *   atan(0)    = 0
 *   atan(-0)   = -0
 *   atan(+inf) = pi/2
 *   atan(-inf) = -pi/2
 *   atan(nan)  = nan
 *
 */
static struct {
    float THEEPS, sqrt3, Range ;
    float P[4], poly_atanf[9];
} atanf_data = {
    .THEEPS = 0x1.6a09e6p-12,
    .sqrt3 = 0x1.bb67aep0,
    .Range = 0x1.126146p-2,
    // Values of factors of pi required to calculate atanf
    .P = {
        0,
        0x1.0c1524p-1,
        0x1.921fb6p0,
        0x1.0c1524p0,
    },
    // Polynomial coefficients obtained using fpminimax algorithm from Sollya
    .poly_atanf = {
        -0x1.555556p-2,
        0x1.99999ap-3,
        -0x1.24924ap-3,
        0x1.c71c7p-4,
        -0x1.745cd2p-4,
        0x1.3b0aeap-4,
        -0x1.1061c6p-4,
        0x1.d1242ap-5,
        -0x1.3a3c92p-5,
    },
};

#define THEEPS atanf_data.THEEPS
#define SQRT3 atanf_data.sqrt3
#define Range atanf_data.Range
#define P atanf_data.P

#define C0 atanf_data.poly_atanf[0]
#define C1 atanf_data.poly_atanf[1]
#define C2 atanf_data.poly_atanf[2]
#define C3 atanf_data.poly_atanf[3]
#define C4 atanf_data.poly_atanf[4]
#define C5 atanf_data.poly_atanf[5]
#define C6 atanf_data.poly_atanf[6]
#define C7 atanf_data.poly_atanf[7]
#define C8 atanf_data.poly_atanf[8]

#define SIGN_MASK32 0x7FFFFFFFU
#define UNIT_F 1.0f

/*
 ********************************************
 * Implementation Notes
 * ---------------------
 * sign = sign(x)
 * x = |x|
 *
 * Reduce the given x into f, so that it falls into the interval [-(2-sqrt(3)),+(2-sqrt(3))]
 * Use the following identities to evaluate atan and reduce the range of x
 *
 * 1. If x > 1,
 *      atan(x) = pi/2 - atan(1/x)
 *
 * 2. If f > (2-sqrt(3)),
 *      atan(x) = pi/6 + atan(f)
 *      where f = (sqrt(3)*x-1)/(x+sqrt(3))
 *
 *      atan(f) is calculated using the polynomial,
 *      f + C0*x^3 + C1*x^5 + C2*x^7 + C3*x^9 + C4*x^11+
 *      C5*x^13 + C6*x^15 + C7*x^17 + C8*x^19
 *
 */
float
ALM_PROTO_OPT(atanf)(float x)
{
    float F, poly, result;
    int32_t n = 0;
    static float piby2 = 1.57079637e+0f;
    uint32_t ux = asuint32(x);
    /* Get sign of input value */
    uint32_t sign = ux & (~SIGN_MASK32);
    ux = ux & SIGN_MASK32;
    F = asfloat(ux);

    /* Check for special cases */
    if(unlikely(ux > PINFBITPATT_SP32)) {
        /* x is inf */
        if((ux & MANTBITS_SP32) == 0x0)
            return asfloat(sign ^ asuint32(piby2));
        else
        /* x is nan */
            return asfloat(sign ^ ux);
    }

    if (F > UNIT_F) {
        F = UNIT_F/F;
        n = 2;
    }

    if (F > Range) {
        F = (F*SQRT3-UNIT_F)/(SQRT3+F);
        n++;
    }

    poly = POLY_EVAL_ODD_19(F, C0, C1, C2, C3, C4, C5, C6, C7, C8);
    if (n > 1) poly = -poly;
    result = P[n]+poly;

    return asfloat(asuint32(result) ^ sign);
}
