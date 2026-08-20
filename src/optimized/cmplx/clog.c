/*
 * Copyright (C) 2008-2026 Advanced Micro Devices, Inc. All rights reserved.
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
 *
 *
 * Implementation Notes
 * --------------------
 * Signature:
 *   double complex clog(double complex z)
 *
 *   IEEE/C99 SPEC:
 *   If z is (-0,+0), the result is (-inf,+pi)   and FE_DIVBYZERO is raised
 *   If z is (+0,+0), the result is (-inf,+0)    and FE_DIVBYZERO is raised
 *   If z is (x,+inf)   (any finite x), the result is (+inf, pi/2)
 *   If z is (x,NaN)    (any finite x), the result is (NaN,NaN), FE_INVALID may be raised
 *   If z is (-inf,y)   (any finite positive y), the result is (+inf, pi)
 *   If z is (+inf,y)   (any finite positive y), the result is (+inf, +0)
 *   If z is (-inf,+inf),                        the result is (+inf, 3*pi/4)
 *   If z is (+inf,+inf),                        the result is (+inf, pi/4)
 *   If z is (+/-inf,NaN),                       the result is (+inf, NaN)
 *   If z is (NaN,y)    (any finite y), the result is (NaN,NaN), FE_INVALID may be raised
 *   If z is (NaN,+inf),                         the result is (+inf, NaN)
 *   If z is (NaN,NaN),                          the result is (NaN,NaN)
 *
 *   Algorithm:
 *     clog(a + i*b) = log|z| + i*atan2(b, a),   |z| = sqrt(a^2 + b^2)
 *
 *   theta = atan2(b, a) handles every C99 special case above; it is computed
 *   locally in each return path so the finite path skips it until needed and
 *   the non-finite path avoids any of the magnitude work below.  For
 *   log|z| we sort |larger| >= |smaller| and pick one of three paths:
 *
 *   (1) inf/NaN input -> bit-pattern return per the C99 table.
 *
 *   (2) |larger| > 2^500 or 0 < |larger| < 2^-500  (larger*larger would
 *       over/underflow):
 *           log|z| = log(|larger|) + 0.5 * log1p((smaller/larger)^2),
 *                                              |smaller/larger| <= 1
 *
 *   (3) Normal path: form |z|^2 as a double-double (h, t) via FMA-based
 *       Two-Product + DD-add (dd_two_prod, dd_add), then
 *           0.5*log1p((h-1) + t)   if 0.5 < h < 2   (h-1 exact by Sterbenz)
 *           0.5*log(h + t)         otherwise
 *       The DD pair avoids the catastrophic cancellation in the
 *       unit-circle band
 */
#include <math.h>
#include <libm_macros.h>
#include <libm_util_amd.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/constants.h>
#include <libm/typehelper.h>

#define CLOG_WIDE_HI_EXP  (1023u + 500u)  /* biased exp of 2^500  = 1523 */
#define CLOG_WIDE_LO_EXP  (1023u - 500u)  /* biased exp of 2^-500 =  523 */


fc64_t
ALM_PROTO_OPT(clog)(fc64_t z) {
    double a = creal(z);
    double b = cimag(z);

    /* Get absolute value of real and imaginary parts */
    uint64_t abs_re = asuint64(a) & POS_BITSET_DP64;
    uint64_t abs_im = asuint64(b) & POS_BITSET_DP64;

    /* Special-case handling for the REAL part.
     *  - If either component is +/-inf  -> Re(clog z) = +inf
     *  - Otherwise, if either is NaN    -> Re(clog z) = NaN
     *  - Otherwise fall through to the finite path below..
     */
    if (abs_re >= POS_INF_F64 || abs_im >= POS_INF_F64) {
        double theta = ALM_PROTO_OPT(atan2)(b, a);
        if (abs_re == POS_INF_F64 || abs_im == POS_INF_F64)
            return CMPLX(asdouble(POS_INF_F64), theta);
        return CMPLX(asdouble(POS_QNAN_F64), theta);
    }

    /* Sort by magnitude */
    double larger  = (abs_re >= abs_im) ? a : b;
    double smaller = (abs_re >= abs_im) ? b : a;

    /* Wide-magnitude path: log|z| = log(|larger|) + 0.5*log1p((smaller/larger)^2).
     * Triggered only when |larger| is so large that larger*larger would
     * overflow, or so small that it would underflow below the min normal. */
    uint64_t abs_larger_bits = asuint64(larger) & POS_BITSET_DP64;
    uint32_t larger_exp      = (uint32_t)(abs_larger_bits >> 52);
    if (abs_larger_bits != 0 &&
        (larger_exp > CLOG_WIDE_HI_EXP || larger_exp < CLOG_WIDE_LO_EXP)) {
        double r = smaller / larger;     /* |r| <= 1 because of the sort */
        double p = ALM_PROTO_OPT(log)(asdouble(abs_larger_bits))
                 + 0.5 * ALM_PROTO_OPT(log1p)(r * r);
        return CMPLX(p, ALM_PROTO_OPT(atan2)(b, a));
    }

    /* Build |z|^2 as a double-double (abs_h, abs_t)*/
    dd_t a2_dd = dd_two_prod(larger,  larger);
    dd_t b2_dd = dd_two_prod(smaller, smaller);
    dd_t sum   = dd_add(a2_dd, b2_dd);
    double abs_h = sum.hi;
    double abs_t = sum.lo;

    /* Near |z|=1 use log1p on (|z|^2 - 1): abs_h-1 is exact by Sterbenz
     * for 0.5 <= abs_h <= 2.0, preserving abs_t. Elsewhere log is fine. */
    double p;
    if (abs_h > 0.5 && abs_h < 2.0) {
        double um1 = (abs_h - 1.0) + abs_t;
        p = 0.5 * ALM_PROTO_OPT(log1p)(um1);
    } else {
        p = 0.5 * ALM_PROTO_OPT(log)(abs_h + abs_t);
    }

    return CMPLX(p, ALM_PROTO_OPT(atan2)(b, a));
}
