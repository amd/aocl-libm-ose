/*
 * Copyright (C) 2025 Advanced Micro Devices, Inc. All rights reserved.
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
   Signature:
   float amd_erfcf(float x)

   Computes the complementary error function erfcf(x) = 1 - erff(x) for a given input x.

   SPEC:
   erfcf(+inf) = 0
   erfcf(-inf) = 2
   erfcf(NaN) = NaN

   Implementation Notes:
   The function uses polynomial approximations for different ranges of x

   1. For |x| < 0.84375:
       - If |x| < 2**-26: return 1-x
       - If x < 1/4: return 1-(x+x*y) where y = P(x²)/Q(x²)
       - Otherwise: return 0.5-r where r = x*y + (x-0.5)

   2. For 0.84375 <= |x| < 1.25:
       - Use rational approximation P(s)/Q(s) where s = |x|-1
       - For x >= 0: return (1-erx) - P/Q
       - For x < 0: return 1 + (erx + P/Q)

   3. For 1.25 <= |x| < 28:
       - Split exponential: exp(-z²-0.5625) * exp((z-x)*(z+x) + R/S)
       - Use different coefficients for |x| < 2.857143 vs |x| >= 2.857143
       - Special case: x < -6 returns 2-tiny

   4. For |x| >= 28:
       - Positive x: underflow (tiny*tiny)
       - Negative x: return 2-tiny

*/

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/poly.h>


static struct
{
    const float tiny;  // Very small value for underflow
    const float one;   // 1.0
    const float two;   // 2.0
    const float zero;  // 0.0
    const float half;  // 0.5
    const float erx;   // erf(1) = 0.845062911510467529297
    const float exp_offset; //

    float poly_bound1[10];
    float poly_bound2[13];
    float poly_bound3[16];
    float poly_bound4[14];

} erfcf_data = {

    .tiny       = 0x1.4484cp-100f,
    .one        = 0x1p0f,
    .two        = 0x1p1f,
    .zero       = 0.0f,
    .half       = 0x1p-1f,
    .erx        = 0x1.b0ac16p-1f,
    .exp_offset = 0x1.2p-1f,

.poly_bound1 =
{
    0x1.06eba8p-3f,   /* pp0 */
    -0x1.4cd7d6p-2f,  /* pp1 */
    -0x1.d2a51ep-6f,  /* pp2 */
    -0x1.7a2912p-8f,  /* pp3 */
    -0x1.8ead62p-16f, /* pp4 */
    0x1.97779cp-2f,   /* qq1 */
    0x1.0a54c6p-4f,   /* qq2 */
    0x1.4d022cp-8f,   /* qq3 */
    0x1.15dc92p-13f,  /* qq4 */
    -0x1.09c434p-18f  /* qq5 */
},

.poly_bound2 =
{
    -0x1.359b8cp-9f,  /* pa0 */
    0x1.a8d00ap-2f,   /* pa1 */
    -0x1.7d241p-2f,   /* pa2 */
    0x1.45fca8p-2f,   /* pa3 */
    -0x1.c63984p-4f,  /* pa4 */
    0x1.22a366p-5f,   /* pa5 */
    -0x1.1bf38p-9f,   /* pa6 */
    0x1.b3e662p-4f,   /* qa1 */
    0x1.14af0ap-1f,   /* qa2 */
    0x1.2635cep-4f,   /* qa3 */
    0x1.02660ep-3f,   /* qa4 */
    0x1.bedc26p-7f,   /* qa5 */
    0x1.88b546p-7f    /* qa6 */
},

.poly_bound3 =
{
    -0x1.434126p-7f,  /* ra0 */
    -0x1.63416ep-1f,  /* ra1 */
    -0x1.51e044p3f,   /* ra2 */
    -0x1.f300aep5f,   /* ra3 */
    -0x1.44cb18p7f,   /* ra4 */
    -0x1.7135cep7f,   /* ra5 */
    -0x1.452656p6f,   /* ra6 */
    -0x1.3a0efcp3f,   /* ra7 */
    0x1.3a6b9cp4f,    /* sa1 */
    0x1.1350c6p7f,    /* sa2 */
    0x1.b290dep8f,    /* sa3 */
    0x1.42b192p9f,    /* sa4 */
    0x1.ad0216p8f,    /* sa5 */
    0x1.b28a3ep6f,    /* sa6 */
    0x1.a47ef8p2f,    /* sa7 */
    -0x1.eeff2ep-5f   /* sa8 */
},

.poly_bound4 =
{
    -0x1.434124p-7f,  /* rb0 */
    -0x1.993ba8p-1f,  /* rb1 */
    -0x1.1c2096p4f,   /* rb2 */
    -0x1.4145d4p7f,   /* rb3 */
    -0x1.3ec882p9f,   /* rb4 */
    -0x1.004616p10f,  /* rb5 */
    -0x1.e384eap8f,   /* rb6 */
    0x1.e568b2p4f,    /* sb1 */
    0x1.45cae2p8f,    /* sb2 */
    0x1.802eb2p10f,   /* sb3 */
    0x1.8ffb76p11f,   /* sb4 */
    0x1.3f219cp11f,   /* sb5 */
    0x1.da874ep8f,    /* sb6 */
    -0x1.670e24p4f    /* sb7 */
}
};

#define TINY       erfcf_data.tiny
#define ONE        erfcf_data.one
#define TWO        erfcf_data.two
#define ZERO       erfcf_data.zero
#define HALF       erfcf_data.half
#define ERX        erfcf_data.erx
#define EXP_OFFSET erfcf_data.exp_offset

#define PP0 erfcf_data.poly_bound1[0]
#define PP1 erfcf_data.poly_bound1[1]
#define PP2 erfcf_data.poly_bound1[2]
#define PP3 erfcf_data.poly_bound1[3]
#define PP4 erfcf_data.poly_bound1[4]
#define QQ1 erfcf_data.poly_bound1[5]
#define QQ2 erfcf_data.poly_bound1[6]
#define QQ3 erfcf_data.poly_bound1[7]
#define QQ4 erfcf_data.poly_bound1[8]
#define QQ5 erfcf_data.poly_bound1[9]

#define PA0 erfcf_data.poly_bound2[0]
#define PA1 erfcf_data.poly_bound2[1]
#define PA2 erfcf_data.poly_bound2[2]
#define PA3 erfcf_data.poly_bound2[3]
#define PA4 erfcf_data.poly_bound2[4]
#define PA5 erfcf_data.poly_bound2[5]
#define PA6 erfcf_data.poly_bound2[6]
#define QA1 erfcf_data.poly_bound2[7]
#define QA2 erfcf_data.poly_bound2[8]
#define QA3 erfcf_data.poly_bound2[9]
#define QA4 erfcf_data.poly_bound2[10]
#define QA5 erfcf_data.poly_bound2[11]
#define QA6 erfcf_data.poly_bound2[12]

#define RA0 erfcf_data.poly_bound3[0]
#define RA1 erfcf_data.poly_bound3[1]
#define RA2 erfcf_data.poly_bound3[2]
#define RA3 erfcf_data.poly_bound3[3]
#define RA4 erfcf_data.poly_bound3[4]
#define RA5 erfcf_data.poly_bound3[5]
#define RA6 erfcf_data.poly_bound3[6]
#define RA7 erfcf_data.poly_bound3[7]
#define SA1 erfcf_data.poly_bound3[8]
#define SA2 erfcf_data.poly_bound3[9]
#define SA3 erfcf_data.poly_bound3[10]
#define SA4 erfcf_data.poly_bound3[11]
#define SA5 erfcf_data.poly_bound3[12]
#define SA6 erfcf_data.poly_bound3[13]
#define SA7 erfcf_data.poly_bound3[14]
#define SA8 erfcf_data.poly_bound3[15]

#define RB0 erfcf_data.poly_bound4[0]
#define RB1 erfcf_data.poly_bound4[1]
#define RB2 erfcf_data.poly_bound4[2]
#define RB3 erfcf_data.poly_bound4[3]
#define RB4 erfcf_data.poly_bound4[4]
#define RB5 erfcf_data.poly_bound4[5]
#define RB6 erfcf_data.poly_bound4[6]
#define SB1 erfcf_data.poly_bound4[7]
#define SB2 erfcf_data.poly_bound4[8]
#define SB3 erfcf_data.poly_bound4[9]
#define SB4 erfcf_data.poly_bound4[10]
#define SB5 erfcf_data.poly_bound4[11]
#define SB6 erfcf_data.poly_bound4[12]
#define SB7 erfcf_data.poly_bound4[13]

// Boundary values for main intervals
#define BOUND1 0x3f580000 /* 0.84375 */
#define BOUND2 0x3fa00000 /* 1.25 */
#define BOUND3 0x41e00000 /* 28 */

// Boundary values for sub-intervals
#define B1_SUB1 0x32800000 /* 2**-26 */
#define B1_SUB2 0x3e800000 /* 1/4 */
#define B3_SUB1 0x4006DB6D /* 1/0.35 ~ 2.857143 */
#define B3_SUB2 0x40c00000 /* 6 */

#define INF_NAN           0x7f800000
#define UPPER32_MASK      0x7fffffff
#define SPLIT_MASK        0xffffe000U

float ALM_PROTO_OPT(erfcf)(float x) {
    int32_t hx;
    uint32_t ix;
    float R, S, P, Q, s, y, z, r;

    hx = (int32_t)asuint32(x);
    ix = (uint32_t)(hx & UPPER32_MASK);

    // Handle special cases
    if (ix >= INF_NAN) {        /* erfc(nan)=nan, erfc(+-inf)=0,2 */
        float ret = (float)(((uint32_t)hx >> 31) << 1) + ONE / x;
        return ret;
    }

    if (ix < BOUND1) {             /* |x|<0.84375 */
        if (ix < B1_SUB1)          /* |x|<2**-26 */
            return ONE - x;
        z = x * x;
        r = POLY_EVAL_5( z, PP0, PP1, PP2, PP3, PP4 );
        s = POLY_EVAL_6(z, ONE, QQ1, QQ2, QQ3, QQ4, QQ5);

        y = r / s;
        if (hx < B1_SUB2) {        /* x<1/4 */
            return ONE - (x + x * y);
        } else {
            r = x * y;
            r += (x - HALF);
            return HALF - r;
        }
    }

    if (ix < BOUND2) {             /* 0.84375 <= |x| < 1.25 */
        s = asfloat(ix) - ONE;
        P = PA0 + s * (PA1 + s * (PA2 + s * (PA3 + s * (PA4 + s * (PA5 + s * PA6)))));
        Q = ONE + s * (QA1 + s * (QA2 + s * (QA3 + s * (QA4 + s * (QA5 + s * QA6)))));
        if (hx >= 0) {
            z = ONE - ERX;
            return z - P / Q;
        } else {
            z = ERX + P / Q;
            return ONE + z;
        }
    }

    if (ix < BOUND3) {             /* |x|<28 */
        x = asfloat(ix);
        s = ONE / (x * x);
        if (ix < B3_SUB1) {        /* |x| < 1/.35 ~ 2.857143*/
            R = POLY_EVAL_8(s, RA0, RA1, RA2, RA3, RA4, RA5, RA6, RA7);
            S = POLY_EVAL_9_0(s, ONE, SA1, SA2, SA3, SA4, SA5, SA6, SA7, SA8);
        } else {                   /* |x| >= 1/.35 ~ 2.857143 */
            if (hx < 0 && ix >= B3_SUB2)
                return TWO - TINY; /* x < -6 */
            R = POLY_EVAL_7(s, RB0, RB1, RB2, RB3, RB4, RB5, RB6);
            S = POLY_EVAL_8(s, ONE, SB1, SB2, SB3, SB4, SB5, SB6, SB7);
        }

        ix = asuint32(x);
        z = asfloat((uint32_t)ix & SPLIT_MASK);
        r = ALM_PROTO_OPT(expf)(-z * z - EXP_OFFSET) * ALM_PROTO_OPT(expf)((z - x) * (z + x) + R / S);
        if (hx > 0) {
            float ret = r / x;
            return ret;
        } else
            return TWO - r / x;
    } else {
        if (hx > 0) {
            return TINY * TINY;    /* Underflow for positive x */
        } else
            return TWO - TINY;     /* 2-tiny for negative x */
    }
}
