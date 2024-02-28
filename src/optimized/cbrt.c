/*
 * Copyright (C) 2008-2023 Advanced Micro Devices, Inc. All rights reserved.
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
 *   double cbrt(double x)
 *
 * Spec:
 * NOTE: The algorithm is optimized from the assembly version of CBRT function
 * To calculate (x)^1/3
 * step 1) Extract exponent and mantissa from input.
 * step 2) Convert input from float to double.
 * step 3) Reduce the input [1, 2)
            3.1) Replace exponent with 3ff i.e 1
            3.2) Or with the mantissa
 * step 4) Scaling factor <= exponent/3 and Cuberoot2Index <= remainder (exponent % 3)
 * step 5) Polynomial approximation on reduced input
 * step 6) Multiply result of Polynomial approximation to cube-root remainder and scale factor
 * step 7) Return : Check for proper sign  and return the result
 *
 * Mathmatical Explanation
 * (x)^(1/3) = (x_d * 2^n)^(1/3)
 *           =  x_d^(1/3) * 2^(n/3)
 *           =  x_d^(1/3) * 2^(Quotient) * 2^(Remainder/3), where x_d is reduced input between [1,2)
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
#include <cbrt_data.h>

#define MANTISSA_MASK_64    0x000FFFFFFFFFFFFF
#define ZERO_POINT_FIVE     0x3FE0000000000000
#define ONE_BY_512          0.001953125                         // 0x3f60000000000000

#define CBRT_EXP_COEFF_1    3.33333333333333314829616256247E-1  // 0x3fd5555555555555
#define CBRT_EXP_COEFF_2    -1.11111111111111104943205418749E-1 // 0xbfbc71c71c71c71c
#define CBRT_EXP_COEFF_3    6.17283950617283916351141215273E-2  // 0x3faf9add3c0ca458
#define CBRT_EXP_COEFF_4    -4.11522633744855967363740489873E-2 // 0xbfa511e8d2b3183b
#define CBRT_EXP_COEFF_5    3.01783264746227734842687340233E-2  // 0x3f9ee7113506ac13
#define CBRT_EXP_COEFF_6    -2.34720317024843770636888251602E-2 // 0xbf98090d6221a247

#define LOW_2_POW_N2        1.77929718607039166806688400583E-8  // 0x3e531ae515c447bb // cbrt(2^-2) Low
#define HIGH_2_POW_N2       6.299605071544647216796875E-1       // 0x3FE428A2F0000000 // cbrt(2^-2) High
#define LOW_2_POW_N1        9.76019226667272715610794680662E-9  // 0x3e44f5b8f20ac166 // cbrt(2^-1) Low
#define HIGH_2_POW_N1       7.93700516223907470703125E-1        // 0x3FE965FEA0000000 // cbrt(2^-1) High
#define LOW_2_POW_0         0.0E0                               // 0x0000000000000000 // cbrt(2^0) Low
#define HIGH_2_POW_0        1.0E0                               // 0x3FF0000000000000 // cbrt(2^0) High
#define LOW_2_POW_P1        3.55859437214078333613376801167E-8  // 0x3e631ae515c447bb // cbrt(2^1) Low
#define HIGH_2_POW_P1       1.259921014308929443359375E0        // 0x3FF428A2F0000000 // cbrt(2^1) High
#define LOW_2_POW_P2        1.95203845333454543122158936132E-8  // 0x3e54f5b8f20ac166 // cbrt(2^2) Low
#define HIGH_2_POW_P2       1.58740103244781494140625E0         // 0x3FF965FEA0000000 // cbrt(2^2) High

double
ALM_PROTO_OPT(cbrt)(double x) {
    uint64_t uix64;
    uint64_t sign = 0;
    uint64_t ix = 0;
    uint64_t ixe = 0;
    uint64_t ixm = 0;

    int64_t quotient = 0;
    int64_t rem = 0;

    ix =  asuint64(x);

    ixe = EXPBITS_DP64 & ix;  // exponent extractor
    ixm = MANTBITS_DP64 & ix; // mantissa extractor

    if (unlikely( ixe == PINFBITPATT_DP64 ))
    {
        if (ixm == 0)
            __alm_handle_errorf(ix, AMD_F_OVERFLOW);
        else
            __alm_handle_errorf(ix|QNAN_MASK_64, AMD_F_INVALID);

        return x + x;
    }

    if ( ixe == 0 )
    {
        // denormal number;
        if (ixm == 0) // is zero
            return 0.0f;
        x = x * TWOPOW53_DP64;

        ix = asuint64(x);

        ixe = EXPBITS_DP64 & ix;  // exponent extractor
        ixm = MANTBITS_DP64 & ix; // mantissa extractor
    }

    uix64 = asuint64(x);

    sign = uix64 >> 63; // extract sign bit

    ixe = ixe >> 52; // shift right 52 bits for exponent value only
    ixe = ixe - 1023; // exponent - 0x3FF, bias removal

    quotient = (int64_t)ixe / 3; // quotient, scale factor
    rem = (int64_t)ixe % 3;

    quotient += 1023;
    uint64_t exponDouble = (uint64_t)quotient << 52;

    uix64 = uix64 & MANTISSA_MASK_64;
    uix64 = uix64 | ZERO_POINT_FIVE;

    uint64_t mant_1 = ixm >> 43;
    uint64_t mant_2 = ixm >> 44;

    mant_1 &= 0x0000000000000001;
    mant_2 |= 0x0000000000000100;

    mant_1 += mant_2;

    double temp = (double)mant_1;
    temp = ONE_BY_512 * temp;

    double r = asdouble(uix64);
    r = r - temp;

    mant_1 = mant_1 - 256;
    temp = asdouble(InverseTable[mant_1]);
    r = temp * r;

    // To calculate cbrt: exp = coeff1 * r + coeff2 * r^2 + coeff3 * r^3 + coeff4 * r^4 + coeff5 * r^5 + coeff6 * r^6

    double r2 = r*r;
    double r3 = r2*r;
    double r4 = r2*r2;
    double r5 = r4*r;
    double r6 = r3*r3;

    double exp = CBRT_EXP_COEFF_1 * r;
    exp += CBRT_EXP_COEFF_2 * r2;
    exp += CBRT_EXP_COEFF_3 * r3;
    exp += CBRT_EXP_COEFF_4 * r4;
    exp += CBRT_EXP_COEFF_5 * r5;
    exp += CBRT_EXP_COEFF_6 * r6;

    double cbrtRem_h = 0;
    double cbrtRem_t = 0;

    switch(rem)
    {
        case -2:
            cbrtRem_h = HIGH_2_POW_N2;
            cbrtRem_t = LOW_2_POW_N2;
            break;

        case -1:
            cbrtRem_h = HIGH_2_POW_N1;
            cbrtRem_t = LOW_2_POW_N1;
            break;

        case 0:
            cbrtRem_h = HIGH_2_POW_0;
            cbrtRem_t = LOW_2_POW_0;
            break;

        case 1:
            cbrtRem_h = HIGH_2_POW_P1;
            cbrtRem_t = LOW_2_POW_P1;
            break;

        case 2:
            cbrtRem_h = HIGH_2_POW_P2;
            cbrtRem_t = LOW_2_POW_P2;
            break;

        default:
            /* The codeflow is not expected to enter default case here!
             * Variable rem can only be -2, -1, 0, 1, 2.
             */ 
            break;
    }

    mant_1 <<= 1; // mant_1 * 2

    double cbrtF_t = asdouble(F_H_L[mant_1]);
    double cbrtF_h = asdouble(F_H_L[mant_1+1]);

    double bH = cbrtF_h * cbrtRem_h;
    double bT = (cbrtF_t * cbrtRem_t) + (cbrtF_t * cbrtRem_h) + (cbrtRem_t * cbrtF_h);

    // ans = (exp * bT) + bT + (exp * bH) + bH
    double ans = (exp * bT);
    ans += bT;
    ans += (exp * bH);
    ans += bH;

    double xd3biasOnly = asdouble(exponDouble);
    ans = ans * xd3biasOnly;
    if(sign)
        ans *= -1;

    return ans;

}
