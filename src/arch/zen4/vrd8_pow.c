/*
 * Copyright (C) 2018-2022, Advanced Micro Devices. All rights reserved.
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

#include <libm_util_amd.h>
#include <libm/alm_special.h>

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>

#define AMD_LIBM_FMA_USABLE 1           /* needed for poly-vec.h */
#include <libm/poly-vec.h>

#include <libm/arch/zen4.h>

typedef struct {
    double head;
    double tail;
} lookup_data;

extern lookup_data log_f_256[];
extern lookup_data log_Finv[];

#define VECTOR_LENGTH 8
#define N 10
#define TAB_F_INV log_Finv
#define TAB_LOG   log_f_256
#define MANT_MASK_N  (0x000FFC0000000000UL)
#if defined(_WIN64) || defined(_WIN32)
    #define DOUBLE_PRECISION_BIAS 1023LL
#else
    #define DOUBLE_PRECISION_BIAS 1023L
#endif
#define DOUBLE_PRECISION_MANTISSA 0x000fffffffffffffUL
#define ONE_BY_TWO 0x3fe0000000000000UL
#define SIGN_MASK   0x7FFFFFFFFFFFFFFFUL

static struct {
    v_u64x8_t mantissa_bits, one_by_two, mant_8_bits;
    v_i64x8_t float_bias;
    v_f64x8_t poly[8];
    v_f64x8_t ln2_head, ln2_tail;
    double ln2_head_scal, ln2_tail_scal;
} v_log_data = {
    .ln2_head = _MM512_SET1_PD8(0x1.62e42e0000000p-1), /* ln(2) head*/
    .ln2_tail = _MM512_SET1_PD8(0x1.efa39ef35793cp-25), /* ln(2) tail*/
    .ln2_head_scal = 0x1.62e42e0000000p-1, /* ln(2) head*/
    .ln2_tail_scal = 0x1.efa39ef35793cp-25, /* ln(2) tail*/
    .float_bias =  _MM512_SET1_I64x8(DOUBLE_PRECISION_BIAS),
    .mantissa_bits = _MM512_SET1_U64x8(DOUBLE_PRECISION_MANTISSA),
    .one_by_two = _MM512_SET1_U64x8(ONE_BY_TWO),
    .mant_8_bits = _MM512_SET1_U64x8(MANT_MASK_N),

    /*
    * Polynomial constants, 1/x! (reciprocal x)
    */
    .poly = { /* skip for 0/1 and 1/1 */
        _MM512_SET1_PD8(0x1.0000000000000p-1),    /* 1/2 */
        _MM512_SET1_PD8(0x1.5555555555555p-2),    /* 1/3 */
        _MM512_SET1_PD8(0x1.0000000000000p-2),    /* 1/4 */
        _MM512_SET1_PD8(0x1.999999999999ap-3),    /* 1/5 */
        _MM512_SET1_PD8(0x1.5555555555555p-3),    /* 1/6 */
        _MM512_SET1_PD8(0x1.2492492492492p-3),    /* 1/7 */
        _MM512_SET1_PD8(0x1.0000000000000p-3),    /* 1/8 */
        _MM512_SET1_PD8(0x1.c71c71c71c71cp-4),    /* 1/9 */
    },
};

static struct {
    v_f64x8_t ln2by_tblsz, Huge, ln2_tblsz_head, ln2_tblsz_tail;
    v_f64x8_t tblsz_ln2;
    v_i64x8_t one;
    v_f64x8_t poly[11];
    v_i64x8_t exp_bias;
    v_u64x8_t exp_max;
} v_exp_data  = {
    .ln2_tblsz_head = _MM512_SET1_PD8(0x1.63p-1),
	.ln2_tblsz_tail = _MM512_SET1_PD8(-0x1.bd0105c610ca8p-13),
    .tblsz_ln2 = _MM512_SET1_PD8(0x1.71547652b82fep+0),
    .one =  _MM512_SET1_I64x8(0x3ff0000000000000L),
    .Huge = _MM512_SET1_PD8(0x1.8000000000000p+52),
    .exp_max = _MM512_SET1_U64x8(0x4086200000000000UL),
    .exp_bias = _MM512_SET1_I64x8(DOUBLE_PRECISION_BIAS),
    .poly = {
		_MM512_SET1_PD8(0x1.0p0),
		_MM512_SET1_PD8(0x1.000000000001p-1),
		_MM512_SET1_PD8(0x1.55555555554a2p-3),
		_MM512_SET1_PD8(0x1.555555554f37p-5),
		_MM512_SET1_PD8(0x1.1111111130dd6p-7),
		_MM512_SET1_PD8(0x1.6c16c1878111dp-10),
		_MM512_SET1_PD8(0x1.a01a011057479p-13),
		_MM512_SET1_PD8(0x1.a01992d0fe581p-16),
		_MM512_SET1_PD8(0x1.71df4520705a4p-19),
		_MM512_SET1_PD8(0x1.28b311c80e499p-22),
		_MM512_SET1_PD8(0x1.ad661ce7af3e3p-26),
	},
}; 

#define MANTISSA_N_BITS v_log_data.mant_8_bits
#define MANTISSA_BITS   v_log_data.mantissa_bits
#define DP64_BIAS       v_log_data.float_bias
#define DP_HALF         v_log_data.one_by_two
#define LN2_HEAD        v_log_data.ln2_head
#define LN2_TAIL        v_log_data.ln2_tail

#define LN2_HEAD_EXP    v_exp_data.ln2_tblsz_head
#define LN2_TAIL_EXP    v_exp_data.ln2_tblsz_tail
#define INVLN2_EXP      v_exp_data.tblsz_ln2
#define EXP_HUGE        v_exp_data.Huge
#define EXP_MAX         v_exp_data.exp_max
#define EXP_BIAS        v_exp_data.exp_bias

/*
 * Short names for polynomial coefficients
 */

#define A1  v_log_data.poly[0]
#define A2  v_log_data.poly[1]
#define A3  v_log_data.poly[2]
#define A4  v_log_data.poly[3]

#define B1  v_exp_data.poly[0]
#define B3  v_exp_data.poly[1]
#define B4  v_exp_data.poly[2]
#define B5  v_exp_data.poly[3]
#define B6  v_exp_data.poly[4]
#define B7  v_exp_data.poly[5]
#define B8  v_exp_data.poly[6]
#define B9  v_exp_data.poly[7]
#define B10 v_exp_data.poly[8]
#define B11 v_exp_data.poly[9]
#define B12 v_exp_data.poly[10]

/*
 *   __m512d ALM_PROTO_OPT(vrd8_pow)(__m512d, __m512d);
 *
 * Spec:
 *   - A slightly relaxed version of the scalar pow.
 *   - Maximum ULP is expected to be less than 3.
 *
 *
 * Implementation Notes:
 * pow(x,y) = e^(y * log(x))
 * 
 *  1. Calculation of log(x) proceeds using a vectorized version of the scalar log algorithm
 *     which returns both head and tail portions for increased accuracy.
 *
 *  2. Computation of e^(y * log(x)) then proceeds similarly to vrd4_exp
 *
 */

__m512d
ALM_PROTO_ARCH_ZN4(vrd8_pow)(__m512d _x,__m512d _y)
{
    __m512d result;
    
    v_u64x8_t ux = as_v8_u64_f64(_x);

    /* This portion of the code is a vectorized version of the scalar log.c, with some checks removed */

    v_i32x8_t int32_exponent;

    v_i64x8_t int_exponent = (v_i64x8_t)( (ux >> 52) - DP64_BIAS );

    v_u64x8_t mant  = ((ux & MANTISSA_BITS) | DP_HALF);

    v_u64x8_t index = ux & MANTISSA_N_BITS;

    v_f64x8_t index_times_half = as_v8_f64_u64(index | DP_HALF);

    index =  index >> (52 - N);

    v_f64x8_t y1  = as_v8_f64_u64(mant);

    v_f64x8_t f = index_times_half - y1;

    v_f64x8_t F_INV_HEAD, F_INV_TAIL;

    v_f64x8_t LOG_256_HEAD, LOG_256_TAIL;

    v_f64x8_t r, r1, u;

    /* Avoiding the use of vgatherpd instruction for performance reasons */

    for(int i = 0; i < VECTOR_LENGTH; i++) {

        int32_t j = (int32_t)index[i];

        int32_exponent[i] = (int32_t)int_exponent[i];

        F_INV_HEAD[i] = TAB_F_INV[j].head;

        F_INV_TAIL[i] = TAB_F_INV[j].tail;

        LOG_256_HEAD[i] = TAB_LOG[j].head;

        LOG_256_TAIL[i] = TAB_LOG[j].tail;

    }

    v_f64x8_t exponent = (v_f64x8_t) _mm512_cvtepi32_pd ((__m256i)int32_exponent);

    r = f * F_INV_TAIL;

    r1 = f * F_INV_HEAD;

    u = r + r1;

    v_f64x8_t z =  r1 - u;

    /* Use Estrin's polynomial to compute log(u) */

    v_f64x8_t u2 = u * u;

    v_f64x8_t a1 = A1  + u * A2;

    v_f64x8_t a2 = A3  + u * A4;

    v_f64x8_t u4 = u2 * u2;

    v_f64x8_t b1 = u2 * a1;

    v_f64x8_t  q1 = b1 + u4 * a2;

    v_f64x8_t poly = (z + r) + q1;

    /* m*log(2) + log(G) - poly */

    v_f64x8_t temp = (LN2_TAIL * exponent - poly) + LOG_256_TAIL;

    v_f64x8_t resT  = temp - u;

    v_f64x8_t resH = LN2_HEAD * exponent + LOG_256_HEAD;

    v_f64x8_t logx_h = resH + resT;

    v_f64x8_t logx_t = resH - logx_h + resT;

    v_f64x8_t ylogx_h = logx_h * _y;

    v_f64x8_t ylogx_t = logx_t * _y + _mm512_fmadd_pd(logx_h, _y, -ylogx_h);

    /* Calculate exp */

    v_u64x8_t v = as_v8_u64_f64(ylogx_h) & SIGN_MASK;

    /* check if y*log(x) > 1024*ln(2) */
    v_i64x8_t condition2 = (v >= EXP_MAX);

    z = ylogx_h * INVLN2_EXP;

    v_f64x8_t dn = z + EXP_HUGE;

    v_i64x8_t n = as_v8_i64_f64(dn);

    dn = dn - EXP_HUGE;

    r = ylogx_h - (dn * LN2_HEAD_EXP);

    v_i64x8_t m = (n + EXP_BIAS) << 52;

    r = (r - (LN2_TAIL_EXP * dn)) + ylogx_t;

    v_f64x8_t poly2 = POLY_EVAL_11(r, B1, B1, B3, B4, B5, B6,
                                  B7, B8, B9, B10, B11, B12);

    result = poly2 * as_v8_f64_i64(m);

    for(int i = 0; i < VECTOR_LENGTH; i++) {
        if(unlikely((condition2)[i])){
            result[i] = ALM_PROTO(pow)(_x[i], _y[i]);
         }
    }

    return result;
}