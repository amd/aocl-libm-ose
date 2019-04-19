/*
 * Copyright (C) 2018, Advanced Micro Devices. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */
#include <stdint.h>
#include <libm_util_amd.h>
#include <libm_amd_paths.h>
#include <libm_special.h>

#if !defined(ENABLE_DEBUG)
#pragma GCC push_options
#pragma GCC optimize ("O2")
#endif  /* !DEBUG */

#include <libm_macros.h>
#include <libm/types.h>

#include <libm/typehelper.h>
#include <libm/compiler.h>

/*
 * N defines the precision with which we deal with 'x'
 * I.O.W (1 << N) is the size  of the look up table
 */

#define N 10
#define TABLE_SIZE (1ULL << N)

#if N == 6                              /* Table size 64 */
extern double exp_v2_two_to_jby64_table[];
#define TABLE_DATA exp_v2_two_to_jby64_table
#define POLY_DEGREE 6

#elif N == 7                            /* Table size 128 */
extern double exp_v2_two_to_jby128_table[];
#define TABLE_DATA exp_v2_two_to_jby128_table
#define POLY_DEGREE 5

#elif N == 8                            /* Table size 256 */
extern double exp_v2_two_to_jby256_table[];
#define TABLE_DATA exp_v2_two_to_jby256_table
#define POLY_DEGREE 4

#elif N == 9                            /* Table size 512 */
extern double exp_v2_two_to_jby512_table[];
#define TABLE_DATA exp_v2_two_to_jby512_table
#define POLY_DEGREE 4

#elif N == 10                           /* Table size 1024 */
extern double exp_v2_two_to_jby1024_table[];
#define TABLE_DATA exp_v2_two_to_jby1024_table
#define POLY_DEGREE 4

#endif

#define MAX_POLYDEGREE 8

struct exp_table {
	double main, head, tail;
};

static struct {
	double table_size;
	double one_by_table_size;
	double ln2;
        double Huge;
	double ALIGN(16) poly[MAX_POLYDEGREE];
} exp2_data = {
#if N == 10
	.table_size        = 0x1.0p+10,
	.one_by_table_size = 0x1.0p-10,
	.Huge              = 0x1.8p+42,
#elif N == 9
	.table_size        = 0x1.0p+9,
	.one_by_table_size = 0x1.0p-9,
	.Huge		   = 0x1.8p+43,
#elif N == 8
	.table_size        = 0x1.0p+8,
	.one_by_table_size = 0x1.0p-8,
	.Huge              = 0x1.8p+44,
#elif N == 7
	.table_size        = 0x1.0p+7,
	.one_by_table_size = 0x1.0p-7,
	.Huge		   = 0x1.8p+45,
#elif N == 6
	.table_size        = 0x1.0p+6,
	.one_by_table_size = 0x1.0p-6,
	.Huge		   = 0x1.8p+46,
#else
#error "N not defined"
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
};

/* C1 is 1 as 1! = 1 and 1/1! = 1 */
#define C2	exp2_data.poly[0]
#define C3	exp2_data.poly[1]
#define C4	exp2_data.poly[2]
#define C5	exp2_data.poly[3]
#define C6	exp2_data.poly[4]
#define C7	exp2_data.poly[5]
#define C8	exp2_data.poly[6]
#define HUGE	exp2_data.Huge
#define REAL_TABLE_SIZE         exp2_data.table_size
#define REAL_1_BY_TABLE_SIZE	exp2_data.one_by_table_size
#define REAL_LN2		exp2_data.ln2

double _exp2_special(double x, double y, uint32_t code);

static inline uint32_t top12(double x)
{
    return asuint64(x) >> 52;
}

/*
 * to avoid compiler optimization
 */
static inline double eval_as_double(double x)
{
    return x;
}

double
FN_PROTOTYPE(exp2_v2)(double x)
{
    double_t    r, q, dn;
    int64_t     n, j, m;
    flt64_t     q1 = {.i = 0,}, q2 = q1;

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
    if (unlikely(exponent > 0x409)) {
        if (exponent >= 0xc09)
            return _exp2_special(x, 0, EXP_Y_ZERO);

	if (exponent >= 0x409)
            return _exp2_special(x, asdouble(PINFBITPATT_DP64), EXP_Y_INF);
    }

#define FAST_INTEGER_CONVERSION 1

#if FAST_INTEGER_CONVERSION
    q1.d = x + HUGE;
    n    = q1.i;
    dn   = q1.d - HUGE;
    r    = x - dn;
#else
    double_t a = x * REAL_TABLE_SIZE;
    n          = cast_double_to_i64(a);
    dn         = cast_i64_to_double(n);
    r          = x - (dn * REAL_1_BY_TABLE_SIZE);
#endif

    r *= REAL_LN2;

    /* table index, for lookup, truncated */
    j = n % TABLE_SIZE;

    /* n-j/TABLE_SIZE, TABLE_SIZE = 1<<N
     * and m <<= 52
     */
    m = (n - j) << (52 - N);


#define ESTRIN_SCHEME  0xee
#define HORNER_SCHEME  0xef

#define POLY_EVAL_METHOD ESTRIN_SCHEME

#ifndef POLY_EVAL_METHOD
#error "Polynomial evaluation method NOT defined"
#endif

#if POLY_EVAL_METHOD == HORNER_SCHEME
#if !defined(POLY_DEGREE)
#define POLY_DEGREE 6
#endif
#if POLY_DEGREE == 7
    q = r * (1 + r * (C2 + r * (C3 + r * (C4 + r * (C5 + r * (C6 + r * C7))))));
#elif   POLY_DEGREE == 6
    q = r * (1 + r * (C2 + r * (C3 + r * (C4 + r * (C5 + r * C6)))));
#elif POLY_DEGREE == 5
    q = r * (1 + r * (C2 + r * (C3 + r * (C4 + r * C5))));
#elif POLY_DEGREE == 4
    q = r * (1 + r * (C2 + r * (C3 + r * C4)));
#elif POLY_DEGREE == 3
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

#if POLY_DEGREE == 4
    q += (r2 * r2) *  C4; /* r^4 * C4 */
#elif POLY_DEGREE == 5
    q += (r2 * r2) * (C4 + r*C5);
#elif POLY_DEGREE == 6
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
    if (unlikely(exponent == 0)) {
        if (m < -1023) {
            /* Process true de-normals */
            m += 1074;
            flt64u_t tmp = {.i = (1 << (uint8_t)m) };
            q2.d = q * tmp.d;
            return q2.d;
        }
    }

    q1.i = m + asuint64(q);
    return q1.d;


#if defined(__ENABLE_IEEE_EXCEPTIONS)

#endif
}

#if !defined(ENABLE_DEBUG)
#pragma GCC pop_options
#endif
