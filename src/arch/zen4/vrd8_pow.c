/*
 * Copyright (C) 2018-2021, Advanced Micro Devices. All rights reserved.
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

#include <libm_macros.h>
#include <libm/compiler.h>
#include <libm_util_amd.h>
#include <libm/types.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly-vec.h>

#include <libm/arch/zen4.h>

#define AMD_LIBM_FMA_USABLE 1           /* needed for poly.h */

typedef struct {
    double head;
    double tail;
} lookup_data;


#define N 10
#define TABLE_SIZE (1ULL << N)
#define MAX_POLYDEGREE  8
extern lookup_data log_f_256[];
extern lookup_data log_Finv[];
extern lookup_data exp_lookup[];
#define TWO_POWER_J_BY_N exp_lookup
#define TAB_F_INV log_Finv
#define TAB_LOG   log_f_256
#define MANT_MASK_N  (0x000FFC0000000000UL)
#define MANT_MASK_N1 (0x0000080000000000UL)
#define DOUBLE_PRECISION_BIAS 1023L
#define DOUBLE_PRECISION_MANTISSA 0x000fffffffffffffUL
#define ONE_BY_TWO 0x3fe0000000000000UL

static struct {
    v_u64x8_t v_min, v_max, mantissa_bits, one_by_two, mant_8_bits;
    v_i64x8_t float_bias;
    double ALIGN(16) poly[MAX_POLYDEGREE];
    v_f64x8_t ln2_head, ln2_tail;
} v_log_data = {
    .ln2_head = _MM512_SET1_PD8(0x1.62e42e0000000p-1), /* ln(2) head*/
    .ln2_tail = _MM512_SET1_PD8(0x1.efa39ef35793cp-25), /* ln(2) tail*/
    .v_min  = _MM512_SET1_U64x8(0x0010000000000000UL),
    .v_max  = _MM512_SET1_U64x8(0x7ff0000000000000UL),
    .float_bias =  _MM512_SET1_I64x8(DOUBLE_PRECISION_BIAS),
    .mantissa_bits = _MM512_SET1_U64x8(DOUBLE_PRECISION_MANTISSA),
    .one_by_two = _MM512_SET1_U64x8(ONE_BY_TWO),
    .mant_8_bits = _MM512_SET1_U64x8(MANT_MASK_N),

    /*
    * Polynomial constants, 1/x! (reciprocal x)
    */
    .poly = { /* skip for 0/1 and 1/1 */
        0x1.0000000000000p-1,    /* 1/2 */
        0x1.5555555555555p-2,    /* 1/3 */
        0x1.0000000000000p-2,    /* 1/4 */
        0x1.999999999999ap-3,    /* 1/5 */
        0x1.5555555555555p-3,    /* 1/6 */
        0x1.2492492492492p-3,    /* 1/7 */
        0x1.0000000000000p-3,    /* 1/8 */
        0x1.c71c71c71c71cp-4,    /* 1/9 */
    },
};


static struct {
    v_f64x8_t ln2by_tblsz, tblsz_byln2, Huge;
    v_f64x8_t ln2_by_n_head, ln2_by_n_tail;
    v_i64x8_t one;
    double ALIGN(16) poly[MAX_POLYDEGREE];
    v_u64x8_t exp_max;
} exp_v4_data  = {
    .ln2_by_n_head = _MM512_SET1_PD8(0x1.62e42f0000000p-11),
    .ln2_by_n_tail = _MM512_SET1_PD8(0x1.DF473DE6AF279p-36),
    .tblsz_byln2 = _MM512_SET1_PD8(0x1.71547652b82fep10),
    .one =  _MM512_SET1_I64x8(0x3ff0000000000000L),
    .Huge = _MM512_SET1_PD8(0x1.8000000000000p+52),
    .exp_max = _MM512_SET1_U64x8(0x4086200000000000UL),
    .poly = {
        0x1.5555555555555p-3,
        0x1.5555555555555p-5,
        0x1.1111111111111p-7,
        0x1.6c16c16c16c17p-10,
    },
};

#define MANTISSA_N_BITS v_log_data.mant_8_bits
#define MANTISSA_BITS   v_log_data.mantissa_bits
#define DP64_BIAS       v_log_data.float_bias
#define DP_HALF         v_log_data.one_by_two
#define V_MIN           v_log_data.v_min
#define V_MAX           v_log_data.v_max
#define V_MASK          v_log_data.v_mask
#define LN2_HEAD        v_log_data.ln2_head
#define LN2_TAIL        v_log_data.ln2_tail
#define INVLN2          exp_v4_data.tblsz_byln2
#define EXP_HUGE        exp_v4_data.Huge
#define LOG2_BY_N_HEAD  exp_v4_data.ln2_by_n_head
#define LOG2_BY_N_TAIL  exp_v4_data.ln2_by_n_tail
#define ONE             exp_v4_data.one
#define EXP_MAX         exp_v4_data.exp_max
/*
 * Short names for polynomial coefficients
 */

#define D0  _MM512_SET1_PD8(0.0)
#define D1  _MM512_SET1_PD8(1.0)
#define D2  _MM512_SET1_PD8(0x1.0000000000000p-1)
#define D3  _MM512_SET1_PD8(exp_v4_data.poly[0])
#define D4  _MM512_SET1_PD8(exp_v4_data.poly[1])
#define D5  _MM512_SET1_PD8(exp_v4_data.poly[2])
#define D6  _MM512_SET1_PD8(exp_v4_data.poly[3])

#define C0  _MM512_SET1_PD8(0.0)
#define C1  _MM512_SET1_PD8(v_log_data.poly[0])
#define C2  _MM512_SET1_PD8(v_log_data.poly[1])
#define C3  _MM512_SET1_PD8(v_log_data.poly[2])
#define C4  _MM512_SET1_PD8(v_log_data.poly[3])

/*
 *   __m512d ALM_PROTO_OPT(vrd8_pow)(__m512d, __m512d);
 *
 * Spec:
 *   - A slightly relaxed version of the scalar powf.
 *   - Maximum ULP is expected to be less than 2.
 *   - Performance is expected to be double of scalar powf
 *
 *
 * Implementation Notes:
 * pow(x,y) = e^(y * log(x))
 *  1. Calculation of log(x):
 *      x = 2^n*(1+f)                                          .... (1)
 *         where n is exponent and is an integer
 *             (1+f) is mantissa ∈ [1,2). i.e., 1 ≤ 1+f < 2    .... (2)
 *
 *      From (1), taking log on both sides
 *      log(x) = log(2^n * (1+f))
 *             = log(2^n) + log(1+f)
 *             = n*log(2) + log(1+f)                           .... (3)
 *
 *      let z = 1 + f
 *             log(x) = n*log(2) + log(z)
 *      z = G + g
 *      log(x) = n*log(2) + log(G + g)
 *
 *      log(x) = n*log(2) + log(2*(G/2 + g/2))
 *
 *      log(x) = n*log(2) + log(2) + log(F + f)  with (0.5 <= F < 1) and (f <= 2 ^ (-10))
 *
 *      log(x) = m * log(2) + log(2) + log(F) + log(1 -(f / F))
 *
 *      Y = (2 ^ (-1)) * (2 ^ (-m)) * (2 ^ m) * A
 *
 *      Now, range of Y is: 0.5 <= Y < 1
 *
 *      F = 0x100 + (first 8 mantissa bits) + (9th mantissa bit)
 *
 *      Now, range of F is: 256 <= F <= 512
 *
 *      F = F / 512
 *
 *      Now, range of F is: 0.5 <= F <= 1
 *
 *      f = -(Y - F), with (f <= 2^(-10))
 *
 *      log(x) = m * log(2) + log(2) + log(F) + log(1 -(f / F))
 *
 *      log(x) = m * log(2) + log(2 * F) + log(1 - r)
 *
 *      r = (f / F), with (r <= 2^(-9))
 *
 *      r = f * (1 / F) with (1 / F) precomputed to avoid division
 *
 *      log(x) = m * log(2) + log(G) - poly
 *
 *      log(G) is precomputed
 *
 *      poly = (r + (r ^ 2) / 2 + (r ^ 3) / 3
 *
 *  2. Computation of e^(y * log(x))
 *      e^x = 2 ^ (x/ln(2))
 *
 *      z =  x/ln(2)
 *
 *      z = n + r where n is the integer part of the number and r is the fraction
 *
 *      2^z = 2^n * 2^r
 *
 *      2^n can be calculated as (n + 1023) << 52
 *
 *      2^r is approximated by a polynomial approximated by sollya
 *
 *      Polynomial Approximation:
 *      For More information refer to tools/sollya/vrs4_expf.sollya
 *
 *
 */
static inline int
check_condition(v_i64x8_t* cond1, v_i64x8_t cond2)
{

    int32_t ret = 0;

    for(int i = 0; i < 8; i++) {
        if((*cond1)[i] || cond2[i]){
            (*cond1)[i] = 1;
            ret = 1;
         }
    }

    return ret;
}

static inline v_f64x8_t
pow_specialcase(v_f64x8_t _x,
                 v_f64x8_t _y,
                 v_f64x8_t result,
                 v_i64x8_t cond)
{
    return call2_v8_f64(ALM_PROTO(pow), _x, _y, result, cond);
}


__m512d
ALM_PROTO_OPT(vrd8_pow)(__m512d _x,__m512d _y)
{
    __m512d result, poly;

    v_u64x8_t ux = as_v8_u64_f64(_x);

    v_i32x8_t int32_exponent;

    v_i64x8_t condition = (ux - V_MIN >= V_MAX - V_MIN);

    v_i64x8_t int_exponent = (((v_i64x8_t)ux) >> 52) - DP64_BIAS;

    v_u64x8_t mant  = ((ux & MANTISSA_BITS) | DP_HALF);

    v_u64x8_t index = ux & MANTISSA_N_BITS;

    v_f64x8_t index_times_half = as_v8_f64_u64(index | DP_HALF);

    index =  index >> (52 - N);

    v_f64x8_t y1  = as_v8_f64_u64(mant);

    v_f64x8_t f = index_times_half - y1;

    v_f64x8_t f_inv_head, f_inv_tail, r, r1, u;

    v_f64x8_t log_256_head, log_256_tail, j_by_N_head, j_by_N_tail;

    /* Avoiding the use of vgatherpd instruction for performance reasons */

    for(int  lane = 0; lane < 8; lane++) {

        int32_t j = (int32_t)index[lane];

        int32_exponent[lane] = (int32_t)int_exponent[lane];

        f_inv_head[lane] = TAB_F_INV[j].head;

        f_inv_tail[lane] = TAB_F_INV[j].tail;

        log_256_head[lane] = TAB_LOG[j].head;

        log_256_tail[lane] = TAB_LOG[j].tail;

    }

    v_f64x8_t exponent = (v_f64x8_t) _mm512_cvtepi32_pd ((__m256i)int32_exponent);

    r = f * f_inv_tail;

    r1 = f * f_inv_head;

    u = r + r1;

    v_f64x8_t z =  r1 - u;

    /* Use Estrin's polynomial to compute log(u) 
     * log(u) = u^2*C1 + u^3*C2 + u^4*C3 + u*5*C4
     * */

    v_f64x8_t u2 = u * u;

    poly = (z + r) + POLY_EVAL_4(u2, C0, C1, C2, C3, C4); 

    /* m*log(2) + log(G) - poly */

    v_f64x8_t temp = (LN2_TAIL * exponent - poly) + log_256_tail;

    v_f64x8_t res_T  = temp - u;

    v_f64x8_t res_H = LN2_HEAD * exponent + log_256_head;

    v_f64x8_t logx_h = res_H + res_T;

    v_f64x8_t logx_t = res_H - logx_h + res_T;

    v_f64x8_t ylogx_h = logx_h * _y;

    v_f64x8_t ylogx_t = logx_t * _y + _mm512_fmadd_pd(logx_h, _y, -ylogx_h);

    /* Calculate exp */

    v_u64x8_t v = as_v8_u64_f64(ylogx_h);

    /* check if y*log(x) > 1024*ln(2) */
    v_i64x8_t condition2 = (v >= EXP_MAX);

    z = ylogx_h * INVLN2;

    v_f64x8_t dn = z + EXP_HUGE;

    v_i64x8_t n = as_v8_i64_f64(dn);

    dn = dn - EXP_HUGE;

    index = (v_u64x8_t)(n & DP64_BIAS);

    r = ylogx_h - (dn * LOG2_BY_N_HEAD);

    v_i64x8_t m = ((n - index) << (52 - N)) + ONE;

    r = (r - (LOG2_BY_N_TAIL * dn)) + ylogx_t;

    /*poly = r +  r^2*D1 + r^3*D2  + r^4*D3 */

    poly = POLY_EVAL_4(r , D0, D1, D2, D3, D4);

    for (int lane = 0; lane < 8; lane++) {

        int32_t j = (int32_t)index[lane];

        j_by_N_head[lane] = TWO_POWER_J_BY_N[j].head;

        j_by_N_tail[lane] = TWO_POWER_J_BY_N[j].tail;

    }

    v_f64x8_t q = j_by_N_tail + poly *  j_by_N_tail;

    z = poly * j_by_N_head;

    r = j_by_N_head + (z + q);

    result = r * as_v8_f64_i64(m);

    if (unlikely(check_condition(&condition, condition2))) {
        return pow_specialcase(_x, _y, result, condition);
    }

    return result;
}


