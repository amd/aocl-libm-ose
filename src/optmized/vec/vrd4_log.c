/*
 * Copyright (C) 2018-2020, Advanced Micro Devices. All rights reserved.
 *
 *
 */

#include <libm_util_amd.h>
#include <libm_special.h>
#include <libm_util_amd.h>
#include <libm_special.h>
#include <libm/poly-vec.h>
#include <libm_macros.h>
#include <libm_amd.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>

/* Contains implementation of double log(double x)
 * Reduce x into the form:
 *        x = (-1)^s*2^n*m
 * s will be always zero, as log is defined for posititve numbers
 * n is an integer known as the exponent
 * m is mantissa
 *
 * x is reduced such that the mantissa, m lies in [2/3,4/3]
 *      x = 2^n*m where m is in [2/3,4/3]
 *      log(x) = log(2^n*m)         		We have log(a*b) = log(a)+log(b)
 *             = log(2^n) + log(m)   		We have log(a^n) = n*log(a)
 *             = n*log(2) + log(m)
 *             = n*log(2) + log(1+(m-1))
 *             = n*log(2) + log(1+f)        Where f = m-1
 *             = n*log(2) + log1p(f)        f lies in [-1/3,+1/3]
 *
 * Thus we have :
 * log(x) = n*log(2) + log1p(f)
 * In the above, the first term n*log(2), n can be calculated by using rightshift operator and the value of log(2)
 * is known and is stored as a constant
 * The second term log1p(F) is approximated by using a polynomial
 */

#define VECTOR_SIZE 4

static struct {
    double poly_log[20];
    v_f64x4_t ln2, ln2_head, ln2_tail;
    v_u64x4_t inf, v_max, v_min;
    v_u64x4_t two_by_three;
} log_data = {
    .two_by_three = _MM_SET1_I64(0x3fe5555555555555),
    .inf = _MM_SET1_I64(0xfff0000000000000),
    .ln2 = _MM_SET1_PD4(0x1.62e42fefa39efp-1),
    .ln2_head = _MM_SET1_PD4(0x1.63p-1),
    .ln2_tail = _MM_SET1_PD4(-0x1.bd0105c610ca8p-13),
    .v_max = _MM_SET1_I64(0x7ff0000000000000),
    .v_min = _MM_SET1_I64(0x0010000000000000),
    /* Polynomial coefficients obtained using fpminimax algorithm from Sollya */
    .poly_log = {
        0x1.0p0,
        -0x1.ffffffffffff8p-2,
        0x1.5555555555b1p-2,
        -0x1.00000000014eep-2,
        0x1.99999998c65d2p-3,
        -0x1.5555555359624p-3,
        0x1.24924982d3265p-3,
        -0x1.000000b030e18p-3,
        0x1.c71c47299f643p-4,
        -0x1.9999569fc809fp-4,
        0x1.74629af3a9782p-4,
        -0x1.555cf41b1e0bfp-4,
        0x1.3aa51860d5cd6p-4,
        -0x1.24080742fb868p-4,
        0x1.1660dbc68088p-4,
        -0x1.061fa86db3d64p-4,
        0x1.9471a01ce7ab9p-5,
        -0x1.73ac251462367p-5,
        0x1.6c6c793f08f4dp-4,
        -0x1.63f7ba7a7111cp-4,
    },
};

static inline v_f64x4_t
log_specialcase(v_f64x4_t _x,
                 v_f64x4_t result,
                 v_i64x4_t cond)
{
    return v_call_f64(ALM_PROTO(log), _x, result, cond);
}

#define EXPSHIFTBITS_SP64 52
#define ln2 log_data.ln2
#define ln2_head log_data.ln2_head
#define ln2_tail log_data.ln2_tail
#define TWO_BY_THREE log_data.two_by_three
#define INF log_data.inf
#define C0  _MM_SET1_PD4(0.0)
#define C1  _MM_SET1_PD4(log_data.poly_log[0])
#define C2  _MM_SET1_PD4(log_data.poly_log[1])
#define C3  _MM_SET1_PD4(log_data.poly_log[2])
#define C4  _MM_SET1_PD4(log_data.poly_log[3])
#define C5  _MM_SET1_PD4(log_data.poly_log[4])
#define C6  _MM_SET1_PD4(log_data.poly_log[5])
#define C7  _MM_SET1_PD4(log_data.poly_log[6])
#define C8  _MM_SET1_PD4(log_data.poly_log[7])
#define C9  _MM_SET1_PD4(log_data.poly_log[8])
#define C10 _MM_SET1_PD4(log_data.poly_log[9])
#define C11 _MM_SET1_PD4(log_data.poly_log[10])
#define C12 _MM_SET1_PD4(log_data.poly_log[11])
#define C13 _MM_SET1_PD4(log_data.poly_log[12])
#define C14 _MM_SET1_PD4(log_data.poly_log[13])
#define C15 _MM_SET1_PD4(log_data.poly_log[14])
#define C16 _MM_SET1_PD4(log_data.poly_log[15])
#define C17 _MM_SET1_PD4(log_data.poly_log[16])
#define C18 _MM_SET1_PD4(log_data.poly_log[17])
#define C19 _MM_SET1_PD4(log_data.poly_log[18])
#define C20 _MM_SET1_PD4(log_data.poly_log[19])
#define V_MIN log_data.v_min
#define V_MAX log_data.v_max


__m256d
ALM_PROTO_OPT(vrd4_log) (__m256d x)
{

    v_f64x4_t m, r, n, f;

    v_i64x4_t ix;

    ix = as_v_u64x4(x);

    v_i64x4_t condition = (as_v_u64x4(x) - V_MIN >= V_MAX - V_MIN);

    ix = (ix - TWO_BY_THREE) & INF;

    v_i64x4_t int_exponent = ix >> EXPSHIFTBITS_SP64;

    v_i32x4_t int32_exponent;

    for(int i = 0; i < VECTOR_SIZE; i++) {

       int32_exponent[i] = int_exponent[i];

    }

    n = _mm256_cvtepi32_pd(int32_exponent);

	/* Reduce the mantissa, m to [2/3, 4/3] */

    m = as_f64(as_v_u64x4(x) - ix);

    f = m - C1;			/* f is in [-1/3,+1/3] */

    /* Compute log1p(f) using Polynomial approximation
     *
     * r = C0 + f*C1 + f^2*C2 + f^3*C3 + .... + f^20*C20
     *
     */

    r =  POLY_EVAL_20(f, C0, C1, C2, C3, C4, C5, C6, C7,
                         C8, C9, C10, C11, C12, C13, C14,
                         C15, C16, C17, C18, C19, C20);

	/* Addition by using head and tail */

    r = n * ln2_head + (n * ln2_tail + r);

    if (unlikely(v4_any_u64_loop(condition))) {
           return log_specialcase(x, r, condition);
    }

    return r;
}


