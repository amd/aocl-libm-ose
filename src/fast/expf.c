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

#define ARG_MIN 0xFFFFFF99
#define ARG_MAX 0x00000058

#define EXPF_FAST_POLY_DEGREE 4

static const struct {
    double_t   sixtyfour_byln2;
    double_t   huge;
    double_t   poly_expf[6];
} expf_fast_data ={
    .sixtyfour_byln2 =  0x1.71547652b82fep+0,
    .huge        =  0x1.8p+52 ,
    /*
     * Polynomial coefficients obtained using Remez algorithm
     */
    .poly_expf = {
#if EXPF_FAST_POLY_DEGREE == 5
        0x1.0000014439a91p0,
        0x1.62e43170e3344p-1,
        0x1.ebf906bc4c115p-3,
        0x1.c6ae2bb88c0c8p-5,
        0x1.3d1079db4ef69p-7,
        0x1.5f8905cb0cc4ep-10,
#elif EXPF_FAST_POLY_DEGREE == 4
        0x1.00000288e2e6fp0,
        0x1.62e0c2aa5f8bfp-1,
        0x1.ebf7c216c3b7bp-3,
        0x1.ca1cfcee087d3p-5,
        0x1.3d61a2ee479p-7,
#endif
    },
};


#define EXPF_FAST_CONST expf_fast_data.sixtyfour_byln2
#define EXPF_FAST_HUGE  expf_fast_data.huge

#define C1 expf_fast_data.poly_expf[0]
#define C2 expf_fast_data.poly_expf[1]
#define C3 expf_fast_data.poly_expf[2]
#define C4 expf_fast_data.poly_expf[3]
#define C5 expf_fast_data.poly_expf[4]
#define C6 expf_fast_data.poly_expf[5]

float
FN_PROTOTYPE_FAST(expf)(float _x)
{
#if 0
    uint32_t ux = asuint32(_x) & 0x7fffffff;

    if (unlikely(ux - ARG_MIN >= (ARG_MAX - ARG_MIN))) {
        /* handle special case */

    }
#endif
    double_t x = (double_t)_x;

    double_t z = x * EXPF_FAST_CONST;

    double_t dn = z + EXPF_FAST_HUGE;

    uint64_t n = asuint64(dn);

    dn -= EXPF_FAST_HUGE;

    double_t r = z - dn;

    /*
     * Polynomial Estrins
     * poly = C1 + C2*r + C3*r^2 + C4*r^3 + C5 *r^4 + C6*r^5
     */
#if EXPF_FAST_POLY_DEGREE == 5
    double_t q = POLY_EVAL_6(r, C1, C2, C3, C4, C5, C6);
#elif EXPF_FAST_POLY_DEGREE == 4
    double_t q = POLY_EVAL_5(r, C1, C2, C3, C4, C5);
#endif

    double_t res = asdouble(asuint64(q) + (n << 52));

    return (float)res;
}

strong_alias (__expf_finite, FN_PROTOTYPE_FAST(expf))
weak_alias (expf, FN_PROTOTYPE_FAST(expf))
strong_alias (__ieee754_expf, FN_PROTOTYPE_FAST(expf))
