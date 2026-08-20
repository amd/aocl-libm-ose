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
 *   float complex clog(float complex x)
 *
 *   IEEE SPEC:
 *   If x is (-0,+0), the result is (-∞,π) and FE_DIVBYZERO is raised
 *   If x is (+0,+0), the result is (-∞,+0) and FE_DIVBYZERO is raised
 *   If x is (x,+∞) (for any finite x), the result is (+∞,π/2)
 *   If x is (x,NaN) (for any finite x), the result is (NaN,NaN) and FE_INVALID may be raised
 *   If x is (-∞,y) (for any finite positive y), the result is (+∞,π)
 *   If x is (+∞,y) (for any finite positive y), the result is (+∞,+0)
 *   If x is (-∞,+∞), the result is (+∞,3π/4)
 *   If x is (+∞,+∞), the result is (+∞,π/4)
 *   If x is (±∞,NaN), the result is (+∞,NaN)
 *   If x is (NaN,y) (for any finite y), the result is (NaN,NaN) and FE_INVALID may be raised
 *   If x is (NaN,+∞), the result is (+∞,NaN)
 *   If x is (NaN,NaN), the result is (NaN,NaN)
 *
 *   Algorithm:
 *
 *   Let x = a + I*b
 *
 *   x in polar form = r * e^(i*theta)
 *
 *   r is the magnitude or the absolute value or x.
 *
 *   theta = atan2(y/x)
 *
 *   log(x) = log(r) + (I * theta)
 *
 *
 */
#include <math.h>
#include <libm_macros.h>
#include <libm_util_amd.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/constants.h>
#include <libm/typehelper.h>

fc32_t
ALM_PROTO_OPT(clogf)(fc32_t x) {

    float a = crealf(x);
    float b = cimagf(x);

    /* Get absolute value of real and imaginary parts */
    uint32_t abs_re = asuint32(a) & 0x7FFFFFFFU;
    uint32_t abs_im = asuint32(b) & 0x7FFFFFFFU;

    /* Compute imaginary part of the logarithm */
    float theta = ALM_PROTO_REF(atan2f)(b, a);

    /* If either component is infinite, return (+inf, theta) */
    if (abs_re == POS_INF_F32 || abs_im == POS_INF_F32)
        return CMPLXF(asfloat(POS_INF_F32), theta);

    /* Switch to double precision for the rest of the computation */
    double da   = (double)a;
    double db   = (double)b;
    double b_sq = db * db;
    double abs2 = da * da + b_sq;

    /* Compute z^2 - 1 in double precision for the log1p branch 
       To avoid catastrophic cancellation, we use log1p when |z|^2 is in (1/2, 2)
       |z|^2 - 1 is computed as (a-1)(a+1) + b^2 */
    double p_d;
    if (abs2 > 0.5 && abs2 < 2.0) {
        double abs2_m1 = (da - 1.0) * (da + 1.0) + b_sq;
        p_d = 0.5 * ALM_PROTO_OPT(log1p)(abs2_m1);
    } else {
        p_d = 0.5 * ALM_PROTO_OPT(log)(abs2);
    }

    return CMPLXF((float)p_d, theta);

}

