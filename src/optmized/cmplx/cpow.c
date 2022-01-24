/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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
 *   double complex cpow(double complex x, double complex y)
 *
  Implementation Notes
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
 *   x^y = r^c*e^(i*θ*c) * e^(i*b*ln⁡(r))*e^(-θ *b)
 *   x^y = r^c*e^(-θ*b) * e^(i* θ*c*b*ln(r))   (5)
 *   Let theta =  θ*c*b*ln(r)
 *   Substituting for theta in (5),
 *   x^y = r^c*e^(-θ*b)* e^(i*theta) (6)
 *
 *   e^(i*theta) = cos(theta) + i * sin(theta) (7)
 *   Substituting for (7) in (6), we get
     x^y = r^c*e^(-θ*b)*(cos⁡(theta)+i*sin⁡(theta
*
 */

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/constants.h>
#include <libm/typehelper.h>
#include <math.h>

#if defined(__clang__)
#define CMPLX(X, Y) __builtin_complex ((double) (X), (double) (Y))
#endif

fc64_t
ALM_PROTO_OPT(cpow)(fc64_t x, fc64_t y) {

    fc64_t w;
    double y_re, y_im, r, theta, abs_x, arg_x;

    y_re = creal(y);

    y_im = cimag(y);

    abs_x = cabs(x);

    /*check if absolute balue of complex number is zero */
    if ((asuint64(abs_x) & (~ALM_F64_SIGN_MASK)) == 0) {

        #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
            return (fc64_t) { 0.0, 0.0 };
        #else
            return CMPLX(0.0, 0.0);
        #endif
    }

    arg_x = carg(x);

    r = pow(abs_x, y_re);

    theta = y_re * arg_x;

    if (asuint64(y_im) != 0) {

        r = r * exp(-y_im * arg_x);

        theta = theta + y_im * log(abs_x);

    }

    /*check if r is infinity */
    if(unlikely((asuint64(r) & (~ALM_F64_SIGN_MASK)) == ALM_F64_INF)) {

        if(asuint64(theta) == 0) {

            #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                return (fc64_t) { r,0 };
             #else
                return CMPLX(r, 0);
            #endif
        }

    #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
         return (fc64_t) { r,r };
    #else
        return CMPLX(r, r);
    #endif
    }

    #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
        w = (fc64_t) { r * cos(theta), r * sin(theta) };
    #else
        w = CMPLX(r * cos(theta), r * sin(theta));
    #endif

    return w;
}

