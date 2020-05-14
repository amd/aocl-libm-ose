/*
 * Copyright (C) 2018-2020, Advanced Micro Devices. All rights reserved.
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
    double head;
    double tail;
} lookup_data;


#define VECTOR_LENGTH 4
#define N 10
#define TABLE_SIZE (1ULL << N)
#define MAX_POLYDEGREE  8
extern lookup_data log_f_256[];
extern lookup_data log_Finv[];
extern lookup_data exp_lookup[];
#define TWO_POWER_J_BY_N exp_lookup
#define TAB_F_INV log_Finv
#define TAB_LOG   log_f_256
#define MANT_MASK_N  (0x000FFC0000000000ULL)
#define MANT_MASK_N1 (0x0000080000000000ULL)
#define DOUBLE_PRECISION_BIAS 1023
#define DOUBLE_PRECISION_MANTISSA 0x000fffffffffffffULL
#define ONE_BY_TWO 0x3fe0000000000000ULL

static struct {
    v_u64x4_t v_min, v_max, mantissa_bits, one_by_two, mant_8_bits;
    v_i64x4_t float_bias;
    double ALIGN(16) poly[MAX_POLYDEGREE];
    v_f64x4_t ln2_head, ln2_tail;
} v_log_data = {
    .ln2_head = _MM_SET1_PD4(0x1.62e42e0000000p-1), /* ln(2) head*/
    .ln2_tail = _MM_SET1_PD4(0x1.efa39ef35793cp-25), /* ln(2) tail*/
    .v_min  = _MM_SET1_I64(0x0010000000000000),
    .v_max  = _MM_SET1_I64(0x7ff0000000000000),
    .float_bias =  _MM_SET1_I64(DOUBLE_PRECISION_BIAS),
    .mantissa_bits = _MM_SET1_I64(DOUBLE_PRECISION_MANTISSA),
    .one_by_two = _MM_SET1_I64(ONE_BY_TWO),
    .mant_8_bits = _MM_SET1_I64(MANT_MASK_N),

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
    v_f64x4_t ln2_by_n_head, ln2_by_n_tail;
    v_i64x4_t one;
    double_t ALIGN(16) poly[MAX_POLYDEGREE];
    v_u64x4_t exp_max;
} exp_v4_data  = {
    .ln2_by_n_head = _MM_SET1_PD4(0x1.62e42f0000000p-11),
    .ln2_by_n_tail = _MM_SET1_PD4(0x1.DF473DE6AF279p-36),
    .tblsz_byln2 = _MM_SET1_PD4(0x1.71547652b82fep10),
    .one =  _MM_SET1_I64(0x3ff0000000000000ULL),
    .Huge = _MM_SET1_PD4(0x1.8000000000000p+52),
    .exp_max = _MM_SET1_I64(0x4086200000000000),
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

#define D1  _MM_SET1_PD4(0x1.0000000000000p-1)
#define D2  _MM_SET1_PD4(exp_v4_data.poly[0])
#define D3  _MM_SET1_PD4(exp_v4_data.poly[1])
#define D4  _MM_SET1_PD4(exp_v4_data.poly[2])
#define D5  _MM_SET1_PD4(exp_v4_data.poly[3])

#define C1  _MM_SET1_PD4(v_log_data.poly[0])
#define C2  _MM_SET1_PD4(v_log_data.poly[1])
#define C3  _MM_SET1_PD4(v_log_data.poly[2])
#define C4  _MM_SET1_PD4(v_log_data.poly[3])

/*
 *   __m256d FN_PROTOTYPE_OPT(vrd4_pow)(__m256d, __m256d);
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
check_condition(v_i64x4_t* cond1, v_i64x4_t cond2)
{

    int32_t ret = 0;

    for(int i = 0; i < 4; i++) {
        if((*cond1)[i] || cond2[i]){
            (*cond1)[i] = 1;
            ret = 1;
         }
    }

    return ret;
}

static inline v_f64x4_t
pow_specialcase(v_f64x4_t _x,
                 v_f64x4_t _y,
                 v_f64x4_t result,
                 v_i64x4_t cond)
{
    return v_call2_f64(FN_PROTOTYPE(pow), _x, _y, result, cond);
}


__m256d
FN_PROTOTYPE_OPT(vrd4_pow)(__m256d _x,__m256d _y)
{
    __m256d result;

    v_u64x4_t ux = as_v_u64x4(_x);

    v_i32x4_t int32_exponent;

    v_i64x4_t condition = (ux - V_MIN >= V_MAX - V_MIN);

    v_i64x4_t int_exponent =  (ux >> 52) - DP64_BIAS;

    v_u64x4_t mant  = ((ux & MANTISSA_BITS) | DP_HALF);

    v_u64x4_t index = ux & MANTISSA_N_BITS;

    v_f64x4_t index_times_half = as_f64(index | DP_HALF);

    index =  index >> (52 - N);

    v_f64x4_t y1  = as_f64(mant);

    v_f64x4_t f = index_times_half - y1;

    v_f64x4_t F_INV_HEAD, F_INV_TAIL;

    v_f64x4_t LOG_256_HEAD, LOG_256_TAIL;

    v_f64x4_t j_by_N_head, j_by_N_tail;

    v_f64x4_t r, r1, u;

    /* Avoiding the use of vgatherpd instruction for performance reasons */

    for(int  lane = 0; lane < 4; lane++) {

        int32_t j = index[lane];

        int32_exponent[lane] = int_exponent[lane];

        F_INV_HEAD[lane] = TAB_F_INV[j].head;

        F_INV_TAIL[lane] = TAB_F_INV[j].tail;

        LOG_256_HEAD[lane] = TAB_LOG[j].head;

        LOG_256_TAIL[lane] = TAB_LOG[j].tail;

    }

    v_f64x4_t exponent =  _mm256_cvtepi32_pd (int32_exponent);

    r = f * F_INV_TAIL;

    r1 = f * F_INV_HEAD;

    u = r + r1;

    v_f64x4_t z =  r1 - u;

    /* Use Estrin's polynomial to compute log(u) */

    v_f64x4_t u2 = u * u;

    v_f64x4_t a1 = C1  + u * C2;

    v_f64x4_t a2 = C3  + u * C4;

    v_f64x4_t u4 = u2 * u2;

    v_f64x4_t b1 = u2 * a1;

    v_f64x4_t  q = b1 + u4 * a2;

    v_f64x4_t poly = (z + r) + q;

    /* m*log(2) + log(G) - poly */

    v_f64x4_t temp = (LN2_TAIL * exponent - poly) + LOG_256_TAIL;

    v_f64x4_t resT  = temp - u;

    v_f64x4_t resH = LN2_HEAD * exponent + LOG_256_HEAD;

    v_f64x4_t logx_h = resH + resT;

    v_f64x4_t logx_t = resH - logx_h + resT;

    v_f64x4_t ylogx_h = logx_h * _y;

    v_f64x4_t ylogx_t = logx_t * _y + _mm256_fmadd_pd(logx_h, _y, -ylogx_h);

    /* Calculate exp */

    v_u64x4_t v = as_v_u64x4(ylogx_h);

    /* check if y*log(x) > 1024*ln(2) */
    v_i64x4_t condition2 = (v >= EXP_MAX);

    z = ylogx_h * INVLN2;

    v_f64x4_t dn = z + EXP_HUGE;

    v_i64x4_t n = as_v_u64x4(dn);

    dn = dn - EXP_HUGE;

    index = n & DP64_BIAS;

    r = ylogx_h - (dn * LOG2_BY_N_HEAD);

    v_i64x4_t m = ((n - index) << (52 - N)) + ONE;

    r = (r - (LOG2_BY_N_TAIL * dn)) + ylogx_t;

    v_f64x4_t qtmp1 = D1 + (D2 * r);

    v_f64x4_t r2 = r * r;

    v_f64x4_t qtmp3 = r + (r2 * qtmp1);

    poly = qtmp3 + r2 * r2  * D3;

    for (int lane = 0; lane < 4; lane++) {

        int32_t j = index[lane];

        j_by_N_head[lane] = TWO_POWER_J_BY_N[j].head;

        j_by_N_tail[lane] = TWO_POWER_J_BY_N[j].tail;

    }

    q = j_by_N_tail + poly *  j_by_N_tail;

    z = poly * j_by_N_head;

    r = j_by_N_head + (z + q);

    result = r * as_f64(m);

    if (unlikely(check_condition(&condition, condition2))) {
        return pow_specialcase(_x, _y, result, condition);
    }

    return result;
}


