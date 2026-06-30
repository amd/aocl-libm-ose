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
 *   void sincos(double x, double *sin, double *cos)
 *
 * Spec:
 *   sin(0)=0, sin(-0)=-0, sin(+-inf)=NaN
 *   cos(0)=1, cos(+-inf)=NaN
 *
 * Implementation:
 *   Fused sin/cos: reduce |x| ONCE and reconstruct both results from the
 *   shared N=64 double-double table (sincos_tbl).  All arithmetic is double /
 *   double-double.
 *
 *     |x| <= pi/4 : direct evaluation of both polynomials (no reduction)
 *     pi/4 < |x| < pi/2 : cos(x)=sin(pi/2-|x|), sin(x)=sign(x)*cos(pi/2-|x|)
 *     |x| >= pi/2 : reduce |x| = m*(pi/32) + (r+rr), m in [0,64); then
 *         sin(|x|) and cos(|x|) come from one table entry; sin's odd sign is
 *         applied at the end.
 *       pi/2 <= |x| < 2^18 : Cody-Waite reduction (2-piece, escalates to 3)
 *       |x| >= 2^18        : Payne-Hanek reduction (remainder_piby32)
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

#include "sincos_tbl.h"            /* shared N=64 double-double sin/cos table */
#include "remainder_piby32.h"      /* Payne-Hanek mod pi/32 reducer          */

/* Inner (table-reduction) minimax coefficients on [-pi/64, pi/64]. */
static const double MS1 = -0x1.5555555555451p-3;
static const double MS2 =  0x1.111111072c563p-7;
static const double MS3 = -0x1.a01321c02ff64p-13;
static const double MC1 = -0x1.fffffffffff3dp-2;
static const double MC2 =  0x1.5555554a3495ap-5;
static const double MC3 = -0x1.6c10bd9b0d3bp-10;

/* Direct (|x| <= pi/4) minimax coefficients. */
static const double S1 = -0x1.5555555555555p-3, S2 =  0x1.1111111110bb3p-7,
                    S3 = -0x1.a01a019e83e5cp-13, S4 =  0x1.71de3796cde01p-19,
                    S5 = -0x1.ae600b42fdfa7p-26, S6 =  0x1.5e0b2f9a43bb8p-33;
static const double C1 =  0x1.5555555555555p-5, C2 = -0x1.6c16c16c16967p-10,
                    C3 =  0x1.A01A019F4EC91p-16, C4 = -0x1.27E4FA17F667Bp-22,
                    C5 =  0x1.1EEB690382EECp-29, C6 = -0x1.907DB47258AA7p-37;

/* Reduction constants. */
#define INV_PIBY32   0x1.45f306dc9c883p+3   /* 1/(pi/32) = 32/pi : k=round(x/(pi/32)) */
#define ALM_SHIFT    0x1.8p52               /* round-to-integer shifter      */
#define PI2_HI       0x1.921fb54442d18p+0   /* pi/2 (nearest double)         */
#define PI2_LO       0x1.1a62633145c07p-54  /* pi/2 - PI2_HI                 */
#define PI32_1       0x1.921fb54400000p-4   /* pi/32 head (24 trailing zeros)*/
#define PI32_1TAIL   0x1.0b4611a626331p-38  /* pi/32 tail 1                  */
#define PI32_2       0x1.0b4611a600000p-38  /* pi/32 head 2 (24 trailing zeros) */
#define PI32_2TAIL   0x1.3198a2e037073p-73  /* pi/32 tail 2 (deep)           */
#define ONE_BY_SIX   0.16666666666666666

#define SIGN_MASK    0x7FFFFFFFFFFFFFFFULL  /* clears the sign bit (magnitude) */
#define SIN_SMALL    0x3F20000000000000ULL  /* 2^-13 */
#define SIN_SMALLER  0x3E40000000000000ULL  /* 2^-27 */
#define PIBY4_BITS   0x3FE921FB54442D18ULL  /* |x| at pi/4 */
#define PIBY2_BITS   0x3FF921FB54442D18ULL  /* |x| at pi/2 */
#define COLD_BITS    0x4110000000000000ULL  /* 2^18 : Cody-Waite <-> Payne-Hanek cut */
#define ESC_BITS     25                     /* 2-CW -> 3-CW escalation depth */
#define INF_BITS     0x7FF0000000000000ULL

/* sin(m*pi/32 + (r+rr)) from the table entry. */
static inline double sincos_sin_compose(int m, double r, double rr)
{
    double T_cos = sincos_tbl[m].c;
    double T_sin = sincos_tbl[m].s;
    double T_cl  = sincos_tbl[m].cl;
    double T_sl  = sincos_tbl[m].sl;
    double r2 = r * r;
    double cp = POLY_EVAL_3(r2, MC1, MC2, MC3, 0.0);
    double sp = POLY_EVAL_3(r2, MS1, MS2, MS3, 0.0);
    double big   = _LIBM_POLY_FMA(r, T_cos, T_sin);
    double bigE  = _LIBM_POLY_FMA(r, T_cos, T_sin - big);
    double spoly = _LIBM_POLY_FMA(T_cos, r * sp, T_sin * cp);
    double tail  = _LIBM_POLY_FMA(r, T_cl, T_sl);
    double cor   = _LIBM_POLY_FMA(rr, _LIBM_POLY_FMA(-r, T_sin, T_cos), bigE);
    return big + _LIBM_POLY_FMA(r2, spoly, tail + cor);
}

/* cos(m*pi/32 + (r+rr)) from the table entry. */
static inline double sincos_cos_compose(int m, double r, double rr)
{
    double T_cos = sincos_tbl[m].c;
    double T_sin = sincos_tbl[m].s;
    double T_cl  = sincos_tbl[m].cl;
    double T_sl  = sincos_tbl[m].sl;
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

void
ALM_PROTO_OPT(sincos)(double x, double *sin, double *cos)
{
    uint64_t ux = asuint64(x);
    uint64_t ax = ux & SIGN_MASK;

    /* ===== |x| < pi/2 : direct evaluation, both outputs ================= */
    if (ax < PIBY2_BITS) {
        if (ax > PIBY4_BITS) {
            /* pi/4 < |x| < pi/2 : q = PI2_HI - |x| exact, in (0, pi/4).
             *   cos(x) = cos(|x|) = sin(q + PI2_LO)
             *   sin(x) = sign(x) * cos(q + PI2_LO) */
            double qd = PI2_HI - asdouble(ax);
            double q2 = qd * qd;
            double q3 = q2 * qd;
            double q4 = q2 * q2;
            /* cos(x) = sin(q + PI2_LO) via the sin form with the
             * double-double pi/2 tail coupled in (accurate near pi/2).
             * Estrin inner: S2+S3 q2+S4 q4+S5 q6+S6 q8. */
            double sq  = POLY_EVAL_5(q2, S2, S3, S4, S5, S6);
            double s2  = 0.5 * PI2_LO;
            double sqi = _LIBM_POLY_FMA(-q3, sq, s2);
            double sqt = _LIBM_POLY_FMA(q2, sqi, -PI2_LO);
            sq = _LIBM_POLY_FMA(-S1, q3, sqt);
            *cos = qd - sq;
            /* sin(x) = sign(x) * cos(q + PI2_LO).  Estrin: C1..C6. */
            double s  = 0.5 * q2;
            double t  = s - 1.0;
            double cq  = POLY_EVAL_6(q2, C1, C2, C3, C4, C5, C6);
            cq = cq * q4;
            double cres = _LIBM_POLY_FMA(-qd, PI2_LO, ((1.0 + t) - s)) + cq - t;
            *sin = __builtin_copysign(cres, x);
            return;
        } else if (ax >= SIN_SMALL) {
            /* 2^-13 <= |x| <= pi/4 : direct polynomials (signed x for sin). */
            double x2 = x * x;
            /* Estrin sin: S1..S6 ; cos: C1..C6. */
            double s  = POLY_EVAL_6(x2, S1, S2, S3, S4, S5, S6);
            *sin = _LIBM_POLY_FMA(x * x2, s, x);
            double p  = POLY_EVAL_6(x2, C1, C2, C3, C4, C5, C6);
            double pi = _LIBM_POLY_FMA(x2, p, -0.5);
            *cos = _LIBM_POLY_FMA(x2, pi, 1.0);
            return;
        } else if (ax > SIN_SMALLER) {
            /* 2^-27 < |x| < 2^-13 */
            *sin = _LIBM_POLY_FMA(-x * x * x, ONE_BY_SIX, x);
            *cos = _LIBM_POLY_FMA(-0.5 * x, x, 1.0);
            return;
        }
        *sin = x;
        *cos = 1.0;
        return;
    }

    /* sincos(inf) = sincos(NaN) = NaN */
    if (unlikely(ax >= INF_BITS)) {
        _sincos_special(x, sin, cos);
        return;
    }

    /* ===== |x| >= pi/2 : N=64 table reduction of |x| (once) ============= */
    uint64_t sign = ux >> 63;                 /* sin is odd; cos is even */
    int      m;
    double   r, rr;

    if (ax >= COLD_BITS) {
        /* |x| >= 2^18 : Payne-Hanek reduction of |x| (no sign fold). */
        remainder_piby32(ax, &m, &r, &rr);
    } else {
        /* pi/2 <= |x| < 2^18 : Cody-Waite mod pi/32 of |x|.  The pi/32 heads
         * carry 24 trailing zero bits (dn*head exact for the whole band) and
         * the tails are seated low (~2^-73) so the leftover rr -- the result
         * near a function zero (where r collapses to 0) -- keeps its low bits.
         * Two-piece head, escalating to three on deep cancellation. */
        double axd = asdouble(ax);            /* |x| (only needed here) */
        double dn = _LIBM_POLY_FMA(axd, INV_PIBY32, ALM_SHIFT);
        uint64_t nm = asuint64(dn); dn -= ALM_SHIFT;
        m = (int)(nm & 0x3F);
        double rh = _LIBM_POLY_FMA(-dn, PI32_1, axd);
        double rt = dn * PI32_1TAIL;
        r = rh - rt;
        int32_t xe = (int32_t)(ax >> 52);
        uint64_t uy = asuint64(r);
        if (xe - (int32_t)((uy << 1) >> 53) > ESC_BITS) {
            double tv = rh; rt = dn * PI32_2; rh = tv - rt;
            rt = _LIBM_POLY_FMA(dn, PI32_2TAIL, -((tv - rh) - rt)); r = rh - rt;
        }
        rr = (rh - r) - rt;
    }

    /* Both results from the SAME reduction; cos(|x|)=cos(x), sin(|x|) gets the
     * input sign (sin is odd). */
    double sres = sincos_sin_compose(m, r, rr);
    *cos = sincos_cos_compose(m, r, rr);
    *sin = sign ? -sres : sres;
}
