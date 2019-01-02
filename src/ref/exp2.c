/*
 * Copyright (C) 2018, Advanced Micro Devices. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */
#include <stdint.h>

#include "libm_macros.h"

#define ALIGN(x) __attribute__((aligned(x)))

typedef union {
	int32_t         i;
	float           f;
} flt32_t;

typedef union {
	int64_t         i;
	double          d;
} flt64_t;

enum {
	LIBM_CONST_FLT_REAL_1,
	LIBM_CONST_FLT_REAL_64,

	/*
	 * Known logarithms
	 */
	LIBM_CONST_FLT_REAL_LN2,

	/*
	 * Reciprocals
	 */
	LIBM_CONST_FLT_1_BY_2,
	LIBM_CONST_FLT_1_BY_6,
	LIBM_CONST_FLT_1_BY_24,
	LIBM_CONST_FLT_1_BY_120,
	LIBM_CONST_FLT_1_BY_720,
};

enum {
	LIBM_CONST_DBL_REAL_1,
	LIBM_CONST_DBL_REAL_64,
	LIBM_CONST_DBL_REAL_1024,

	/*
	 * Known logarithms
	 */
	LIBM_CONST_DBL_LN2,

	/*
	 * Reciprocals
	 */
	LIBM_CONST_DBL_1_BY_2,
	LIBM_CONST_DBL_1_BY_6,
	LIBM_CONST_DBL_1_BY_24,
	LIBM_CONST_DBL_1_BY_64,
	LIBM_CONST_DBL_1_BY_120,
	LIBM_CONST_DBL_1_BY_720,
	LIBM_CONST_DBL_1_BY_1024,
};

//#define RODATA __attribute__((section (".rodata")))

// Polynomial constants, 1/x! (reciprocal of factorial(x))
#define MAX_POLYORDER 8
flt64_t poly[MAX_POLYORDER + 2] = {	/* for 0! and 1! */
	{.i = 0x3ff0000000000000ULL}, 	/* 1/0! = 1/1    */
	{.i = 0x3ff0000000000000ULL},	/* 1/1! = 1/1    */
	{.i = 0x3fe0000000000000ULL},	/* 1/2! = 1/2    */
	{.i = 0x3fc5555555555555ULL},	/* 1/3! = 1/6    */
	{.i = 0x3fa5555555555555ULL},	/* 1/4! = 1/24   */
	{.i = 0x3f81111111111111ULL},	/* 1/5! = 1/120  */
	{.i = 0x3f56c16c16c16c17ULL},	/* 1/6! = 1/720  */
	{.i = 0x3f2a01a01a01a01aULL},	/* 1/7! = 1/5040 */
	{.i = 0x0},			/* 1/8! = 1/40320*/
	{.i = 0x0},			/* 1/9!          */
};

#define C2	poly[2].d
#define C3	poly[3].d
#define C4	poly[4].d
#define C5	poly[5].d
#define C6	poly[6].d
#define C7	poly[7].d

flt64_t math_table_64[] = {
	/*
	 * Integers with real equivalant
	 */
	[LIBM_CONST_DBL_REAL_1]   = { .i = 0x3ff0000000000000ULL},
	[LIBM_CONST_DBL_REAL_64]  = { .i = 0x4050000000000000ULL},
	[LIBM_CONST_DBL_REAL_1024]= { .i = 0x4090000000000000ULL},
	/*
	 * Reciprocals
	 */
	[LIBM_CONST_DBL_1_BY_2]	  = { .i = 0x3fe0000000000000ULL},
	[LIBM_CONST_DBL_1_BY_6]	  = { .i = 0x3fc5555555555555ULL},
	[LIBM_CONST_DBL_1_BY_24]  = { .i = 0x3fa5555555555555ULL},
	[LIBM_CONST_DBL_1_BY_64]  = { .i = 0x3F90000000000000ULL},
	[LIBM_CONST_DBL_1_BY_120] = { .i = 0x3f81111111111111ULL},
	[LIBM_CONST_DBL_1_BY_720] = { .i = 0x3f56c16c16c16c17ULL},
	[LIBM_CONST_DBL_1_BY_1024]= { .i = 0x3F50000000000000ULL},
	[LIBM_CONST_DBL_LN2]	  = { .i = 0x3FE62E42FEFA39EFULL},	/* ln(2) */
};

static inline   int32_t
cast_double_to_int32( double x )
{
	return ( int32_t ) x;
}

static inline double
cast_i32_to_double( int32_t x )
{
	return ( double )x;
}

static inline double
cast_i64_to_double( int64_t x )
{
	return ( double )x;
}

extern double   __two_to_jby64_table[];
extern double   __two_to_jby64_tail_table[];
extern double   __two_to_jby64_head_table[];

extern double  __two_to_jby1024_table[];
extern double  __two_to_jby1024_tail_table[];
extern double  __two_to_jby1024_head_table[];

#define OPT_O1 __attribute__((optimize("O1")))
#define OPT_O2 __attribute__((optimize("O2")))
#define OPT_O3 __attribute__((optimize("O3")))
#define OPT_Og __attribute__((optimize("Og")))

#define OPTIMIZE(x) __attribute__((optimize("O"#x)))

double  OPTIMIZE(3)
FN_PROTOTYPE( exp2_v1 ) ( double x )
{
	double          a, dn, r, q;
	int64_t         n, j;

	flt64_t         m, q1;

	// a = x * 64.0
	a = x * math_table_64[LIBM_CONST_DBL_REAL_64].d;

	// n = (int) a
	n = ( int64_t ) a;

	// dn = (double) n
	dn = ( double ) n;

	// r = x - (dn / 64)
	r = x - ( dn * math_table_64[LIBM_CONST_DBL_1_BY_64].d );

	r *= math_table_64[LIBM_CONST_DBL_LN2].d;

	j = n & 0x3f;                      /* last 6-digits, index to lookup */

	m.i = ( n - j ) / 64;

	// r + ((r*r)*(1/2 + (r*1/6))) +
	// ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))

	double r2 = r * r;
	q = r + ( r2) *
		( math_table_64[LIBM_CONST_DBL_1_BY_2].d +
		  r * ( math_table_64[LIBM_CONST_DBL_1_BY_6].d ) );

	q += ( ( r2 ) * ( r2 ) ) *   /* r^4 */
		( math_table_64[LIBM_CONST_DBL_1_BY_24].d +
		  r * ( math_table_64[LIBM_CONST_DBL_1_BY_120].d +
			  ( r * math_table_64[LIBM_CONST_DBL_1_BY_720].d ) ) );

	// f(j)*q + f1 + f2
	q1.d = ( q * __two_to_jby64_table[j] ) +
		__two_to_jby64_tail_table[j] + __two_to_jby64_head_table[j];

	/*
	 * Processing denormals
	 */
	if ( ( m.i <= -1022LL ) ) {        /* -1022 */
		if ( q1.d > math_table_64[LIBM_CONST_DBL_REAL_1].d )
			goto process_normals;
		if ( m.i < -1022LL ) {
			/*
			 * Process true de-normals
			 */
			m.i += 1074;
			flt64_t         tmp = {.i = ( 1 << ( uint8_t ) m.i ) };
			q1.d *= tmp.d;
		}
	}

process_normals:
	q1.i += ( m.i << 52 );

	return q1.d;
}

double OPTIMIZE(3)
FN_PROTOTYPE( exp2_v2 )( double x )
{
	double          a, dn, r, q;
	int64_t         n, j;

	flt64_t         m, q1;

	a = x * math_table_64[LIBM_CONST_DBL_REAL_1024].d;

	n = ( int64_t ) a;

	dn = ( double ) n;

	r = x - (dn * math_table_64[LIBM_CONST_DBL_1_BY_1024].d);

	r *= math_table_64[LIBM_CONST_DBL_LN2].d;

	j = n & 0x3ff;                      /* last 6-digits, index to lookup */

	m.i = (n - j) / 1024;

#define HORNORS_POLY 1
#ifdef HORNORS_POLY
	q = r * (1 +
		 r * (C2 +
		      r * (C3 +
			   r * (C4 +
				r * (C5 +
				     r * C6)))));
#else  /* Estrin's */
	// r + ((r*r)*(1/2 + (r*1/6))) +
	// ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))

	double r2 = r * r;
	q = r + (r2 * (C2  +
		       r * C3 );

	q += (r2 * r2) *   /* r^4 */
		 (C4 +
		  r * (C5 +
		       (r * C6)));
#endif

	// f(j)*q + f1 + f2
	q1.d = q * __two_to_jby1024_table[j] +
	       __two_to_jby1024_head_table[j] +
	       __two_to_jby1024_tail_table[j];

	/*
	 * Processing denormals
	 */
	if ( m.i <= -1022LL ) {        /* -1022 */
		if ( q1.d > math_table_64[LIBM_CONST_DBL_REAL_1].d )
			goto process_normals;
		if ( m.i < -1022LL ) {
			/*
			 * Process true de-normals
			 */
			m.i += 1074;
			flt64_t tmp = {.i = ( 1 << ( uint8_t ) m.i ) };
			q1.d *= tmp.d;
		}
	}

process_normals:
	q1.i += ( m.i << 52 );

	return q1.d;

}
