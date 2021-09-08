/*
 * Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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

#include <libm_util_amd.h>
#include <libm/alm_special.h>

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/poly.h>

#include <libm/typehelper.h>
#include <libm/compiler.h>


/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 *
 * Signature:
 *      double log10(double)
 *
 * Spec:
 *      log10(x)
 *              = log10(x)             if x ∈ F and x > 0
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

#define LOG10_MASK_SIGN      0x7FFFFFFF
#define N                    8
#define LOG10_POLY_DEGREE    8

static struct {
    double   log10_2_head, log10_2_tail;  /* log10(2) */
    double   poly[LOG10_POLY_DEGREE];
    double   log10_e;
} log10_data = {
    .log10_2_head = 0x1.344135p-2,
    .log10_2_tail = 0x1.3ef3fde623e25p-31,
    .log10_e      = 0x1.bcb7b1526e50fp-2,
    .poly         = {
        1.0,
        0x1.0000000000000p-1,          /* 1/2 */
        0x1.5555555555555p-2,          /* 1/3 */
        0x1.0000000000000p-2,          /* 1/4 */
        0x1.999999999999ap-3,          /* 1/5 */
        0x1.5555555555555p-3,          /* 1/6 */
        0x1.2492492492492p-3,          /* 1/7 */
        0x1.0000000000000p-3,          /* 1/8 */
    },
};

#define LOW  0x0010000000000000U
#define HIGH 0x7ff0000000000000U
#define ALM_LOG10E      log10_data.log10_e
#define ALM_LOG102_HEAD log10_data.log10_2_head
#define ALM_LOG102_TAIL log10_data.log10_2_tail

#define C1  log10_data.poly[0]
#define C2	log10_data.poly[1]
#define C3	log10_data.poly[2]
#define C4	log10_data.poly[3]
#define C5	log10_data.poly[4]
#define C6	log10_data.poly[5]
#define C7	log10_data.poly[6]
#define C8	log10_data.poly[7]

struct log10_table {
    double lead, tail;
};

extern struct log10_table log10_table_256[];
extern        double      log_f_inv_256[];
#define TAB_F_INV         log_f_inv_256
#define ALM_TAB_LOG10     log10_table_256

static inline uint64_t top12(double x)
{
    /* 12 are the exponent bits */
    return asuint64(x) >> (64 - 12);
}

#include "kern/log1p.c"

static inline double
log10_near_one(double x)
{
    static const struct {
        double head, tail;
    } log10e = {
        .head = 0x1.bcb78p-2,          /* 4.34293746948242187500e-01 */
        .tail = 0x1.8a93728719535p-21, /* 7.3495500964015109100644e-7 */
    };

    double res = ALM_PROTO_KERN(log1p)(x, log10e.head, log10e.tail);

    return res;
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
 *                                     so, 0.5 <= F/2 < 1,   f/2 < 2^-9
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
 *             = m*log10(2) +  log10(F)   +   ln(1+r) * log10(e)
 *
 */

double
ALM_PROTO_OPT(log10)(double x)
{
    double    q, r, dexpo, j_times_half;

    uint64_t  ux   = asuint64(x);
    uint64_t  expo = top12(x);
    flt64_t mant  = {.u = ux & MANTBITS_DP64};

    if (unlikely ((ux - LOW) >= (HIGH - LOW))){

        if (2 * ux == 0)
            return alm_log2_special(x, asdouble(NINFBITPATT_DP64), AMD_F_DIVBYZERO);

        if (ux == PINFBITPATT_DP64)
            return x;

        if ((ux >= HIGH ) || (ux >> 63)) {

            if( (ux & QNANBITPATT_DP64) == QNANBITPATT_DP64)
               return x;

            return alm_log2_special(x, asdouble(ux | QNANBITPATT_DP64), ALM_E_IN_X_NAN);

        }

        /* Denormals : adjust mantissa */

        mant.u   |= ONEEXPBITS_DP64;
        mant.d   -= 1.0;
        expo      = (mant.u >> EXPSHIFTBITS_DP64) - 2045;
        mant.u   &= MANTBITS_DP64;
        ux        = mant.u;
        dexpo    = cast_i64_to_double((int64_t) expo);
    }
    else {

        expo    -= EXPBIAS_DP64;
        dexpo    = cast_i64_to_double((int64_t) expo);

    }

    /* Values very close to 1, x in [0.9375, 1.625] */

#define NEAR_ONE_LO asuint64(1 - 0x1.0p-4)
#define NEAR_ONE_HI asuint64(1 + 0x1.1p-4)

    if (unlikely(ux - NEAR_ONE_LO < NEAR_ONE_HI - NEAR_ONE_LO)) {
        return log10_near_one(x);
    }

    /*
     * Mask for first N bits of mantissa
     * Mask for N+1th bit
     */
#define MANT_MASK_N    (((1ULL << N) - 1) << (EXPSHIFTBITS_DP64 - N))
#define MANT_MASK_N1   (1ULL << (EXPSHIFTBITS_DP64 - N - 1))

    uint64_t mant_n  = ux & MANT_MASK_N;
    uint64_t mant_n1 = ux & MANT_MASK_N1;

    /*
     * mant[52:52-N] + mant[52-N:52-N-1] << 1 (mistery)
     * not sure why...
     */
    uint64_t j = (mant_n + (mant_n1 << 1));

    mant.i        |= HALFEXPBITS_DP64;               /* F */
    j_times_half   = asdouble(HALFEXPBITS_DP64 | j); /* Y */

    j >>= (52 - N);

    /* f = F - Y */
    double_t f = j_times_half - mant.d;

    r = f * TAB_F_INV[j];

    struct log10_table *tb_entry = &((struct log10_table*)ALM_TAB_LOG10)[j];

    q = r * (1 + r * (C2 + r * (C3 + r * (C4 + r * (C5 + r * C6)))));

    q = q * ALM_LOG10E;

    /* m*log(2) + log(G) - poly * log10(e)*/
    q  = (dexpo * ALM_LOG102_TAIL) - q;
    q += tb_entry->tail;

    q += (dexpo * ALM_LOG102_HEAD) + tb_entry->lead;

    return q;
}

