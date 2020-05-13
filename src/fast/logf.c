/*
 * Copyright (C) 2019, Advanced Micro Devices. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 *         Siva Srinivas Koukula <skolukul@amd.com>
 */
#include <stdint.h>
#include <libm_util_amd.h>
#include <libm_special.h>

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/poly.h>

static struct {
    float poly[9];
    float ln2;
} logf_data = {
    .ln2 = 0x1.62e42fefp-1f,
    // Polynomial coefficients obtained using fpminimax algorithm from Sollya
    .poly = {
        0x1.0000000f500p0f,
        -0x1.ffffd272p-2f,
        0x1.5554e6dap-2f,
        -0x1.00150b01p-2f,
        0x1.99d8391ap-3f,
        -0x1.50224b33p-3f,
        0x1.1e779278p-3f,
        -0x1.3b751efcp-3f,
        0x1.1fde870dp-3f,
    },
};

#define ln2 logf_data.ln2
#define C1 logf_data.poly[0]
#define C2 logf_data.poly[1]
#define C3 logf_data.poly[2]
#define C4 logf_data.poly[3]
#define C5 logf_data.poly[4]
#define C6 logf_data.poly[5]
#define C7 logf_data.poly[6]
#define C8 logf_data.poly[7]
#define C9 logf_data.poly[8]


/*
 * Implementation Notes:
 *     reduce 'x' into the form:
 *        x = (-1)^s*2^n*m
 * 's' will be always zero, as log is defined for posititve numbers
 * 'n' is an integer known as the exponent
 * 'm' is mantissa
 *
 * x is reduced such that the mantissa, m lies in [2/3,4/3]
 *      x = 2^n*m where m is in [2/3,4/3]
 *      log(x) = log(2^n*m)         We have log(a*b) = log(a)+log(b)
 *             = log(2^n) + log(m)   We have log(a^n) = n*log(a)
 *             = n*log(2) + log(m)
 *             = n*log(2) + log(1+(m-1))
 *             = n*log(2) + log(1+f)        Where f = m-1
 *             = n*log(2) + log1p(f)        f lies in [-1/3,+1/3]
 *
 * Thus we have :
 * log(x) = n*log(2) + log1p(f)
 *            log1p(F) is approximated by using a polynomial
 */

float
FN_PROTOTYPE_FAST(logf)(float x)
{
    float m, r, n, f;

    // 0 and FLT_MAX
    if ((x > 0.0f) && (x <= 0x1.fffffep+127)) {
        // Get the exponent
        int32_t ux = (asuint32(x) - 0x3f2aaaab) & 0xff800000;
        n = (float)(ux >> EXPSHIFTBITS_SP32) ;
        // Get the mantissa, m is in [2/3, 4/3]
        m = asfloat(asuint32(x) - ux);

        f = m - 1.0f;// f is in [-1/3,+1/3]

        // Compute log1p(f) using Polynomial approximation
        r = POLY_EVAL_9(f, C1, C2, C3, C4, C5, C6, C7, C8, C9);

        r = n*ln2 + r;

    } else {
        r = x + x;  // Output NaNs for the inputs NaN's
        if (x  < 0.0f) r =  0.0f / 0.0f; //  NaN
        if (x == 0.0f) r = -1.0f / 0.0f; // -Inf
    }

    return r;

}

strong_alias (__logf_finite, FN_PROTOTYPE_FAST(logf))
weak_alias (logf, FN_PROTOTYPE_FAST(logf))
strong_alias (__ieee754_logf, FN_PROTOTYPE_FAST(logf))
