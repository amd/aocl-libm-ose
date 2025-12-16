/*
 * Copyright (C) 2025, Advanced Micro Devices. All rights reserved.
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
 * C implementation of erfcf single precision 512-bit vector version (vrs16)
 *
 * Signature:
 *    v_f32x16_t amd_vrs16_erfcf(v_f32x16_t x)
 *
 * Computes the complementary error function erfcf(x) = 1 - erff(x) for a given input x.
 *
 * SPEC:
 * erfcf(+inf) = 0
 * erfcf(-inf) = 2
 * erfcf(NaN) = NaN
 *
 * Implementation Notes:
 * The function uses polynomial approximations for different ranges of x.
 * 1. For |x| < 0.84375,
 *     erfcf(x) = 1 - (2/sqrt(pi)) * x * Poly
 *     where Poly is a direct polynomial approximation of erfcf
 *
 * 2. For 0.84375 <= |x| < 1.25,
 *     erfcf(x) = (1 - erx) - Poly1(s)/Poly2(s),
 *     where s = |x| - 1,
 *     erx = erf(1) = 0.845062911510467529297,
 *     Poly1 and Poly2 are rational approximations for erfcf
 *
 * 3. For 1.25 <= |x| < 28,
 *     erfcf(x) = exp(-x^2) * Poly1(s) / Poly2(s),
 *     where computation depends on sub-ranges
 *
 * 4. For |x| >= 28,
 *     Results in underflow
 *
 */

#include <stdint.h>
#include <emmintrin.h>

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>
#define AMD_LIBM_FMA_USABLE 1
#include <libm/poly-vec.h>
#include <libm/arch/zen4.h>

static const struct {
    v_u32x16_t   bound1, bound2, bound3;
    v_u32x16_t   b1_sub1, b1_sub2, b3_sub1, b3_sub2;
    v_u32x16_t   sign_mask, split_mask, sign_bit_mask;
    v_u32x16_t   inf_nan;
    v_f32x16_t   tiny, one, two, zero, half, erx;
    v_f32x16_t   poly_bound1[10];
    v_f32x16_t   poly_bound2[13];
    v_f32x16_t   poly_bound3[16];
    v_f32x16_t   poly_bound4[14];
} v_erfcf_data = {
    .bound1      = _MM512_SET1_U32x16(0x3f580000U),  /* 0.84375 */
    .bound2      = _MM512_SET1_U32x16(0x3fa00000U),  /* 1.25 */
    .bound3      = _MM512_SET1_U32x16(0x41e00000U),  /* 28 */
    .b1_sub1     = _MM512_SET1_U32x16(0x32800000U),  /* 2**-26 */
    .b1_sub2     = _MM512_SET1_U32x16(0x3e800000U),  /* 1/4 */
    .b3_sub1     = _MM512_SET1_U32x16(0x4006DB6DU),  /* 1/0.35 ~ 2.857143 */
    .b3_sub2     = _MM512_SET1_U32x16(0x40c00000U),  /* 6 */
    .sign_mask   = _MM512_SET1_U32x16(0x7fffffffU),
    .split_mask  = _MM512_SET1_U32x16(0xffffe000U),
    .sign_bit_mask = _MM512_SET1_U32x16(0x80000000U),
    .inf_nan     = _MM512_SET1_U32x16(0x7f800000U),
    .tiny        = _MM512_SET1_PS16(1e-30f),
    .one         = _MM512_SET1_PS16(1.0f),
    .two         = _MM512_SET1_PS16(2.0f),
    .zero        = _MM512_SET1_PS16(0.0f),
    .half        = _MM512_SET1_PS16(0.5f),
    .erx         = _MM512_SET1_PS16(8.4506291151e-01f),

    .poly_bound1 = {
        _MM512_SET1_PS16(1.2837916613e-01f),   /* pp0 */
        _MM512_SET1_PS16(-3.2504209876e-01f),  /* pp1 */
        _MM512_SET1_PS16(-2.8481749818e-02f),  /* pp2 */
        _MM512_SET1_PS16(-5.7702702470e-03f),  /* pp3 */
        _MM512_SET1_PS16(-2.3763017452e-05f),  /* pp4 */
        _MM512_SET1_PS16(3.9791721106e-01f),   /* qq1 */
        _MM512_SET1_PS16(6.5022252500e-02f),   /* qq2 */
        _MM512_SET1_PS16(5.0813062117e-03f),   /* qq3 */
        _MM512_SET1_PS16(1.3249473704e-04f),   /* qq4 */
        _MM512_SET1_PS16(-3.9602282413e-06f)   /* qq5 */
    },

    .poly_bound2 = {
        _MM512_SET1_PS16(-2.3621185683e-03f),  /* pa0 */
        _MM512_SET1_PS16(4.1485610604e-01f),   /* pa1 */
        _MM512_SET1_PS16(-3.7220788002e-01f),  /* pa2 */
        _MM512_SET1_PS16(3.1834661961e-01f),   /* pa3 */
        _MM512_SET1_PS16(-1.1089469492e-01f),  /* pa4 */
        _MM512_SET1_PS16(3.5478305072e-02f),   /* pa5 */
        _MM512_SET1_PS16(-2.1663755178e-03f),  /* pa6 */
        _MM512_SET1_PS16(1.0642088205e-01f),   /* qa1 */
        _MM512_SET1_PS16(5.4039794207e-01f),   /* qa2 */
        _MM512_SET1_PS16(7.1828655899e-02f),   /* qa3 */
        _MM512_SET1_PS16(1.2617121637e-01f),   /* qa4 */
        _MM512_SET1_PS16(1.3637083583e-02f),   /* qa5 */
        _MM512_SET1_PS16(1.1984500103e-02f)    /* qa6 */
    },

    .poly_bound3 = {
        _MM512_SET1_PS16(-9.8649440333e-03f),  /* ra0 */
        _MM512_SET1_PS16(-6.9385856390e-01f),  /* ra1 */
        _MM512_SET1_PS16(-1.0558626175e+01f),  /* ra2 */
        _MM512_SET1_PS16(-6.2375331879e+01f),  /* ra3 */
        _MM512_SET1_PS16(-1.6239666748e+02f),  /* ra4 */
        _MM512_SET1_PS16(-1.8460508728e+02f),  /* ra5 */
        _MM512_SET1_PS16(-8.1287437439e+01f),  /* ra6 */
        _MM512_SET1_PS16(-9.8143291473e+00f),  /* ra7 */
        _MM512_SET1_PS16(1.9651271820e+01f),   /* sa1 */
        _MM512_SET1_PS16(1.3765776062e+02f),   /* sa2 */
        _MM512_SET1_PS16(4.3456588745e+02f),   /* sa3 */
        _MM512_SET1_PS16(6.4538726807e+02f),   /* sa4 */
        _MM512_SET1_PS16(4.2900814819e+02f),   /* sa5 */
        _MM512_SET1_PS16(1.0863500214e+02f),   /* sa6 */
        _MM512_SET1_PS16(6.5702495575e+00f),   /* sa7 */
        _MM512_SET1_PS16(-6.0424413532e-02f)   /* sa8 */
    },

    .poly_bound4 = {
        _MM512_SET1_PS16(-9.8649431020e-03f),  /* rb0 */
        _MM512_SET1_PS16(-7.9928326607e-01f),  /* rb1 */
        _MM512_SET1_PS16(-1.7757955551e+01f),  /* rb2 */
        _MM512_SET1_PS16(-1.6063638306e+02f),  /* rb3 */
        _MM512_SET1_PS16(-6.3756646729e+02f),  /* rb4 */
        _MM512_SET1_PS16(-1.0250950928e+03f),  /* rb5 */
        _MM512_SET1_PS16(-4.8351919556e+02f),  /* rb6 */
        _MM512_SET1_PS16(3.0338060379e+01f),   /* sb1 */
        _MM512_SET1_PS16(3.2579251099e+02f),   /* sb2 */
        _MM512_SET1_PS16(1.5367296143e+03f),   /* sb3 */
        _MM512_SET1_PS16(3.1998581543e+03f),   /* sb4 */
        _MM512_SET1_PS16(2.5530502930e+03f),   /* sb5 */
        _MM512_SET1_PS16(4.7452853394e+02f),   /* sb6 */
        _MM512_SET1_PS16(-2.2440952301e+01f)   /* sb7 */
    },
};

#define SIGN_MASK v_erfcf_data.sign_mask
#define SPLIT_MASK v_erfcf_data.split_mask
#define SIGN_BIT_MASK v_erfcf_data.sign_bit_mask
#define INF_NAN   v_erfcf_data.inf_nan
#define BOUND1    v_erfcf_data.bound1
#define BOUND2    v_erfcf_data.bound2
#define BOUND3    v_erfcf_data.bound3
#define B1_SUB1   v_erfcf_data.b1_sub1
#define B1_SUB2   v_erfcf_data.b1_sub2
#define B3_SUB1   v_erfcf_data.b3_sub1
#define B3_SUB2   v_erfcf_data.b3_sub2

#define TINY      v_erfcf_data.tiny
#define ONE       v_erfcf_data.one
#define TWO       v_erfcf_data.two
#define ZERO      v_erfcf_data.zero
#define HALF      v_erfcf_data.half
#define ERX       v_erfcf_data.erx

/* Polynomial coefficients for |x| < 0.84375 */
#define PP0 v_erfcf_data.poly_bound1[0]
#define PP1 v_erfcf_data.poly_bound1[1]
#define PP2 v_erfcf_data.poly_bound1[2]
#define PP3 v_erfcf_data.poly_bound1[3]
#define PP4 v_erfcf_data.poly_bound1[4]
#define QQ1 v_erfcf_data.poly_bound1[5]
#define QQ2 v_erfcf_data.poly_bound1[6]
#define QQ3 v_erfcf_data.poly_bound1[7]
#define QQ4 v_erfcf_data.poly_bound1[8]
#define QQ5 v_erfcf_data.poly_bound1[9]

/* Polynomial coefficients for 0.84375 <= |x| < 1.25 */
#define PA0 v_erfcf_data.poly_bound2[0]
#define PA1 v_erfcf_data.poly_bound2[1]
#define PA2 v_erfcf_data.poly_bound2[2]
#define PA3 v_erfcf_data.poly_bound2[3]
#define PA4 v_erfcf_data.poly_bound2[4]
#define PA5 v_erfcf_data.poly_bound2[5]
#define PA6 v_erfcf_data.poly_bound2[6]
#define QA1 v_erfcf_data.poly_bound2[7]
#define QA2 v_erfcf_data.poly_bound2[8]
#define QA3 v_erfcf_data.poly_bound2[9]
#define QA4 v_erfcf_data.poly_bound2[10]
#define QA5 v_erfcf_data.poly_bound2[11]
#define QA6 v_erfcf_data.poly_bound2[12]

/* Polynomial coefficients for 1.25 <= |x| < 2.857143 */
#define RA0 v_erfcf_data.poly_bound3[0]
#define RA1 v_erfcf_data.poly_bound3[1]
#define RA2 v_erfcf_data.poly_bound3[2]
#define RA3 v_erfcf_data.poly_bound3[3]
#define RA4 v_erfcf_data.poly_bound3[4]
#define RA5 v_erfcf_data.poly_bound3[5]
#define RA6 v_erfcf_data.poly_bound3[6]
#define RA7 v_erfcf_data.poly_bound3[7]
#define SA1 v_erfcf_data.poly_bound3[8]
#define SA2 v_erfcf_data.poly_bound3[9]
#define SA3 v_erfcf_data.poly_bound3[10]
#define SA4 v_erfcf_data.poly_bound3[11]
#define SA5 v_erfcf_data.poly_bound3[12]
#define SA6 v_erfcf_data.poly_bound3[13]
#define SA7 v_erfcf_data.poly_bound3[14]
#define SA8 v_erfcf_data.poly_bound3[15]

/* Polynomial coefficients for 2.857143 <= |x| < 28 */
#define RB0 v_erfcf_data.poly_bound4[0]
#define RB1 v_erfcf_data.poly_bound4[1]
#define RB2 v_erfcf_data.poly_bound4[2]
#define RB3 v_erfcf_data.poly_bound4[3]
#define RB4 v_erfcf_data.poly_bound4[4]
#define RB5 v_erfcf_data.poly_bound4[5]
#define RB6 v_erfcf_data.poly_bound4[6]
#define SB1 v_erfcf_data.poly_bound4[7]
#define SB2 v_erfcf_data.poly_bound4[8]
#define SB3 v_erfcf_data.poly_bound4[9]
#define SB4 v_erfcf_data.poly_bound4[10]
#define SB5 v_erfcf_data.poly_bound4[11]
#define SB6 v_erfcf_data.poly_bound4[12]
#define SB7 v_erfcf_data.poly_bound4[13]

#define SCALAR_ERFCF ALM_PROTO_OPT(erfcf)

static inline int test_condition_for_all_v16(v_u32x16_t cond) {
    for(int i = 0; i < 16; i++) {
        if(cond[i] != 0xffffffff)
            return 0;
    }
    return 1;
}

v_f32x16_t
ALM_PROTO_ARCH_ZN4(vrs16_erfcf)(v_f32x16_t _x) {
    v_f32x16_t result;

    /* Get absolute value and sign information */
    v_u32x16_t hx = as_v16_u32_f32(_x);
    v_u32x16_t ix = hx & SIGN_MASK;
    v_f32x16_t x_abs = as_v16_f32_u32(ix);

    // Check for NaN or Inf
    v_u32x16_t inf_nan_cond = ix >= INF_NAN;
    if(unlikely(any_v16_u32_loop(inf_nan_cond))) {
        v_u32x16_t inf_cond = ix == INF_NAN;
        v_u32x16_t sign_bit = hx >> 31;
        v_f32x16_t inf_result = as_v16_f32_u32((sign_bit << 1) + as_v16_u32_f32(ONE));
        v_f32x16_t nan_result = _x + _x; // NaN propagation
        __mmask16 inf_mask = _mm512_cmp_epi32_mask(inf_cond, _mm512_set1_epi32(-1), _MM_CMPINT_EQ);
        result = _mm512_mask_blend_ps(inf_mask, nan_result, inf_result);
        return result;
    }

    // Range 1: |x| < 0.84375
    v_u32x16_t cond1 = ix < BOUND1;
    if(test_condition_for_all_v16(cond1)) {
        v_u32x16_t sub1_cond = ix < B1_SUB1;
        v_u32x16_t sub2_cond = hx < B1_SUB2; // Note: using hx (not ix) to check sign
        
        // For very small values: return 1 - x
        v_f32x16_t small_result = ONE - _x;
        
        v_f32x16_t z = _x * _x;
        v_f32x16_t r = POLY_EVAL_4(z, PP0, PP1, PP2, PP3, PP4);
        v_f32x16_t s = POLY_EVAL_5(z, ONE, QQ1, QQ2, QQ3, QQ4, QQ5);
        v_f32x16_t y = r / s;
        
        // For x < 1/4: return 1 - (x + x*y)
        v_f32x16_t path1_result = ONE - (_x + _x * y);
        
        // For x >= 1/4: return 0.5 - r where r = x*y + (x - 0.5)
        v_f32x16_t r_temp = _x * y + (_x - HALF);
        v_f32x16_t path2_result = HALF - r_temp;
        
        // Select based on conditions
        __mmask16 sub2_mask = _mm512_cmp_epi32_mask(sub2_cond, _mm512_set1_epi32(-1), _MM_CMPINT_EQ);
        __mmask16 sub1_mask = _mm512_cmp_epi32_mask(sub1_cond, _mm512_set1_epi32(-1), _MM_CMPINT_EQ);
        result = _mm512_mask_blend_ps(sub2_mask, path2_result, path1_result);
        result = _mm512_mask_blend_ps(sub1_mask, result, small_result);
        return result;
    }

    // Range 2: 0.84375 <= |x| < 1.25
    v_u32x16_t cond2 = ix < BOUND2;
    v_u32x16_t cond2a = ix >= BOUND1;
    if(test_condition_for_all_v16(cond2) && test_condition_for_all_v16(cond2a)) {
        v_f32x16_t s = x_abs - ONE;
        
        v_f32x16_t P = POLY_EVAL_6(s, PA0, PA1, PA2, PA3, PA4, PA5, PA6);
        v_f32x16_t Q = POLY_EVAL_6(s, ONE, QA1, QA2, QA3, QA4, QA5, QA6);
        
        v_u32x16_t pos_cond = ~(hx & SIGN_BIT_MASK); // Check if positive
        v_f32x16_t pos_result = (ONE - ERX) - P / Q;
        v_f32x16_t neg_result = ONE + (ERX + P / Q);
        
        __mmask16 pos_mask = _mm512_cmp_epi32_mask(pos_cond, _mm512_set1_epi32(-1), _MM_CMPINT_EQ);
        result = _mm512_mask_blend_ps(pos_mask, neg_result, pos_result);
        return result;
    }

    // Range 3: 1.25 <= |x| < 28
    v_u32x16_t cond3 = ix < BOUND3;
    v_u32x16_t cond3a = ix >= BOUND2;
    if(test_condition_for_all_v16(cond3) && test_condition_for_all_v16(cond3a)) {
        v_f32x16_t s = ONE / (x_abs * x_abs);
        v_f32x16_t R, S;
        
        // Check sub-range for different polynomial coefficients
        v_u32x16_t sub_cond = ix < B3_SUB1;
        if(test_condition_for_all_v16(sub_cond)) {
            // |x| < 2.857143
            R = POLY_EVAL_7(s, RA0, RA1, RA2, RA3, RA4, RA5, RA6, RA7);
            S = POLY_EVAL_8(s, ONE, SA1, SA2, SA3, SA4, SA5, SA6, SA7, SA8);
        } else {
            // |x| >= 2.857143
            v_u32x16_t neg_cond = hx & SIGN_BIT_MASK;
            v_u32x16_t large_cond = neg_cond & (ix >= B3_SUB2);
            if(test_condition_for_all_v16(large_cond)) {
                return TWO - TINY;
            }
            
            R = POLY_EVAL_6(s, RB0, RB1, RB2, RB3, RB4, RB5, RB6);
            S = POLY_EVAL_7(s, ONE, SB1, SB2, SB3, SB4, SB5, SB6, SB7);
        }
        
        // High-precision exponential calculation matching scalar implementation
        v_u32x16_t x_hp_bits = ix & SPLIT_MASK;
        v_f32x16_t z = as_v16_f32_u32(x_hp_bits);
        
        // Two-part exponential calculation for higher accuracy
        v_f32x16_t exp_hi_arg = -(z * z) - _MM512_SET1_PS16(0.5625f);
        v_f32x16_t exp_lo_arg = (z - x_abs) * (z + x_abs) + R / S;
        
        v_f32x16_t exp_hi = amd_vrs16_expf(exp_hi_arg);
        v_f32x16_t exp_lo = amd_vrs16_expf(exp_lo_arg);
        v_f32x16_t r = exp_hi * exp_lo;
        
        v_f32x16_t final_result = r / x_abs;
        
        v_u32x16_t pos_cond = ~(hx & SIGN_BIT_MASK); // Check if positive
        v_f32x16_t pos_result = final_result;
        v_f32x16_t neg_result = TWO - final_result;
        
        __mmask16 pos_mask = _mm512_cmp_epi32_mask(pos_cond, _mm512_set1_epi32(-1), _MM_CMPINT_EQ);
        result = _mm512_mask_blend_ps(pos_mask, neg_result, pos_result);
        return result;
    }

    // Range 4: |x| >= 28 - handle underflow
    v_u32x16_t cond4 = ix >= BOUND3;
    if(test_condition_for_all_v16(cond4)) {
        v_u32x16_t pos_cond = ~(hx & SIGN_BIT_MASK); // Check if positive
        v_f32x16_t pos_result = TINY * TINY;  // Underflow
        v_f32x16_t neg_result = TWO - TINY;
        __mmask16 pos_mask = _mm512_cmp_epi32_mask(pos_cond, _mm512_set1_epi32(-1), _MM_CMPINT_EQ);
        result = _mm512_mask_blend_ps(pos_mask, neg_result, pos_result);
        return result;
    }

    /* For the remaining cases, where the vector spans more than one region,
       call the scalar function. */
    for(uint32_t i = 0; i < 16; i++) {
        result[i] = SCALAR_ERFCF(_x[i]);
    }
    return result;
}