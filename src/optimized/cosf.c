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
 */

/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   float cosf(float x)
 *
 * Spec:
 *   cosf(0)    = 1
 *   cosf(-0)   = 1
 *   cosf(inf)  = NaN
 *   cosf(-inf) = NaN
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 *
 * cos is EVEN, so cos(x) = cos(|x|).  All arithmetic is carried out in double
 * precision; the reduced argument is evaluated with minimax sin/cos polynomials
 * and the result is rounded to float on return.
 *
 *   |x| <= pi/4 : direct evaluation (x*x is sign-independent).
 *       |x| < 2^-13          : cosf(x) = 1 - x^2/2
 *       2^-13 <= |x| <= pi/4 : cos minimax polynomial
 *
 *   pi/4 < |x| < pi/2 : cosf(x) = sin(pi/2 - |x|).
 *       q = pi/2_hi - |x| is exact (Sterbenz) and lies in (0, pi/4).
 *
 *   |x| >= pi/2 : reduce |x| = k*(pi/2) + r, region = k & 3, then
 *       region 0 :  cos(r)      region 2 : -cos(r)
 *       region 1 : -sin(r)      region 3 :  sin(r)
 *     pi/2 <= |x| < 2^25 : two-piece Cody-Waite reduction.
 *     |x| >= 2^25        : Payne-Hanek reduction.
 ******************************************
 */

#include <stdint.h>
#include <math.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>

#include "remainder_piby2_f.h"   /* Payne-Hanek mod-pi/2 reduction (cold path) */

/* Reduction constants. */
#define INV_PI_2    0x1.45f306dc9c883p-1    /* 2/pi                          */
#define PI2_HI      0x1.921fb54442d18p+0    /* pi/2 (nearest double)         */
#define PI2_HI_CW   0x1.921fb54400000p+0    /* pi/2 high part (24 low zeros) */
#define PI2_MID     0x1.0b4611a626331p-34   /* pi/2 mid part                 */
#define ALM_SHIFT   0x1.8p52                /* round-to-integer shifter      */

/* Minimax coefficients on [0, pi/4]:
 *   sin(r) = r + r^3 * (S1 + S2 r^2 + S3 r^4)
 *   cos(r) = 1 - r^2/2 + r^4 * (C1 + C2 r^2 + C3 r^4)
 */
static const double S1 = -0x1.5555529be884fp-3;
static const double S2 =  0x1.110c219cff89cp-7;
static const double S3 = -0x1.9ac6d7159dfebp-13;
static const double C1 =  0x1.5555543f4680cp-5;
static const double C2 = -0x1.6c12cf63bdbe5p-10;
static const double C3 =  0x1.9bd72c760e1a5p-16;

#define PIBY4_BITS  0x3F490FDBu     /* pi/4  as a float bit-pattern */
#define PIBY2_BITS  0x3FC90FDBu     /* pi/2  as a float bit-pattern */
#define COS_SMALL   0x3C000000u     /* 2^-13 */
#define COLD_BITS   0x4C000000u     /* 2^25 : Cody-Waite <-> Payne-Hanek cut */
#define INF_BITS    0x7F800000u

/* sin(r) ~= r + r^3 * (S1 + S2 r^2 + S3 r^4) -- takes the reduced r. */
static inline double sin_poly(double r)
{
    double r2 = r * r;
    double x3 = r * r2;
    double p  = POLY_EVAL_3(r2, S1, S2, S3, 0.0);
    return _LIBM_POLY_FMA(x3, p, r);
}

/* cos(r) ~= 1 - r^2/2 + r^4 * (C1 + C2 r^2 + C3 r^4) -- takes the reduced r.
 * The leading 1 - r^2/2 and the inner poly are evaluated so the independent
 * sub-expressions can issue in parallel; one final FMA folds the high term. */
static inline double cos_poly(double r)
{
    double r2 = r * r;
    double x4 = r2 * r2;
    double cp = POLY_EVAL_3(r2, C1, C2, C3, 0.0); /* (C1 + C2 r^2) + C3 r^4 */
    double t  = _LIBM_POLY_FMA(-0.5, r2, 1.0);  /* 1 - r^2/2            */
    return _LIBM_POLY_FMA(x4, cp, t);
}

float
ALM_PROTO_OPT(cosf)(float x)
{
    uint32_t axf = asuint32(x) & 0x7FFFFFFFu;   /* cos even -> work on |x| */

    /* cosf(inf) = cosf(-inf) = cosf(NaN) = NaN */
    if (unlikely(axf >= INF_BITS))
        return _cosf_special(x);

    /* |x| <= pi/4 : direct cos polynomial (x*x is sign-independent). */
    if (axf <= PIBY4_BITS) {
        if (unlikely(axf < COS_SMALL)) {
            double xd = (double)x;
            return (float)_LIBM_POLY_FMA(-0.5 * xd, xd, 1.0);
        }
        return (float)cos_poly((double)x);
    }

    /* pi/4 < |x| < pi/2 : cos(x) = sin(pi/2 - |x|), q exact (Sterbenz). */
    if (axf < PIBY2_BITS) {
        double q = PI2_HI - (double)asfloat(axf);
        return (float)sin_poly(q);
    }

    /* |x| >= pi/2 : reduce mod pi/2 -> (region, r). */
    int    region;
    double r;

    if (axf >= COLD_BITS) {
        /* Payne-Hanek reduction on the |x| bit-pattern. */
        remainder_piby2_f(axf, &region, &r);
    } else {
        /* Two-piece Cody-Waite reduction mod pi/2. */
        double absxd = (double)asfloat(axf);
        double dn = _LIBM_POLY_FMA(absxd, INV_PI_2, ALM_SHIFT);
        uint64_t nm = asuint64(dn);
        region = (int)(nm & 0x3);
        dn -= ALM_SHIFT;
        double rh = _LIBM_POLY_FMA(-dn, PI2_HI_CW, absxd);
        r = _LIBM_POLY_FMA(-dn, PI2_MID, rh);
    }

    /* Reconstruction (cos is even -> no input sign):
     *   region 0 :  cos(r) ; 1 : -sin(r) ; 2 : -cos(r) ; 3 : sin(r). */
    double result;
    if ((region & 1) == 0) {
        result = cos_poly(r);
        if (region == 2) result = -result;
    } else {
        result = sin_poly(r);
        if (region == 1) result = -result;
    }

    return (float)result;
}
