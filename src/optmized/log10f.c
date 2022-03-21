/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "libm_macros.h"
#include "libm_util_amd.h"

#include <libm/types.h>
#include <libm/alm_special.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>

#include "logf_data.h"

/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 *
 * Signature:
 *      float log10f(float)
 *
 * Spec:
 *      log10f(x)
 *              = log10f(x)             if x ∈ F and x > 0
 *              = -inf                  if x ∈ {-0,0}
 *              = +inf                  if x = +inf
 *
 *              = +--                   otherwise
 *                | qNaN (INVALID)      if x {+inf,0,-inf}
 *                | qNaN                if x is qNaN
 *                | qNaN (INVALID)      if x is signaling NaN
 *                +--
 *
 */

#define LOG10F_MASK_SIGN      0x7FFFFFFF
#define LOG10F_N              8
#define LOG10F_POLY_DEGREE    2

static struct {
    float   log10_2, log10_2_head, log10_2_tail;  /* log10(2) */
    float   log10_e, log10_e_head, log10_e_tail;  /* log10(e) */
    float   poly[LOG10F_POLY_DEGREE];
    float   poly1[LOG10F_POLY_DEGREE];
} log10f_data = {
    .log10_2       = 0x1.344136p-2,
    .log10_2_head  = 0x1.34p-2f,      /* 0x3e9a0000 = 0.30078125 */
    .log10_2_tail  = 0x1.04d426p-12f, /* 0x39826a13 = 0.000248745636782 */

    .log10_e       = 0x1.bcb7b2p-2f,
    .log10_e_head  = 0x1.bcp-2f,
    .log10_e_tail  = 0x1.6f62a4p-11f,

    /* Polynomial constants */
    .poly = {
        0.5f,           /* C1 = 1/2 */
        0x1.555556p-2f, /* C2 = 1/3 */
    },
    //.tab = logf_lookup,

    /* Polynomial constants for cases near to 1 */
    .poly1 = {
        0x1.555556p-4f,  /* A1 = 8.33333333333317923934e-02f */
        0x1.99999ap-7f,  /* A2 = 1.25000000037717509602e-02f */
    },
};

#define LOG10_2      log10f_data.log10_2
#define LOG10_2_HEAD log10f_data.log10_2_head
#define LOG10_2_TAIL log10f_data.log10_2_tail

extern struct log10f_table log10f_lookup[1<<LOG10F_N];

static inline float
inline_log101pf(float x)
{
    float r, q, u, v, v2, correction;

    r = x - 1.0f;
    u = r / (2.0f + r);

    correction = u * r;

    v = u + u;
    v2 = v * v;

#define A1         log10f_data.poly1[0]
#define A2         log10f_data.poly1[1]
#define LOG10_E        log10f_data.log10_e
#define LOG10_E_HEAD   log10f_data.log10_e_head
#define LOG10_E_TAIL   log10f_data.log10_e_tail

    q = v * v2 * (A1 + A2 * v2) - correction;

    float f, rt, rtop, s, t, z;

    rtop = asfloat(asuint32(r) & 0x0ffff000);

    rt = r - rtop;

    f = q + rt;

    s = LOG10_E_HEAD * f;
    z = LOG10_E_TAIL * rtop;
    t = LOG10_E_TAIL * f + z;

    float w = s + t;

    return w + rtop * LOG10_E_HEAD;
}


/*
 * Implementation Notes:
 *    x  =  2^m * (1+A)                where 1 <= A <= 2
 *    x  =  2^m * (1 + (G + g))        where 1 <= 1+G < 2 and g < 2^(-8)
 *
 *    Let F = (1 + G) and f = g
 *       x  = 2^m*(F + f)              so 1 <= F < 2, f < 2^(-8)
 *
 *       x  = (2^m) * 2 * (F/2+f/2)
 *                                      so, 0.5 <= F/2 < 1,   f/2 < 2^-9
 *
 *   log10(x) = log10(2^m) + log10(2) + log10(F/2 + f/2)
 *            =>    A      +    B     +      C
 *
 *           A = log10(2^m)  = m * log10(2)         ... (1)
 *           B = log10(2)                           ... (2)
 *           C = log10(F/2*(1+f/F))
 *             => log10(F/2) + log10(1+f/F)         ... (3)
 *
 *
 *   log10(x)  = m*log10(2) + log10(2) + log10(F/2) + log10(1+f/F)
 *                            \___________________/   let r = 1+f/F
 *                           (combining => log10(F))
 *
 *         Let, ln = log to base 'e'
 *
 *                      ln(x)
 *         log  (x)  = --------     and      1/ln(10) => log10(e)
 *            10        ln(10)
 *
 *  From (1), (2) and (3)
 *
 *             = m*log10(2) +    log10(F)      +   ln(1+r) * log10(e)
 *
 */

float
ALM_PROTO_OPT(log10f)(float x)
{
    uint32_t ux;

    ux = asuint32(x);

    if (unlikely (ux - 0x00800000 >= 0x7f800000 - 0x00800000))
    {  /* x < 0x1p-126 or inf or nan. */
        uint32_t sign = ux & SIGNBIT_SP32;

        if (x * 2 == 0)                /* log10(0) = -inf */
            return alm_logf_special(x, asfloat(NINFBITPATT_SP32), ALM_E_IN_X_ZERO);

        if (sign)        /* x is -ve */
            return alm_logf_special(x, asfloat(QNANBITPATT_SP32), ALM_E_IN_X_NEG);

        if (x != x)  /* nan */
            return alm_logf_special(x, asfloat(QNANBITPATT_SP32), ALM_E_IN_X_NAN);

        if ((ux & PINFBITPATT_SP32) == PINFBITPATT_SP32)           /* log10(inf) = inf */
            return asfloat(PINFBITPATT_SP32 | sign);

        /*
         * 'x' has to be denormal, Normalize it
         * there is a possibility that only the last (23'rd) bit is set
         * Hence multiply by 2^23 to bring to 1.xxxxx format.
         */
        ux = asuint32(x * 0x1p23f);
        ux -= (23 << 23);
    }

    int32_t expo = (((int32_t)ux) >> EXPSHIFTBITS_SP32) - EMAX_SP32;
    float   f_expo = (float)expo;

#define NEAR_ONE_LO asuint32(1.0f - 0x1.0p-4f)  /* 1.0 - 0.0625     = 0.9375 */
#define NEAR_ONE_HI asuint32(1.0f + 0x1.1p-4f)  /* 1.0 + 0.06640625 = 1.06640625 */

    /* Values very close to 1, e^(-1/16) <= x <= e^(1/16)*/
    if (unlikely(ux - NEAR_ONE_LO < NEAR_ONE_HI - NEAR_ONE_LO)) {
        return inline_log101pf(x);
    }

    /*
     * Here onwards, 'x' is neither -ve, nor close to 1
     */
    uint32_t mant, mant1, idx;
    float    y, f, finv, r, r2, q;

    mant   = ux & MANTBITS_SP32;

#define LOG10F_MASK_MANT_ALL7 0x007f0000
#define LOG10F_MASK_MANT8     0x00008000

    mant1  = ux & LOG10F_MASK_MANT_ALL7;
    /* This step is needed for more accuracy */
    /* mant1 += ((ux & LOG10F_MASK_MANT8) << 1); */

    idx = mant1 >> 16;

    y = asfloat(mant  |= HALFEXPBITS_SP32);
    f = asfloat(mant1 |= HALFEXPBITS_SP32);

    struct log10f_table *tbl = &log10f_lookup[idx];

    finv = tbl->f_inv;

    r = (f - y) * finv;

#define C1 log10f_data.poly[0]
#define C2 log10f_data.poly[1]

    r2 = r * r;                 /* r^2 */
    q = r + (r2 * (C1 + r * C2));

#if 1
    q = LOG10_E * q;
    q = LOG10_2_TAIL * f_expo - q;

    float w = LOG10_2_HEAD * f_expo + tbl->f_128_head;

    q = tbl->f_128_tail + q + w;
#else
    float z = tbl->f_128_tail - q * LOG10_E;

    q = LOG10_2 * f_expo + tbl->f_128_head + z;
#endif

    return q;
}
