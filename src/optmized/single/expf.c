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
#define EXPF_POLY_DEGREE 4
#if EXPF_N == 5
#define EXPF_POLY_DEGREE 3
#elif EXPF_N == 4
#define EXPF_POLY_DEGREE 3
#endif

#define EXPF_TABLE_SIZE (1 << EXPF_N)
#define EXPF_MAX_POLY_DEGREE 4

#define EXP_Y_ZERO      2
#define EXP_Y_INF       3
/*
 * expf_data.h needs following to be defined before include
 *    - EXPF_N
 *    - EXPF_POLY_DEGREE
 *    - EXPF_TABLE_SIZE
 *    - EXPF_MAX_POLY_DEGREE
 */

#include "expf_data.h"

static struct expf_data expf_v2_data = {
    .ln2by_tblsz = 0x1.62e42fefa39efp-7,
    .tblsz_byln2 = 0x1.71547652b82fep+6,
    .Huge = 0x1.8000000000000p+52,
#if EXPF_N == 6
    .table_v3 = (double*)L__two_to_jby64_table,
#elif EXPF_N == 5
    .table_v3 = (double*)L__two_to_jby32_table,
#endif

    .poly = {
        1.0,    /* 1/1! = 1 */
        0x1.0000000000000p-1,   /* 1/2! = 1/2    */
        0x1.5555555555555p-3,   /* 1/3! = 1/6    */
        0x1.cacccaa4ba57cp-5,   /* 1/4! = 1/24   */
    },
};

#define C1	expf_v2_data.poly[0]
#define C2	expf_v2_data.poly[1]
#define C3  expf_v2_data.poly[2]
#define C4  expf_v2_data.poly[3]

#define EXPF_LN2_BY_TBLSZ  expf_v2_data.ln2by_tblsz
#define EXPF_TBLSZ_BY_LN2  expf_v2_data.tblsz_byln2
#define EXPF_HUGE	   expf_v2_data.Huge
#define EXPF_TABLE         expf_v2_data.table

#define EXPF_FARG_MIN -0x1.9fe368p6f    /* log(0x1p-150) ~= -103.97 */
#define EXPF_FARG_MAX  0x1.62e42ep6f    /* log(0x1p128)  ~=   88.72  */

double _exp_special(double x, double y, uint32_t code);

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
FN_PROTOTYPE_OPT(expf)(float x)
{
    double_t  q, dn, r, z;
    uint64_t n, j;

    uint32_t top = top12f(x);

    if (unlikely (top > top12f(88.0f))) {
        if(isnanf(x))
            return x;

        if (asuint32(x) == asuint32(-INFINITY))
            return 0.0f;

        if (x > EXPF_FARG_MAX){
            if(asuint32(x) == PINFBITPATT_SP32)
                return asfloat(PINFBITPATT_SP32);

            return  _exp_special(x, asdouble(PINFBITPATT_DP64),  EXP_Y_INF);
        }

        if (x < EXPF_FARG_MIN){
            return _exp_special(x, 0.0, EXP_Y_ZERO);;
        }
    }

    z = x *  EXPF_TBLSZ_BY_LN2;

    /*
     * n  = (int) scale(x)
     * dn = (double) n
     */
#undef FAST_INTEGER_CONVERSION
#define FAST_INTEGER_CONVERSION 1
#if FAST_INTEGER_CONVERSION
    dn = z + EXPF_HUGE;

    n    = asuint64(dn);

    dn  -=  EXPF_HUGE;
#else
    n = z;
    dn = cast_i32_to_float(n);

#endif

    r  = x - dn * EXPF_LN2_BY_TBLSZ;

    j  = n % EXPF_TABLE_SIZE;

    double_t qtmp  = C2 + (C3 * r);

    double_t r2 = r * r;

    double_t tbl = asdouble(L__two_to_jby64_table[j] + (n << (52 - EXPF_N)));

    q  = r  + (r2 * qtmp);

    double_t result = tbl + tbl* q;

    return (float_t)(result);
}
