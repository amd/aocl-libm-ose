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
 *   Fused sin/cos: reduce |x| modulo pi/2 ONCE, evaluate sin(r) and cos(r), and
 *   select both results by quadrant.  All arithmetic is in double.
 *
 *     |x| <= pi/4 : direct evaluation of both polynomials (no reduction)
 *     |x| > pi/4  : reduce |x| = k*(pi/2) + r, region = k & 3; then for r:
 *         sin(|x|): region 0->sin, 1->cos, 2->-sin, 3->-cos
 *         cos(|x|): region 0->cos, 1->-sin, 2->-cos, 3->sin
 *       sin's odd sign is applied at the end.
 *       pi/4 < |x| < 2^22 : Cody-Waite reduction
 *       |x| >= 2^22       : Payne-Hanek reduction
 */

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>

#include "remainder_piby2_f.h"  /* Payne-Hanek mod pi/2 reducer */

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

/* Reduction constants. */
#define INV_PI_2   0x1.45f306dc9c883p-1    /* 2/pi */
#define PI2_HEAD   0x1.921fb54400000p+0    /* pi/2 head */
#define PI2_TAIL   0x1.0b4611a626331p-34   /* pi/2 tail */
#define ALM_SHIFT  0x1.8p52                /* round-to-integer shifter */

/* Float-bit thresholds. */
#define PIBY4_BITS    0x3F490FDBu          /* pi/4  */
#define SIN_SMALL     0x3C000000u          /* 2^-7  */
#define SIN_SMALLER   0x39000000u          /* 2^-13 */
#define COLD_BITS     0x4A800000u          /* 2^22 : Cody-Waite <-> Payne-Hanek cut */
#define INF_BITS      0x7F800000u

static inline double sinpoly(double r, double r2)
{
    double x3 = r2 * r;
    return r + x3 * POLY_EVAL_3(r2, S1, S2, S3, S4);
}
static inline double cospoly(double r2)
{
    double t  = 1.0 - 0.5 * r2;
    double x4 = r2 * r2;
    return t + x4 * POLY_EVAL_3(r2, C1, C2, C3, C4);
}

void
ALM_PROTO_OPT(sincosf)(float x, float *sin, float *cos)
{
    uint32_t uxf = asuint32(x);
    uint32_t axf = uxf & POS_BITSET_F32;

    /* NaN / Inf */
    if (unlikely(axf >= INF_BITS)) {
        _sincosf_special(x, sin, cos);
        return;
    }

    /* |x| <= pi/4 : direct evaluation of both outputs. */
    if (axf <= PIBY4_BITS) {
        if (axf < SIN_SMALLER) {            /* |x| < 2^-13 */
            *sin = x;
            *cos = 1.0f;
            return;
        }
        double xd = (double)x, x2 = xd * xd;
        if (axf < SIN_SMALL) {              /* 2^-13 <= |x| < 2^-7 */
            *sin = (float)(xd - xd * x2 * ONE_BY_SIX);
            *cos = (float)(1.0 - 0.5 * x2);
            return;
        }
        *sin = (float)sinpoly(xd, x2);      /* signed x -> free sign */
        *cos = (float)cospoly(x2);
        return;
    }

    uint32_t sign  = uxf >> 31;
    double   absxd = (double)asfloat(axf);

    /* |x| > pi/4 : reduce |x| mod pi/2 -> (region, r), once. */
    int    region = 0;
    double r = 0.0;

    if (axf >= COLD_BITS) {
        /* Payne-Hanek reduction on the |x| bit-pattern. */
        remainder_piby2_f(axf, &region, &r);
    } else {
        /* Two-piece Cody-Waite reduction mod pi/2. */
        double dn   = _LIBM_POLY_FMA(absxd, INV_PI_2, ALM_SHIFT);
        uint64_t nm = asuint64(dn);
        region = (int)(nm & 0x3);
        dn -= ALM_SHIFT;
        double rhead = _LIBM_POLY_FMA(-dn, PI2_HEAD, absxd);
        double rtail = dn * PI2_TAIL;
        r = rhead - rtail;
    }

    /* Evaluate both on-sector polynomials once. */
    double r2 = r * r;
    double sr = sinpoly(r, r2);
    double cr = cospoly(r2);

    /* Quadrant select on |x| results:
     *   sin(|x|): 0->sr, 1->cr, 2->-sr, 3->-cr
     *   cos(|x|): 0->cr, 1->-sr, 2->-cr, 3->sr  */
    double s_use, c_use;
    if ((region & 1) == 0) {
        s_use = sr;  c_use = cr;
    } else {
        s_use = cr;  c_use = sr;
    }
    if (region & 2) s_use = -s_use;
    if (region == 1 || region == 2) c_use = -c_use;

    /* cos is even -> cos(x) = cos(|x|); sin is odd -> apply input sign. */
    float sf = (float)s_use;
    *cos = (float)c_use;
    *sin = sign ? -sf : sf;
}
