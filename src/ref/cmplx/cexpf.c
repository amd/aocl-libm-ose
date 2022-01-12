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
 */

#include <libm_macros.h>
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/constants.h>


fc32_t
ALM_PROTO_REF(cexpf)(fc32_t z)
{
    flt32_t     re, im;
    f32_t       zy_re, zy_im;
    #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
        re.f = crealf(z);
        im.f = (float)cimagf(z);
    #else
        re.f = crealf(z);
        im.f = (float)cimag(z);
    #endif

    if((re.u & ALM_F32_SIGN_MASK) == 0) {
        if((im.u & ALM_F32_SIGN_MASK) == 0) {
            zy_re = 1.0f;
            zy_im = 0.0f;
        } else {
            ALM_PROTO(sincosf)(im.f, &zy_im, &zy_re);
        }
    } else {
        if((im.u & ALM_F32_SIGN_MASK) == 0) {
            zy_re = ALM_PROTO(expf)(re.f);
            zy_im = 0.0f;
        } else {
            f32_t t = ALM_PROTO(expf)(re.f);
            ALM_PROTO(sincosf)(im.f, &zy_im, &zy_re);
            zy_re *= t;
            zy_im *= t;
        }

    }

    #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
        return (fc32_t) { zy_re, zy_im };
    #else
        return zy_re + I*zy_im;
    #endif
}

