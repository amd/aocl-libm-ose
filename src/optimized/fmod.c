/*
 * Copyright (C) 2008-2024 Advanced Micro Devices, Inc. All rights reserved.
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

/******************************************
 * Implementation Notes:
 * This is a literal translation of ASM routine for fmodf + optimizations
 * 
 * Prototype:
 * double fmod(double x, double y)
 * 
 * Algorithm:
 * As designed in the ASM fmod float variant routine.
 * The same algorithm is extended into C, for double variant,
 * with optimizations wherever possible.
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

double ALM_PROTO_OPT(fmod)(double x, double y)
{
    uint64_t ax, ay;

    ax = asuint64(x);
    ay = asuint64(y);

    ax &= ~SIGNBIT_DP64;
    ay &= ~SIGNBIT_DP64;

    // Input value checks for NAN, INF
    if(ay == 0)
    {
        return __alm_handle_error(ay | QNANBITPATT_DP64, AMD_F_INVALID);
    }

    if(unlikely((ax & EXPBITS_DP64) >= EXPBITS_DP64))
    {
        // X is NAN or INF
        if( (ax & EXPBITS_DP64) == EXPBITS_DP64)
            return __alm_handle_error(ay | QNANBITPATT_DP64, AMD_F_INVALID);
        else
            #ifdef WINDOWS
                __alm_handle_error(ay | QNANBITPATT_DP64, AMD_F_INVALID);
            #else
                return x + x;
            #endif
    }

    if(unlikely((ay & EXPBITS_DP64) > EXPBITS_DP64))
    {
        return 1.0;
    }

    if(ax == ay)
    {
        return 0.0;
    }

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
        if(xe == 0 || ye == 0 || diff_exp > 52)
        {
            /* This is a special case of fmod function applicable only for double variant.
             * The x87 assembly instruction FPREM1 (Floating Point Partial fmod) has to be used for accurate calculation.
             * Since its equivalent is not available in C, the assembly version function is called directly instead.
             */
            double result = __amd_bas64_fmod(x, y);
            return result;
        }
        quo = diff_exp / 24;

        scale = ((24 * quo) + 1023) << 52;
    }

    double w = asdouble((uint64_t)scale) * ady;
    double two_p_MINUS_24 = asdouble(0x3E70000000000000);
    double t=0, temp2=0;

    while(quo > 0)
    {
        quo--;

        t = adx / w;
        uint64_t tu = (uint64_t)t;
        t = (double)(tu);

        adx = adx - (t * w);
        w *= two_p_MINUS_24;
    }

    temp2 = adx / w;
    uint64_t tu = (uint64_t)temp2;
    temp2 = (double)(tu);

    adx = adx - (temp2 * w);

    if(x<0)
    {
        adx = 0.0 - adx;
    }
    return adx;
}
