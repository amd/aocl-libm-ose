/*
 * Copyright (C) 2019, AMD. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm_amd_paths.h>
#include <libm_special.h>

#if !defined(DEBUG) && defined(__GNUC__) && !defined(__clang__)
#pragma GCC push_options
#pragma GCC optimize ("O3")
#endif  /* !DEBUG */

#include "expm1f_data.h"

#define DATA expm1f_v2_data

/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   float expm1f(float x)
 *
 * Description:
 *        Calculates closest approximation to (exp(x) - 1); where, exp(x) = e^x.
 *
 * Spec:
 *   expm1f(x)
 *          = expm1f(x)         if x ∈ F and |x| >= FLT_MIN
 *          = x                 if x ∈ F, and |x| < FLT_MIN
 *          = +/-x              if x = {-0, 0}, preserve sign
 *          = -0                if x = -0
 *          = -1                if x = -inf
 *          = +inf              if x = +inf
 *          = NaN               otherwise
 *
 *    expm1f(x) will overflow approximately when x > ln(FLT_MAX)
 *
 * Implementation Notes:
 *
 */
float expm1f_special(float x, float y, U32 code);

float
FN_PROTOTYPE(expm1f_v2)(float x)
{
    flt64_t  q1;
    double   q;
    uint32_t ux = asuint32(x);

    if (unlikely (ux < DATA.x.min && ux > DATA.x.max) ) { /* max/min arg */
#if defined(ENABLE_IEEE_exceptions)
        if (ux == 0x7f800000)
            return x;

        return expm1f_special(x, PINFBITPATT_SP32, 3);
#else
        return asfloat(PINFBITPATT_SP32);
#endif
    }

    double_t dx = (double)x;

    /* Near one code path */
    if (unlikely (ux > DATA.threshold.lo && ux < DATA.threshold.hi )) {
	    if (ux < 0x80000000U)
		    return x;

#define A1 DATA.poly[0]
#define A2 DATA.poly[1]
#define A3 DATA.poly[2]
#define A4 DATA.poly[3]
#define A5 DATA.poly[4]

	    double dx2 = dx * dx;
	    double q = dx2  * dx * (A1 + dx * (A2 + dx*(A3 + dx*(A4 + dx*(A5)))));
	    q = dx2 + (dx * 0.5);
	    return (float)q;
    }

    double dy = dx * DATA._64_by_ln2;

    q1.d = dy + DATA.Huge;
    int     n = q1.i;
    double  dn= q1.d - DATA.Huge;

    double r = x - dn * DATA.ln2_by_64;

    int j = n & 0x3f;

    int m = (n - j) >> 6;

#define C1 1/2.0
#define C2 1/6.0
    q = r + r * r * (C1 + (C2 * r));

    double f = DATA.tab[j];

    q1.i = (1023ULL - m) << 52;

    q1.d = f * q + (f - q1.d);

    j    = n >> EXPM1F_N;

    q = asdouble(q1.i + ((uint64_t)j << 52));

    return (float)q;
}

#if defined(ENABLE_GLIBC_API)
    strong_alias (__expm1f_finite, FN_PROTOTYPE(expm1f_v2))
    weak_alias (__ieee754_expm1f, FN_PROTOTYPE(expm1f_v2))
#endif

#if defined(ENABLE_LIBM_API)
    strong_alias (__expm1f, FN_PROTOTYPE(expm1f_v2))
    strong_alias (expm1f, FN_PROTOTYPE(expm1f_v2))
#endif

#if defined(ENABLE_AMDLIBM_API)
    strong_alias (amd_expm1f, FN_PROTOTYPE(expm1f_v2))
#endif


#if !defined(DEBUG) && defined(__GNUC__) && !defined(__clang__)
#pragma GCC pop_options
#endif
