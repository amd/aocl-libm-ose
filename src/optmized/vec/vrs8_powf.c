/*
 * Copyright (C) 2018-2020, Advanced Micro Devices. All rights reserved.
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
#include <immintrin.h>
#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#define AMD_LIBM_FMA_USABLE 1           /* needed for poly.h */
#include <libm/poly-vec.h>

#define VECTOR_LENGTH 4
#define N 8
#define TABLE_SIZE (1ULL << N)
#define MAX_POLYDEGREE  8
extern double log_256[];
extern const double log_f_inv_256[];
#define TAB_F_INV log_f_inv_256
#define TAB_LOG   log_256
#define MANT_MASK_N  (0x007F8000)
#define MANT_MASK_N1 (0x0000080000000000ULL)
#define SINGLE_PRECISION_BIAS 127
#define SINGLE_PRECISION_MANTISSA 0x007FFFFF
#define ONE_BY_TWO 0x3f000000

static struct {
    v_u32x8_t mantissa_bits, one_by_two, mant_8_bits;
    v_i32x8_t float_bias;
    v_u32x8_t v_min, v_max;
    double ALIGN(16) poly[MAX_POLYDEGREE];
    v_f64x4_t ln2;
} v_log_data = {
    .ln2    = _MM_SET1_PD4(0x1.62e42fefa39efp-1), /* ln(2) */
    .v_min  = _MM256_SET1_PS8(0x00800000),
    .v_max  = _MM256_SET1_PS8(0x7f800000),
    .float_bias     =  _MM256_SET1_I32(SINGLE_PRECISION_BIAS),
    .mantissa_bits  =  _MM256_SET1_I32(SINGLE_PRECISION_MANTISSA),
    .one_by_two     =  _MM256_SET1_I32(ONE_BY_TWO),
    .mant_8_bits    =  _MM256_SET1_I32(MANT_MASK_N),

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
    v_f64x4_t ln2by_tblsz, tblsz_byln2, Huge;
    double_t ALIGN(16) poly[MAX_POLYDEGREE];
    v_u64x4_t expf_max, mask;
} expf_v4_data  = {
    .ln2by_tblsz = _MM_SET1_PD4(0x1.62e42fefa39efp-7),
    .tblsz_byln2 = _MM_SET1_PD4(0x1.71547652b82fep+0),
    .Huge = _MM_SET1_PD4(0x1.8000000000000p+52),
    .expf_max = _MM_SET1_I64(0x4056000000000000),
    .mask = _MM_SET1_I64(0x7fffffffffffffff),
    .poly = {
        0x1.0000014439a91p0,
        0x1.62e43170e3344p-1,
        0x1.ebf906bc4c115p-3,
        0x1.c6ae2bb88c0c8p-5,
        0x1.3d1079db4ef69p-7,
        0x1.5f8905cb0cc4ep-10
    },
};

#define SP_BIAS         v_log_data.float_bias
#define V_MIN           v_log_data.v_min
#define V_MAX           v_log_data.v_max
#define V_MASK          v_log_data.v_mask
#define LN2             v_log_data.ln2
#define MANTISSA_BITS   v_log_data.mantissa_bits
#define HALF            v_log_data.one_by_two
#define MANT_8_BITS     v_log_data.mant_8_bits
#define INVLN2          expf_v4_data.tblsz_byln2
#define EXPF_HUGE       expf_v4_data.Huge
#define EXPF_MAX        expf_v4_data.expf_max
#define DP64_MASK       expf_v4_data.mask

/*
 * Short names for polynomial coefficients
 */

#define D1  _MM_SET1_PD4(expf_v4_data.poly[0])
#define D2  _MM_SET1_PD4(expf_v4_data.poly[1])
#define D3  _MM_SET1_PD4(expf_v4_data.poly[2])
#define D4  _MM_SET1_PD4(expf_v4_data.poly[3])
#define D5  _MM_SET1_PD4(expf_v4_data.poly[4])
#define D6  _MM_SET1_PD4(expf_v4_data.poly[5])

#define C2  _MM_SET1_PD4(v_log_data.poly[0])
#define C3  _MM_SET1_PD4(v_log_data.poly[1])


/*
 *   __m256 ALM_PROTO_OPT(vrs8_powf)(__m256, __m256);
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
 *      log(x) = n*log(2) + log(z)
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
v_any_u32(v_i32x8_t cond)
{
    for(int i=0; i< 8; i++){
        if(cond[i])
            return 1;
    }

    return 0;
}

static inline void
update_condition(v_i32x8_t* cond1, v_i64x4_t cond2, uint32_t lane)
{

    uint32_t k = lane << 2;
    for(uint32_t i = 0; i < 4; i++) {
        if((*cond1)[i + k] || cond2[i]){
            (*cond1)[i + k] = 1;
         }
    }
}

static inline v_f32x8_t
powf_specialcase(v_f32x8_t _x,
                 v_f32x8_t _y,
                 v_f32x8_t result,
                 v_i32x8_t cond)
{
    return call2_v8_f32(ALM_PROTO(powf), _x, _y, result, cond);
}

static inline v_f64x4_t
look_table_access(const double* table,
                  const int vector_size,
                  v_u32x4_t indices)
{
     uint64_t j;
     v_f64x4_t ret;
     for(int i = 0; i < vector_size; i++) {
        j = indices[i];
        ret[i] = table[j];
     }
     return ret;
}


__m256
ALM_PROTO_OPT(vrs8_powf)(__m256 x,__m256 y)
{

    v_u32x8_t u;

    v_f32x4_t ret_array[2];

    v_f32x8_t ret;

    u = as_v8_u32_f32(x);

    v_i32x8_t condition = (u >= V_MAX);

    v_i32x8_t int_exponent = (((v_i32x8_t)u) >> 23) - SP_BIAS;

    v_f32x4_t _f[2];

    v_f32x4_t _y[2];

    v_i32x4_t exponent_array[2];

    v_i32x4_t index_array[2];

    /*
     * As AOCC was not able to unroll the following intrinsics, they
     * have been done manually
     */

    _y[0] = _mm256_extractf128_ps(y, 0);

    _y[1] = _mm256_extractf128_ps(y, 1);

    exponent_array[0] = _mm256_extractf128_si256(int_exponent, 0);

    exponent_array[1] = _mm256_extractf128_si256(int_exponent, 1);

    v_u32x8_t mant  = ((u & MANTISSA_BITS) | HALF);

    v_u32x8_t index = u & MANT_8_BITS;

    v_f32x8_t index_times_half = as_v8_f32_u32(index | HALF);

    v_f32x8_t y1 = as_v8_f32_u32(mant);

    index =  index >> (23 - N);

    v_f32x8_t f = index_times_half - y1;

    _f[0] = _mm256_extractf128_ps(f, 0);

    _f[1] = _mm256_extractf128_ps(f, 1);

    index_array[0] = _mm256_extractf128_si256(index, 0);

    index_array[1] = _mm256_extractf128_si256(index, 1);


    for(uint32_t lane = 0; lane < 2; lane++) {

        v_f64x4_t yd = _mm256_cvtps_pd(_y[lane]);

        v_f64x4_t exponent =  _mm256_cvtepi32_pd (exponent_array[lane]);

        v_f64x4_t fd = _mm256_cvtps_pd(_f[lane]);

        v_f64x4_t F_INV, LOG_256, r;

        /* Avoiding the use of vgatherpd instruction for performance reasons */

        F_INV = look_table_access(TAB_F_INV, VECTOR_LENGTH, index_array[lane]);

        LOG_256 = look_table_access(TAB_LOG, VECTOR_LENGTH, index_array[lane]);

        r = fd * F_INV;

        v_f64x4_t poly = r + r * r * (C2  + r * C3);

        /* m*log(2) + log(G) - poly */

        v_f64x4_t temp  = (exponent * LN2) + LOG_256;

        temp -= poly;

        v_f64x4_t ylogx = temp * yd;

        /* Calculate exp */

        v_u64x4_t v = as_v4_u64_f64(ylogx);

        v = v & DP64_MASK;

        /* Check if y * log(x) > ln(2) * 127 */

        v_i64x4_t condition2 = (v >= EXPF_MAX);

        v_f64x4_t z = ylogx * INVLN2;

        v_f64x4_t dn = z + EXPF_HUGE;

        v_u64x4_t n = as_v4_u64_f64(dn);

        dn = dn - EXPF_HUGE;

        r = z - dn;

        v_f64x4_t result = POLY_EVAL_5(r, D1, D2, D3, D4, D5, D6);

        ret_array[lane] = _mm256_cvtpd_ps(as_v4_f64_u64(as_v4_u64_f64(result) + (n << 52)));

        update_condition(&condition, condition2, lane);

    }

    ret =  _mm256_setr_m128(ret_array[0], ret_array[1]);

    if (unlikely(v_any_u32(condition))) {

       return powf_specialcase(x, y, ret, condition);

    }


    return ret;

}


