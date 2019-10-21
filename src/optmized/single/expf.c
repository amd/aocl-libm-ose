/*
 * Copyright (C) 2019, AMD. All rights reserved.
 *
G * Author: Prem Mallappa <pmallapp@amd.com>
 */

/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   double exp(double x)
 *
 * Spec:
 *   exp(1) = e
 *   exp(x) = 1           if x ∈ F and exp(x) ≠ eˣ
 *   exp(x) = 1           if x = -0
 *   exp(x) = +inf        if x = +inf
 *   exp(x) = 0           if x = -inf
 *   exp(x) = eˣ
 *
 *   exp(x) overflows     if (approximately) x > ln(FLT_MAX) i.e., 88.72..
 */

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm_special.h>

#include <libm_macros.h>
#include <libm/types.h>

#include <libm/typehelper.h>
#include <libm/compiler.h>


#define EXPF_N 6

#if EXPF_N == 5
#define EXPF_POLY_DEGREE 3
#elif EXPF_N == 4
#define EXPF_POLY_DEGREE 3
#endif

#define EXPF_TABLE_SIZE (1 << EXPF_N)
#define EXPF_MAX_POLY_DEGREE 3

/*
 * expf_data.h needs following to be defined before include
 *    - EXPF_N
 *    - EXPF_POLY_DEGREE
 *    - EXPF_TABLE_SIZE
 *    - EXPF_MAX_POLY_DEGREE
 */

#include "expf_data.h"

static const struct expf_data expf_v2_data = {
    .ln2by_tblsz = 0x1.62e42fefa39efp-7,
    .tblsz_byln2 = 0x1.71547652b82fep+6,

    .Huge = 0x1.8000000000000p+52,
#if EXPF_N == 6
    .table_v3 = (double*)L__two_to_jby64_table,
#elif EXPF_N == 5
    .table_v3 = (double*)L__two_to_jby32_table,,
#endif

    .poly = {
        0x1.0000000000000p-1,	/* 1/2! = 1/2    */
        0x1.5555555555555p-3,	/* 1/3! = 1/6    */
        0x1.5555555555555p-5,	/* 1/4! = 1/24   */
    },
};

#define C1	expf_v2_data.poly[0]
#define C2	expf_v2_data.poly[1]

#define EXPF_LN2_BY_TBLSZ  expf_v2_data.ln2by_tblsz
#define EXPF_TBLSZ_BY_LN2  expf_v2_data.tblsz_byln2
#define EXPF_HUGE	   expf_v2_data.Huge
#define EXPF_TABLE         expf_v2_data.table

#define EXPF_FARG_MIN -0x1.9fe368p6f    /* log(0x1p-150) ~= -103.97 */
#define EXPF_FARG_MAX  0x1.62e42ep6f    /* log(0x1p128)  ~=   88.72  */

static uint32_t
top12f(float x)
{
    flt32_t f = {.f = x};
    return f.i >> 20;
}

/******************************************
* Implementation Notes
* ---------------------
*
* 0. Choose 'N' as 5, EXPF_TBL_SZ = 2^N i.e 32
*
* 1. Argument Reduction
 ******************************************/
#undef EXPF_N
#define EXPF_N 6

#undef EXPF_TABLE_SIZE
#define EXPF_TABLE_SIZE (1 << EXPF_N)

float
FN_PROTOTYPE(expf_v2)(float x)
{
    double  q, dn, r;
    flt64_t q2;
    uint64_t n, j;

    uint32_t top = top12f(x);
    if (unlikely (top > top12f(88.0f))) {
        if (asuint32(x) == asuint32(-INFINITY))
            return 0.0f;

        if (x > EXPF_FARG_MAX)
            return asfloat(PINFBITPATT_SP32);

        if (x < EXPF_FARG_MIN)
            return 0.0f;
    }

    double dx = (x * EXPF_TBLSZ_BY_LN2) + 0.5;

    /*
     * n  = (int) scale(x)
     * dn = (double) n
     */
#undef FAST_INTEGER_CONVERSION
#define FAST_INTEGER_CONVERSION 1
#if FAST_INTEGER_CONVERSION
    flt64_t q1;
    q1.d = dx + EXPF_HUGE;
    n    = q1.i;
    dn   = q1.d - EXPF_HUGE;
#else
    n  = cast_float_to_i32(dx);
    dn = cast_i32_to_float(n);
#endif

    r  = x - dn * EXPF_LN2_BY_TBLSZ;

    j  = n % EXPF_TABLE_SIZE;

    uint64_t m = (n - j) >> EXPF_N;

    double r2 = r * r;
    q  = r + r2 * (C1 + C2 * r);

    double tbl = expf_v2_data.table_v3[j];

    q2.d = tbl + (tbl * q);

    q2.i += (m << 52);

    return (float)q2.d;
}


