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
 *   |x| > pi/4 : reduce |x| = k*(pi/2) + r, region = k & 3, then
 *       region 0 :  sin(r)      region 2 : -sin(r)
 *       region 1 :  cos(r)      region 3 : -cos(r)
 *       and the input sign is folded onto the result (sin is odd).
 *     pi/4 < |x| < 2^22 : two-piece Cody-Waite reduction.
 *     |x| >= 2^22       : Payne-Hanek reduction.
 ******************************************
 */

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
#define INV_PI_2    0x1.45f306dc9c883p-1    /* 2/pi */
#define PI2_HEAD    0x1.921fb54400000p+0    /* pi/2 head */
#define PI2_TAIL    0x1.0b4611a626331p-34   /* pi/2 tail */
#define ALM_SHIFT   0x1.8p52                /* round-to-integer shifter */

/* Minimax coefficients on [0, pi/4]:
 *   sin(r) = r + r^3 * (S1 + S2 r^2 + S3 r^4 + S4 r^6)
 *   cos(r) = 1 - r^2/2 + r^4 * (C1 + C2 r^2 + C3 r^4 + C4 r^6)
 */
static const double S1 = -0x1.555555498da0fp-3;
static const double S2 =  0x1.11110755d4c09p-7;
static const double S3 = -0x1.a00ee7ff9dc16p-13;
static const double S4 =  0x1.6cb68bd474643p-19;
static const double C1 =  0x1.55555542ef976p-5;
static const double C2 = -0x1.6c16b1f30ee6bp-10;
static const double C3 =  0x1.a00e2e56ec4d6p-16;
static const double C4 = -0x1.23b4eb603b655p-22;

#define ONE_BY_SIX  0x1.5555555555555p-3   /* 1/6 in double */

#define PIBY4_BITS  0x3F490FDBu     /* pi/4  as a float bit-pattern */
#define SIN_SMALL   0x3C000000u     /* 2^-7  */
#define SIN_SMALLER 0x39000000u     /* 2^-13 */
#define COLD_BITS   0x4A800000u     /* 2^22 : Cody-Waite <-> Payne-Hanek cut */
#define INF_BITS    0x7F800000u

float
ALM_PROTO_OPT(sinf)(float x)
{
    uint32_t uxf = asuint32(x);
    uint32_t axf = uxf & POS_BITSET_F32;

    /* sinf(inf) = sinf(-inf) = sinf(NaN) = NaN */
    if (unlikely(axf >= INF_BITS))
        return _sinf_special(x);

    /* |x| <= pi/4 : direct evaluation on the signed value. */
    if (axf <= PIBY4_BITS) {
        if (axf < SIN_SMALLER) return x;
        if (axf < SIN_SMALL) {
            double xd = (double)x;
            return (float)(xd - xd * xd * xd * ONE_BY_SIX);
        }
        double xd = (double)x, x2 = xd * xd, x3 = x2 * xd;
        return (float)(xd + x3 * POLY_EVAL_3(x2, S1, S2, S3, S4));
    }

    /* |x| > pi/4 : sign folded at the end (sin is odd); work on |x|. */
    uint32_t sign  = uxf >> 31;
    double   absxd = (double)asfloat(axf);

    /* reduce mod pi/2 -> (region, r). */
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
        double rhead = _LIBM_POLY_FMA(-dn, PI2_HEAD, absxd);
        double rtail = dn * PI2_TAIL;
        r = rhead - rtail;
    }

    /* Reconstruction: region&1 picks sin/cos; the quadrant sign and the input
     * sign are folded together by a single parity test (sin is odd). */
    double x2 = r * r;
    double result;
    if (region & 1) {
        double x4 = x2 * x2;
        double t  = 1.0 - 0.5 * x2;
        result = t + x4 * POLY_EVAL_3(x2, C1, C2, C3, C4);
    } else {
        double x3 = x2 * r;
        result = r + x3 * POLY_EVAL_3(x2, S1, S2, S3, S4);
    }

    uint32_t flip = sign ^ (uint32_t)(region >> 1);
    if (flip & 1)
        return (float)(-result);
    return (float)result;
}
