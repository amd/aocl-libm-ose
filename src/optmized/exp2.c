/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 */


/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   double exp2(double x)
 *
 * Spec:
 *   exp2(1) = 2
 *   exp2(x) = 1           if x ∈ F and exp2(x) ≠ 2^x and
 *			   log2(1 - (epsilon/(2 * r))) < x  and
 *                         x < log2(1+(epsilon/2)
 *   exp2(x) = 1           if x = -0
 *   exp2(x) = +inf        if x = +inf
 *   exp2(x) = 0           if x = -inf
 *   exp2(x) = 2^x
 *
 *   exp2(x) overflows     if (approximately) x > ln(DBL_MAX). (709.782712893384)
 */

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm_special.h>

#if defined(__GNUC__) && !defined(__clang__) && !defined(ENABLE_DEBUG)
#pragma GCC push_options
#pragma GCC optimize ("O2")
#endif  /* !DEBUG */

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>

#include <libm/typehelper.h>
#include <libm/compiler.h>

/*
 * N defines the precision with which we deal with 'x'
 * I.O.W (1 << N) is the size  of the look up table
 */

#define EXP2_N 6
#define EXP2_TABLE_SIZE (1 << EXP2_N)

#if EXP2_N == 6                              /* Table size 64 */
#define EXP2_POLY_DEGREE 6

#elif EXP2_N == 7                            /* Table size 128 */
#define EXP2_POLY_DEGREE 5

#endif

#define EXP2_MAX_POLYDEGREE 8

struct exp_table {
	double main, head, tail;
};

static const struct {
	double table_size;
	double one_by_table_size;
	double ln2;
        double Huge;
	double ALIGN(16) poly[EXP2_MAX_POLYDEGREE];
	struct exp_table table[EXP2_TABLE_SIZE];
} exp2_data = {
#if EXP2_N == 10
	.table_size        = 0x1.0p+10,
	.one_by_table_size = 0x1.0p-10,
	.Huge              = 0x1.8p+42,
#elif EXP2_N == 9
	.table_size        = 0x1.0p+9,
	.one_by_table_size = 0x1.0p-9,
	.Huge		   = 0x1.8p+43,
#elif EXP2_N == 8
	.table_size        = 0x1.0p+8,
	.one_by_table_size = 0x1.0p-8,
	.Huge              = 0x1.8p+44,
#elif EXP2_N == 7
	.table_size        = 0x1.0p+7,
	.one_by_table_size = 0x1.0p-7,
	.Huge		   = 0x1.8p+45,
#elif EXP2_N == 6
	.table_size        = 0x1.0p+6,
	.one_by_table_size = 0x1.0p-6,
	.Huge		   = 0x1.8p+46,
#else
#error "EXP2_N not defined"
#endif
	.ln2  = 0x1.62e42fefa39efp-1, /* ln(2) */
        /*
         * Polynomial constants, 1/x! (reciprocal of factorial(x))
         * To make better use of cache line, we dont store 0! and 1!
         */
	.poly = {	/* skip for 0! and 1! */
		0x1.0000000000000p-1,	/* 1/2! = 1/2    */
		0x1.5555555555555p-3,	/* 1/3! = 1/6    */
		0x1.5555555555555p-5,	/* 1/4! = 1/24   */
		0x1.1111111111111p-7 ,	/* 1/5! = 1/120  */
		0x1.6c16c16c16c17p-10,	/* 1/6! = 1/720  */
		0x1.a01a01a01a01ap-13,	/* 1/7! = 1/5040 */
		0x1.a01a01a01a01ap-16,	/* 1/8! = 1/40320*/
		0x1.71de3a556c734p-19,	/* 1/9! = 1/322880*/
	},

	.table = {
#if EXP2_N == 6
#include "data/_exp_tbl_64_interleaved.data"

#elif EXP2_N == 7
#include "data/_exp_tbl_128_interleaved.data"
#endif
	},
};

/* C1 is 1 as 1! = 1 and 1/1! = 1 */
#define C2	exp2_data.poly[0]
#define C3	exp2_data.poly[1]
#define C4	exp2_data.poly[2]
#define C5	exp2_data.poly[3]
#define C6	exp2_data.poly[4]
#define C7	exp2_data.poly[5]
#define C8	exp2_data.poly[6]
#define LIBM_EXP2_HUGE		exp2_data.Huge
#define REAL_TABLE_SIZE         exp2_data.table_size
#define REAL_1_BY_TABLE_SIZE	exp2_data.one_by_table_size
#define REAL_LN2		exp2_data.ln2
#define TABLE_DATA		exp2_data.table
#define MAX_X			exp2_data.x.max
#define MIN_X			exp2_data.x.min

#define UMAX_X			0x4090000000000000
#define UMIN_X			0xc090c80000000000

#define FMAX_X			 0x1.000p+10
#define FMIN_X			-0x1.0c8p+10

double _exp2_special(double x, double y, uint32_t code);

static inline uint32_t top12(double x)
{
    return asuint64(x) >> 52;
}


double
FN_PROTOTYPE(exp2_v2)(double x)
{
    double_t    r, q, dn;
    int64_t     n, j, m;
    flt64_t     q1 = {.i = 0,};

#define EXP_X_NAN       1
#define EXP_Y_ZERO      2
#define EXP_Y_INF       3

    /*
     * Top 11 bits, ignoring sign bit
     * this is with BIAS
     */
    uint32_t exponent = top12(x) & 0x7ff;
    /*
     * 11-bit 'exponent' is compared with, 12-bit unsigned value
     * one comparison for multiple decisions
     */
    if (unlikely (exponent - top12(0x1p-54) >= top12(512.0) - top12(0x1p-54))) {
	    if (exponent - top12 (0x1p-54) >= 0x80000000)
		    return 1.0;

	    if (x >= FMAX_X) {
                if (isnan(x))
                    return  _exp2_special(x, asdouble(QNANBITPATT_DP64), EXP_Y_INF);

                return  _exp2_special(x, asdouble(PINFBITPATT_DP64), EXP_X_NAN);
	    }

	    if (x <= FMIN_X) {
		    if (asuint64(x) == NINFBITPATT_DP64)
                        return  _exp2_special(x, x, EXP_Y_ZERO);

		    return _exp2_special(x, 0.0, EXP_Y_ZERO);
	    }

	    // flag de-normals to process at the end
	    exponent = 0xfff;
    }

#define FAST_INTEGER_CONVERSION 1

#if FAST_INTEGER_CONVERSION
    q1.d = x + LIBM_EXP2_HUGE;
    n    = q1.i;
    dn   = q1.d - LIBM_EXP2_HUGE;
    r    = x - dn;
#else
    double_t a = x * REAL_TABLE_SIZE;
    n          = cast_double_to_i64(a);
    dn         = cast_i64_to_double(n);
    r          = x - (dn * REAL_1_BY_TABLE_SIZE);
#endif

    r *= REAL_LN2;

    /* table index, for lookup, truncated */
    j = n % EXP2_TABLE_SIZE;

    /* n-j/TABLE_SIZE, TABLE_SIZE = 1<<N
     * and m <<= 52
     */
    m = (n - j) << (52 - EXP2_N);

#define ESTRIN_SCHEME  0xee
#define HORNER_SCHEME  0xef

#define POLY_EVAL_METHOD ESTRIN_SCHEME

#ifndef POLY_EVAL_METHOD
#error "Polynomial evaluation method NOT defined"
#endif

#if POLY_EVAL_METHOD == HORNER_SCHEME
#if !defined(EXP2_POLY_DEGREE)
#define EXP2_POLY_DEGREE 6
#endif
#if EXP2_POLY_DEGREE == 7
    q = r * (1 + r * (C2 + r * (C3 + r * (C4 + r * (C5 + r * (C6 + r * C7))))));
#elif   EXP2_POLY_DEGREE == 6
    q = r * (1 + r * (C2 + r * (C3 + r * (C4 + r * (C5 + r * C6)))));
#elif EXP2_POLY_DEGREE == 5
    q = r * (1 + r * (C2 + r * (C3 + r * (C4 + r * C5))));
#elif EXP2_POLY_DEGREE == 4
    q = r * (1 + r * (C2 + r * (C3 + r * C4)));
#elif EXP2_POLY_DEGREE == 3
    q = r * (1 + r * (C2 + r * C3));
#else  /* Poly order <=2 */
    q = r * (1 + r * C2);
#endif	/* Order <=2 && Order == 3 */

#elif POLY_EVAL_METHOD == ESTRIN_SCHEME
    /* Estrin's */
    // r + ((r*r)*(1/2 + (r*1/6))) +
    // ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))

    double_t r2 = r * r;
    q = r + (r2 * (C2  + r * C3));

#if EXP2_POLY_DEGREE == 4
    q += (r2 * r2) *  C4; /* r^4 * C4 */
#elif EXP2_POLY_DEGREE == 5
    q += (r2 * r2) * (C4 + r*C5);
#elif EXP2_POLY_DEGREE == 6
    q += (r2 * r2) * (C4 + r * (C5 + r*C6));
#endif
#else
    #warning "POLY_EVAL_METHOD is not defined"
#endif  /* if HORNER_SCHEME || ESTRIN_SCHEME */

    /* f(j)*q + f1 + f2 */
    struct exp_table *tbl = &((struct exp_table*)TABLE_DATA)[j];
    q = q * tbl->main + tbl->head + tbl->tail;

    /*
     * Processing denormals
     */
    if (unlikely(exponent == 0xfff)) {
        int m1 = (n - j) >> EXP2_N;
        if (m1 <= -1022)
            if (m1 < -1022 || q < 1.0) {
                /* Process true de-normals */
                m1 += 1074;
                flt64u_t tmp = {.i = (1ULL << (uint8_t)m1) };
                return q * tmp.d;
            }
    }

    q1.d = asdouble(m + asuint64(q));
    return q1.d;
}

#if !defined(__GNUC__) && !defined(__clang__) && defined(ENABLE_DEBUG)
#pragma GCC pop_options
#endif

