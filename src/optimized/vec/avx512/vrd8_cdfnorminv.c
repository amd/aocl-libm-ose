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
 * C implementation of cdfnorminv double precision 512-bit vector version (vrd8)
 *
 * Signature:
 *    v_f64x8_t amd_vrd8_cdfnorminv(v_f64x8_t x)
 *
 * Computes the inverse standard normal CDF for input x in domain (0, 1).
 *
 * SPEC:
 * cdfnorminv(0) = -inf
 * cdfnorminv(1) = +inf
 * cdfnorminv(NaN) = NaN
 * cdfnorminv(x < 0) = -QNaN
 * cdfnorminv(x > 1) = -QNaN
 *
 * Implementation Notes:
 * Uses Wichura's AS241 algorithm. Regions defined by q = x - 0.5:
 *
 * 1. Central region (|q| <= 0.425, i.e., x in [0.075, 0.925]):            [Vector]
 *     cdfnorminv(x) = q * P(r)/Q(r), where r = 0.180625 - q^2
 * 2. Tail region (r <= 5.0, where r = sqrt(-log(min(x, 1-x)))):           [Vector]
 *     cdfnorminv(x) = P(r-1.6)/Q(r-1.6), negated if x < 0.5
 * 3. Extreme tail (r > 5.0, subnormal inputs):                            [Scalar fallback]
 *     Falls back to scalar for accuracy
 * 4. Mixed ranges:                                                        [Scalar fallback]
 *     Calls scalar cdfnorminv for each element
 */

#include <stdint.h>
#include <immintrin.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly-vec.h>

/* Local macros for 8-element vectors */
#define V8_SET1_U64(x) {(x), (x), (x), (x), (x), (x), (x), (x)}
#define V8_SET1_F64(x) {(x), (x), (x), (x), (x), (x), (x), (x)}

static const struct {
    v_u64x8_t   one_u, zero_u;
    v_u64x8_t   sign_mask;
    v_u64x8_t   pos_inf, neg_inf, neg_qnan;
    v_f64x8_t   one, half;
    v_f64x8_t   bound_1;       /* 0.425 */
    v_f64x8_t   bound_2;       /* 5.0 */
    v_f64x8_t   offset_1;      /* 0.180625 */
    v_f64x8_t   offset_2;      /* 1.6 */
    v_f64x8_t   poly_bound_1[16];    /* 8 num + 8 den - central region */
    v_f64x8_t   poly_bound_1_H[16];  /* Head parts */
    v_f64x8_t   poly_bound_1_T[16];  /* Tail parts */
    v_f64x8_t   poly_bound_2[16];    /* 8 num + 8 den - tail region */
} v8_cdfnorminv_data = {
    .one_u       = V8_SET1_U64(0x3FF0000000000000),  /* 1.0 */
    .zero_u      = V8_SET1_U64(0x0000000000000000),  /* 0.0 */
    .sign_mask   = V8_SET1_U64(0x7FFFFFFFFFFFFFFF),
    .pos_inf     = V8_SET1_U64(0x7FF0000000000000),
    .neg_inf     = V8_SET1_U64(0xFFF0000000000000),
    .neg_qnan    = V8_SET1_U64(0xFFF8000000000000),
    .one         = V8_SET1_F64(1.0),
    .half        = V8_SET1_F64(0.5),
    .bound_1     = V8_SET1_F64(0x1.b333333333333p-2),  /* 0.425 */
    .bound_2     = V8_SET1_F64(0x1.4p+2),              /* 5.0 */
    .offset_1    = V8_SET1_F64(0x1.71eb851eb851fp-3),  /* 0.180625 */
    .offset_2    = V8_SET1_F64(0x1.999999999999ap+0),  /* 1.6 */

    /* Central region coefficients (|q| <= 0.425) - Wichura AS241 */
    .poly_bound_1 = {
        /* Numerator P10-P17 */
        V8_SET1_F64(0x1.b18d91e9eef75p+1),
        V8_SET1_F64(0x1.0a4888b1a436ep+7),
        V8_SET1_F64(0x1.ece5d2213c0ccp+10),
        V8_SET1_F64(0x1.ad1d8cd4ee71dp+13),
        V8_SET1_F64(0x1.66c3e869b752ap+15),
        V8_SET1_F64(0x1.06c1c55b78f2p+16),
        V8_SET1_F64(0x1.052d26b2e45e4p+15),
        V8_SET1_F64(0x1.39a296f7d925ep+11),
        /* Denominator Q10-Q17 */
        V8_SET1_F64(0x1p+0),
        V8_SET1_F64(0x1.5281b386e1ab5p+5),
        V8_SET1_F64(0x1.5797efdc8b3f7p+9),
        V8_SET1_F64(0x1.512322e75c89fp+12),
        V8_SET1_F64(0x1.4b772d5d65266p+14),
        V8_SET1_F64(0x1.3317caa64f4bep+15),
        V8_SET1_F64(0x1.c0e457cb1ae76p+14),
        V8_SET1_F64(0x1.46a7eca984b69p+12)
    },

    .poly_bound_1_H = {
        /* Numerator Head */
        V8_SET1_F64(0x1.b18d918p+1),
        V8_SET1_F64(0x1.0a48888p+7),
        V8_SET1_F64(0x1.ece5d2p+10),
        V8_SET1_F64(0x1.ad1d8c8p+13),
        V8_SET1_F64(0x1.66c3e8p+15),
        V8_SET1_F64(0x1.06c1c5p+16),
        V8_SET1_F64(0x1.052d268p+15),
        V8_SET1_F64(0x1.39a2968p+11),
        /* Denominator Head */
        V8_SET1_F64(0x1p+0),
        V8_SET1_F64(0x1.5281b38p+5),
        V8_SET1_F64(0x1.5797ef8p+9),
        V8_SET1_F64(0x1.5123228p+12),
        V8_SET1_F64(0x1.4b772dp+14),
        V8_SET1_F64(0x1.3317ca8p+15),
        V8_SET1_F64(0x1.c0e4578p+14),
        V8_SET1_F64(0x1.46a7ec8p+12)
    },

    .poly_bound_1_T = {
        /* Numerator Tail */
        V8_SET1_F64(0x1.a7bbdd4p-25),
        V8_SET1_F64(0x1.8d21b7p-20),
        V8_SET1_F64(0x1.09e066p-17),
        V8_SET1_F64(0x1.53b9c74p-13),
        V8_SET1_F64(0x1.a6dd4a8p-11),
        V8_SET1_F64(0x1.6de3c8p-10),
        V8_SET1_F64(0x1.9722f2p-12),
        V8_SET1_F64(0x1.df64978p-15),
        /* Denominator Tail */
        V8_SET1_F64(0x0p+0),
        V8_SET1_F64(0x1.b86ad4p-25),
        V8_SET1_F64(0x1.722cfdcp-17),
        V8_SET1_F64(0x1.9d7227cp-14),
        V8_SET1_F64(0x1.7594998p-12),
        V8_SET1_F64(0x1.327a5fp-12),
        V8_SET1_F64(0x1.2c6b9d8p-12),
        V8_SET1_F64(0x1.4c25b48p-15)
    },

    /* Tail region coefficients (r <= 5.0) - Wichura AS241 */
    .poly_bound_2 = {
        /* Numerator P20-P27 */
        V8_SET1_F64(0x1.6c665fde9526ap+0),
        V8_SET1_F64(0x1.2857748cab19bp+2),
        V8_SET1_F64(0x1.713f71462256ap+2),
        V8_SET1_F64(0x1.d2ecb1a3d02c4p+1),
        V8_SET1_F64(0x1.453cc085375b2p+0),
        V8_SET1_F64(0x1.ef2abb9b85c37p-3),
        V8_SET1_F64(0x1.744eb6c45ec67p-6),
        V8_SET1_F64(0x1.9615ac0b7ace9p-11),
        /* Denominator Q20-Q27 */
        V8_SET1_F64(0x1p+0),
        V8_SET1_F64(0x1.06cefbb46a449p+1),
        V8_SET1_F64(0x1.ad278e6526633p+0),
        V8_SET1_F64(0x1.61292f23385c9p-1),
        V8_SET1_F64(0x1.2f5123394f04p-3),
        V8_SET1_F64(0x1.f207a7eab17bfp-7),
        V8_SET1_F64(0x1.1f18cbfdf2728p-11),
        V8_SET1_F64(0x1.20d3f686439e4p-30)
    }
};

#define ONE_U      v8_cdfnorminv_data.one_u
#define ZERO_U     v8_cdfnorminv_data.zero_u
#define SIGN_MASK  v8_cdfnorminv_data.sign_mask
#define POS_INF    v8_cdfnorminv_data.pos_inf
#define NEG_INF    v8_cdfnorminv_data.neg_inf
#define NEG_QNAN   v8_cdfnorminv_data.neg_qnan
#define ONE        v8_cdfnorminv_data.one
#define HALF       v8_cdfnorminv_data.half
#define BOUND_1    v8_cdfnorminv_data.bound_1
#define BOUND_2    v8_cdfnorminv_data.bound_2
#define OFFSET_1   v8_cdfnorminv_data.offset_1
#define OFFSET_2   v8_cdfnorminv_data.offset_2

/* Central region polynomial coefficients */
#define P10  v8_cdfnorminv_data.poly_bound_1[0]
#define P11  v8_cdfnorminv_data.poly_bound_1[1]
#define P12  v8_cdfnorminv_data.poly_bound_1[2]
#define P13  v8_cdfnorminv_data.poly_bound_1[3]
#define P14  v8_cdfnorminv_data.poly_bound_1[4]
#define P15  v8_cdfnorminv_data.poly_bound_1[5]
#define P16  v8_cdfnorminv_data.poly_bound_1[6]
#define P17  v8_cdfnorminv_data.poly_bound_1[7]
#define Q10  v8_cdfnorminv_data.poly_bound_1[8]
#define Q11  v8_cdfnorminv_data.poly_bound_1[9]
#define Q12  v8_cdfnorminv_data.poly_bound_1[10]
#define Q13  v8_cdfnorminv_data.poly_bound_1[11]
#define Q14  v8_cdfnorminv_data.poly_bound_1[12]
#define Q15  v8_cdfnorminv_data.poly_bound_1[13]
#define Q16  v8_cdfnorminv_data.poly_bound_1[14]
#define Q17  v8_cdfnorminv_data.poly_bound_1[15]

#define P10H v8_cdfnorminv_data.poly_bound_1_H[0]
#define P11H v8_cdfnorminv_data.poly_bound_1_H[1]
#define P12H v8_cdfnorminv_data.poly_bound_1_H[2]
#define P13H v8_cdfnorminv_data.poly_bound_1_H[3]
#define P14H v8_cdfnorminv_data.poly_bound_1_H[4]
#define P15H v8_cdfnorminv_data.poly_bound_1_H[5]
#define P16H v8_cdfnorminv_data.poly_bound_1_H[6]
#define P17H v8_cdfnorminv_data.poly_bound_1_H[7]
#define Q10H v8_cdfnorminv_data.poly_bound_1_H[8]
#define Q11H v8_cdfnorminv_data.poly_bound_1_H[9]
#define Q12H v8_cdfnorminv_data.poly_bound_1_H[10]
#define Q13H v8_cdfnorminv_data.poly_bound_1_H[11]
#define Q14H v8_cdfnorminv_data.poly_bound_1_H[12]
#define Q15H v8_cdfnorminv_data.poly_bound_1_H[13]
#define Q16H v8_cdfnorminv_data.poly_bound_1_H[14]
#define Q17H v8_cdfnorminv_data.poly_bound_1_H[15]

#define P10T v8_cdfnorminv_data.poly_bound_1_T[0]
#define P11T v8_cdfnorminv_data.poly_bound_1_T[1]
#define P12T v8_cdfnorminv_data.poly_bound_1_T[2]
#define P13T v8_cdfnorminv_data.poly_bound_1_T[3]
#define P14T v8_cdfnorminv_data.poly_bound_1_T[4]
#define P15T v8_cdfnorminv_data.poly_bound_1_T[5]
#define P16T v8_cdfnorminv_data.poly_bound_1_T[6]
#define P17T v8_cdfnorminv_data.poly_bound_1_T[7]
#define Q10T v8_cdfnorminv_data.poly_bound_1_T[8]
#define Q11T v8_cdfnorminv_data.poly_bound_1_T[9]
#define Q12T v8_cdfnorminv_data.poly_bound_1_T[10]
#define Q13T v8_cdfnorminv_data.poly_bound_1_T[11]
#define Q14T v8_cdfnorminv_data.poly_bound_1_T[12]
#define Q15T v8_cdfnorminv_data.poly_bound_1_T[13]
#define Q16T v8_cdfnorminv_data.poly_bound_1_T[14]
#define Q17T v8_cdfnorminv_data.poly_bound_1_T[15]

/* Tail region polynomial coefficients */
#define P20  v8_cdfnorminv_data.poly_bound_2[0]
#define P21  v8_cdfnorminv_data.poly_bound_2[1]
#define P22  v8_cdfnorminv_data.poly_bound_2[2]
#define P23  v8_cdfnorminv_data.poly_bound_2[3]
#define P24  v8_cdfnorminv_data.poly_bound_2[4]
#define P25  v8_cdfnorminv_data.poly_bound_2[5]
#define P26  v8_cdfnorminv_data.poly_bound_2[6]
#define P27  v8_cdfnorminv_data.poly_bound_2[7]
#define Q20  v8_cdfnorminv_data.poly_bound_2[8]
#define Q21  v8_cdfnorminv_data.poly_bound_2[9]
#define Q22  v8_cdfnorminv_data.poly_bound_2[10]
#define Q23  v8_cdfnorminv_data.poly_bound_2[11]
#define Q24  v8_cdfnorminv_data.poly_bound_2[12]
#define Q25  v8_cdfnorminv_data.poly_bound_2[13]
#define Q26  v8_cdfnorminv_data.poly_bound_2[14]
#define Q27  v8_cdfnorminv_data.poly_bound_2[15]

#define SCALAR_CDFNORMINV ALM_PROTO_OPT(cdfnorminv)

static inline int test_condition_for_all(v_u64x8_t cond) {
    return _mm512_movepi64_mask((__m512i)cond) == (__mmask8)0xFF;
}

/* Convert mask from comparison result to __mmask8 for AVX-512 blend */
static inline __mmask8 to_mask8(v_u64x8_t cond) {
    return _mm512_cmpneq_epi64_mask((__m512i)cond, (__m512i)ZERO_U);
}

v_f64x8_t
ALM_PROTO_OPT(vrd8_cdfnorminv)(v_f64x8_t _x) {
    v_f64x8_t result;
    v_u64x8_t zero_u64 = {0, 0, 0, 0, 0, 0, 0, 0};

    v_u64x8_t ux = as_v8_u64_f64(_x);
    v_u64x8_t sign = ux & ~SIGN_MASK;
    v_u64x8_t ux_abs = ux & SIGN_MASK;

    /* Special cases: NaN, outside domain [0,1] */
    v_u64x8_t valid_cond = (sign == zero_u64) & (ux_abs > ZERO_U) & (ux_abs < ONE_U);
    v_u64x8_t special_cond = ~valid_cond;

    if (unlikely(test_condition_for_all(special_cond))) {
        /* All elements need special handling */
        v_u64x8_t one_cond = (ux == ONE_U);
        v_u64x8_t zero_cond = (ux_abs == ZERO_U);
        v_u64x8_t nan_cond = (ux_abs > POS_INF);
        v_u64x8_t oob_cond = (sign != zero_u64) | (ux_abs > ONE_U);

        v_f64x8_t pos_inf_result = as_v8_f64_u64(POS_INF);
        v_f64x8_t neg_inf_result = as_v8_f64_u64(NEG_INF);
        v_f64x8_t nan_result = _x - _x;  /* Propagate NaN */
        v_f64x8_t qnan_result = as_v8_f64_u64(NEG_QNAN);

        /* Apply in reverse priority order */
        result = _mm512_mask_blend_pd(to_mask8(oob_cond), _x, qnan_result);
        result = _mm512_mask_blend_pd(to_mask8(nan_cond), result, nan_result);
        result = _mm512_mask_blend_pd(to_mask8(zero_cond), result, neg_inf_result);
        result = _mm512_mask_blend_pd(to_mask8(one_cond), result, pos_inf_result);
        return result;
    }

    /* Compute q = x - 0.5 and check if in central region */
    v_f64x8_t q = _x - HALF;
    v_f64x8_t q_abs = as_v8_f64_u64(as_v8_u64_f64(q) & SIGN_MASK);

    /* Central region: |q| <= 0.425 and valid input */
    v_u64x8_t central_cond = (as_v8_u64_f64(q_abs) <= as_v8_u64_f64(BOUND_1)) & valid_cond;

    if (test_condition_for_all(central_cond)) {
        /* All elements in central region */
        v_f64x8_t r = OFFSET_1 - q * q;
        v_f64x8_t P = POLY_EVAL_ESTRIN_8_TAIL_2(r,
            P10H, P10T, P11H, P11T,
            P12, P13, P14, P15, P16, P17);
        v_f64x8_t Q = POLY_EVAL_ESTRIN_8_TAIL_2(r,
            Q10H, Q10T, Q11H, Q11T,
            Q12, Q13, Q14, Q15, Q16, Q17);
        return q * (P / Q);
    }

    /* Tail regions: need to compute r = sqrt(-log(min(x, 1-x))) */
    v_u64x8_t tail_cond = ~central_cond & valid_cond;

    if (test_condition_for_all(tail_cond)) {
        /* All elements in tail region */
        /* sign_q != 0 means q < 0, i.e., x < 0.5, so use x; otherwise use 1-x */
        v_u64x8_t sign_q = as_v8_u64_f64(q) & ~SIGN_MASK;
        v_f64x8_t min_x = _mm512_mask_blend_pd(to_mask8(sign_q), ONE - _x, _x);
        v_f64x8_t r = ALM_PROTO(vrd8_sqrt)(-ALM_PROTO(vrd8_log)(min_x));

        /* Check if tail (r <= 5.0) or extreme tail (r > 5.0) */
        v_u64x8_t tail_normal = (as_v8_u64_f64(r) <= as_v8_u64_f64(BOUND_2));

        if (test_condition_for_all(tail_normal)) {
            /* All in normal tail (r <= 5.0) */
            r = r - OFFSET_2;
            v_f64x8_t P = POLY_EVAL_HORNER_8(r, P20, P21, P22, P23, P24, P25, P26, P27);
            v_f64x8_t Q = POLY_EVAL_HORNER_8(r, Q20, Q21, Q22, Q23, Q24, Q25, Q26, Q27);
            v_f64x8_t val = P / Q;
            /* Negate if x < 0.5 (sign_q has MSB set) */
            return _mm512_mask_blend_pd(to_mask8(sign_q), val, -val);
        }

        /* Extreme tail (r > 5.0) falls through to scalar as ALM_PROTO(vrd8_log)
         * isn't accurate enough for subnormals and errors get amplified with further computation. */
    }

    /* Mixed ranges fall back to scalar */
    for (uint64_t i = 0; i < 8; i++) {
        result[i] = SCALAR_CDFNORMINV(_x[i]);
    }
    return result;
}
