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
 * Argument Reduction for every x into the interval [-(2-sqrt(3)),+(2-sqrt(3))]
 * Use the following identities
 * atan(x) = pi/2 - atan(1/x)                when x > 1
 *         = pi/6 + atan(f)                  when f > (2-sqrt(3))
 * where f = (sqrt(3)*x-1)/(x+sqrt(3))
 *
 * All elements are approximated by using polynomial of degree 7
 */

#include <libm/alm_special.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>

static struct{
            float sqrt3, range, piby_2, unit;
            unsigned int mask;
            float pi[4], poly_atanf[3];
            } atanf_data = {
                .sqrt3  = 0x1.bb67aep0,
                .range  = 0x1.126146p-2,
                .piby_2 = 0x1.921fb6p0,
                .unit   = 1.0f,
                .mask   = 0x7FFFFFFFU,
                .pi     = {
                            0,
                            0x1.0c1524p-1,
                            0x1.921fb6p0,
                            0x1.0c1524p0,
                            },
                .poly_atanf = {
                            -0x1.5552f2p-2,
                            0x1.9848ap-3,
                            -0x1.066ac8p-3,
                            },
                        };

#define SQRT3  atanf_data.sqrt3
#define RANGE  atanf_data.range
#define PI     atanf_data.pi
#define PIBY_2 atanf_data.piby_2
#define UNIT   atanf_data.unit
#define SIGN_MASK   atanf_data.mask

#define C0 atanf_data.poly_atanf[0]
#define C1 atanf_data.poly_atanf[1]
#define C2 atanf_data.poly_atanf[2]

/*
 ********************************************
 * Implementation Notes
 * ---------------------
 * sign = sign(xi)
 * xi = |xi|
 *
 * Argument reduction: Use the following identities
 *
 * 1. If xi > 1,
 *      atan(xi) = pi/2 - atan(1/xi)
 *
 * 2. If f > (2-sqrt(3)),
 *      atan(x) = pi/6 + atan(f)
 *      where f = (sqrt(3)*xi-1)/(xi+sqrt(3))
 *
 *      atan(f) is calculated using the polynomial of degree 7,
 *
 */
float
ALM_PROTO_FAST(atanf)(float x)
{
    float F, poly, result;
    uint32_t n = 0;
    uint32_t ux = asuint32(x);
    /* Get sign of input value */
    uint32_t sign = ux & (~SIGN_MASK);
    ux = ux & SIGN_MASK;
    F = asfloat(ux);

    if (F > UNIT) {
        F = UNIT / F;
        n = 2;
    }

    if (F > RANGE) {
        F = (F * SQRT3 - UNIT) / (SQRT3 + F);
        n++;
    }

    poly = POLY_EVAL_ODD_7(F, C0, C1, C2);

    if (n > 1) poly = -poly;
    result = PI[n] + poly;

    return asfloat(asuint32(result) ^ sign);
}

strong_alias (__atanf_finite, ALM_PROTO_FAST(atanf))
weak_alias (amd_atanf, ALM_PROTO_FAST(atanf))
weak_alias (atanf, ALM_PROTO_FAST(atanf))