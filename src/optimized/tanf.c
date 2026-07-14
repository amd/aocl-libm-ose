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
 *   float tan(float x)
 *
 * Spec:
 *   tanf(0)   = 0
 *   tanf(-0)  = 0
 *   tanf(inf) = NaN
 *   tanf(NaN) = NaN
 *
 * Table-assisted method: reduce x mod pi/32 -> (m, r); tan(m*pi/32 + r) via
 * a tan/cot addition formula using a per-index table value.
 *
 * Implementation Notes
 * --------------------
 * if x is inf/NaN            -> return NaN (invalid for inf).
 * if |x| < 2^-13            -> underflow; return x.
 * if 2^-13 <= |x| < 2^-7    -> tan(x) = x + x^3/3.
 * if 2^-7 <= |x| <= 3pi/8   -> tan(x) = x + x^3 * P(x^2)/Q(x^2) (no reduction).
 * if 3pi/8 < |x| < 2^20     -> Cody-Waite reduce to (m, r), m = low 5 bits.
 * if |x| >= 2^20            -> Payne-Hanek reduce to (m, r).
 *
 * Reconstruct: v = tanf_tbl[m], t ~= tan(r); use cot form (1+v*t)/(v-t) for
 * 8 <= m < 24 (angle near pi/2), else tan form (v+t)/(1-v*t).
 */

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>

#include "remainder_piby32_f.h"
#include "tanf_tbl.h"

#define ALM_TANF_ZERO    0x0u
#define ALM_TANF_ARG_MIN 0x39000000u   /* 2^-13 */
#define ALM_TANF_SMALL_X 0x3C000000u   /* 2^-7  */
#define ALM_TANF_3PIBY8  0x3F96CBE4u   /* 3pi/8  (fast-path cutoff) */
#define ALM_TANF_COLD    0x49800000u   /* 2^20   (CW <-> PH cut)    */

#define ONE_BY_THREE  0.333333333333333333
#define ALM_SHIFT     0x1.8p+52
#define TWO_BY_32_PI  0x1.45f306dc9c883p+3
#define PI32_1        0x1.921fb54400000p-4
#define PI32_1TAIL    0x1.0b4611a626331p-38

/* tan(r) - r over r^3 on [-pi/64, pi/64] */
#define TR1  0x1.555552b3c6b3dp-2
#define TR2  0x1.11554dbd8da02p-3

/* tan(x) = x + x^3 * P(x^2)/Q(x^2) on [0, 3pi/8] */
#define P0   0x1.55555559b73b6p-2
#define P1  -0x1.09fcd268e82f5p-6
#define P2   0x1.c7454814457bep-18
#define Q1  -0x1.cb79009a981c2p-2
#define Q2   0x1.204f82bec008cp-6

static inline float
tan_kernel(double x)
{
    double u  = x * x;
    double pn = POLY_EVAL_2(u, P0, P1, P2);        /* P0 + P1*u + P2*u^2 */
    double pd = POLY_EVAL_2(u, 1.0, Q1, Q2);       /* 1  + Q1*u + Q2*u^2 */
    return (float)(x + x * u * (pn / pd));
}

static inline double
tan_compose(int m, double r)
{
    double v = tanf_tbl[m];
    double u  = r * r;
    double t  = r + r * u * _LIBM_POLY_FMA(TR2, u, TR1);
    if (m >= 8 && m < 24) {
        return _LIBM_POLY_FMA(v, t, 1.0) / (v - t);
    }
    return (v + t) / _LIBM_POLY_FMA(-v, t, 1.0);
}

static float
__tanf_special(uint32_t uxf)
{
    if (uxf & MANTBITS_SP32) {
        if (uxf & QNAN_MASK_32)
            return __alm_handle_errorf(uxf | QNAN_MASK_32, AMD_F_NONE);
        return __alm_handle_errorf(uxf | QNAN_MASK_32, AMD_F_INVALID);
    }
    return __alm_handle_errorf(INDEFBITPATT_SP32, AMD_F_INVALID);
}

float ALM_PROTO_OPT(tanf)(float x)
{
    uint32_t uxf  = asuint32(x);
    uint32_t sign = uxf & SIGNBIT_SP32;
    uint32_t axf  = uxf & ~SIGNBIT_SP32;

    if (unlikely((axf & PINFBITPATT_SP32) == PINFBITPATT_SP32))
        return __tanf_special(uxf);

    double sx = (double) x;

    if (axf <= ALM_TANF_3PIBY8) {            
        if (axf < ALM_TANF_SMALL_X) {
            if (axf == ALM_TANF_ZERO)
                return x;
            if (axf < ALM_TANF_ARG_MIN)
                return __alm_handle_errorf(uxf, AMD_F_UNDERFLOW | AMD_F_INEXACT);
            return (float)(sx + (sx * sx * sx * ONE_BY_THREE));
        }
        return tan_kernel(sx);
    }

    int    m;
    double r;

    if (unlikely(axf >= ALM_TANF_COLD)) {
        remainder_piby32_f(axf, &m, &r);
        if (sign) { m = (32 - m) & 0x1F; r = -r; }
    }
    else {
        double dn = _LIBM_POLY_FMA(sx, TWO_BY_32_PI, ALM_SHIFT);
        m = (int)(asuint64(dn) & 0x1F);
        dn -= ALM_SHIFT;
        double rhead = _LIBM_POLY_FMA(-dn, PI32_1, sx);
        r = rhead - dn * PI32_1TAIL;
    }

    return (float) tan_compose(m, r);
}
