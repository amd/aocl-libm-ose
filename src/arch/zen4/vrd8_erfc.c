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
 * C implementation of erfc double precision 512-bit vector version (vrd8)
 *
 * Signature:
 *    v_f64x8_t amd_vrd8_erfc(v_f64x8_t x)
 *
 * Computes the complementary error function erfc(x) = 1 - erf(x) for a given input x.
 *
 * SPEC:
 * erfc(+inf) = 0
 * erfc(-inf) = 2
 * erfc(NaN) = NaN
 *
 * Implementation Notes:
 * The function uses polynomial approximations for different ranges of x.
 * 1. For |x| < 0.84375,
 *     erfc(x) = 1 - (2/sqrt(pi)) * x * Poly
 *     where Poly is a direct polynomial approximation of erfc
 *
 * 2. For 0.84375 <= |x| < 1.25,
 *     erfc(x) = (1 - erx) - Poly1(s)/Poly2(s),
 *     where s = |x| - 1,
 *     erx = erf(1) = 0.845062911510467529297,
 *     Poly1 and Poly2 are rational approximations for erfc
 *
 * 3. For 1.25 <= |x| < 2.85,
 *       erfc(x) = exp(-x^2) * Poly1(s) / Poly2(s),
 *       where s = |x| - 2
 *
 * 4. For 2.85 <= |x| < 28,
 *       erfc = exp(-x^2) * Poly(s) / x,
 *       where s = 1/x^2
 *
 * 5. For |x| >= 28,
 *       Results in underflow
 *
 */

#include <stdint.h>

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly-vec.h>
#include <libm/arch/zen4.h>

static const struct {
    v_u64x8_t   bound1, bound2, bound3;
    v_u64x8_t   b1_sub1, b1_sub2, b3_sub1, b3_sub2;
    v_u64x8_t   sign_mask, mask_32;
    v_u64x8_t   inf_nan, inf;
    v_f64x8_t   tiny, one, two, zero, erx, exp_offset;
    v_f64x8_t   poly_bound1[10];
    v_f64x8_t   poly_bound2[13];
    v_f64x8_t   poly_bound3[16];
    v_f64x8_t   poly_bound4[14];
} v8_erfc_data = {
    .bound1      = _MM512_SET1_U64x8((uint64_t)0x3FEB0000ULL),  /* 0.84375 */
    .bound2      = _MM512_SET1_U64x8((uint64_t)0x3FF40000ULL),  /* 1.25 */
    .bound3      = _MM512_SET1_U64x8((uint64_t)0x403C0000ULL),  /* 28 */
    .b1_sub1     = _MM512_SET1_U64x8((uint64_t)0x3E300000ULL),  /* 2**-28 */
    .b1_sub2     = _MM512_SET1_U64x8((uint64_t)0x00800000ULL),  /* 2**-23 */
    .b3_sub1     = _MM512_SET1_U64x8((uint64_t)0x4006DB6DULL),  /* 1/0.35 ~ 2.857 */
    .b3_sub2     = _MM512_SET1_U64x8((uint64_t)0x40180000ULL),  /* 6 */
    .sign_mask   = _MM512_SET1_U64x8((uint64_t)0x7FFFFFFFFFFFFFFFULL),
    .mask_32     = _MM512_SET1_U64x8((uint64_t)0x7FFFFFFFULL),
    .inf_nan     = _MM512_SET1_U64x8((uint64_t)0x7FF00000ULL),
    .inf         = _MM512_SET1_U64x8((uint64_t)0x7FF0000000000000ULL),
    .tiny        = _MM512_SET1_PD8(0x1.0p-997),
    .one         = _MM512_SET1_PD8(1.0),
    .two         = _MM512_SET1_PD8(2.0),
    .zero        = _MM512_SET1_PD8(0.0),
    .erx         = _MM512_SET1_PD8(0x1.b0ac16p-1),
    .exp_offset  = _MM512_SET1_PD8(0x1.2p-1),


    .poly_bound1 = {
    _MM512_SET1_PD8(0x1.06eba8214db68p-3),  // pp0
    _MM512_SET1_PD8(-0x1.4cd7d691cb913p-2), // pp1
    _MM512_SET1_PD8(-0x1.d2a51dbd7194fp-6), // pp2
    _MM512_SET1_PD8(-0x1.7a291236668e4p-8), // pp3
    _MM512_SET1_PD8(-0x1.8ead6120016acp-16),// pp4
    _MM512_SET1_PD8(0x1.97779cddadc09p-2),  // qq1
    _MM512_SET1_PD8(0x1.0a54c5536cebap-4),  // qq2
    _MM512_SET1_PD8(0x1.4d022c4d36b0fp-8),  // qq3
    _MM512_SET1_PD8(0x1.15dc9221c1a1p-13),  // qq4
    _MM512_SET1_PD8(-0x1.09c4342a2612p-18)  // qq5
},
.poly_bound2 = {
    _MM512_SET1_PD8(-0x1.359b8bef77538p-9), // pa0
    _MM512_SET1_PD8(0x1.a8d00ad92b34dp-2),  // pa1
    _MM512_SET1_PD8(-0x1.7d240fbb8c3f1p-2), // pa2
    _MM512_SET1_PD8(0x1.45fca805120e4p-2),  // pa3
    _MM512_SET1_PD8(-0x1.c63983d3e28ecp-4), // pa4
    _MM512_SET1_PD8(0x1.22a36599795ebp-5),  // pa5
    _MM512_SET1_PD8(-0x1.1bf380a96073fp-9), // pa6
    _MM512_SET1_PD8(0x1.b3e6618eee323p-4),  // qa1
    _MM512_SET1_PD8(0x1.14af092eb6f33p-1),  // qa2
    _MM512_SET1_PD8(0x1.2635cd99fe9a8p-4),  // qa3
    _MM512_SET1_PD8(0x1.02660e763351fp-3),  // qa4
    _MM512_SET1_PD8(0x1.bedc26b51dd1cp-7),  // qa5
    _MM512_SET1_PD8(0x1.88b545735151dp-7)   // qa6
},
.poly_bound3 = {
    _MM512_SET1_PD8(-0x1.43412600d6435p-7), // ra0
    _MM512_SET1_PD8(-0x1.63416e4ba736p-1), // ra1
    _MM512_SET1_PD8(-0x1.51e0441b0e726p3),  // ra2
    _MM512_SET1_PD8(-0x1.f300ae4cba38dp5),  // ra3
    _MM512_SET1_PD8(-0x1.44cb184282266p7),  // ra4
    _MM512_SET1_PD8(-0x1.7135cebccabb2p7),  // ra5
    _MM512_SET1_PD8(-0x1.4526557e4d2f2p6),  // ra6
    _MM512_SET1_PD8(-0x1.3a0efc69ac25cp3),  // ra7
    _MM512_SET1_PD8(0x1.3a6b9bd707687p4),   // sa1
    _MM512_SET1_PD8(0x1.1350c526ae721p7),   // sa2
    _MM512_SET1_PD8(0x1.b290dd58a1a71p8),   // sa3
    _MM512_SET1_PD8(0x1.42b1921ec2868p9),   // sa4
    _MM512_SET1_PD8(0x1.ad02157700314p8),   // sa5
    _MM512_SET1_PD8(0x1.b28a3ee48ae2cp6),   // sa6
    _MM512_SET1_PD8(0x1.a47ef8e484a93p2),   // sa7
    _MM512_SET1_PD8(-0x1.eeff2ee749a62p-5)  // sa8
},
.poly_bound4 = {
    _MM512_SET1_PD8(-0x1.4341239e86f4ap-7), // rb0
    _MM512_SET1_PD8(-0x1.993ba70c285dep-1), // rb1
    _MM512_SET1_PD8(-0x1.1c209555f995ap4),  // rb2
    _MM512_SET1_PD8(-0x1.4145d43c5ed98p7),  // rb3
    _MM512_SET1_PD8(-0x1.3ec881375f228p9),  // rb4
    _MM512_SET1_PD8(-0x1.004616a2e5992p10), // rb5
    _MM512_SET1_PD8(-0x1.e384e9bdc383fp8),  // rb6
    _MM512_SET1_PD8(0x1.e568b261d519p4),    // sb1
    _MM512_SET1_PD8(0x1.45cae221b9f0ap8),   // sb2
    _MM512_SET1_PD8(0x1.802eb189d5118p10),  // sb3
    _MM512_SET1_PD8(0x1.8ffb7688c246ap11),  // sb4
    _MM512_SET1_PD8(0x1.3f219cedf3be6p11),  // sb5
    _MM512_SET1_PD8(0x1.da874e79fe763p8),   // sb6
    _MM512_SET1_PD8(-0x1.670e242712d62p4)   // sb7
},
};

#define SIGN_MASK v8_erfc_data.sign_mask
#define MASK_32   v8_erfc_data.mask_32
#define INF_NAN   v8_erfc_data.inf_nan
#define INF       v8_erfc_data.inf
#define BOUND1    v8_erfc_data.bound1
#define BOUND2    v8_erfc_data.bound2
#define BOUND3    v8_erfc_data.bound3
#define B1_SUB1   v8_erfc_data.b1_sub1
#define B1_SUB2   v8_erfc_data.b1_sub2
#define B3_SUB1   v8_erfc_data.b3_sub1
#define B3_SUB2   v8_erfc_data.b3_sub2

#define TINY      v8_erfc_data.tiny
#define ONE       v8_erfc_data.one
#define TWO       v8_erfc_data.two
#define ZERO      v8_erfc_data.zero
#define ERX       v8_erfc_data.erx
#define EXP_OFFSET v8_erfc_data.exp_offset

/* Polynomial coefficients for |x| < 0.84375 */
#define PP0 v8_erfc_data.poly_bound1[0]
#define PP1 v8_erfc_data.poly_bound1[1]
#define PP2 v8_erfc_data.poly_bound1[2]
#define PP3 v8_erfc_data.poly_bound1[3]
#define PP4 v8_erfc_data.poly_bound1[4]
#define QQ1 v8_erfc_data.poly_bound1[5]
#define QQ2 v8_erfc_data.poly_bound1[6]
#define QQ3 v8_erfc_data.poly_bound1[7]
#define QQ4 v8_erfc_data.poly_bound1[8]
#define QQ5 v8_erfc_data.poly_bound1[9]

/* Polynomial coefficients for 0.84375 <= |x| < 1.25 */
#define PA0 v8_erfc_data.poly_bound2[0]
#define PA1 v8_erfc_data.poly_bound2[1]
#define PA2 v8_erfc_data.poly_bound2[2]
#define PA3 v8_erfc_data.poly_bound2[3]
#define PA4 v8_erfc_data.poly_bound2[4]
#define PA5 v8_erfc_data.poly_bound2[5]
#define PA6 v8_erfc_data.poly_bound2[6]
#define QA1 v8_erfc_data.poly_bound2[7]
#define QA2 v8_erfc_data.poly_bound2[8]
#define QA3 v8_erfc_data.poly_bound2[9]
#define QA4 v8_erfc_data.poly_bound2[10]
#define QA5 v8_erfc_data.poly_bound2[11]
#define QA6 v8_erfc_data.poly_bound2[12]

/* Polynomial coefficients for 1.25 <= |x| < 2.85 */
#define RA0 v8_erfc_data.poly_bound3[0]
#define RA1 v8_erfc_data.poly_bound3[1]
#define RA2 v8_erfc_data.poly_bound3[2]
#define RA3 v8_erfc_data.poly_bound3[3]
#define RA4 v8_erfc_data.poly_bound3[4]
#define RA5 v8_erfc_data.poly_bound3[5]
#define RA6 v8_erfc_data.poly_bound3[6]
#define RA7 v8_erfc_data.poly_bound3[7]
#define SA1 v8_erfc_data.poly_bound3[8]
#define SA2 v8_erfc_data.poly_bound3[9]
#define SA3 v8_erfc_data.poly_bound3[10]
#define SA4 v8_erfc_data.poly_bound3[11]
#define SA5 v8_erfc_data.poly_bound3[12]
#define SA6 v8_erfc_data.poly_bound3[13]
#define SA7 v8_erfc_data.poly_bound3[14]
#define SA8 v8_erfc_data.poly_bound3[15]

/* Polynomial coefficients for 2.85 <= |x| < 28 */
#define RB0 v8_erfc_data.poly_bound4[0]
#define RB1 v8_erfc_data.poly_bound4[1]
#define RB2 v8_erfc_data.poly_bound4[2]
#define RB3 v8_erfc_data.poly_bound4[3]
#define RB4 v8_erfc_data.poly_bound4[4]
#define RB5 v8_erfc_data.poly_bound4[5]
#define RB6 v8_erfc_data.poly_bound4[6]
#define SB1 v8_erfc_data.poly_bound4[7]
#define SB2 v8_erfc_data.poly_bound4[8]
#define SB3 v8_erfc_data.poly_bound4[9]
#define SB4 v8_erfc_data.poly_bound4[10]
#define SB5 v8_erfc_data.poly_bound4[11]
#define SB6 v8_erfc_data.poly_bound4[12]
#define SB7 v8_erfc_data.poly_bound4[13]

#define MASK_32_DP64 _MM512_SET1_U64x8((uint64_t)0x7fffffff00000000ULL)

#define SCALAR_ERFC ALM_PROTO_OPT(erfc)

static inline int test_condition_for_all(v_u64x8_t cond) {
    for(int i = 0; i < 8; i++) {
        if(cond[i] != 0xffffffffffffffff)
            return 0;
    }
    return 1;
}

v_f64x8_t
ALM_PROTO_ARCH_ZN4(vrd8_erfc)(v_f64x8_t _x) {
    v_f64x8_t result;

    /* Get absolute value of inputs and extract sign bit*/
    v_u64x8_t ux = as_v8_u64_f64(_x);
    v_u64x8_t sign = ux & ~SIGN_MASK;
    v_u64x8_t ux_abs = ux & SIGN_MASK;
    v_f64x8_t x = as_v8_f64_u64(ux_abs);

    v_u64x8_t ix = (ux_abs >> 32) & MASK_32;

    __mmask8 sign_mask = _mm512_cmp_pd_mask(_x, ZERO, _CMP_LT_OQ);

    // Check for NaN or Inf
    v_u64x8_t inf_nan_cond = ix >= INF_NAN;
    if(unlikely(test_condition_for_all(inf_nan_cond))) {
        v_u64x8_t inf_cond = ux_abs == INF;
        __mmask8 inf_cond_mask = _mm512_cmp_pd_mask(as_v8_f64_u64(inf_cond), ZERO, _CMP_NEQ_OQ);
    
        v_f64x8_t inf_result = _mm512_mask_blend_pd(sign_mask, ZERO, TWO);
        v_f64x8_t nan_result = _x - _x; // return NaN
        result = _mm512_mask_blend_pd(inf_cond_mask, nan_result, inf_result);
        return result;
    }

    // Range 1: |x| < 0.84375
    v_u64x8_t cond1 = ix < BOUND1;
    if(test_condition_for_all(cond1)) {

        v_u64x8_t sub1_cond = ix < B1_SUB1;
        v_u64x8_t sub2_cond = ix < B1_SUB2;

        // For very small values
        v_f64x8_t small_result1 = ONE - (_x + _x * _x);
        v_f64x8_t small_result2 = ONE - _x;

        v_f64x8_t z = _x * _x;
        v_f64x8_t r = POLY_EVAL_4(z, PP0, PP1, PP2, PP3, PP4);
        v_f64x8_t s = POLY_EVAL_5(z, ONE, QQ1, QQ2, QQ3, QQ4, QQ5);
        v_f64x8_t y = r / s;
        v_f64x8_t poly_result = ONE - (_x + _x * y);

        // Select appropriate result based on sub-conditions
        __mmask8 sub1_mask = _mm512_movepi64_mask((__m512i)sub1_cond);
        result = _mm512_mask_blend_pd(sub1_mask, poly_result, small_result2);
        __mmask8 sub2_mask = _mm512_movepi64_mask((__m512i)sub2_cond);
        result = _mm512_mask_blend_pd(sub2_mask, result, small_result1);
        return result;
    }

    // Range 2: 0.84375 <= |x| < 1.25
    v_u64x8_t cond2 = ix < BOUND2;
    v_u64x8_t cond2a = ix >= BOUND1;
    if(test_condition_for_all(cond2) && test_condition_for_all(cond2a)) {

        v_f64x8_t s = x - ONE;

        v_f64x8_t P = POLY_EVAL_6(s, PA0, PA1, PA2, PA3, PA4, PA5, PA6);
        v_f64x8_t Q = POLY_EVAL_6(s, ONE, QA1, QA2, QA3, QA4, QA5, QA6);

        v_f64x8_t ratio = P / Q;
        v_f64x8_t pos_result = (ONE - ERX) - ratio;
        v_f64x8_t neg_result = ONE + (ERX + ratio);

        result = _mm512_mask_blend_pd(sign_mask, pos_result, neg_result);
        return result;
    }

   // Range 3: 1.25 <= |x| < 28
    v_u64x8_t cond3 = ix < BOUND3;
    v_u64x8_t cond3a = ix >= BOUND2;
    if(test_condition_for_all(cond3) && test_condition_for_all(cond3a)) {

        v_f64x8_t s = ONE / (x * x);
        v_f64x8_t R, S;

        // Check sub-range for different polynomial coefficients
        v_u64x8_t sub_cond = ix < B3_SUB1;
        if(test_condition_for_all(sub_cond)) {
            // |x| < 2.857
            R = POLY_EVAL_7(s, RA0, RA1, RA2, RA3, RA4, RA5, RA6, RA7);
            S = POLY_EVAL_8(s, ONE, SA1, SA2, SA3, SA4, SA5, SA6, SA7, SA8);

        } else {
            // |x| >= 2.857
            v_u64x8_t large_cond = (sign != (v_u64x8_t){0, 0}) & (ix >= B3_SUB2);
            if(test_condition_for_all(large_cond)) {
                return TWO;
            }

            R = POLY_EVAL_6(s, RB0, RB1, RB2, RB3, RB4, RB5, RB6);
            S = POLY_EVAL_7(s, ONE, SB1, SB2, SB3, SB4, SB5, SB6, SB7);
        }

        // High-precision exponential calculation
        // Clear lower 32 bits for higher precision like scalar version
        v_u64x8_t ux_hp = ux_abs & (v_u64x8_t)MASK_32_DP64;
        v_f64x8_t z = as_v8_f64_u64(ux_hp);

        // Two-part exponential calculation for higher accuracy
        v_f64x8_t exp_hi_arg = -(z * z) - EXP_OFFSET;
        v_f64x8_t exp_lo_arg = (z - x) * (z + x) + R / S;

        v_f64x8_t exp_hi = amd_vrd8_exp(exp_hi_arg);
        v_f64x8_t exp_lo = amd_vrd8_exp(exp_lo_arg);
        v_f64x8_t r = exp_hi * exp_lo;

        v_f64x8_t final_result = r / x;
        v_f64x8_t pos_result = final_result;
        v_f64x8_t neg_result = TWO - final_result;

        result = _mm512_mask_blend_pd(sign_mask, pos_result, neg_result);
        return result;
    }

    // Range 4: |x| >= 28 - handle underflow
    v_u64x8_t cond4 = ix >= BOUND3;
    if(test_condition_for_all(cond4)) {
        v_f64x8_t pos_result = TINY * TINY;  // Underflow
        v_f64x8_t neg_result = TWO;
        result = _mm512_mask_blend_pd(sign_mask, pos_result, neg_result);
        return result;
    }

    /* For the remaining cases, where the vector spans more than one region,
       call the scalar function. */
    for(uint64_t i = 0; i < 8; i++) {
        result[i] = SCALAR_ERFC(_x[i]);
    }
    return result;
}