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

#define FTHRESH_HI  0x1.c8ff7ep-3f
#define FTHRESH_LO -0x1.269622p-2

#define THRESH_HI  0x3E647FBFU	/* log(1 + 1/4) =  0.223144 */
#define THRESH_LO  0xBE934B11U	/* log(1 - 1/4) = -0.287682 */
/* HI - LO = 0x7fd134ae */
/* LO - HI = 0x802ecb52 */

#define ARG_MIN 0xC18AA122U              /* ~= -17.32867 */
#define ARG_MAX 0x42B19999U              /* ~=  88.79999 */
/* MIN - MAX =  0x7ed9078a */
/* MAX - MIN =  0x8126f877 */

#define FARG_MIN -0x1.154244p+4f
#define FARG_MAX 0x1.633332p+6f

float
FN_PROTOTYPE(expm1f_v2)(float x)
{
    flt64_t  q1;
    double   q;
    //uint32_t ux = asuint32(x);

    if (unlikely (x > FARG_MAX) || x < FARG_MIN) {
        if (x > FARG_MAX)
            return asfloat(PINFBITPATT_SP32);

        if (x < FARG_MIN)
            return -1.0f;

        return asfloat(QNANBITPATT_SP32);
    }

    if (unlikely (x > FTHRESH_LO) && (x < FTHRESH_HI)) {

#define A1 DATA.poly[0]
#define A2 DATA.poly[1]
#define A3 DATA.poly[2]
#define A4 DATA.poly[3]
#define A5 DATA.poly[4]

        double dx = (double)x;
        double dx2 = dx * dx;
        double q = dx2 * dx * (A1 + dx * (A2 + dx*(A3 + dx*(A4 + dx*(A5)))));
        q += (dx2 * 0.5) + dx;
        return (float)q;
    }

    double dy = x * DATA._64_by_ln2;

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
