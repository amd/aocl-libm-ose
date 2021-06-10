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
#include <libm_special.h>
#include <immintrin.h>
#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#define AMD_LIBM_FMA_USABLE 1           /* needed for poly.h */
#include <libm/poly-vec.h>

#include <libm/arch/zen4.h>

#define _MM512_SET1_PD8(x)                                          \
    _Generic((x),                                                   \
             double : (v_f64x8_t){(x), (x), (x), (x), (x), (x), (x), (x)})

#define _MM512_SET1_I64x8(x)                                            \
    _Generic((x),                                                       \
             int64_t: (v_i64x8_t){(x), (x), (x), (x), (x), (x), (x), (x)})

#define _MM512_SET1_U64x8(x)                                            \
    _Generic((x),                                                       \
             uint64_t: (v_u64x8_t){(x), (x), (x), (x), (x), (x), (x), (x)})


#define VECTOR_LENGTH 8
#define N 8
#define TABLE_SIZE (1ULL << N)
#define MAX_POLYDEGREE  8
extern double log_256[];
extern const double log_f_inv_256[];
#define TAB_F_INV log_f_inv_256
#define TAB_LOG   log_256
#define MANT_MASK_N  (0x000FF00000000000UL)
#define MANT_MASK_N1 (0x0000080000000000UL)
#define SINGLE_PRECISION_BIAS 127
#define DOUBLE_PRECISION_MANTISSA 0x000fffffffffffffUL
#define ONE_BY_TWO 0x3fe0000000000000UL

static struct {
    v_u64x8_t mantissa_bits, one_by_two, mant_8_bits;
    v_i32x16_t float_bias;
    v_u32x16_t v_min, v_max;
    double ALIGN(16) poly[MAX_POLYDEGREE];
    v_f64x8_t ln2;
} v_log_data = {
    .ln2    = _MM512_SET1_PD8(0x1.62e42fefa39efp-1), /* ln(2) */
    .v_min  = _MM256_SET1_PS8(0x00800000),
    .v_max  = _MM256_SET1_PS8(0x7f800000),
    .float_bias     =  _MM256_SET1_PS8(SINGLE_PRECISION_BIAS),
    .mantissa_bits  = _MM512_SET1_U64x8(DOUBLE_PRECISION_MANTISSA),
    .one_by_two     = _MM512_SET1_U64x8(ONE_BY_TWO),
    .mant_8_bits    = _MM512_SET1_U64x8(MANT_MASK_N),

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
    double_t ALIGN(16) poly[MAX_POLYDEGREE];
    v_u64x8_t expf_max;
} expf_v4_data  = {
    .ln2by_tblsz = _MM512_SET1_PD8(0x1.62e42fefa39efp-7),
    .tblsz_byln2 = _MM512_SET1_PD8(0x1.71547652b82fep+0),
    .Huge = _MM512_SET1_PD8(0x1.8000000000000p+52),
    .expf_max = _MM512_SET1_U64x8(0x4056000000000000UL),
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
#define INVLN2      expf_v4_data.tblsz_byln2
#define EXPF_HUGE   expf_v4_data.Huge
#define EXPF_MAX    expf_v4_data.expf_max

/*
 * Short names for polynomial coefficients
 */

#define D1  _MM512_SET1_PD8(expf_v4_data.poly[0])
#define D2  _MM512_SET1_PD8(expf_v4_data.poly[1])
#define D3  _MM512_SET1_PD8(expf_v4_data.poly[2])
#define D4  _MM512_SET1_PD8(expf_v4_data.poly[3])
#define D5  _MM512_SET1_PD8(expf_v4_data.poly[4])
#define D6  _MM512_SET1_PD8(expf_v4_data.poly[5])

#define C2  _MM512_SET1_PD8(v_log_data.poly[0])
#define C3  _MM512_SET1_PD8(v_log_data.poly[1])


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


static inline void
update_condition(v_i32x16_t* cond1, v_i64x8_t cond2, int32_t lane)
{

    uint32_t k = (uint32_t)lane << 2;
    for(uint32_t i = 0; i < 8; i++) {
        if((*cond1)[i + k] || cond2[i]){
            (*cond1)[i + k] = 1;
         }
    }
}

static inline v_f32x16_t
powf_specialcase(v_f32x16_t _x,
                 v_f32x16_t _y,
                 v_f32x16_t result,
                 v_i32x16_t cond)
{
    return call2_v16_f32(ALM_PROTO(powf), _x, _y, result, cond);
}

static inline v_f64x8_t
look_table_access(const double* table,
                  const int vector_size,
                  v_u64x8_t indices)
{
     uint64_t j;
     v_f64x8_t ret;
     for(int i = 0; i < vector_size; i++) {
        j = indices[i];
        ret[i] = table[j];
     }
     return ret;
}


v_f32x16_t
ALM_PROTO_OPT(vrs16_powf)(v_f32x16_t x,v_f32x16_t y)
{

    v_u32x16_t u;

    v_f32x8_t ret_array[2];

    v_f32x16_t ret;

    u = as_v16_u32_f32(x);

    v_i32x16_t condition = (u - V_MIN >= V_MAX - V_MIN);

    v_i32x16_t int_exponent = (v_i32x16_t)(u >> 23) - SP_BIAS;

    v_f32x8_t _x[2];

    v_f32x8_t _y[2];

    v_i32x8_t exponent_array[2];

    /*
     * As AOCC was not able to unroll the following intrinsics, they
     * have been done manually
     */

    _x[0] = (v_f32x8_t){x[0], x[1], x[2], x[3],
                        x[4], x[5], x[6], x[7]};

    _x[1] = (v_f32x8_t){x[8], x[9], x[10], x[11],
                        x[12], x[13], x[14], x[15]};

    _y[0] = (v_f32x8_t){y[0], y[1], y[2], y[3],
                        y[4], y[5], y[6], y[7]};


    _y[1] = (v_f32x8_t){y[8], y[9], y[10], y[11],
                        y[12], y[13], y[14], y[15]};


    exponent_array[0] = (v_i32x8_t){int_exponent[0], int_exponent[1], int_exponent[2], int_exponent[3],
    int_exponent[4], int_exponent[5], int_exponent[6], int_exponent[7]};

    exponent_array[1] = (v_i32x8_t){int_exponent[8], int_exponent[9], int_exponent[10], int_exponent[11],
                        int_exponent[12], int_exponent[13], int_exponent[14], int_exponent[15]};

    for(int lane = 0; lane < 2; lane++) {

        v_f64x8_t xd = _mm512_cvtps_pd(_x[lane]);

        v_f64x8_t yd = _mm512_cvtps_pd(_y[lane]);

        v_u64x8_t ux = as_v8_i64_f64(xd);

        v_f64x8_t exponent =  _mm512_cvtepi32_pd ((__m256i)exponent_array[lane]);

        v_u64x8_t mant  = ((ux & MANTISSA_BITS) | HALF);

        v_u64x8_t index = ux & MANT_8_BITS;

        v_f64x8_t index_times_half = as_v8_f64_i64(index | HALF);

        index =  index >> (52 - N);

        v_f64x8_t y1  = as_v8_f64_i64(mant);

        v_f64x8_t f = index_times_half - y1;

        v_f64x8_t F_INV, LOG_256, r;

        /* Avoiding the use of vgatherpd instruction for performance reasons */

        F_INV = look_table_access(TAB_F_INV, VECTOR_LENGTH, index);

        LOG_256 = look_table_access(TAB_LOG, VECTOR_LENGTH, index);

        r = f * F_INV;

        v_f64x8_t r2 = r * r;                /* r^2 */

        v_f64x8_t temp = C2  + r * C3;

        v_f64x8_t q = r + r2 * temp;

        /* m*log(2) + log(G) - poly */

        temp  = (exponent * LN2) + LOG_256;

        temp -= q;

        v_f64x8_t ylogx = temp * yd;

        /* Calculate exp */

        v_u64x8_t v = as_v8_i64_f64(ylogx);

        /* Check if y * log(x) > ln(2) * 127 */

        v_i64x8_t condition2 = (v >= EXPF_MAX);

        v_f64x8_t z = ylogx * INVLN2;

        v_f64x8_t dn = z + EXPF_HUGE;

        v_u64x8_t n = as_v8_i64_f64(dn);

        dn = dn - EXPF_HUGE;

        r = z - dn;

        v_f64x8_t qtmp1 = D1 + (D2 * r);

        v_f64x8_t qtmp2 = D3 + (D4 * r);

        r2 = r * r;

        v_f64x8_t qtmp3 = D5 + (D6 * r);

        q =  qtmp1 + r2 * qtmp2;

        v_f64x8_t result = q + r2 * r2 * qtmp3;

        ret_array[lane] = _mm512_cvtpd_ps(as_v8_f64_i64(as_v8_i64_f64(result) + (n << 52)));

        update_condition(&condition, condition2, lane);

    }

    ret =  _mm512_setr_ps(ret_array[0][0], ret_array[0][1], ret_array[0][2], ret_array[0][3],
                          ret_array[0][4], ret_array[0][5], ret_array[0][6], ret_array[0][7],
                          ret_array[1][0], ret_array[1][1], ret_array[1][2], ret_array[1][3],
                          ret_array[1][4], ret_array[1][5], ret_array[1][6], ret_array[1][7]);

    if (unlikely(any_v16_u32_loop(condition))) {
       return powf_specialcase(x, y, ret, condition);
    }


    return ret;

}


