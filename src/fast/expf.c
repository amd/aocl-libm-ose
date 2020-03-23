/*
 * Copyright (C) 2019-2020, AMD. All rights reserved.
 */

/*
 * Description: Faster version of expf()
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */

/*
 * Implementation Notes
 * ----------------------
 * 1. Argument Reduction:
 *      e^x = 2^(x/ln2)                          --- (1)
 *
 *      Let x/ln(2) = z                          --- (2)
 *
 *      Let z = n + r , where n is an integer    --- (3)
 *                      |r| <= 1/2
 *
 *     From (1), (2) and (3),
 *      e^x = 2^z
 *          = 2^(N+r)
 *          = (2^N)*(2^r)                        --- (4)
 *
 * 2. Polynomial Evaluation
 *   From (4),
 *     r   = z - N
 *     2^r = C1 + C2*r + C3*r^2 + C4*r^3 + C5 *r^4 + C6*r^5
 *
 * 4. Reconstruction
 *      Thus,
 *        e^x = (2^N) * (2^r)
 *
 *
 */

#include <stdint.h>

#include <libm_util_amd.h>
#include <libm_special.h>
#include <libm_macros.h>
#include <libm_amd.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>

#include <libm/poly.h>

#define EXPF_N 6
#define EXPF_POLY_DEGREE 3

#define EXPF_TABLE_SIZE (1 << EXPF_N)
#define EXPF_MAX_POLY_DEGREE 4

/*
 * expf_data.h needs following to be defined before include
 *    - EXPF_N
 *    - EXPF_POLY_DEGREE
 *    - EXPF_TABLE_SIZE
 *    - EXPF_MAX_POLY_DEGREE
 */

#include "expf_data.h"

static const
struct expf_data expf_v2_data = {
    .ln2by_tblsz = 0x1.62e42fefa39efp-7,
    .tblsz_byln2 = 0x1.71547652b82fep+6,
    .Huge = 0x1.8000000000000p+52,
    .table_v3 = __two_to_jby64,
    .poly = {
        1.0,    /* 1/1! = 1 */
        0x1.0000000000000p-1,   /* 1/2! = 1/2    */
        0x1.5555555555555p-3,   /* 1/3! = 1/6    */
        0x1.cacccaa4ba57cp-5,   /* 1/4! = 1/24   */
    },
};

#define C1  expf_v2_data.poly[0]
#define C2  expf_v2_data.poly[1]
#define C3  expf_v2_data.poly[2]
#define C4  expf_v2_data.poly[3]

#define EXPF_LN2_BY_TBLSZ  expf_v2_data.ln2by_tblsz
#define EXPF_TBLSZ_BY_LN2  expf_v2_data.tblsz_byln2
#define EXPF_HUGE	   expf_v2_data.Huge
#define EXPF_TABLE         expf_v2_data.table_v3

float
FN_PROTOTYPE_FAST(expf)(float x)
{
    double_t  q, dn, r, z;
    uint64_t n, j;

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

    double_t tbl = asdouble(asuint64(EXPF_TABLE[j]) + (n << (52 - EXPF_N)));

    q  = r  + (r2 * qtmp);

    double_t result = tbl + tbl* q;

    return (float_t)(result);
}


strong_alias (__expf_finite, FN_PROTOTYPE_FAST(expf))
weak_alias (amd_expf, FN_PROTOTYPE_FAST(expf))
weak_alias (expf, FN_PROTOTYPE_FAST(expf))
