/*
 * Copyright (C) 2008-2025 Advanced Micro Devices, Inc. All rights reserved.
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

#include <stdint.h>
#include <math.h>
#include <float.h>

#include "libm_macros.h"
#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>

#include <stdio.h>

#define FMOD_X_NAN   1
#define FMOD_Y_ZERO  2
#define FMOD_X_INF   3

float ALM_PROTO_OPT(fmodf)(float x, float y)
{
    uint32_t fax, fay;

    fax = asuint32(x);
    fay = asuint32(y);

    fax &= ~SIGNBIT_SP32;
    fay &= ~SIGNBIT_SP32;

    /*Check if y in NaN. If yes, return NaN */
    if(unlikely(fay > POS_INF_F32))
    {
        return x * y;
    }

    /* Check if y is Zero. If yes, return NaN and raise exception*/
    if(unlikely(fay == 0))
    {
        return _fmodf_special(x, asfloat(fay | QNANBITPATT_SP32), FMOD_Y_ZERO);
    }

    /* Check if x is NaN or INF */
    if(unlikely((fax & EXPBITS_SP32) >= EXPBITS_SP32))
    {
       /* x is NaN. Return NaN. Raise exception for Windows */
       if(fax > POS_INF_F32)
       {
        #ifdef WINDOWS
            __alm_handle_errorf(fay | QNANBITPATT_SP32, AMD_F_INVALID);
        #else
            return x + x;
        #endif
       }
       /* x is INF. Return NaN and raise exception */
       else
       {
         return _fmodf_special(x, asfloat(fay | QNANBITPATT_SP32), FMOD_X_INF);
       }
    }

    if(unlikely((fay & EXPBITS_SP32) > EXPBITS_SP32))
    {
        return (x*y)/(x*y);
    }

    if(fax == fay)
    {
        return (0.0f * x);
    }

    double dx = (double)x;
    double dy = (double)y;

    uint64_t ax = asuint64(dx);
    uint64_t ay = asuint64(dy);

    ax &= POS_BITSET_DP64;
    ay &= POS_BITSET_DP64;

    double adx = asdouble(ax);
    double ady = asdouble(ay);

    uint64_t xe = (EXPBITS_DP64 & ax) >> 52;
    uint64_t ye = (EXPBITS_DP64 & ay) >> 52;

    if(adx < ady)
    {
        return x;
    }

    int64_t scale = 0x3FF0000000000000;
    int64_t quo = 0;

    if(ye < xe)
    {
        int64_t diff_exp = (int64_t)(xe - ye);
        quo = diff_exp / 24;

        scale = 24 * quo;
        scale += 1023;
        scale = scale << 52;
    }

    double w = asdouble((uint64_t)scale) * ady;
    double two_p_MINUS_24 = asdouble(0x3E70000000000000);
    double t=0, temp2=0;

    while(1)
    {
        quo--;
        if(quo < 0)
        {
            break;
        }

        t = adx / w;
        uint64_t tu = (uint64_t)t;
        t = (double)(tu);

        t *= w;
        w *= two_p_MINUS_24;
        adx -= t;
    }

    temp2 = adx / w;
    uint64_t tu = (uint64_t)temp2;
    temp2 = (double)(tu);

    double temp3 = temp2 * w;
    adx -= temp3;

    if(dx<0)
    {
        adx = 0.0 - adx;
    }

    return (float)adx;
}
