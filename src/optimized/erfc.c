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
    double amd_erfc(double x)

    Computes the complementary error function erfc(x) = 1 - erf(x) for a given input x.

    SPEC:
    erfc(+inf) = 0
    erfc(-inf) = 2
    erfc(NaN) = NaN

    Implementation Notes:
    The function uses polynomial approximations for different ranges of x.
    1. For |x| < 0.84375,
        erfc(x) = 1 - (2/sqrt(pi)) * x * Poly
        where Poly is a direct polynomial approximation of erfc

    2. For 0.84375 <= |x| < 1.25,
        erfc(x) = (1 - erx) - Poly1(s)/Poly2(s),
        where s = |x| - 1,
        erx = erf(1) = 0.845062911510467529297,
        Poly1 and Poly2 are rational approximations for erfc

    3. For 1.25 <= |x| < 2.85,
        erfc(x) = exp(-x^2) * Poly1(s) / Poly2(s),
        where s = |x| - 2

    4. For 2.85 <= |x| < 28,
        erfc = exp(-x^2) * Poly(s) / x,
        where s = 1/x^2

    5. For |x| >= 28,
        Results in underflow

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
    const double tiny;  // 1e-300
    const double one;   // 1.0
    const double two;   // 2.0
    const double erx;   // erf(1) = 0.845062911510467529297
    const double zero;  // 0.0
    const double exp_offset; // 0.5625

    double poly_bound1[10];
    double poly_bound2[13];
    double poly_bound3[16];
    double poly_bound4[14];

} erfc_data = {

    .tiny = 0x1.0p-997,
    .one = 0x1.0000000000000p+0,
    .two = 0x1.0000000000000p+1,
    .erx = 0x1.b0ac160000000000012e3b40a0e9b4f7dda7edf83p-1,
    .zero = 0.0,
    .exp_offset = 0x1.2p-1,

    .poly_bound1 =
    {
        0x1.06eba8214db680000b5c4a02192bc0d6775dbd04bp-3,  // pp0
        -0x1.4cd7d691cb912ffff6987571c49e9d5ba58d149c5p-2, // pp1
        -0x1.d2a51dbd7194effffb74e34dee8a8644b88b11b3ep-6, // pp2
        -0x1.7a291236668e4000004af036bb54e7960cc41c4f2p-8, // pp3
        -0x1.8ead6120016ac0000c950dc869ed1fffa26ca217p-16, // pp4
        0x1.97779cddadc08ffffa6f2a8ba9fe88775385c792cp-2,  // qq1
        0x1.0a54c5536ceb9ffffe5b83de8d54d1d859ddd053p-4,   // qq2
        0x1.4d022c4d36b0effffa6aab7584eb7aa72d4d52381p-8,  // qq3
        0x1.15dc9221c1a1000010eaa201e576bdac5425d4cecp-13, // qq4
        -0x1.09c4342a2611fffffdc3d1f21657df14b5e0d73aap-18 // qq5
    },

    .poly_bound2 =
    {
        -0x1.359b8bef77537ffffd00cd7db189169c135b0709bp-9, // pa0
        0x1.a8d00ad92b34d00003094afebc742b7931a6a8ed4p-2,  // pa1
        -0x1.7d240fbb8c3f10000201e552688bbd4704ca223cep-2, // pa2
        0x1.45fca805120e3fffff21d90ca984b44fa204f2922p-2,  // pa3
        -0x1.c63983d3e28ebffffcb512cdfee4ff1d14c943147p-4, // pa4
        0x1.22a36599795eb00004e47e44e1af4adb3a1b804ebp-5,  // pa5
        -0x1.1bf380a96073effffe9016b6c7695cc1eb76d5c63p-9, // pa6
        0x1.b3e6618eee322fffedf109720e69b787d2a8af47ep-4,  // qa1
        0x1.14af092eb6f32fffffb26da4f3c1d10f8be335a8ap-1,  // qa2
        0x1.2635cd99fe9a84dcdea6acce65a692b76a712d542p-4,  // qa3
        0x1.02660e763351f0000ee976fe1ade4642fceee8977p-3,  // qa4
        0x1.bedc26b51dd1bfffe3363319a9ab3a1ef6ecce4fep-7,  // qa5
        0x1.88b545735151cfffef1055eecc85eb2cd72dc3fafp-7   // qa6
    },

    .poly_bound3 =
    {
        -0x1.43412600d6434ffffec496db0cd3707c198dd6271p-7, // ra0
        -0x1.63416e4ba73600000162ed78c0955e314512b0775p-1, // ra1
        -0x1.51e0441b0e7260001d36ad666d98c71023f089094p3,  // ra2
        -0x1.f300ae4cba38cffffb5b4296e56c42604f78e437ep5,  // ra3
        -0x1.44cb18428226600000a60c65617d9c6fb1abc81eep7,  // ra4
        -0x1.7135cebccabb200007979c917e36c9a353e19a109p7,  // ra5
        -0x1.4526557e4d2f20000039cc717a16b9a82f06f2425p6,  // ra6
        -0x1.3a0efc69ac25bfffff638d3d3bda1ba981b5cc7dcp3,  // ra7
        0x1.3a6b9bd707686ffffadfc8a525f269065368e2764p4,   // sa1
        0x1.1350c526ae720fffff4c5c64a3d79231987f28af7p7,   // sa2
        0x1.b290dd58a1a70ffffb2640dca2933c47a2e8cfa94p8,   // sa3
        0x1.42b1921ec286800000c6bb664ad512d462c86a211p9,   // sa4
        0x1.ad02157700314000085cb016d035fa67bf652f8c9p8,   // sa5
        0x1.b28a3ee48ae2bfffeef6cc811822cb3127fd43ee6p6,   // sa6
        0x1.a47ef8e484a9300003252be49f8bac45531d7858ep2,   // sa7
        -0x1.eeff2ee749a61ffffe5a5c50f92f26656ca87e842p-5  // sa8
    },

    .poly_bound4 =
    {
        -0x1.4341239e86f49ffffe4659d1d17ea32909d8160d9p-7, // rb0
        -0x1.993ba70c285ddffffe7702a68a6f9977883533835p-1, // rb1
        -0x1.1c209555f9959ffff92f6216fd7e506f6413487a6p4,  // rb2
        -0x1.4145d43c5ed97ffff618c3b35140e6e4484d3089ap7,  // rb3
        -0x1.3ec881375f228000037258d0436149bded60e37dap9,  // rb4
        -0x1.004616a2e5991fffe9609c038e118374b38bafe77p10, // rb5
        -0x1.e384e9bdc383f0000857219e40106bb0067945b84p8,  // rb6
        0x1.e568b261d518fffffa4bf153b57730b6ba5d07e87p4,   // sb1
        0x1.45cae221b9f09ffff75192b5b7d01f2ece01e2f2ap8,   // sb2
        0x1.802eb189d51180001123a27f16a309250d31a97e6p10,  // sb3
        0x1.8ffb7688c2469ffffaadf853fc2038cb2862b5c1bp11,  // sb4
        0x1.3f219cedf3be5ffff5837ebacc2c56a80445bd428p11,  // sb5
        0x1.da874e79fe762ffff85b879b49284d665f3955176p8,   // sb6
        -0x1.670e242712d62000034c906dd34ed9d3593352b1bp4   // sb7
    },

};

#define TINY       erfc_data.tiny
#define ONE        erfc_data.one
#define TWO        erfc_data.two
#define ERX        erfc_data.erx
#define ZERO       erfc_data.zero
#define EXP_OFFSET erfc_data.exp_offset

#define PP0 erfc_data.poly_bound1[0]
#define PP1 erfc_data.poly_bound1[1]
#define PP2 erfc_data.poly_bound1[2]
#define PP3 erfc_data.poly_bound1[3]
#define PP4 erfc_data.poly_bound1[4]
#define QQ1 erfc_data.poly_bound1[5]
#define QQ2 erfc_data.poly_bound1[6]
#define QQ3 erfc_data.poly_bound1[7]
#define QQ4 erfc_data.poly_bound1[8]
#define QQ5 erfc_data.poly_bound1[9]

#define PA0 erfc_data.poly_bound2[0]
#define PA1 erfc_data.poly_bound2[1]
#define PA2 erfc_data.poly_bound2[2]
#define PA3 erfc_data.poly_bound2[3]
#define PA4 erfc_data.poly_bound2[4]
#define PA5 erfc_data.poly_bound2[5]
#define PA6 erfc_data.poly_bound2[6]
#define QA1 erfc_data.poly_bound2[7]
#define QA2 erfc_data.poly_bound2[8]
#define QA3 erfc_data.poly_bound2[9]
#define QA4 erfc_data.poly_bound2[10]
#define QA5 erfc_data.poly_bound2[11]
#define QA6 erfc_data.poly_bound2[12]

#define RA0 erfc_data.poly_bound3[0]
#define RA1 erfc_data.poly_bound3[1]
#define RA2 erfc_data.poly_bound3[2]
#define RA3 erfc_data.poly_bound3[3]
#define RA4 erfc_data.poly_bound3[4]
#define RA5 erfc_data.poly_bound3[5]
#define RA6 erfc_data.poly_bound3[6]
#define RA7 erfc_data.poly_bound3[7]
#define SA1 erfc_data.poly_bound3[8]
#define SA2 erfc_data.poly_bound3[9]
#define SA3 erfc_data.poly_bound3[10]
#define SA4 erfc_data.poly_bound3[11]
#define SA5 erfc_data.poly_bound3[12]
#define SA6 erfc_data.poly_bound3[13]
#define SA7 erfc_data.poly_bound3[14]
#define SA8 erfc_data.poly_bound3[15]

#define RB0 erfc_data.poly_bound4[0]
#define RB1 erfc_data.poly_bound4[1]
#define RB2 erfc_data.poly_bound4[2]
#define RB3 erfc_data.poly_bound4[3]
#define RB4 erfc_data.poly_bound4[4]
#define RB5 erfc_data.poly_bound4[5]
#define RB6 erfc_data.poly_bound4[6]
#define SB1 erfc_data.poly_bound4[7]
#define SB2 erfc_data.poly_bound4[8]
#define SB3 erfc_data.poly_bound4[9]
#define SB4 erfc_data.poly_bound4[10]
#define SB5 erfc_data.poly_bound4[11]
#define SB6 erfc_data.poly_bound4[12]
#define SB7 erfc_data.poly_bound4[13]


#define SIGN_MASK         0x7FFFFFFFFFFFFFFF
#define INF               0x7ff0000000000000
#define UPPER32_MASK      0x7fffffff
#define UPPER32_MASK_DP64 0x7fffffff00000000ULL
#define INF_NAN           0x7ff00000


/* Boundary values for intervals */

/* Main boundary values */
#define BOUND1 0x3feb0000 /* 0.84375 */
#define BOUND2 0x3ff40000 /* 1.25 */
#define BOUND3 0x403c0000 /* 28 */

/* Boundary values for sub-intervals */
#define B1_SUB1 0x3e300000 /* 2**-28 */
#define B1_SUB2 0x00800000 /* 2**-23 */
#define B3_SUB1 0x4006DB6D /* 1/0.35 ~ 2.857 */
#define B3_SUB2 0x40180000 /* 6 */

double ALM_PROTO_OPT(erfc)(double x) {

    double R, S, P, Q, s, y, z, r;
    uint64_t ux;
    uint32_t ix;
    uint64_t sign = 0;

    ux = asuint64(x);
    sign =  ux & SIGNBIT_DP64;
    ux = ux & ~SIGNBIT_DP64;
    ix = ( ux >> 32 )& UPPER32_MASK;

    // Check for NaN or Inf
    if (unlikely(ix >= INF_NAN)) {
        if (ux == INF) {
            return (sign) ? TWO : ZERO; // erfc(Inf) = 0, erfc(-Inf) = 2
        }
        return x - x; // return NaN
    }

    if (ix < BOUND1) {
        /* |x| < 0.84375 */
        if (ix < B1_SUB1) {
            /* |x| < 2**-28 */
            if (ix < B1_SUB2)
                return ONE - (x + x * x);
            return ONE - x;
        }
        z = x * x;

        r = POLY_EVAL_5( z, PP0, PP1, PP2, PP3, PP4 );
        s = POLY_EVAL_6(z, ONE, QQ1, QQ2, QQ3, QQ4, QQ5);

        y = r / s;
        return ONE - (x + x * y);
    }

    else if (ix < BOUND2) {
        /* 0.84375 <= |x| < 1.25*/
        s = asdouble(ux) - ONE;

        P = POLY_EVAL_7(s, PA0, PA1, PA2, PA3, PA4, PA5, PA6);
        Q = POLY_EVAL_7(s, ONE, QA1, QA2, QA3, QA4, QA5, QA6);

        if(sign)
        {
            z = ERX + P / Q;
            return ONE + z;
        }
        else
        {
            z = ONE - ERX;
            return z - P / Q;
        }
    }

    else if (ix < BOUND3) {
        /* |x| < 28 */
        x = asdouble(ux & SIGN_MASK);
        s = ONE / (x * x);

        if (ix < B3_SUB1) {
            /* |x| < 1/0.35 ~ 2.857 */

            R = POLY_EVAL_8(s, RA0, RA1, RA2, RA3, RA4, RA5, RA6, RA7);

            S = POLY_EVAL_9_0(s, ONE, SA1, SA2, SA3, SA4, SA5, SA6, SA7, SA8);
        } else {
            /* |x| >= 1/0.35 ~ 2.857 */
            if (sign && ix >= B3_SUB2) return TWO;

            R = POLY_EVAL_7(s, RB0, RB1, RB2, RB3, RB4, RB5, RB6);

            S = POLY_EVAL_8(s, ONE, SB1, SB2, SB3, SB4, SB5, SB6, SB7);
        }

        ux = ux & UPPER32_MASK_DP64; // Clear the lower 32 bits for higher precision
        z = asdouble(ux);

        /* Use higher precision calculation of exp */
        double exp_hi = ALM_PROTO_OPT(exp)(-z * z - EXP_OFFSET);
        double exp_lo = ALM_PROTO_OPT(exp)((z - x) * (z + x) + R / S);
        r = exp_hi * exp_lo;

        if(sign)
            return TWO - r / x;
        else
            return r / x;
    } else {
        /* |x| >= 28 - handle underflow */
        if(sign)
            return TWO;
        else
            return TINY * TINY; // Underflow case, return a very small value
    }

}
