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
 * C implementation of erfcinv double precision 512-bit vector version (vrd8)
 *
 * Signature:
 *    v_f64x8_t amd_vrd8_erfcinv(v_f64x8_t x)
 *
 * Computes the inverse complementary error function erfcinv(x) = erfinv(1-x) for a given input x.
 *
 * SPEC:
 * erfcinv(0) = +inf
 * erfcinv(2) = -inf
 * erfcinv(NaN) = NaN
 *
 * Implementation Notes:
 * Ranges defined by x and a = |1-x|:
 *
 * 1. For x in [0, 1e-100],                                              [Scalar fallback]
 *     erfcinv(x) = 1/z * P(z)/Q(z), where z = 1/sqrt(-ln(x))
 * 2. For x in (1e-100, 0.0625] OR (1.9375, 2),                          [Vector, combined]
 *     Range1: erfcinv, z = 1/sqrt(-ln(x)), result = P/Q
 *     Range4: erfinv,  z = 1/sqrt(-ln(2-x)), result = -P/Q
 *     (Table 58 Blair et al for both)
 * 3. For x in [0.25, 1.75] (a <= 0.75),                                 [Vector]
 *     erfinv(1-x) = z * P(s)/Q(s), where s = z^2 - 0.5625, z = 1-x
 *     (Table 22 Blair et al)
 * 4. For x in (0.0625, 0.25) OR (1.75, 1.9375] (0.75 < a <= 0.9375),   [Vector]
 *     erfinv(1-x) = z * P(s)/Q(s), where s = z^2 - 0.87890625, z = 1-x
 *     (Table 37 Blair et al)
 * 5. For mixed ranges (except Range1+Range4 mix),                       [Scalar fallback]
 *     calls scalar erfcinv for each element
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
#include <libm/arch/zen4.h>

/* Local macros for 8-element vectors */
#define V8_SET1_U64(x) {(x), (x), (x), (x), (x), (x), (x), (x)}
#define V8_SET1_F64(x) {(x), (x), (x), (x), (x), (x), (x), (x)}

static const struct {
    v_u64x8_t   bound1, bound2, bound3, bound4, b1_sub1;
    v_u64x8_t   sign_mask;
    v_u64x8_t   inf_nan, inf, neg_qnan;
    v_f64x8_t   one, two, zero, exp_offset3, exp_offset4;
    v_f64x8_t   poly_bound2_H[19];  /* 11 num + 8 den, Table 58 Blair et al */
    v_f64x8_t   poly_bound2_T[4];   /* Tail coefficients */
    v_f64x8_t   poly_bound3[18];    /* 9 num + 9 den, Table 22 Blair et al */
    v_f64x8_t   poly_bound4_H[15];  /* 8 num + 7 den, Table 37 Blair et al */
    v_f64x8_t   poly_bound4_T[4];   /* Tail coefficients */
} v8_erfcinv_data = {
     .bound1      = V8_SET1_U64(0x3FB0000000000000),  /* 0.0625 */
     .bound2      = V8_SET1_U64(0x3FE8000000000000),  /* 0.75 */
     .bound3      = V8_SET1_U64(0x3FEE000000000000),  /* 0.9375 */
     .bound4      = V8_SET1_U64(0x4000000000000000),  /* 2.0 */
     .b1_sub1     = V8_SET1_U64(0x2B2BFF2EE48E0530),  /* 1e-100 */
     .sign_mask   = V8_SET1_U64(0x7FFFFFFFFFFFFFFF),
     .inf_nan     = V8_SET1_U64(0x7FF0000000000000),
     .inf         = V8_SET1_U64(0x7FF0000000000000),
     .neg_qnan    = V8_SET1_U64(0xFFF8000000000000),
     .one         = V8_SET1_F64(1.0),
     .two         = V8_SET1_F64(2.0),
     .zero        = V8_SET1_F64(0.0),
     .exp_offset3 = V8_SET1_F64(0x1.2p-1),
     .exp_offset4 = V8_SET1_F64(0.87890625),

    /* poly_bound2_H: 19 elements (11 num + 8 den) - Table 58 Blair et al */
    .poly_bound2_H = {
        /* Numerator P20H-P210H */
        V8_SET1_F64(0x1.d98db2f393c93p-15),
        V8_SET1_F64(0x1.5ea5105db7d1bp-8),
        V8_SET1_F64(0x1.099dcb79c5e37p-3),
        V8_SET1_F64(0x1.0ae8df6736f5ap+0),
        V8_SET1_F64(0x1.6a463706dce53p+1),
        V8_SET1_F64(0x1.501296c195ce7p+1),
        V8_SET1_F64(0x1.0a1bd4227162ep+1),
        V8_SET1_F64(0x1.7451fe635fbb3p-1),
        V8_SET1_F64(0x1.11ae803f200b1p-4),
        V8_SET1_F64(-0x1.237ce1b409b07p-6),
        V8_SET1_F64(0x1.25db922abee60p-9),
        /* Denominator Q20H-Q27H */
        V8_SET1_F64(0x1.d98d1a3412e13p-15),
        V8_SET1_F64(0x1.5ea77aa937936p-8),
        V8_SET1_F64(0x1.09f744281bf43p-3),
        V8_SET1_F64(0x1.0de629224eb8ap+0),
        V8_SET1_F64(0x1.84dafe0d3b477p+1),
        V8_SET1_F64(0x1.e1add024b4c8dp+1),
        V8_SET1_F64(0x1.f06bab8543d1ap+1),
        V8_SET1_F64(0x1.04c46273c9ec0p+1)
    },

    /* poly_bound2_T: Tail coefficients (Table 58 Blair et al.) */
    .poly_bound2_T = {
        V8_SET1_F64(-0x1.f4222b6157b42p-70),  /* P20T */
        V8_SET1_F64(-0x1.de62daca659d5p-62),  /* P21T */
        V8_SET1_F64(-0x1.2d837f1682e6ap-70),  /* Q20T */
        V8_SET1_F64(0x1.5fbb45bed5641p-62)    /* Q21T */
    },

    /* poly_bound3: 18 elements (9 num + 9 den) - Table 22 Blair et al */
    .poly_bound3 = {
        /* Numerator P30-P38 */
        V8_SET1_F64(-0x1.7360798246422p+10),
        V8_SET1_F64(0x1.7e030587e98dbp+13),
        V8_SET1_F64(-0x1.3d0fcbb7f8c46p+15),
        V8_SET1_F64(0x1.107dd185aef42p+16),
        V8_SET1_F64(-0x1.02f03af1b371ep+16),
        V8_SET1_F64(0x1.0d3ff4261257ap+15),
        V8_SET1_F64(-0x1.1c278c101bc75p+13),
        V8_SET1_F64(0x1.fed4e855def62p+9),
        V8_SET1_F64(-0x1.f8ca50679dd18p+4),
        /* Denominator Q30-Q38 */
        V8_SET1_F64(-0x1.566bf73b936f7p+10),
        V8_SET1_F64(0x1.766e6c8610465p+13),
        V8_SET1_F64(-0x1.4e45ce6f4050ep+15),
        V8_SET1_F64(0x1.39f93b45f1adfp+16),
        V8_SET1_F64(-0x1.4d7ca13cef74dp+16),
        V8_SET1_F64(0x1.90cdbc53c0b15p+15),
        V8_SET1_F64(-0x1.02180fb6327f8p+14),
        V8_SET1_F64(0x1.394139cc04100p+11),
        V8_SET1_F64(-0x1.0cfb9cdf1ef00p+7)
    },

    /* poly_bound4_H: 15 elements (8 num + 7 den) - Table 37 Blair et al */
    .poly_bound4_H = {
        /* Numerator P40H-P47H */
        V8_SET1_F64(-0x1.f3596123109edp-7),
        V8_SET1_F64(0x1.60b8fe375999ep-2),
        V8_SET1_F64(-0x1.779bb9bef7c0fp+1),
        V8_SET1_F64(0x1.786ea384470a2p+3),
        V8_SET1_F64(-0x1.6a7c1453c85d3p+4),
        V8_SET1_F64(0x1.31f0fc5613142p+4),
        V8_SET1_F64(-0x1.5ea6c007d4dbbp+2),
        V8_SET1_F64(0x1.e66f265ce9e50p-3),
        /* Denominator Q40H-Q46H */
        V8_SET1_F64(-0x1.636b2dcf4edbep-7),
        V8_SET1_F64(0x1.0b5411e2acf29p-2),
        V8_SET1_F64(-0x1.3413109467a0bp+1),
        V8_SET1_F64(0x1.563e8136c554ap+3),
        V8_SET1_F64(-0x1.7b77aab1dcafbp+4),
        V8_SET1_F64(0x1.8a3e174e05ddcp+4),
        V8_SET1_F64(-0x1.4075c56404eecp+3)
    },

    /* poly_bound4_T: Tail coefficients (Table 37 Blair et al.) */
    .poly_bound4_T = {
        V8_SET1_F64(-0x1.353bedfc52e09p-61),  /* P40T */
        V8_SET1_F64(0x1.39125c8724d70p-57),   /* P41T */
        V8_SET1_F64(-0x1.8907cbccf0337p-62),  /* Q40T */
        V8_SET1_F64(-0x1.9070eaf055b4ep-56)   /* Q41T */
    }
};

#define BOUND1    v8_erfcinv_data.bound1
#define BOUND2    v8_erfcinv_data.bound2
#define BOUND3    v8_erfcinv_data.bound3
#define BOUND4    v8_erfcinv_data.bound4
#define B1_SUB1   v8_erfcinv_data.b1_sub1
#define SIGN_MASK v8_erfcinv_data.sign_mask
#define INF_NAN   v8_erfcinv_data.inf_nan
#define INF       v8_erfcinv_data.inf
#define NEG_QNAN  v8_erfcinv_data.neg_qnan
#define ONE       v8_erfcinv_data.one
#define TWO       v8_erfcinv_data.two
#define ZERO      v8_erfcinv_data.zero
#define EXP_OFFSET_3 v8_erfcinv_data.exp_offset3
#define EXP_OFFSET_4 v8_erfcinv_data.exp_offset4

#define P20  v8_erfcinv_data.poly_bound2_H[0]
#define P21  v8_erfcinv_data.poly_bound2_H[1]
#define P22  v8_erfcinv_data.poly_bound2_H[2]
#define P23  v8_erfcinv_data.poly_bound2_H[3]
#define P24  v8_erfcinv_data.poly_bound2_H[4]
#define P25  v8_erfcinv_data.poly_bound2_H[5]
#define P26  v8_erfcinv_data.poly_bound2_H[6]
#define P27  v8_erfcinv_data.poly_bound2_H[7]
#define P28  v8_erfcinv_data.poly_bound2_H[8]
#define P29  v8_erfcinv_data.poly_bound2_H[9]
#define P210 v8_erfcinv_data.poly_bound2_H[10]
#define Q20  v8_erfcinv_data.poly_bound2_H[11]
#define Q21  v8_erfcinv_data.poly_bound2_H[12]
#define Q22  v8_erfcinv_data.poly_bound2_H[13]
#define Q23  v8_erfcinv_data.poly_bound2_H[14]
#define Q24  v8_erfcinv_data.poly_bound2_H[15]
#define Q25  v8_erfcinv_data.poly_bound2_H[16]
#define Q26  v8_erfcinv_data.poly_bound2_H[17]
#define Q27  v8_erfcinv_data.poly_bound2_H[18]
#define P20T v8_erfcinv_data.poly_bound2_T[0]
#define P21T v8_erfcinv_data.poly_bound2_T[1]
#define Q20T v8_erfcinv_data.poly_bound2_T[2]
#define Q21T v8_erfcinv_data.poly_bound2_T[3]

#define P30  v8_erfcinv_data.poly_bound3[0]
#define P31  v8_erfcinv_data.poly_bound3[1]
#define P32  v8_erfcinv_data.poly_bound3[2]
#define P33  v8_erfcinv_data.poly_bound3[3]
#define P34  v8_erfcinv_data.poly_bound3[4]
#define P35  v8_erfcinv_data.poly_bound3[5]
#define P36  v8_erfcinv_data.poly_bound3[6]
#define P37  v8_erfcinv_data.poly_bound3[7]
#define P38  v8_erfcinv_data.poly_bound3[8]
#define Q30  v8_erfcinv_data.poly_bound3[9]
#define Q31  v8_erfcinv_data.poly_bound3[10]
#define Q32  v8_erfcinv_data.poly_bound3[11]
#define Q33  v8_erfcinv_data.poly_bound3[12]
#define Q34  v8_erfcinv_data.poly_bound3[13]
#define Q35  v8_erfcinv_data.poly_bound3[14]
#define Q36  v8_erfcinv_data.poly_bound3[15]
#define Q37  v8_erfcinv_data.poly_bound3[16]
#define Q38  v8_erfcinv_data.poly_bound3[17]

#define P40  v8_erfcinv_data.poly_bound4_H[0]
#define P41  v8_erfcinv_data.poly_bound4_H[1]
#define P42  v8_erfcinv_data.poly_bound4_H[2]
#define P43  v8_erfcinv_data.poly_bound4_H[3]
#define P44  v8_erfcinv_data.poly_bound4_H[4]
#define P45  v8_erfcinv_data.poly_bound4_H[5]
#define P46  v8_erfcinv_data.poly_bound4_H[6]
#define P47  v8_erfcinv_data.poly_bound4_H[7]
#define Q40  v8_erfcinv_data.poly_bound4_H[8]
#define Q41  v8_erfcinv_data.poly_bound4_H[9]
#define Q42  v8_erfcinv_data.poly_bound4_H[10]
#define Q43  v8_erfcinv_data.poly_bound4_H[11]
#define Q44  v8_erfcinv_data.poly_bound4_H[12]
#define Q45  v8_erfcinv_data.poly_bound4_H[13]
#define Q46  v8_erfcinv_data.poly_bound4_H[14]
#define P40T v8_erfcinv_data.poly_bound4_T[0]
#define P41T v8_erfcinv_data.poly_bound4_T[1]
#define Q40T v8_erfcinv_data.poly_bound4_T[2]
#define Q41T v8_erfcinv_data.poly_bound4_T[3]

#define SCALAR_ERFCINV ALM_PROTO_OPT(erfcinv)

static inline int test_condition_for_all(v_u64x8_t cond) {
    for(int i = 0; i < 8; i++) {
        if(cond[i] != 0xFFFFFFFFFFFFFFFFULL)
            return 0;
    }
    return 1;
}

/* Convert mask from comparison result to __mmask8 for AVX-512 blend */
static inline __mmask8 to_mask8(v_u64x8_t cond) {
    __mmask8 mask = 0;
    for(int i = 0; i < 8; i++) {
        if(cond[i]) mask |= (__mmask8)(1 << i);
    }
    return mask;
}

v_f64x8_t
ALM_PROTO_ARCH_ZN4(vrd8_erfcinv)(v_f64x8_t _x) {
    v_f64x8_t result;
    v_u64x8_t zero_u64 = {0, 0, 0, 0, 0, 0, 0, 0};

    v_u64x8_t ux = as_v8_u64_f64(_x);
    v_u64x8_t sign = ux & ~SIGN_MASK;
    v_u64x8_t ux_abs = ux & SIGN_MASK;

    /* Special cases : NaN, outside domain */
    v_u64x8_t special_cond = (sign != zero_u64) | (ux_abs > BOUND4);
    if(unlikely(test_condition_for_all(special_cond))) {
        v_u64x8_t zero_cond = (ux_abs == zero_u64);
        v_u64x8_t nan_cond = (ux_abs >= INF_NAN);
        v_u64x8_t oob_cond = (sign != zero_u64) | (ux_abs > BOUND4);
        
        v_f64x8_t inf_result = as_v8_f64_u64(INF);
        v_f64x8_t nan_result = _x - _x;  /* Propagate NaN for NaN inputs */
        v_f64x8_t qnan_result = as_v8_f64_u64(NEG_QNAN);  /* Error NaN for out-of-bounds */
        
        /* Apply in reverse priority order (last blend has highest priority) */
        /* Negative or > 2 returns error -QNaN */
        result = _mm512_mask_blend_pd(to_mask8(oob_cond), _x, qnan_result);
        /* NaN input returns x-x (preserves NaN sign) */
        result = _mm512_mask_blend_pd(to_mask8(nan_cond), result, nan_result);
        /* x == 0 returns +INF (highest priority) */
        result = _mm512_mask_blend_pd(to_mask8(zero_cond), result, inf_result);
        return result;
    }

    v_u64x8_t ua;
    {
        v_f64x8_t z = ONE - _x;
        v_f64x8_t a = as_v8_f64_u64(as_v8_u64_f64(z) & SIGN_MASK);  // abs(z)
        ua = as_v8_u64_f64(a);
    }

    /* Combined Range 1 & 4: Both use Table 58 Blair et al with similar computation
     * Range 1: erfcinv for x in (1e-100, 0.0625] - direct erfcinv
     * Range 4: erfinv for x in (1.9375, 2) - via erfinv(1-x) relationship
     * Differences handled via blend:
     *   - z input: _x (Range1) vs TWO-_x (Range4)
     *   - result sign: positive (Range1) vs negative (Range4)
     */
    v_u64x8_t cond1 = (ux_abs > B1_SUB1) & (ux_abs <= BOUND1);
    v_u64x8_t cond4 = (ux_abs > BOUND1) & (ua > BOUND3) & (ua < BOUND4);
    v_u64x8_t cond1_or_4 = cond1 | cond4;
    if(test_condition_for_all(cond1_or_4)) {
        /* For Range1: log_arg = _x, For Range4: log_arg = TWO - _x */
        v_f64x8_t log_arg = _mm512_mask_blend_pd(to_mask8(cond1), TWO - _x, _x);
        v_f64x8_t z = ONE / ALM_PROTO(vrd8_sqrt)(-ALM_PROTO(vrd8_log)(log_arg));

        v_f64x8_t P = POLY_EVAL_ESTRIN_11_TAIL_2(z, P20, P20T, P21, P21T,
            P22, P23, P24, P25, P26, P27, P28, P29, P210);
        v_f64x8_t Q = POLY_EVAL_ESTRIN_9_TAIL_2(z, Q20, Q20T, Q21, Q21T,
            Q22, Q23, Q24, Q25, Q26, Q27, ONE);
        Q *= z;

        v_f64x8_t r = P / Q;
        /* For Range4, negate the result; for Range1, keep positive */
        return _mm512_mask_blend_pd(to_mask8(cond1), -r, r);
    }

    /* Range 2: (erfinv for a <= 0.75) - Table 22 Blair et al */
    v_u64x8_t cond2 = (ux_abs > BOUND1) & (ua <= BOUND2);
    if(test_condition_for_all(cond2)) {
        v_f64x8_t z = ONE - _x;
        v_f64x8_t z2 = z * z - EXP_OFFSET_3;
        v_f64x8_t P = POLY_EVAL_HORNER_9(z2, P30, P31, P32, P33, P34, P35, P36, P37, P38);
        v_f64x8_t Q = POLY_EVAL_HORNER_10_1(z2, Q30, Q31, Q32, Q33, Q34, Q35, Q36, Q37, Q38, ONE);
        return z * (P / Q);
    }

    /* Range 3: (erfinv for a <= 0.9375) - Table 37 Blair et al */
    v_u64x8_t cond3 = (ua > BOUND2) & (ua <= BOUND3);
    if(test_condition_for_all(cond3)) {
        v_f64x8_t z = ONE - _x;
        v_f64x8_t z2 = ((_x * _x) - (TWO * _x)) + (ONE - EXP_OFFSET_4);
        v_f64x8_t P = POLY_EVAL_ESTRIN_8_TAIL_2(z2, P40, P40T, P41, P41T,
            P42, P43, P44, P45, P46, P47);
        v_f64x8_t Q = POLY_EVAL_ESTRIN_8_TAIL_2(z2, Q40, Q40T, Q41, Q41T,
            Q42, Q43, Q44, Q45, Q46, ONE);
        return z * (P / Q);
    }

    /* For mixed ranges, call scalar function */
    for(uint64_t i = 0; i < 8; i++) {
        result[i] = SCALAR_ERFCINV(_x[i]);
    }
    return result;
}
