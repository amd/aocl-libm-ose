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
 *   double complex cexp(double complex x)
 *
 *   Let x = a + I*b
 *
 *   exp(x) = exp(a) + exp(I * b)
 *
 *   exp(I * b) = cos(b) + I*sin(b) (in polar form)
 *
 *   exp(x) = exp(a)*cos(b) + I*exp(a)*sin(b)
 *
 */

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/constants.h>
#include <libm/typehelper.h>
#include <stdio.h>

#if defined(__clang__)
#define CMPLX(X, Y) __builtin_complex ((double) (X), (double) (Y))
#endif

fc64_t
ALM_PROTO_OPT(cexp)(fc64_t z)
{
    double re, im;
    double zy_re, zy_im;

    const double MAX_ARG = 0x1.62e42fefa39efp+9;
    const double EXP_MAX_ARG = 0x1.fffffffffff2ap+1023;

    re = creal(z);

    im = cimag(z);

    if((asuint64(re) & ~ALM_F64_SIGN_MASK) == 0) {

        if((asuint64(im) & ~ALM_F64_SIGN_MASK) == 0) {

            zy_re = 1.0;

            zy_im = 0.0;

        } else {

            ALM_PROTO(sincos)(im, &zy_im, &zy_re);

        }
    } else {

        if((asuint64(im) & ~ALM_F64_SIGN_MASK) == 0) {

            zy_re = ALM_PROTO(exp)(re);

            zy_im = 0.0;

        } else {
            if(re > MAX_ARG) {

                double t = re - MAX_ARG;

                ALM_PROTO(sincos)(im, &zy_im, &zy_re);

                double r =  ALM_PROTO(exp)(t);

                zy_re *= r;

                zy_im *= r;

                zy_re *= EXP_MAX_ARG;

                zy_im *= EXP_MAX_ARG;

                #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
                    return (fc64_t) { zy_re, zy_im };
                #else
                    return CMPLX(zy_re, zy_im);
                #endif


            }

            double t = ALM_PROTO(exp)(re);

            ALM_PROTO(sincos)(im, &zy_im, &zy_re);

            zy_re *= t;

            zy_im *= t;

        }
    }


    #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
        return (fc64_t) { zy_re, zy_im };
    #else
        return CMPLX(zy_re, zy_im);
    #endif

}

