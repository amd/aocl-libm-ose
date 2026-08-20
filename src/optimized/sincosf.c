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
 *   void sincosf(float x, float *sin, float *cos)
 *
 * Spec:
 *   sinf(0)=0, sinf(-0)=-0, sinf(+-inf)=NaN
 *   cosf(0)=1, cosf(+-inf)=NaN
 *
 * Implementation:
 *   Fused sin/cos: reduce |x| modulo pi/2 ONCE, evaluate sin(r) and cos(r) once
 *   each, and select both results by quadrant.  All arithmetic is in double.
 *
 *     |x| <= pi/4 : direct evaluation of both polynomials (no reduction)
 *     pi/4 < |x| < pi/2 : cos(x)=sin(pi/2-|x|), sin(x)=sign(x)*cos(pi/2-|x|)
 *     |x| >= pi/2 : reduce |x| = k*(pi/2) + r, region = k & 3; then for r:
 *         sin(|x|): region 0->sin, 1->cos, 2->-sin, 3->-cos
 *         cos(|x|): region 0->cos, 1->-sin, 2->-cos, 3->sin
 *       sin's odd sign is applied at the end.
 *       pi/2 <= |x| < 2^25 : Cody-Waite reduction
 *       |x| >= 2^25        : Payne-Hanek reduction
 */

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>
#include <math.h>

#include "remainder_piby2_f.h"  /* Payne-Hanek mod pi/2 reducer */

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

/* Reduction constants. */
#define INV_PI_2   0x1.45f306dc9c883p-1
#define PI2_HI     0x1.921fb54442d18p+0    /* pi/2                          */
#define PI2_HI_CW  0x1.921fb54400000p+0    /* pi/2 high part (24 low zeros) */
#define PI2_MID    0x1.0b4611a626331p-34   /* pi/2 mid part                 */
#define ALM_SHIFT  0x1.8p52

/* Float-bit thresholds. */
#define PIBY4_BITS    0x3F490FDBu          /* pi/4  */
#define PIBY2_BITS    0x3FC90FDBu          /* pi/2  */
#define SIN_SMALL     0x3C000000u          /* 2^-7  */
#define SIN_SMALLER   0x39000000u          /* 2^-13 */
#define COLD_BITS     0x4C000000u          /* 2^25 : Cody-Waite <-> Payne-Hanek cut */
#define INF_BITS      0x7F800000u

static inline double sinpoly(double r, double r2)
{
    double x3 = r * r2;
    double p  = POLY_EVAL_3(r2, S1, S2, S3, 0.0);
    return _LIBM_POLY_FMA(x3, p, r);
}
static inline double cospoly(double r, double r2)
{
    double x4 = r2 * r2;
    double cp = POLY_EVAL_3(r2, C1, C2, C3, 0.0);
    double s = 0.5 * r2;
    double t = 1.0 - s;
    return _LIBM_POLY_FMA(x4, cp, t);
}

void
ALM_PROTO_OPT(sincosf)(float x, float *sin, float *cos)
{
    uint32_t uxf = asuint32(x);
    uint32_t axf = uxf & 0x7FFFFFFFu;

    /* NaN / Inf */
    if (unlikely(axf >= INF_BITS)) {
        _sincosf_special(x, sin, cos);
        return;
    }

    /* ===== |x| <= pi/4 : direct evaluation, both outputs ================ */
    if (axf <= PIBY4_BITS) {
        if (axf < SIN_SMALLER) {            /* |x| < 2^-13 */
            *sin = x;
            *cos = 1.0f;
            return;
        }
        if (axf < SIN_SMALL) {              /* 2^-13 <= |x| < 2^-7 */
            double xd = (double)x;
            *sin = (float)_LIBM_POLY_FMA(-xd * xd * xd, 0.166666666666666, xd);
            *cos = (float)_LIBM_POLY_FMA(-0.5 * xd, xd, 1.0);
            return;
        }
        double xd = (double)x, x2 = xd * xd;
        *sin = (float)sinpoly(xd, x2);      /* signed x -> free sign */
        *cos = (float)cospoly(xd, x2);
        return;
    }

    uint32_t sign  = uxf >> 31;
    double   absxd = (double)asfloat(axf);

    /* ===== pi/4 < |x| < pi/2 : Sterbenz pair on q = PI2_HI - |x| ======== */
    if (axf < PIBY2_BITS) {
        double q  = PI2_HI - absxd;             /* exact */
        double q2 = q * q;
        /* cos(x) = sin(pi/2 - |x|) = sin(q) */
        *cos = (float)sinpoly(q, q2);
        /* sin(x) = sign(x) * cos(pi/2 - |x|) = sign(x) * cos(q) */
        float c = (float)cospoly(q, q2);
        *sin = sign ? -c : c;
        return;
    }

    /* ===== |x| >= pi/2 : reduce |x| mod pi/2 -> (region, r), ONCE ======= */
    int    region = 0;
    double r = 0.0;

    if (axf >= COLD_BITS) {
        /* |x| >= 2^25 : Payne-Hanek reduction (region for |x|). */
        remainder_piby2_f(axf, &region, &r);
    } else {
        /* pi/2 <= |x| < 2^25 : Cody-Waite mod pi/2 (2-piece head). */
        double dn   = _LIBM_POLY_FMA(absxd, INV_PI_2, ALM_SHIFT);
        uint64_t nm = asuint64(dn);
        region = (int)(nm & 0x3);
        dn -= ALM_SHIFT;
        double rh = _LIBM_POLY_FMA(-dn, PI2_HI_CW, absxd);
        r = _LIBM_POLY_FMA(-dn, PI2_MID, rh);
    }

    /* Evaluate both on-sector polynomials once. */
    double r2 = r * r;
    double sr = sinpoly(r, r2);
    double cr = cospoly(r, r2);

    /* Quadrant select (region for |x|):
     *   sin(|x|): 0->sr, 1->cr, 2->-sr, 3->-cr
     *   cos(|x|): 0->cr, 1->-sr, 2->-cr, 3->sr  */
    double s_use, c_use;
    if ((region & 1) == 0) {
        s_use = sr;  c_use = cr;            /* region 0 / 2 */
    } else {
        s_use = cr;  c_use = sr;            /* region 1 / 3 */
    }
    if (region & 2) { s_use = -s_use; }     /* region 2 / 3: sin negated */
    if (region == 1 || region == 2) { c_use = -c_use; }

    /* cos is even -> cos(x) = cos(|x|); sin is odd -> apply input sign. */
    float cf = (float)c_use;
    float sf = (float)s_use;
    *cos = cf;
    *sin = sign ? -sf : sf;
}
