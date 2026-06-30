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
 *   double cos(double x)
 *
 * Spec:
 *   cos(0)    = 1
 *   cos(-0)   = 1
 *   cos(inf)  = NaN
 *   cos(-inf) = NaN
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 *
 * cos is EVEN, so cos(x) = cos(|x|).
 *
 *   |x| < pi/2 : direct evaluation, no reduction.
 *       |x| < 2^-27          : cos(x) = 1
 *       2^-27 <= |x| < 2^-13 : cos(x) = 1 - x^2/2
 *       2^-13 <= |x| < pi/4  : cos minimax polynomial
 *       pi/4 <= |x| < pi/2   : cos(x) = sin(pi/2 - |x|), q exact (Sterbenz)
 *
 *   |x| >= pi/2 : reduce modulo pi/32 to an index m (0..63) and a
 *       double-double residual (r, rr), then reconstruct
 *           cos(x) = cos(m*pi/32 + r)
 *                  = cos(m*pi/32)*cos(r) - sin(m*pi/32)*sin(r)
 *       from a 64-entry double-double table of {cos, sin} at m*pi/32.
 *
 *       The mod-pi/32 reduction uses a Cody-Waite split whose pi/32 chunks
 *       carry 24 trailing zero bits in their heads (so each dn*chunk is exact)
 *       and tails seated low enough that the leftover rr -- which is the result
 *       near a cos zero (where r collapses to 0) -- retains its low bits.  This
 *       keeps the near-zero worst cases below 1 ULP without a heavier reducer.
 *
 *       pi/2 <= |x| < 2^18 : two/three-piece Cody-Waite (escalates on deep
 *                            cancellation).
 *       |x| >= 2^18        : Payne-Hanek reduction.
 ******************************************
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

#include "sincos_tbl.h"         /* N=64 double-double cos/sin table at m*pi/32 */
#include "remainder_piby32.h"   /* Payne-Hanek mod-pi/32 reduction (cold path) */

/* Reduction constants. */
#define INV_PIBY32   0x1.45f306dc9c883p+3   /* 1/(pi/32) = 32/pi : k=round(x/(pi/32)) */
#define ALM_SHIFT    0x1.8p52               /* round-to-integer shifter      */
#define PI2_HI       0x1.921fb54442d18p+0   /* pi/2 (nearest double)         */
#define PI2_LO       0x1.1a62633145c07p-54  /* pi/2 - PI2_HI                 */
#define PI32_1       0x1.921fb54400000p-4   /* pi/32 head (24 trailing zeros)*/
#define PI32_1TAIL   0x1.0b4611a626331p-38  /* pi/32 tail 1                  */
#define PI32_2       0x1.0b4611a600000p-38  /* pi/32 head 2 (24 trailing zeros) */
#define PI32_2TAIL   0x1.3198a2e037073p-73  /* pi/32 tail 2 (deep)           */

/* Minimax sin/cos coefficients on [0, pi/4] (direct, |x| < pi/4). */
static const double S1 = -0x1.5555555555555p-3;
static const double S2 =  0x1.1111111110bb3p-7;
static const double S3 = -0x1.a01a019e83e5cp-13;
static const double S4 =  0x1.71de3796cde01p-19;
static const double S5 = -0x1.ae600b42fdfa7p-26;
static const double S6 =  0x1.5e0b2f9a43bb8p-33;
static const double C1 =  0x1.5555555555555p-5;
static const double C2 = -0x1.6c16c16c16967p-10;
static const double C3 =  0x1.A01A019F4EC91p-16;
static const double C4 = -0x1.27E4FA17F667Bp-22;
static const double C5 =  0x1.1EEB690382EECp-29;
static const double C6 = -0x1.907DB47258AA7p-37;

/* Inner (table-reduction) minimax coefficients on [-pi/64, pi/64]. */
static const double MS1 = -0x1.5555555555451p-3;
static const double MS2 =  0x1.111111072c563p-7;
static const double MS3 = -0x1.a01321c02ff64p-13;
static const double MC1 = -0x1.fffffffffff3dp-2;
static const double MC2 =  0x1.5555554a3495ap-5;
static const double MC3 = -0x1.6c10bd9b0d3bp-10;

#define PIBY4_BITS  0x3FE921FB54442D18ULL   /* |x| at pi/4   */
#define PIBY2_BITS  0x3FF921FB54442D18ULL   /* |x| at pi/2   */
#define COS_SMALL   0x3F20000000000000ULL   /* 2^-13         */
#define COS_SMALLER 0x3E40000000000000ULL   /* 2^-27         */
#define COLD_BITS   0x4110000000000000ULL   /* 2^18 : Cody-Waite <-> Payne-Hanek cut */
#define ESC_BITS    25                      /* 2-CW -> 3-CW escalation depth */
#define INF_BITS    0x7FF0000000000000ULL

/* Reconstruct cos(m*pi/32 + (r + rr)) from the double-double table entry. */
static inline double cos_compose(int m, double r, double rr)
{
    double T_cos = sincos_tbl[m].c,  T_sin = sincos_tbl[m].s;
    double T_cl  = sincos_tbl[m].cl, T_sl  = sincos_tbl[m].sl;
    double r2 = r * r;
    double cp = POLY_EVAL_3(r2, MC1, MC2, MC3, 0.0);
    double sp = POLY_EVAL_3(r2, MS1, MS2, MS3, 0.0);
    double big   = _LIBM_POLY_FMA(-r, T_sin, T_cos);
    double bigE  = _LIBM_POLY_FMA(-r, T_sin, T_cos - big);
    double spoly = _LIBM_POLY_FMA(-T_sin, r * sp, T_cos * cp);
    double tail  = _LIBM_POLY_FMA(-r, T_sl, T_cl);
    double cor   = _LIBM_POLY_FMA(rr, _LIBM_POLY_FMA(-r, T_cos, -T_sin), bigE);
    return big + _LIBM_POLY_FMA(r2, spoly, tail + cor);
}

double
ALM_PROTO_OPT(cos)(double x)
{
    uint64_t ux = asuint64(x);
    uint64_t ax = ux & 0x7FFFFFFFFFFFFFFFULL;

    /* |x| < pi/2 : direct evaluation. */
    if (ax < PIBY2_BITS) {
        if (ax > PIBY4_BITS) {
            /* pi/4 <= |x| < pi/2 : cos(x) = sin(pi/2 - |x|). */
            double qd = PI2_HI - asdouble(ax);     /* exact (Sterbenz) */
            double q2 = qd * qd, q3 = q2 * qd;
            double p  = POLY_EVAL_5(q2, S2, S3, S4, S5, S6);
            double s2 = 0.5 * PI2_LO;
            double inner = _LIBM_POLY_FMA(-q3, p, s2);
            double t     = _LIBM_POLY_FMA(q2, inner, -PI2_LO);
            p            = _LIBM_POLY_FMA(-S1, q3, t);
            return qd - p;
        }
        if (ax >= COS_SMALL) {
            double x2 = x * x;
            double p  = POLY_EVAL_6(x2, C1, C2, C3, C4, C5, C6);
            double inner = _LIBM_POLY_FMA(x2, p, -0.5);
            return _LIBM_POLY_FMA(x2, inner, 1.0);
        }
        if (ax >= COS_SMALLER)
            return _LIBM_POLY_FMA(-0.5 * x, x, 1.0);
        return 1.0;
    }

    /* cos(inf) = cos(-inf) = cos(NaN) = NaN */
    if (unlikely(ax >= INF_BITS))
        return _cos_special(x);

    /* cos is EVEN -> reduce |x| directly; no input-sign fix-up is needed. */
    double axd = asdouble(ax);                 /* |x| */
    int    m;
    double r, rr;

    if (ax < COLD_BITS) {
        /* pi/2 <= |x| < 2^18 : inline Cody-Waite reduction mod pi/32.  The
         * 24-trailing-zero heads make each dn*chunk exact; the deep tails seat
         * the leftover (rr) low enough to stay sub-ULP at the cos zeros. */
        double dn = _LIBM_POLY_FMA(axd, INV_PIBY32, ALM_SHIFT);
        uint64_t nm = asuint64(dn);
        dn -= ALM_SHIFT;
        m = (int)(nm & 0x3F);

        double rh = _LIBM_POLY_FMA(-dn, PI32_1, axd);
        double rt = dn * PI32_1TAIL;
        r = rh - rt;

        int32_t xe = (int32_t)(ax >> 52);
        uint64_t uy = asuint64(r);
        int32_t edif = xe - (int32_t)((uy << 1) >> 53);
        if (edif > ESC_BITS) {
            double tv = rh;
            rt = dn * PI32_2;
            rh = tv - rt;
            rt = _LIBM_POLY_FMA(dn, PI32_2TAIL, -((tv - rh) - rt));
            r  = rh - rt;
        }
        rr = (rh - r) - rt;
        return cos_compose(m, r, rr);
    }

    /* |x| >= 2^18 : Payne-Hanek mod-pi/32 reduction of |x| (no sign fix-up). */
    remainder_piby32(ax, &m, &r, &rr);
    return cos_compose(m, r, rr);
}
