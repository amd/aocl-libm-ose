/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
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
   double amd_cdfnorminv(double x)

   Computes the inverse of the standard normal cumulative distribution function
   for a given input x in the domain (0, 1).

   SPEC:
   cdfnorminv(0) = -inf
   cdfnorminv(1) = +inf
   cdfnorminv(NaN) = NaN
   cdfnorminv(x < 0) = NaN
   cdfnorminv(x > 1) = NaN

   Implementation Notes:
   Uses Wichura's AS241 algorithm with high-precision polynomial evaluation.
   The function uses rational approximations for different ranges of x:
   1. For x in [0.075, 0.925] (central region):
       cdfnorminv(x) = q * P1(r)/Q1(r), where q = x - 0.5 and r = 0.180625 - q^2
   2. For x in [exp(-25), 0.075) and (0.925, 1-exp(-25)] (tail region, r <= 5):
       cdfnorminv(x) = P2(r)/Q2(r), where r = sqrt(-log(min(x, 1-x))) - 1.6
   3. For x in (0, exp(-25)) and (1-exp(-25), 1) (extreme tail region, r > 5):
       cdfnorminv(x) = P3(r)/Q3(r), where r = sqrt(-log(min(x, 1-x))) - 5.0
       Note: exp(-25) ~= 1.3888e-11
*/

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/poly.h>


static const struct
{
    const double one;       // 1.0
    const double half;      // 0.5
    const double bound_2;   // 5.0 - extreme tail boundary
    const double offset_1;  // 0.180625 - central region offset
    const double offset_2;  // 1.6 - tail region offset

    // Wichura (AS241) polynomial coefficients for cdfnorminv
    // P1/Q1: 8 coefficients: C0, C1 [Head and Tail]; C2, C3, C4, C5, C6, C7 [Normal]
    double poly_bound_1[12];   // 6 num, 6 den - central region [0.075, 0.925]
    double poly_bound_1_H[4]; // head parts (P10H, P11H, Q10H, Q11H)
    double poly_bound_1_T[4]; // tail parts (P10T, P11T, Q10T, Q11T)
    
    double poly_bound_2[16];   // 8 num, 8 den - tail region
    double poly_bound_3[16];   // 8 num, 8 den - extreme tail region

} cdfnorminv_data = {

    .one = 0x1p+0,                    // 1.0
    .half = 0x1p-1,                   // 0.5
    .bound_2 = 0x1.4p+2,              // 5.0
    .offset_1 = 0x1.71eb851eb851fp-3, // 0.180625
    .offset_2 = 0x1.999999999999ap+0, // 1.6

    // Central region coefficients (|q| <= 0.425, i.e., p in [0.075, 0.925])
    .poly_bound_1 =
    {
        // Numerator (P12..P17)
        // 0x1.b18d91e9eef75p+1, // P10 (unused)
        // 0x1.0a4888b1a436ep+7, // P11 (unused)
        0x1.ece5d2213c0ccp+10,
        0x1.ad1d8cd4ee71dp+13,
        0x1.66c3e869b752ap+15,
        0x1.06c1c55b78f2p+16,
        0x1.052d26b2e45e4p+15,
        0x1.39a296f7d925ep+11,

        // Denominator (Q12..Q17)
        // 0x1p+0,                // Q10 (unused)
        // 0x1.5281b386e1ab5p+5,   // Q11 (unused)
        0x1.5797efdc8b3f7p+9,
        0x1.512322e75c89fp+12,
        0x1.4b772d5d65266p+14,
        0x1.3317caa64f4bep+15,
        0x1.c0e457cb1ae76p+14,
        0x1.46a7eca984b69p+12
    },

    .poly_bound_1_H =
    {
        // Numerator (P10H, P11H)
        0x1.b18d918p+1,
        0x1.0a48888p+7,
        // 0x1.ece5d2p+10,   // P12H (unused)
        // 0x1.ad1d8c8p+13,  // P13H (unused)
        // 0x1.66c3e8p+15,   // P14H (unused)
        // 0x1.06c1c5p+16,   // P15H (unused)
        // 0x1.052d268p+15,  // P16H (unused)
        // 0x1.39a2968p+11,  // P17H (unused)

        // Denominator (Q10H, Q11H)
        0x1p+0,
        0x1.5281b38p+5,
        // 0x1.5797ef8p+9,   // Q12H (unused)
        // 0x1.5123228p+12,  // Q13H (unused)
        // 0x1.4b772dp+14,   // Q14H (unused)
        // 0x1.3317ca8p+15,  // Q15H (unused)
        // 0x1.c0e4578p+14,  // Q16H (unused)
        // 0x1.46a7ec8p+12   // Q17H (unused)
    },

    .poly_bound_1_T =
    {
        // Numerator (P10T, P11T)
        0x1.a7bbdd4p-25,
        0x1.8d21b7p-20,
        // 0x1.09e066p-17,   // P12T (unused)
        // 0x1.53b9c74p-13,  // P13T (unused)
        // 0x1.a6dd4a8p-11,  // P14T (unused)
        // 0x1.6de3c8p-10,   // P15T (unused)
        // 0x1.9722f2p-12,   // P16T (unused)
        // 0x1.df64978p-15,  // P17T (unused)

        // Denominator (Q10T, Q11T)
        0x0p+0,
        0x1.b86ad4p-25,
        // 0x1.722cfdcp-17,  // Q12T (unused)
        // 0x1.9d7227cp-14,  // Q13T (unused)
        // 0x1.7594998p-12,  // Q14T (unused)
        // 0x1.327a5fp-12,   // Q15T (unused)
        // 0x1.2c6b9d8p-12,  // Q16T (unused)
        // 0x1.4c25b48p-15   // Q17T (unused)
    },

    // Tail region coefficients (r <= 5, i.e., x in [exp(-25), 0.075) or (0.925, 1-exp(-25)])
    .poly_bound_2 =
    {
        // Numerator
        0x1.6c665fde9526ap+0,
        0x1.2857748cab19bp+2,
        0x1.713f71462256ap+2,
        0x1.d2ecb1a3d02c4p+1,
        0x1.453cc085375b2p+0,
        0x1.ef2abb9b85c37p-3,
        0x1.744eb6c45ec67p-6,
        0x1.9615ac0b7ace9p-11,

        // Denominator
        0x1p+0,
        0x1.06cefbb46a449p+1,
        0x1.ad278e6526633p+0,
        0x1.61292f23385c9p-1,
        0x1.2f5123394f04p-3,
        0x1.f207a7eab17bfp-7,
        0x1.1f18cbfdf2728p-11,
        0x1.20d3f686439e4p-30
    },

    // Extreme tail region coefficients (r > 5, i.e., x in (0, exp(-25)) or (1-exp(-25), 1))
    .poly_bound_3 =
    {
        // Numerator
        0x1.aa1b1c13ee526p+2,
        0x1.5daea6e875003p+2,
        0x1.c8ea6461fa445p+0,
        0x1.2fad9315255cfp-2,
        0x1.b2b41193b4ee7p-6,
        0x1.45c1908425345p-10,
        0x1.c6ec6cc59e02ap-16,
        0x1.afb74d693bf93p-23,

        // Denominator
        0x1p+0,
        0x1.331d34fc7d77fp-1,
        0x1.186eb183443fbp-3,
        0x1.e76f93215462ap-7,
        0x1.9c8bc979dc5d7p-11,
        0x1.35c2c496374bfp-16,
        0x1.31446f740b9ep-23,
        0x1.269bff1f8c19p-49
    },

};

#define ONE        cdfnorminv_data.one
#define HALF       cdfnorminv_data.half
#define BOUND_2    cdfnorminv_data.bound_2
#define OFFSET_1   cdfnorminv_data.offset_1
#define OFFSET_2   cdfnorminv_data.offset_2

// poly_bound_1: 12 elements (6 numerator, 6 denominator)
// #define P10  // unused
// #define P11  // unused
#define P12  cdfnorminv_data.poly_bound_1[0]
#define P13  cdfnorminv_data.poly_bound_1[1]
#define P14  cdfnorminv_data.poly_bound_1[2]
#define P15  cdfnorminv_data.poly_bound_1[3]
#define P16  cdfnorminv_data.poly_bound_1[4]
#define P17  cdfnorminv_data.poly_bound_1[5]
// #define Q10  // unused
// #define Q11  // unused
#define Q12  cdfnorminv_data.poly_bound_1[6]
#define Q13  cdfnorminv_data.poly_bound_1[7]
#define Q14  cdfnorminv_data.poly_bound_1[8]
#define Q15  cdfnorminv_data.poly_bound_1[9]
#define Q16  cdfnorminv_data.poly_bound_1[10]
#define Q17  cdfnorminv_data.poly_bound_1[11]

#define P10H cdfnorminv_data.poly_bound_1_H[0]
#define P11H cdfnorminv_data.poly_bound_1_H[1]
// #define P12H  // unused
// #define P13H  // unused
// #define P14H  // unused
// #define P15H  // unused
// #define P16H  // unused
// #define P17H  // unused
#define Q10H cdfnorminv_data.poly_bound_1_H[2]
#define Q11H cdfnorminv_data.poly_bound_1_H[3]
// #define Q12H  // unused
// #define Q13H  // unused
// #define Q14H  // unused
// #define Q15H  // unused
// #define Q16H  // unused
// #define Q17H  // unused

#define P10T cdfnorminv_data.poly_bound_1_T[0]
#define P11T cdfnorminv_data.poly_bound_1_T[1]
// #define P12T  // unused
// #define P13T  // unused
// #define P14T  // unused
// #define P15T  // unused
// #define P16T  // unused
// #define P17T  // unused
#define Q10T cdfnorminv_data.poly_bound_1_T[2]
#define Q11T cdfnorminv_data.poly_bound_1_T[3]
// #define Q12T  // unused
// #define Q13T  // unused
// #define Q14T  // unused
// #define Q15T  // unused
// #define Q16T  // unused
// #define Q17T  // unused

// poly_bound_2: 16 elements (8 numerator, 8 denominator)
#define P20  cdfnorminv_data.poly_bound_2[0]
#define P21  cdfnorminv_data.poly_bound_2[1]
#define P22  cdfnorminv_data.poly_bound_2[2]
#define P23  cdfnorminv_data.poly_bound_2[3]
#define P24  cdfnorminv_data.poly_bound_2[4]
#define P25  cdfnorminv_data.poly_bound_2[5]
#define P26  cdfnorminv_data.poly_bound_2[6]
#define P27  cdfnorminv_data.poly_bound_2[7]
#define Q20  cdfnorminv_data.poly_bound_2[8]
#define Q21  cdfnorminv_data.poly_bound_2[9]
#define Q22  cdfnorminv_data.poly_bound_2[10]
#define Q23  cdfnorminv_data.poly_bound_2[11]
#define Q24  cdfnorminv_data.poly_bound_2[12]
#define Q25  cdfnorminv_data.poly_bound_2[13]
#define Q26  cdfnorminv_data.poly_bound_2[14]
#define Q27  cdfnorminv_data.poly_bound_2[15]

// poly_bound_3: 16 elements (8 numerator, 8 denominator)
#define P30  cdfnorminv_data.poly_bound_3[0]
#define P31  cdfnorminv_data.poly_bound_3[1]
#define P32  cdfnorminv_data.poly_bound_3[2]
#define P33  cdfnorminv_data.poly_bound_3[3]
#define P34  cdfnorminv_data.poly_bound_3[4]
#define P35  cdfnorminv_data.poly_bound_3[5]
#define P36  cdfnorminv_data.poly_bound_3[6]
#define P37  cdfnorminv_data.poly_bound_3[7]
#define Q30  cdfnorminv_data.poly_bound_3[8]
#define Q31  cdfnorminv_data.poly_bound_3[9]
#define Q32  cdfnorminv_data.poly_bound_3[10]
#define Q33  cdfnorminv_data.poly_bound_3[11]
#define Q34  cdfnorminv_data.poly_bound_3[12]
#define Q35  cdfnorminv_data.poly_bound_3[13]
#define Q36  cdfnorminv_data.poly_bound_3[14]
#define Q37  cdfnorminv_data.poly_bound_3[15]

#define UPPER32_MASK      0x7fffffff

// Boundary values for domain checks
#define ONEU      0x3FF0000000000000 // 1.0
#define ONEU32    0x3FF00000         // 1.0 (upper 32 bits)
#define ZERO      0x0000000000000000 // 0.0
#define BOUND_1U32 0x3FDB3333        // 0.425 (upper 32 bits)

double ALM_PROTO_OPT(cdfnorminv)(double x) {
    uint64_t ux;
    uint32_t ix;
    uint64_t sign;
    double p_, q_, r, val;

    ux = asuint64(x);
    sign = ux & SIGNBIT_DP64;
    ux = ux & ~SIGNBIT_DP64;
    ix = (ux >> 32) & UPPER32_MASK;

    /* special cases : NaN, outside domain [0,1], -0
     * For Nan : ix >= INF_NAN (hence ix > ONEU32)
     * For x < 0 : sign is true
     * For x > 1 : ix > ONEU32
     * For x = -0 : sign is true and ix == 0 
    */
    if (unlikely(sign || ix > ONEU32))
    {
        if (ux == ZERO) // -0
            return asdouble(NEG_INF_F64);
        if (ux > POS_INF_F64) // propagate NaN
            return x - x;
        else 
            return alm_cdfnorminv_special(asdouble(NEG_QNAN_F64));
    }

    // cdfnorminv(1) = +inf
    if (ux == ONEU)
        return asdouble(POS_INF_F64);

    // cdfnorminv(0) = -inf
    if (ux == ZERO)
        return asdouble(NEG_INF_F64);

    p_ = x;
    q_ = p_ - HALF;

    uint64_t uq = asuint64(q_);
    sign = uq & SIGNBIT_DP64;
    uq = uq & ~SIGNBIT_DP64;
    uint32_t iq = (uq >> 32) & UPPER32_MASK;

    if (iq <= BOUND_1U32) {
        // Central region: p in [0.075, 0.925]
        r = OFFSET_1 - q_ * q_;
        double P = POLY_EVAL_ESTRIN_8_TAIL_2(r,
            P10H, P10T, P11H, P11T,
            P12, P13, P14, P15, P16, P17);
        double Q = POLY_EVAL_ESTRIN_8_TAIL_2(r,
            Q10H, Q10T, Q11H, Q11T,
            Q12, Q13, Q14, Q15, Q16, Q17);
        val = q_ * (P / Q);
    } else {
        // Tail regions: p in (0, 0.075) or (0.925, 1.0)
        r = sign ? p_ : (ONE - p_);
        r = ALM_PROTO(sqrt)(-ALM_PROTO(log)(r));
        /*
         * Tail region:
         * sqrt(-log(min(p, 1-p))) <= 5
         * -log(min(p, 1-p)) <= 25
         * log(min(p, 1-p)) >= -25
         * min(p, 1-p) >= exp(-25) ~= 1.3888e-11
         */
        if (r <= BOUND_2) {
            // Tail region: r <= 5
            r -= OFFSET_2;
            double P = POLY_EVAL_HORNER_8(r,
                P20, P21, P22, P23, P24, P25, P26, P27);
            double Q = POLY_EVAL_HORNER_8(r,
                Q20, Q21, Q22, Q23, Q24, Q25, Q26, Q27);
            val = P / Q;
        } else {
            // Extreme tail region: r > 5
            r -= BOUND_2;
            double P = POLY_EVAL_HORNER_8(r,
                P30, P31, P32, P33, P34, P35, P36, P37);
            double Q = POLY_EVAL_HORNER_8(r,
                Q30, Q31, Q32, Q33, Q34, Q35, Q36, Q37);
            val = P / Q;
        }

        if (sign)
            val = -val;
    }

    return val;
}
