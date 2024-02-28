/* Correctly-rounded hyperbolic tangent function for binary32 value.

Copyright (c) 2022, Alexei Sibidanov.
Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.

This file was originally developed as part of the CORE-MATH project
(https://core-math.gitlabpages.inria.fr/).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   float tanhf(float x)
 *
 * Spec:
 *  tanhf(0)    =  0
 *  tanhf(-0)   =  0
 *  tanhf(inf)  =  1
 *  tanhf(-inf) = -1
 *
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 * To compute tanhf(float x)
 * The input argument is then reduced to one of these three intervals:
 *
 * 1. 0 <= |x| < 0x39000000 (approx 2.98e-8)
 *    In this case, tanhf(x) = x
 *
 * 2. 0x39000000 <= |x| < 0x9.02cb2ffffd19d464063fp0f (approx 9.01)
 *    In this case, tanhf(x) is approximated as the ratio of two polynomials of degree 8.
 *
 * 3. 0x9.02cb2ffffd19d464063fp0f <= |x| < +inf
 *    In this case, tanhf(x) = 1 - 0x1p-25f (approx 2.98e-8)
 *    Subtraction of half unit roundoff ensures precison at the boundary.
 *
 * Max ULP of implementation: 0.5
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
#include <libm/alm_special.h>

static struct
{
    float max_arg;
    double cn[8], cd[8];
} tanhf_data = {
    .max_arg = 0x9.02cb2ffffd19d464063fp0f,
    .cn = {0x1p+0, 0x1.30877b8b72d33p-3, 0x1.694aa09ae9e5ep-8,
           0x1.4101377abb729p-14, 0x1.e0392b1db0018p-22, 0x1.2533756e546f7p-30,
           0x1.d62e5abe6ae8ap-41, 0x1.b06be534182dep-54
           },
    .cd = {0x1p+0, 0x1.ed99131b0ebeap-2, 0x1.0d27ed6c95a69p-5,
           0x1.7cbdaca0e9fccp-11, 0x1.b4e60b892578ep-18, 0x1.a6f707c5c71abp-26,
           0x1.35a8b6e2cd94cp-35, 0x1.ca8230677aa01p-47
           },
};

#define C1 tanhf_data.poly_tanhf[0]
#define C2 tanhf_data.poly_tanhf[1]
#define C3 tanhf_data.poly_tanhf[2]
#define C4 tanhf_data.poly_tanhf[3]
#define C5 tanhf_data.poly_tanhf[4]
#define C6 tanhf_data.poly_tanhf[5]
#define C7 tanhf_data.poly_tanhf[6]

#define CN0 tanhf_data.cn[0]
#define CN1 tanhf_data.cn[1]
#define CN2 tanhf_data.cn[2]
#define CN3 tanhf_data.cn[3]
#define CN4 tanhf_data.cn[4]
#define CN5 tanhf_data.cn[5]
#define CN6 tanhf_data.cn[6]
#define CN7 tanhf_data.cn[7]

#define CD0 tanhf_data.cd[0]
#define CD1 tanhf_data.cd[1]
#define CD2 tanhf_data.cd[2]
#define CD3 tanhf_data.cd[3]
#define CD4 tanhf_data.cd[4]
#define CD5 tanhf_data.cd[5]
#define CD6 tanhf_data.cd[6]
#define CD7 tanhf_data.cd[7]

#define TANHF_MAX_ARG tanhf_data.max_arg

#define TANHF_SMALL_ARG 0x39000000
#define TANHF_SIGN_MASK32 ~(1U << 31)

float ALM_PROTO_OPT(tanhf)(float x)
{

    float y;
    uint32_t sign, ux;

    /* Get sign of input argument */
    ux = asuint32(x);
    sign = ux & (~TANHF_SIGN_MASK32);

    /* Get absolute value of input argument */
    y = asfloat(ux & TANHF_SIGN_MASK32);

    /* Check for Special cases */
    ux = asuint32(y);

    /* |x| is small enough that tanhf(x) = x */
    if (ux < TANHF_SMALL_ARG)
    {

        if (ux == POS_ZERO_F32)
            /* For +/- 0 */
            return x;
        else
            /* For underflow */
            return _tanhf_special(x);
    }
    else if (ux > PINFBITPATT_SP32)
        /* For +/-inf */
        return x + x;
    if (y > TANHF_MAX_ARG)
        /* For x > max_arg */
        return asfloat(asuint32(1.0f - 0x1p-25f) ^ sign);

    /* For most of the input range we approximate using the ratio of two polynomials */ 
    double z = (double)x;
    double z2 = z * z, z4 = z2 * z2, z8 = z4 * z4;
    double n0 = CN0 + z2 * CN1, n2 = CN2 + z2 * CN3, n4 = CN4 + z2 * CN5, n6 = CN6 + z2 * CN7;
    n0 += z4 * n2;
    n4 += z4 * n6;
    n0 += z8 * n4;
    double d0 = CD0 + z2 * CD1, d2 = CD2 + z2 * CD3, d4 = CD4 + z2 * CD5, d6 = CD6 + z2 * CD7;
    d0 += z4 * d2;
    d4 += z4 * d6;
    d0 += z8 * d4;
    double r = z * n0 / d0;
    return (float)r;
}
