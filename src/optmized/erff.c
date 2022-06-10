/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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
 *
 * Signature:
 * float amd_erff(float x);
 * Computes the error function of x
 *
 * Maximum ULP is 4
 * This implementation is not IEEE 754 compliant and does not add NaN
 *
 *
 *
 * Implementation Notes
 * ----------------------
 *  if x > 3.9192059040069580078125f
 *     return 1.0
 *
 *  if 0.974708974361419677734375f < x <= 3.9192059040069580078125f
 *      return -9.334529419817705e-2 + x * (1.770500458970012 + x * (-1.9873639831178735 + x * (3.272308933339834 +
 *              x * (-4.402751805448494 + x * (3.7608972096338276 + x * (-2.0966214980804354 +
 *              x * (0.7930106628290651 + x * (-0.20671156774709162 + x * (3.677264344574519e-2 +
 *              x * (-4.2806518649629016e-3 + x * (2.94780430932192e-4 + x * (-9.12466047071725e-6))))))))))));
 *
 *
 *  if 5.40909968549385666847229003906E-5f x < 0.974708974361419677734375f
 *      return -1.7016133165416813e-11f + x * (1.1283791722401213f + x * (-2.574859935324604e-7f + x * (-0.3761213392501202f +
 *               x * (-5.1212385080044055e-5f + x * (0.11314475379382989f + x * (-1.1672466078260007e-3f + x * (-2.393903580433729e-2f +
 *               x * (-4.896886935591104e-3f +  x * (1.0606170926673056e-2f + x * (-3.656783296404107e-3f + x * 4.0345739971510983e-4f))))))
 *
 *  if 6.60290488951886800350621342659E-9f < x < 5.40909968549385666847229003906E-5f
 *       return -1.860188690104156e-15f + x * (1.1283791677145343f + x * (-3.0517569546146566e-5f));
 *
 *  if x <= 6.60290488951886800350621342659E-9f
 *       return  x + 0x1.06eba8p-3f * x;
 *
 *
 *
 * if x is +/- zero, return infinity with FE_DIV_BY_ZERO exception
 * if x is +/- infinity, infinity is returned
 * if x is NaN, NaN is returned
 * In all other cases, result is exact.
 *
 * Algorithm:
 * 1. Obtain bit pattern of x.
 *    Let ux = asuint64(x)
 *
 * 2. logbf(x) = ((ux & 0x7f800000) >> 23) - 127
 */

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>

#define BOUND1 6.60290488951886800350621342659E-9f
#define BOUND2 5.40909968549385666847229003906E-5f
#define BOUND3 0.974708974361419677734375f
#define BOUND4 3.9192059040069580078125f

#define A1 0x1.06eba8p-3f

#define B1 -1.860188690104156e-15f
#define B2 1.1283791677145343f
#define B3 -3.0517569546146566e-5f

#define C1  -1.7016133165416813e-11f
#define C2  1.1283791722401213f
#define C3  -2.574859935324604e-7f
#define C4  -0.3761213392501202f
#define C5  -5.1212385080044055e-5f
#define C6  0.11314475379382989f
#define C7  -1.1672466078260007e-3f
#define C8  -2.393903580433729e-2f
#define C9  -4.896886935591104e-3f
#define C10 1.0606170926673056e-2f
#define C11 -3.656783296404107e-3f
#define C12 4.0345739971510983e-4f

#define D1  -9.334529419817705e-2
#define D2  1.770500458970012
#define D3  -1.9873639831178735
#define D4  3.272308933339834
#define D5  -4.402751805448494
#define D6  3.7608972096338276
#define D7  -2.0966214980804354
#define D8  0.7930106628290651
#define D9  -0.20671156774709162
#define D10 3.677264344574519e-2
#define D11 -4.2806518649629016e-3
#define D12 2.94780430932192e-4
#define D13 -9.12466047071725e-6

float ALM_PROTO_OPT(erff)(float x) {

    uint32_t ux;

    ux = asuint32(x);

    ux = ux & ~SIGNBIT_SP32;

    uint32_t sign =  ux & SIGNBIT_SP32;

    float result;

    if( ux <= asuint32(BOUND1)) {

            result = x + A1 * x;

            result = asfloat(sign | asuint32(result));

     }
     else if(ux <= asuint32(BOUND2)) {

            result = B1 + x * (B2 + x * (B3));

            result = asfloat(sign | asuint32(result));

     }
     else if((ux < asuint32(BOUND3))) {

            result = C1 + x * (C2 + x * (C3 + x * (C4 + x * (C5 + x * (C6 + x * (C7 + x * (C8 +
                     x * (C9 + x * (C10 + x * (C11 + x * C12))))))))));

            result = asfloat(sign | asuint32(result));

     }
     else if((ux <= asuint32(BOUND4))) {

            double dx = (double)x;

            double ret = D1 + dx * (D2 + dx * (D3 + dx * (D4 + dx * (D5 + dx * (D6 + dx * (D7 +
                         dx * (D8 + dx * (D9 + dx * (D10 + dx * (D11 + dx * (D12 + dx * (D13))))))))))));

            result = (float)ret;

            result = asfloat(sign | asuint32(result));

    }
    else {

             result = 1.0f;

             result = asfloat(sign | asuint32(result));

    }

    return result;

}


