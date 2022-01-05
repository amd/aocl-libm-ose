/*
 * Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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
 *   float complex cpowf(float complex x, float complex y)
 *
 *   Let x = a + I*b
 *
 *   Let y = c + I*d
 *
 *   x^y = (a+i*b) ^ (c+i*b)
 *   x^y = (a+i*b)^c + (a+i*b)^(i*b) (1)
 *
 *   Expressing a + i*b in polar form,
 *   a + i*b = r*e^(i*θ) (2) where θ is the argument of the complex number = tan^(-1)⁡(b/a)
 *   and r is the magnitude of the complex number = √(a^2+ b^2 )
 *   Argument of a complex number can be computed using the cargs() function
 *    Magnitude of a complex number can be completed using cabs() function
 *
 *   Substituting (2) in (1) we get,
 *   x^y = (r*e^(i*θ))^c * (r*e^(i*θ))^(i*b)
 *   x^y = r^c * e^(i*θ*c) * r^(i*b) * e^(-θ*b)  as i * i = -1 (3)
 *
 *   r^(i*b)= e^(i*b*ln(r)) (4)
 *   Substituting (4) in (3), we get,
 *   x^y = r^c*e^(i*θ*c) * e^(i*b*ln⁡(r))*e^(-θ*b)
 *   x^y = r^c*e^(-θ*b) * e^(i* θ*c*b*ln(r))   (5)
 *   Let theta =  θ*c*b*ln(r)
 *   Substituting for theta in (5),
 *   x^y = r^c*e^(-θ*b)* e^(i*theta) (6)
 *
 *   e^(i*theta) = cos(theta) + i * sin(theta) (7)
 *   Substituting for (7) in (6), we get
     x^y = r^c*e^(-θ*b)*(cos⁡(theta)+i*sin⁡(theta

 */

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/constants.h>
#include <libm/typehelper.h>
#include <math.h>
#include <stdio.h>

#if defined(__clang__)
#define CMPLXF(X, Y) __builtin_complex ((double) (X), (double) (Y))
#endif

fc32_t
ALM_PROTO_OPT(cpowf)(fc32_t x, fc32_t y) {

    fc32_t w;
    float y_re, y_im, r, theta, abs_x, arg_x;

    y_re = crealf(y);

    y_im = cimagf(y);

    abs_x = cabsf(x);

    /*check if absolute value of x is zero */
    if (unlikely((asuint32(abs_x) & (~ALM_F32_SIGN_MASK)) == 0)) {

        return CMPLXF(0.0f, 0.0f);

    }

    arg_x = cargf(x);

    r = powf(abs_x, y_re);

    theta = y_re * arg_x;

    if (asuint32(y_im) != 0) {

        /*compute only if imaginary part of y is not zero */

        r = r * expf(-y_im * arg_x);

        theta = theta + y_im * logf(abs_x);

    }

    /*check if r is infinity */
    if(unlikely((asuint32(r) & (~ALM_F32_SIGN_MASK)) == ALM_F32_INF_MASK)) {

        if(asuint32(theta) == 0) {

            return CMPLXF(r, 0);

        }

        return CMPLXF(r, r);

    }

    w = CMPLXF(r * cosf(theta), r * sinf(theta));

    return w;
}

