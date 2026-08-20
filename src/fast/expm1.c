/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
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

/*
 * Signature:
 *   double expm1_fast(double x)
 *
 * Computes exp(x) - 1 with faithful accuracy in all finite inputs.
 *
 * Special cases handled:
 *   expm1(+0)              = +0
 *   expm1(-0)              = -0
 *   expm1(+inf)            = +inf
 *   expm1(-inf)            = -1
 *   expm1(+NaN)            = NaN
 *   expm1(-NaN)            = NaN
 *   expm1(x > ~709.78)     = +inf        (overflow, sets ERANGE)
 *   expm1(x < ~-37.4)      = -1          (saturates to -1)
 *
 ********************************************
 * Implementation Notes
 * ---------------------
 * To compute expm1(double x):
 *
 * Path 1: |x| < ln(2)/2  — direct minimax polynomial, no table lookup
 *
 *   1a. Tiny: |x| < 2^-54
 *       expm1(x) = x  (result rounds to x exactly, no computation needed)
 *
 *   1b. Short: 2^-54 <= |x| < 2^-9
 *       expm1(x) = x + x^2 * ((C2 + C3*x) + x^2*(T4 + T5*x))
 *       4-coefficient degree-5 poly evaluated with three FMA levels.
 *
 *   1c. Normal poly: 2^-9 <= |x| < ln(2)/2
 *       expm1(x) = x + x^2 * P(x)
 *       P(x) = C2 + C3*x + C4*x^2 + ... + C13*x^11  (12 minimax coefficients)
 *       Evaluated using POLY_EVAL_12_ESTRIN_FMA (5-level Estrin-FMA, ~25 cycles).
 *
 * Path 2: |x| >= ln(2)/2  — table-driven Tang-style reduction
 *
 *   2a. Argument reduction: choose n so that x = n*ln(2)/64 + r, |r| <= ln(2)/128
 *       m = n >> 6 (integer part),  j = n & 63 (fractional table index)
 *
 *   2b. Table lookup for 2^(j/64):
 *       f = tbl->main,  f1 = tbl->head,  f2 = tbl->tail  (Veltkamp split)
 *
 *   2c. e^r - 1 approximated with degree-6 Estrin poly (5 FMA levels):
 *       q = r + r^2*(C2+C3*r) + r^4*(C4+C5*r+C6*r^2)
 *
 *   2d. Reconstruction (branchless):
 *       expm1 = 2^m * ((f1 - 2^(-m)) + f*q + f2)
 *       Near-overflow (m_shift >= 1024) handled in cold outlined helper.
 *
 *   2e. Overflow (x > ~709.78): sets ERANGE, returns +inf via alm_expm1_special.
 *       Saturation (x < ~-37.4): returns -1.0.
 *       NaN/Inf: propagated via the overflow cold path.
 */

#include <stdint.h>
#include <libm_macros.h>
#include <libm_util_amd.h>
#include <libm_errno_amd.h>
#include <libm/amd_funcs_internal.h>
#include <libm/alm_special.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/poly.h>

#define EXP_N 6
#include "../optimized/exp_data.h"

static struct {
    uint64_t ln2_over_2;          /* |x| <  ln(2)/2: use polynomial path         */
    uint64_t expm1_overflow;      /* |x| >= this   : overflow / special handling  */
    uint64_t tiny;                /* |x| <  2^-54  : return x exactly            */
    uint64_t small;               /* |x| <  2^-9   : use short 4-coeff poly      */
    uint64_t abs_mask;            /* strip sign bit to get |x| bit pattern        */
    double   near_overflow_scale; /* 2^1023: final scale in near-overflow path  */
    /* Short poly (|x| < 2^-9): x^4 and x^5 Taylor terms */
    double t4;                    /* 1/4! = 1/24                                  */
    double t5;                    /* 1/5! = 1/120                                 */
    /* Main poly (|x| < ln2/2) — degree-13 Sollya minimax: C2..C13 */
    double c[12];
} expm1_data = {
    .ln2_over_2          = 0x3fd62e42fefa39efULL,
    .expm1_overflow      = 0x40862e42fefa39f0ULL,
    .tiny                = 0x3c90000000000000ULL,
    .small               = 0x3f60000000000000ULL,
    .abs_mask            = 0x7fffffffffffffffULL,
    .near_overflow_scale = 0x1p1023,
    .t4   = 0x1.5555555555555p-5,   /* 1/24   */
    .t5   = 0x1.1111111111111p-7,   /* 1/120  */
    .c = {
        0x1p-1,                 /* C2:  1/2    */
        0x1.5555555555555p-3,   /* C3:  1/6    */
        0x1.5555555555556p-5,   /* C4         */
        0x1.111111111137cp-7,   /* C5         */
        0x1.6c16c16c15fffp-10,  /* C6         */
        0x1.a01a019eaafc6p-13,  /* C7         */
        0x1.a01a01a815ae6p-16,  /* C8         */
        0x1.71de404b5e836p-19,  /* C9         */
        0x1.27e4d820b1d15p-22,  /* C10        */
        0x1.ae4dbfe848a07p-26,  /* C11        */
        0x1.1f78d10bd1b27p-29,  /* C12        */
        0x1.81472fc184122p-33,  /* C13        */
    },
};

#define ALM_EXPM1_LN2_OVER_2      expm1_data.ln2_over_2
#define ALM_EXPM1_OVERFLOW        expm1_data.expm1_overflow
#define ALM_EXPM1_TINY            expm1_data.tiny
#define ALM_EXPM1_SMALL           expm1_data.small
#define ALM_EXPM1_ABS_MASK        expm1_data.abs_mask
#define ALM_EXPM1_NEAR_OVF_SCALE  expm1_data.near_overflow_scale
#define ALM_EXPM1_T4              expm1_data.t4
#define ALM_EXPM1_T5              expm1_data.t5
#define ALM_EXPM1_C2              expm1_data.c[0]
#define ALM_EXPM1_C3              expm1_data.c[1]
#define ALM_EXPM1_C4              expm1_data.c[2]
#define ALM_EXPM1_C5              expm1_data.c[3]
#define ALM_EXPM1_C6              expm1_data.c[4]
#define ALM_EXPM1_C7              expm1_data.c[5]
#define ALM_EXPM1_C8              expm1_data.c[6]
#define ALM_EXPM1_C9              expm1_data.c[7]
#define ALM_EXPM1_C10             expm1_data.c[8]
#define ALM_EXPM1_C11             expm1_data.c[9]
#define ALM_EXPM1_C12             expm1_data.c[10]
#define ALM_EXPM1_C13             expm1_data.c[11]

/*
 * Cold paths separated to keep the hot path in fewer cache lines.
 */
NOINLINE_COLD static double expm1_overflow(double x, uint64_t ux, uint64_t ax)
{
    if (ax > PINFBITPATT_DP64)
        return x + x;
    if (ux >> 63)
        return -1.0;
    __set_errno(ERANGE);
    return alm_expm1_special(asdouble(PINFBITPATT_DP64), ALM_E_OVERFLOW);
}

NOINLINE_COLD static double expm1_near_overflow(double_t q, double_t f, double_t f1, double_t f2)
{
    q = _LIBM_POLY_FMA(f, q, f1 + f2);
    q *= 2.0;
    q *= ALM_EXPM1_NEAR_OVF_SCALE;
    return q;
}

double
ALM_PROTO_FAST(expm1)(double x)
{
    uint64_t ux = asuint64(x);
    uint64_t ax = ux & ALM_EXPM1_ABS_MASK;

    /*
     * Path 1: |x| < ln(2)/2
     * Direct minimax polynomial — no tables, no cancellation.
     */
    if (ax < ALM_EXPM1_LN2_OVER_2) {
        if (unlikely(ax < ALM_EXPM1_TINY))
            return x;

        double x2 = x * x;

        /* Short poly for |x| < 2^-9 (degree 5, 3 FMA levels).
         * Evaluates: x + x^2 * ((C2 + C3*x) + x^2*(T4 + T5*x)) */
        if (ax < ALM_EXPM1_SMALL) {
            double p01  = _LIBM_POLY_FMA(ALM_EXPM1_C3, x, ALM_EXPM1_C2);
            double p23  = _LIBM_POLY_FMA(ALM_EXPM1_T5, x, ALM_EXPM1_T4);
            double inner = _LIBM_POLY_FMA(x2, p23, p01);
            return _LIBM_POLY_FMA(x2, inner, x);
        }

        /*
         * Degree-13 minimax for |x| < ln2/2, Estrin-FMA evaluation.
         * expm1(x) = x + x^2 * P(x), P degree 11 (12 coefficients).
         */
        double s = POLY_EVAL_12_ESTRIN_FMA(x,
                       ALM_EXPM1_C2,  ALM_EXPM1_C3,
                       ALM_EXPM1_C4,  ALM_EXPM1_C5,
                       ALM_EXPM1_C6,  ALM_EXPM1_C7,
                       ALM_EXPM1_C8,  ALM_EXPM1_C9,
                       ALM_EXPM1_C10, ALM_EXPM1_C11,
                       ALM_EXPM1_C12, ALM_EXPM1_C13);

        return _LIBM_POLY_FMA(x2, s, x);
    }

    /* Overflow / special — outlined cold */
    if (unlikely(ax >= ALM_EXPM1_OVERFLOW))
        return expm1_overflow(x, ux, ax);

    /*
     * Path 2: table-driven for |x| >= ln(2)/2
     */
    double_t a = x * EXP_TBLSZ_BY_LN2;
    flt64_t q1 = {.i = 0};
    q1.d = a + EXP_HUGE;
    int64_t n = q1.i;
    double_t dn = q1.d - EXP_HUGE;

    int64_t m_shift = (int64_t)dn >> EXP_N;

    double_t r = (x + dn * EXP_LN2_BY_TBLSZ_HEAD) + dn * EXP_LN2_BY_TBLSZ_TAIL;
    int64_t j = n & (EXP_TABLE_SIZE - 1);

    /* e^r - 1 polynomial (Estrin degree 6, FMA):
     * q = r + r^2*(C2+C3*r) + r^4*(C4+C5*r+C6*r^2) */
    double_t r2 = r * r;
    double_t p12 = _LIBM_POLY_FMA(r, EXP_C3, EXP_C2);
    double_t q   = _LIBM_POLY_FMA(r2, p12, r);
    double_t p34 = _LIBM_POLY_FMA(r, EXP_C6, EXP_C5);
    double_t p3  = _LIBM_POLY_FMA(r, p34, EXP_C4);
    q = _LIBM_POLY_FMA(r2 * r2, p3, q);

    const struct exp_table *tbl = &((const struct exp_table*)EXP_TABLE_DATA)[j];
    double_t f  = tbl->main;
    double_t f1 = tbl->head;
    double_t f2 = tbl->tail;

    int64_t m = (n - j) << (52 - EXP_N);

    /* Near-overflow: separated cold */
    if (unlikely(m_shift >= 1024))
        return expm1_near_overflow(q, f, f1, f2);

    /*
     * Branchless reconstruction: expm1 = 2^m * ((f1 - 2^(-m)) + f*q + f2)
     * Computing (t+f2) can run in parallel with polynomial evaluation.
     */
    flt64_t s;
    s.u = (uint64_t)(1023 - m_shift) << 52;
    double_t t = f1 - s.d;
    q = _LIBM_POLY_FMA(f, q, t + f2);
    return asdouble((uint64_t)m + asuint64(q));
}

strong_alias (__expm1_finite, ALM_PROTO_FAST(expm1))
weak_alias (amd_expm1, ALM_PROTO_FAST(expm1))
weak_alias (expm1, ALM_PROTO_FAST(expm1))
