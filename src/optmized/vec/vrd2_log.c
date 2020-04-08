/*
 * Copyright (C) 2018-2020, AMD. All rights reserved.
 *
 *
 */

#include <libm_util_amd.h>
#include <libm_special.h>

#include <libm_macros.h>
#include <libm_amd.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>

#define AMD_LIBM_FMA_USABLE 1           /* needed for poly.h */
#include <libm/poly-vec.h>

typedef struct {
    double lead;
    double tail;
} lookup_data;


#define VECTOR_LENGTH 2
#define N 8
#define TABLE_SIZE (1ULL << N)
#define MAX_POLYDEGREE  8
extern lookup_data log_table_256[];
extern double log_f_inv_256[];
#define TAB_F_INV log_f_inv_256
#define TAB_LOG   log_table_256
#define MANT_MASK_N  (0x000FF00000000000ULL)
#define DOUBLE_PRECISION_BIAS 1023
#define DOUBLE_PRECISION_MANTISSA 0x000fffffffffffffULL
#define ONE_BY_TWO 0x3fe0000000000000ULL
#define SCALAR_LOG FN_PROTOTYPE(log)

static struct {
    v_u64x2_t v_min, v_max, mantissa_bits, one_by_two, mant_8_bits;
    v_u64x2_t mant_9_bit;
    v_i64x2_t float_bias;
    double ALIGN(16) poly[MAX_POLYDEGREE];
    v_f64x2_t one, ln2_head, ln2_tail;
} v_log_data = {
    .ln2_head = _MM_SET1_PD2(0x1.62e42e0000000p-1), /* ln(2) head*/
    .ln2_tail = _MM_SET1_PD2(0x1.efa39ef35793cp-25), /* ln(2) tail*/
    .one =    _MM_SET1_PD2(0x1.0p+0),
    .v_min  = _MM_SET1_I64x2(0x0010000000000000),
    .v_max  = _MM_SET1_I64x2(0x7ff0000000000000),
    .float_bias =  _MM_SET1_I64x2(DOUBLE_PRECISION_BIAS),
    .mantissa_bits = _MM_SET1_I64x2(DOUBLE_PRECISION_MANTISSA),
    .one_by_two = _MM_SET1_I64x2(ONE_BY_TWO),
    .mant_8_bits = _MM_SET1_I64x2(MANT_MASK_N),
    .mant_9_bit = _MM_SET1_I64x2(0x0000080000000000ULL),

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


#define MANTISSA_N_BITS v_log_data.mant_8_bits
#define MANTISSA_9_BIT  v_log_data.mant_9_bit
#define MANTISSA_BITS   v_log_data.mantissa_bits
#define DP64_BIAS       v_log_data.float_bias
#define DP_HALF         v_log_data.one_by_two
#define V_MIN           v_log_data.v_min
#define V_MAX           v_log_data.v_max
#define V_MASK          v_log_data.v_mask
#define LN2_HEAD        v_log_data.ln2_head
#define LN2_TAIL        v_log_data.ln2_tail
#define ONE             v_log_data.one

/*
 * Short names for polynomial coefficients
 */
#define C1  _MM_SET1_PD2(v_log_data.poly[0])
#define C2  _MM_SET1_PD2(v_log_data.poly[1])
#define C3  _MM_SET1_PD2(v_log_data.poly[2])
#define C4  _MM_SET1_PD2(v_log_data.poly[3])
#define C5  _MM_SET1_PD2(v_log_data.poly[4])

/*
 *   __m128d FN_PROTOTYPE_OPT(vrd2_log)(__m128d);
 *
 * Spec:
 *   - A slightly relaxed version of the scalar powf.
 *   - Maximum ULP is expected to be less than 2.
 *   - Performance is expected to be double of scalar powf
 *
 *
 * Implementation Notes:
 *
 * Calculation of log(x):
 *      x = 2^n*(1+f)                                          ....(1)
 *
 *      log(x) = log(2^n * (1+f))
 *             = log(2^n) + log(1+f)
 *             = n*log(2) + log(1+f)                           ....(2)
 *
 *      let z = 1 + f
 *
 *      log(x) = n*log(2) + log(z)
 *      z = G + g
 *      log(x) = n*log(2) + log(G + g)
 *
 *      log(x) = n*log(2) + log(2*(G/2 + g/2))
 *
 *      log(x) = n*log(2) + log(2) + log(F + f)  with (0.5 <= F < 1) and (f <= 2 ^ (-9))
 *
 *      log(x) = m * log(2) + log(2) + log(F) + log(1 -(f / F))
 *
 *      Y = (2 ^ (-1)) * (2 ^ (-m)) * (2 ^ m) * A
 *
 *      Now, range of Y is: 0.5 <= Y < 1
 *
 *      F = 0x100 + (first 8 mantissa bits)
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
 *
 */

static inline int
v_any_u64(v_i64x2_t cond)
{
    const v_i64x2_t zero = _MM_SET1_I64x2(0);
    return _mm_testz_si128(cond, zero);
}


/*
 * On x86, 'cond' contains all 0's for false, and all 1's for true
 * IOW, 0=>false, -1=>true
 */

static inline v_f64x2_t
log_specialcase(v_f64x2_t _x,
                 v_f64x2_t result,
                 v_i64x2_t cond)
{
     return (v_f64x2_t) {
         cond[0] ? SCALAR_LOG(_x[0]) : result[0],
         cond[1] ? SCALAR_LOG(_x[1]) : result[1],
     };
}


__m128d
FN_PROTOTYPE_OPT(vrd2_log)(__m128d _x)
{

    v_u64x2_t ux = as_v_u64x2(_x);

    v_i64x2_t condition = (ux - V_MIN >= V_MAX - V_MIN);

    v_i64x2_t int_exponent =  (ux >> 52) - DP64_BIAS;

    v_u64x2_t mant  = ((ux & MANTISSA_BITS) | DP_HALF);

    v_u64x2_t mant_n = (ux & MANTISSA_9_BIT) << 1;

    v_u64x2_t index = (ux & MANTISSA_N_BITS) + mant_n;

    v_f64x2_t index_times_half = as_f64x2(index | DP_HALF);

    index =  index >> (52 - N);

    v_f64x2_t y1  = as_f64x2(mant);

    v_f64x2_t f = index_times_half - y1;

    v_f64x2_t F_INV;

    v_f64x2_t LOG_256_HEAD, LOG_256_TAIL;

    v_f64x2_t exponent, u;

    /*
     * Avoiding the use of vgatherpd instruction for performance reasons
     * The compiler is expected to unroll this loop
     *
     */

    for(int  lane = 0; lane < VECTOR_LENGTH; lane++) {

        int32_t j = index[lane];

        exponent[lane] = int_exponent[lane];

        F_INV[lane] = TAB_F_INV[j];

        LOG_256_HEAD[lane] = TAB_LOG[j].lead;

        LOG_256_TAIL[lane] = TAB_LOG[j].tail;

    }

    u = f * F_INV;

    /* Use Horner's scheme to evaluate polynomial to compute log(u)
     *
     *  q = u * (1 + u * (C1 + u * (C2 + u * (C3 + u * (C4 + u * C5)))));
     *
     */

    v_f64x2_t  poly = u * (ONE + u * (C1 + u * (C2 + u * (C3 + u * (C4 + u * C5)))));

    /* m*log(2) + log(G) - poly */

    v_f64x2_t q = (LN2_TAIL * exponent - poly) + LOG_256_TAIL;

    q += LN2_HEAD * exponent + LOG_256_HEAD;

    if (unlikely(v_any_u64(condition))) {
        return log_specialcase(_x, q, condition);
    }

    return q;

}


