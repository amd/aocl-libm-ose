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
 *   float sinf(float x)
 *
 * Spec:
 *   sinf(0)    = 0
 *   sinf(-0)   = -0
 *   sinf(inf)  = NaN
 *   sinf(-inf) = NaN
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 *
 * All arithmetic is carried out in double precision; the reduced argument is
 * evaluated with minimax sin/cos polynomials and the result is rounded to
 * float on return.
 *
 *   |x| <= pi/4 : direct evaluation on the SIGNED value (no reduction).
 *       |x| < 2^-13          : sinf(x) = x
 *       2^-13 <= |x| < 2^-7  : sinf(x) = x - x^3/6
 *       2^-7  <= |x| <= pi/4 : sin minimax polynomial
 *
 *   pi/4 < |x| < pi/2 : sinf(x) = sign(x) * cos(pi/2 - |x|).
 *       q = pi/2_hi - |x| is exact (Sterbenz) and lies in (0, pi/4).
 *
 *   |x| >= pi/2 : reduce |x| = k*(pi/2) + r, region = k & 3, then
 *       region 0 :  sin(r)      region 2 : -sin(r)
 *       region 1 :  cos(r)      region 3 : -cos(r)
 *       and the input sign is folded onto the result (sin is odd).
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

#define ONE_BY_SIX  0.166666666666666f

#define PIBY4_BITS  0x3F490FDBu     /* pi/4  as a float bit-pattern */
#define PIBY2_BITS  0x3FC90FDBu     /* pi/2  as a float bit-pattern */
#define SIN_SMALL   0x3C000000u     /* 2^-7  */
#define SIN_SMALLER 0x39000000u     /* 2^-13 */
#define COLD_BITS   0x4C000000u     /* 2^25 : Cody-Waite <-> Payne-Hanek cut */
#define INF_BITS    0x7F800000u

/* sin(r) ~= r + r^3 * (S1 + S2 r^2 + S3 r^4) */
static inline double sin_poly(double r, double r2)
{
    double x3 = r * r2;
    double p  = POLY_EVAL_3(r2, S1, S2, S3, 0.0);
    return _LIBM_POLY_FMA(x3, p, r);
}

/* cos(r) ~= 1 - r^2/2 + r^4 * (C1 + C2 r^2 + C3 r^4) */
static inline double cos_poly(double r, double r2)
{
    double x4 = r2 * r2;
    double cp = POLY_EVAL_3(r2, C1, C2, C3, 0.0);
    double s = 0.5 * r2;
    double t = 1.0 - s;
    return _LIBM_POLY_FMA(x4, cp, t);
}

float
ALM_PROTO_OPT(sinf)(float x)
{
    uint32_t uxf = asuint32(x);
    uint32_t axf = uxf & 0x7FFFFFFFu;

    /* sinf(inf) = sinf(-inf) = sinf(NaN) = NaN */
    if (unlikely(axf >= INF_BITS))
        return _sinf_special(x);

    /* |x| <= pi/4 : direct evaluation on the signed value. */
    if (axf <= PIBY4_BITS) {
        if (axf < SIN_SMALLER) return x;
        if (axf < SIN_SMALL) {
            double xd = (double)x;
            return (float)_LIBM_POLY_FMA(-xd * xd * xd, ONE_BY_SIX, xd);
        }
        double xd = (double)x, x2 = xd * xd;
        return (float)sin_poly(xd, x2);
    }

    /* |x| > pi/4 : sign folded at the end (sin is odd); work on |x|. */
    uint32_t sign  = uxf >> 31;
    double   absxd = (double)asfloat(axf);

    /* pi/4 < |x| < pi/2 : sin(x) = sign(x) * cos(pi/2 - |x|). */
    if (axf < PIBY2_BITS) {
        double r  = PI2_HI - absxd;          /* exact (Sterbenz) */
        double r2 = r * r;
        float  res = (float)cos_poly(r, r2);
        return sign ? -res : res;
    }

    /* |x| >= pi/2 : reduce mod pi/2 -> (region, r). */
    int    region = 0;
    double r = 0.0;

    if (axf >= COLD_BITS) {
        /* Payne-Hanek reduction on the |x| bit-pattern. */
        remainder_piby2_f(axf, &region, &r);
    } else {
        /* Two-piece Cody-Waite reduction mod pi/2. */
        double dn = _LIBM_POLY_FMA(absxd, INV_PI_2, ALM_SHIFT);
        uint64_t nm = asuint64(dn);
        region = (int)(nm & 0x3);
        dn -= ALM_SHIFT;
        double rh = _LIBM_POLY_FMA(-dn, PI2_HI_CW, absxd);
        r = _LIBM_POLY_FMA(-dn, PI2_MID, rh);
    }

    /* Reconstruction (sin is odd -> input sign folded at the end):
     *   region 0 :  sin(r) ; 1 : cos(r) ; 2 : -sin(r) ; 3 : -cos(r). */
    double r2 = r * r;
    double result;
    if ((region & 1) == 0) {
        result = sin_poly(r, r2);
        if (region == 2) result = -result;
    } else {
        result = cos_poly(r, r2);
        if (region == 3) result = -result;
    }

    float res = (float)result;
    return sign ? -res : res;
}
