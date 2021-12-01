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

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/alm_special.h>

#if !defined(DEBUG) && defined(__GNUC__) && !defined(__clang__)
#pragma GCC push_options
#pragma GCC optimize ("O3")
#endif  /* !DEBUG */

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>

#include <libm/typehelper.h>
#include <libm/compiler.h>

#define LOG_N		7
#define LOG_POLY_DEGREE 5
#include "log_data.h"

#define FLAG_X_ZERO 0x1
#define FLAG_X_NEG  0x2
#define FLAG_X_NAN  0x3


static inline
uint32_t top16(double d)
{
    return (uint32_t)(asuint64(d) >> (64-16));
}


static inline double_t
__log1p_v3(double_t f)
{
    double_t g = 1 / (2.0 + f);
    double_t u = 2.0 * f * g;
    double_t v = u * u;
    double_t q;

#define A1 log_data.poly1[0]
#define A2 log_data.poly1[1]
#define A3 log_data.poly1[2]
#define A4 log_data.poly1[3]

    q = u * v * (A1 + v * (A2 + v * (A3 + v * A4)));

    /*
     * (1) Define M := 12 for single precision, and define M := 24 for double precision.
     * (2) u1 := u rounded (or truncated) to M significant bits.
     * (3) fi := f rounded (or truncated) to M significant bits.
     * (4) f2:=f-fl.
     * (5) u2:= ((2*(f-u,)-u,*f,)-u,*f2)*g.
     */
    flt64_t u1 = {.u = asuint64(u) & 0xfffffffff0000000UL};
    flt64_t f1 = {.u = asuint64(f) & 0xfffffffff0000000UL};

    double f2 = f - f1.d;

    double u2 = ((2.0 * (f - u1.d) - u1.d * f1.d) - u1.d * f2) * g;

    /* Result = u1 + (u2 + q) */
    q = u1.d + (u2 + q);

    return q;

}

/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   double log1p(double x)
 *
 * Spec:
 *   log1p(x)
 *          = log1p(x)          if x ∈ F and x >= -1 and |x| >= DBL_MIN
 *          = x                 if x ∈ F, and |x| < DBL_MIN
 *          = +/-x              if x = {-0, 0}, preserve sign
 *          = -0                if x = -0
 *          = -inf              if x = -1
 *          = +inf              if x = +inf
 *          = NaN               otherwise
 *
 * If |x| < 2^-53 return (8* x - b) * 1/8, purpose is to return 'x'-tiny,
 *  correctly rounded in all possible modes, b = 2^-1074 for double precision.
 *
 *
 * Implementation Notes:
 *   Find 'm' such that
 *     1 <= 2^-m (1 + x) < 2
 *
 *   Determine 'F' and 'f' such that
 *     1+x ~= 2^m(F + f)
 *
 */

double FN_PROTOTYPE(log1p_v3)(double x);

double
FN_PROTOTYPE(log1p_v3)(double x)
{

    return __log1p_v3(x);
}

/*
* ISO-IEC-10967-2: Elementary Numerical Functions
* Signature:
*   double log(double x)
*
* Spec:
*   log(x)
*          = log(x)            if x ∈ F and x > 0
*          = x                 if x = qNaN
*          = 0                 if x = 1
*          = -inf              if x = (-0, 0}
*          = NaN               otherwise
*
* Implementation Notes:
*
*    Find 'm' such that
*       1 <= 2^-m * x < 2
*
*    Find 'F' and 'f' such that
*       x = 2^m (F + f)
*
*    F = 1 + j*2^-7        j = 0,1,2,3,...,2^7
*    |f| <= 2^-8
*
*    log(x) = log(2^m) + log(F + f)
*           =          + log(F(1+f/F)
*           = m*log(2) + log(F) + log(1 + f/F)         ---(1)
*
*    Let r = f/F
*      (alternating sign)
*      log(1+f/F) = r - 1/2*r^2 + 1/3*r^3 - 1/4*r^4 +...
*/

double FN_PROTOTYPE(log_v3)(double x);

double
FN_PROTOTYPE(log_v3)(double x)
{
    /*
     * Procedure 1:
     *     x  = 2^m * (F + f)
     * log(x) = log(2^m) + log(F + f)
     *    from(1),
     *          | ln(2^m(F + f) | >= 1/16,  ( b^x = y, then log_b_(y) = x )
     *
     * so,
     *    2^m(F+f) >= e^(1/16)
     *
     *
     *
     *
     */
    uint32_t    top, j;
    uint64_t    ux;
    double_t    q, f, r;

    ux = asuint64(x);

#define NEAR_ONE_LO asuint64(1 - 0x1.0p-5)
#define NEAR_ONE_HI asuint64(1 + 0x1.1p-5)

    /* Values very close to 1, e^(-1/16) <= x <= e^(1/16)*/
    if (unlikely(ux - NEAR_ONE_LO < NEAR_ONE_HI - NEAR_ONE_LO)) {
        return __log1p_v3(x);
    }

    /*
     * top16 bits will show
     * sign of 'x'         - 1  bit
     * exponent of 'x'     - 11 bits
     * top mantissa of 'x' - 4 bits
     */
    top = top16(x);
    if (unlikely (top - 0x0010 >= 0x7ff0 - 0x0010)) {
        if (isinf(x))
            return alm_log_special(x, asdouble(PINFBITPATT_DP64), FLAG_X_ZERO);

        /* x is denormal */
        ux = asuint64 (x * 0x1p52);
        ux -= 52ULL << 52;
    }

    /*
     * We are here means, x is neither -ve, nor close-to-one
     * extract exponent and unbias it
     */
    const flt64_split_t f64 = {.d = x};
    int64_t m = f64.f.expo - 1023;

    /*
     * Y = 2^-m * x , so that 1 <= 2^-m*X < 2
     * Y is nothing but mantissa
     */
    flt64_t Y = {.u = f64.f.mantissa};

    j = (uint32_t)(Y.u >> (52 - LOG_N));    /* Top ''LOG_N' bits of mantissa */

    /* actually Y.d = x * (asdouble)((1023 - m) << 52); */
    Y.u |=  (0x3ffULL << 52);           /* 0x3ff0.0000.0000.0000 */

    /*
     * F = Y * 2^7
     * will not overflow as Y is only mantissa and 1 <= Y < 2
     * use eval_as_double(), otherwise, GCC might optimize this and next statement
     */
    int64_t F = cast_double_to_i64(0x1.0p7 * Y.d);

    /* Round to nearest integer ( F * 2^-7 ) */
    double_t F1 = eval_as_double((double_t)F * 0x1.0p-7);

    f = Y.d - F1;

    const struct log_table *tab = &((const struct log_table *)LOG_TAB)[j];
    r = f * tab->f_inv;

    /* re-defining to shorten the lines */
#define C1 LOG_C1
#define C2 LOG_C2
#define C3 LOG_C3
#define C4 LOG_C4
#define C5 LOG_C5
#define C6 LOG_C6
#define C7 LOG_C7

#define HORNER_SCHEME  0xff
#define ESTRIN_SCHEME  0xff

#define POLY_EVAL_METHOD HORNER_SCHEME

#if !defined(POLY_EVAL_METHOD)
#warning "POLY_EVAL_METHOD is not defined"
#endif
#if !defined(LOG_POLY_DEGREE)
#warning "LOG_POLY_DEGREE is not defined"
#endif

#if (POLY_EVAL_METHOD == HORNER_SCHEME)
#if LOG_POLY_DEGREE == 7
    q = r * r * (C1 + r * (C2 + r * (C3 + r * (C4 + r * (C5 + r * (C6 + r * C7))))));
#elif LOG_POLY_DEGREE == 6
    q = r * r * (C1 + r * (C2 + r * (C3 + r * (C4 + r * (C5 + r * C6)))));
#elif LOG_POLY_DEGREE == 5
    q = r * r * (C1 + r * (C2 + r * (C3 + r * (C4 + r * C5))));
#elif LOG_POLY_DEGREE == 4
    q = r * r * (C1 + r * (C2 + r * (C3 + r * C4)));
#elif LOG_POLY_DEGREE == 3
    q = r * r * (C1 + r * (C2 + r * C3));
#endif	/* Order <=2 && Order == 3 */

#elif POLY_EVAL_METHOD == ESTRIN_SCHEME
    /* Estrin's */
    // r + ((r*r)*(1/2 + (r*1/6))) +
    // ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))

    double_t r2 = r * r;                /* r^2 */
    q = r + (r2 * (C2  + r * C3));

#if LOG_POLY_DEGREE == 4
    q += (r2 * r2) *  C4; /* r^4 * C4 */
#elif LOG_POLY_DEGREE == 5
    q += (r2 * r2) * (C4 + r*C5);
#elif LOG_POLY_DEGREE == 6
    q += (r2 * r2) * (C4 + r * (C5 + r*C6));
#endif
#endif  /* if HORNER_SCHEME || ESTRIN_SCHEME */

#define TAB_HEAD(j) log_data.table[j].head
#define TAB_TAIL(j) log_data.table[j].tail

    double_t l_lead = ((double)m * LN2_HEAD) + tab->head;
    double_t l_tail = ((double)m * LN2_TAIL) + tab->tail;

    /* Parens are necessary to keep the precision */
    q = l_lead + (r + (q + l_tail));

    return q;
}

#if defined(ENABLE_GLIBC_API)
    weak_alias (__log_finite, FN_PROTOTYPE(log_v3))
    weak_alias (__ieee754_log, FN_PROTOTYPE(log_v3))
    weak_alias (__ieee754_log_fma, FN_PROTOTYPE(log_v3))
#endif

#if defined(ENABLE_LIBM_API)
    weak_alias (log, FN_PROTOTYPE(log_v3))
#endif

#if defined(ENABLE_AMDLIBM_API)
    weak_alias (amd_log, FN_PROTOTYPE(log_v3))
#endif

#if !defined(ENABLE_GLIBC_API) && !defined(ENABLE_LIBM_API) && !defined(ENABLE_AMDLIBM_API)
/*
 * No API call is defined, the functions to be called directly
 * like FN_PROTOTYPE(exp2_v2)(arg)
 * useful for internal testing
 */
#endif

#if !defined(DEBUG) && defined(__GNUC__) && !defined(__clang__)
#pragma GCC pop_options
#endif
