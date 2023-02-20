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
 * float hypotf(float a, float b)
 *
 * IEEE SPEC:
 * Returns sqrt(x*x + y*y) with no overflow or underflow unless
 * the result warrants it.
 * x or y is infinity. ISO C99 defines that we must
 * return +infinity, even if the other argument is NaN
 *
 * Algorithm:
 *
 * hypotf(x,y) = sqrt(x^2 + y^2)
 *
 */

#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include "libm_inlines_amd.h"

float ALM_PROTO_OPT(hypotf)(float x, float y)
{
    double dx, dy, dr, retval;

    double large;

    uint32_t avx, avy;
    uint32_t val;

    avx = asuint32(x);

    avx &= ~SIGNBIT_SP32;

    avy = asuint32(y);

    avy &= ~SIGNBIT_SP32;

    val = PINFBITPATT_SP32;

    if (avx >= PINFBITPATT_SP32 || avy >= PINFBITPATT_SP32) {

        if ((avx == POS_INF_F32) || (avy == POS_INF_F32)) {

            if(avx > POS_INF_F32) {
                #ifdef WINDOWS
                return  asfloat(PINFBITPATT_SP32) ;
                #else
                if(!(avx & QNAN_MASK_32)) //x is snan
                    return __alm_handle_errorf(avx, AMD_F_INVALID);
                #endif
            }
            if(avy > POS_INF_F32) {
                #ifdef WINDOW
                return asfloat(PINFBITPATT_SP32);
                #else
                if(!(avy & QNAN_MASK_32)) //y is snan
                    return __alm_handle_errorf(avy, AMD_F_INVALID);
                #endif
            }
            return asfloat(PINFBITPATT_SP32);
        }

        if((avx >= POS_INF_F32) || (avy >= POS_INF_F32)) {

            if(avx >= POS_INF_F32) {

                    #ifdef WINDOWS
                    return __alm_handle_errorf(asuint32(x) | QNAN_MASK_32, 0);
                    #else
                    if(!(avx & QNAN_MASK_32)) //x is snan
                        return __alm_handle_errorf(asuint32(x) | QNAN_MASK_32, AMD_F_INVALID);
                    #endif
            }
            if(avy >= POS_INF_F32) {

                #ifdef WINDOWS
                return __alm_handle_errorf(asuint32(y) | QNAN_MASK_32, 0);
                #else
                if(!(avy & QNAN_MASK_32)) //y is snan
                    return __alm_handle_errorf(asuint32(y) | QNAN_MASK_32, AMD_F_INVALID);
                #endif
            }
            return __alm_handle_errorf(POS_QNAN_F32, 0);
        }
    }

    large = 3.40282346638528859812e+38; /* 0x47efffffe0000000 */

    dx = x, dy = y;

    if(avx == 0x0) {

        return asfloat(avy);

    }
    if(avy == 0x0) {

        return asfloat(avx);

    }

    dr = (dx * dx + dy * dy);

    retval = sqrt(dr);

    if (retval > large) {

        return __alm_handle_errorf(PINFBITPATT_SP32, AMD_F_INEXACT | AMD_F_OVERFLOW);

    }
    else if((avx != 0) && (avy != 0)) {

        val = asuint32((float)(retval)) >> EXPSHIFTBITS_SP32;

        if(val == 0x0) {

            return __alm_handle_errorf(asuint32((float)retval), AMD_F_INEXACT | AMD_F_UNDERFLOW);

        }
    }

    return (float)retval;
}


