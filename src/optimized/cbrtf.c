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
 */

/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   float cbrtf(float x)
 *
 * Spec:
 * To calculate (x)^1/3
 * step 1) Extract exponent and mentissa from input.
 * step 2) Convert input form float to double.
 * step 3) Reduce the input [1, 2)
            3.1) Replace expnent with 3ff i.e 1
            3.2) Or with the metissa
 * step 4) Scaling factor <= exponent/3 and Cuberoot2Index <= remineder (exponent % 3)
 * step 5) Polynomial approximation on reduced input
 * step 6) Multiply result of Polynomial approximation to cube-root reminder and scale factor
 * step 7) Return : Check for proper sign  and return the result
 *
 * Mathmatical Explanation
 * (x)^(1/3) = (x_d * 2^n)^(1/3)
 *           =  x_d^(1/3) * 2^(n/3)
 *           =  x_d^(1/3) * 2^(Quotient) * 2^(Reminder/3), where x_d is reduced input between [1,2)
 *
 *
*/

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <immintrin.h>

#include <libm_macros.h>
#include <libm/types.h>

#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/alm_special.h>
#include <cbrtf_data.h>

#define MANTISSA_MASK_64 0x000FFFFFFFFFFFFF
#define SIGN_MASK_64 0x7FFFFFFFFFFFFFFF
#define ONE_MASK_64 0x3FF0000000000000
#define DENORMAL_FACTOR 0.00492156660115184840104118
#define TWOPOW23 8388608.0f

#define CBRT2 1.2599210498948731648		/* 2^(1/3) */
#define SQR_CBRT2 1.5874010519681994748		/* 2^(2/3) */
#define CBRT2_RECIP 0.7937005259840997374  /* 2 ^ (-1/3) */
#define SQR_CBRT2_RECIP 0.6299605249474365823 /*  2 ^ (-2/3) */

static const double cuberoot[5] = {
    SQR_CBRT2_RECIP, //  2 ^ (-2/3)
    CBRT2_RECIP, // 2 ^ (-1/3)
    1.0, // 2 ^ (0/3)
    CBRT2, // 2 ^ (1/3)
    SQR_CBRT2 // 2 ^ (2/3)
};

float
ALM_PROTO_OPT(cbrtf)(float x){
    float_t xf = x;
    double_t xd = 0.0;
    double_t xdRed = 0.0;
    uint64_t uix64;
    uint64_t sign = 0;
    uint32_t ix = 0;
    int32_t ixe = 0;
    int32_t ixm = 0;
    int32_t denornmal = 0;

    ix =  asuint32(xf);

    ixe = EXPBITS_SP32 & ix; // exponent extactor
    ixm = MANTBITS_SP32 & ix; // mentissa extactor

    if ( ixe == PINFBITPATT_SP32 ){
        if (ixm == 0)
            __alm_handle_errorf(ix, AMD_F_OVERFLOW);
        else
            __alm_handle_errorf(ix|QNAN_MASK_32, AMD_F_INVALID);

        return x + x;
    }

    if ( ixe == 0 ) { // denormal number
        denornmal = 1;
        if (ixm == 0)  return 0.0f; // is zero
        xf = xf * TWOPOW23;

        ix = asuint32(xf);

        ixe = EXPBITS_SP32 & ix; // exponent extactor
        ixm = MANTBITS_SP32 & ix; // mentissa extactor
    }

    xd = (double)xf;

    uix64 = asuint64(xd);

    sign = uix64 >> 63;//SIGN_MASK_64; // extract sign bit

    ixe = ixe >> 23; // shr 23 bits for exponent value only
    ixm = ixm >> 15; // index for the reciprocal, only 8 bits left

    ixe = ixe - 0x7F; //exponent - 0x7F, bias removal
    int32_t quotient = (int32_t)(ixe / 3); // quotient, scale factor

    // remainder. Possible remainder are [-2 -1 0 1 2] + 2
    int32_t remainder = (ixe % 3) + 2 ; // [0 1 2 3 4]

    quotient += 1023;
    uint64_t exponDouble = (uint64_t)quotient << 52;

    uix64 = uix64 & MANTISSA_MASK_64; //0x000FFFFFFFFFFFFF;
    uix64 = uix64 | ONE_MASK_64; //0x3FF0000000000000

    // Reduced input
    uix64 = SIGN_MASK_64 & uix64;

    // set double form i64 reduced input
    xdRed = asdouble(uix64);

    double_t xd3biasOnly;

    xd3biasOnly = asdouble(exponDouble);
    xdRed = xdRed * DoubleReciprocalTable[ixm];

    xdRed = xdRed - 1;

    double_t  t = xdRed * xdRed * -0.1111111111111111 +
                    xdRed * 0.3333333333333333;

    t = t + 1.0;

    t = t * xd3biasOnly; // mul: 2 ^ Quotient
    t = t * cuberoot[remainder]; // mul : 2 ^ Remainder
    t = t * CubeRootTable[ixm]; // [1 , 2) ^ (1 / 3)

    if ( denornmal )
        t = t * DENORMAL_FACTOR;

    if ( sign )
        t *= -1.0;

    return (float)t;
}
