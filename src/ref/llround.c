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

#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>

#ifdef WINDOWS
/*In windows llong long int is 64 bit and long int is 32 bit.
  In Linux long long int and long int both are of size 64 bit*/
long long int ALM_PROTO_REF(llround)(double d)
{
    UT64 u64d;
    UT64 u64Temp,u64result;
    int intexp, shift;
    U64 sign;
    long long int result;

    u64d.f64 = u64Temp.f64 = d;

    if ((u64d.u32[1] & 0X7FF00000) == 0x7FF00000)
    {
        /*the number is infinity*/
        //Got to raise range or domain error
		__alm_handle_error("llround", __amd_lround, u64d.u64, _DOMAIN, AMD_F_NONE, EDOM, d, 0.0, 1);
		return SIGNBIT_DP64; /*GCC returns this when the number is out of range*/
    }

    u64Temp.u32[1] &= 0x7FFFFFFF;
    intexp = (u64d.u32[1] & 0x7FF00000) >> 20;
    sign = u64d.u64 & 0x8000000000000000;
    intexp -= 0x3FF;

    /* 1.0 x 2^-1 is the smallest number which can be rounded to 1 */
    if (intexp < -1)
        return (0);

    /* 1.0 x 2^31 (or 2^63) is already too large */
    if (intexp >= 63)
    {
        /*Based on the sign of the input value return the MAX and MIN*/
        result = 0x8000000000000000; /*Return LONG MIN*/
		__alm_handle_error("llround", __amd_lround, result, _DOMAIN, AMD_F_NONE, EDOM, d, 0.0, 1);
        return result;
    }

    u64result.f64 = u64Temp.f64;
    /* >= 2^52 is already an exact integer */
    if (intexp < 52)
    {
        /* add 0.5, extraction below will truncate */
        u64result.f64 = u64Temp.f64 + 0.5;
    }

    intexp = ((u64result.u32[1] >> 20) & 0x7ff) - 0x3FF;

    u64result.u32[1] &= 0xfffff;
    u64result.u32[1] |= 0x00100000; /*Mask the last exp bit to 1*/
    shift = intexp - 52;

    if(shift < 0)
        u64result.u64 = u64result.u64 >> (-shift);
    if(shift > 0)
        u64result.u64 = u64result.u64 << (shift);

    result = u64result.u64;

    if (sign)
        result = -result;

    return result;
}

#else //WINDOWS 
/*llroundf is equivalent to the linux implementation of 
  lroundf. Both long int and long long int are of the same size*/
long long int ALM_PROTO_REF(llround)(double d)
{
    long long int result;
    result = FN_PROTOTYPE(lround)(d);
    return result;
}

#endif
