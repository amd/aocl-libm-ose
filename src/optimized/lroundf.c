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
 */

/* Implementation notes
    long int amd_lroundf (float x);
    when x = NAN, return x, raise FE_INVALID
    when x is QNAN, return x
    when x is INF, return x.
*/

#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/typehelper.h>

long int ALM_PROTO_OPT(lroundf)(float x)
{
    uint32_t ux, ux_temp, uresult, sign;
    int intexp, shift;
    long int result;
    float r;

    ux = ux_temp = asuint32(x);

    if (unlikely((ux & POS_INF_F32) == POS_INF_F32))
    {
        /*else the number is infinity*/
        //Raise range or domain error
        #ifdef WIN64
            return (long)__alm_handle_errorf(SIGNBIT_SP32, AMD_F_NONE);
        #else
            if((ux & POS_BITSET_F32) == POS_INF_F32)
                return (long)SIGNBIT_DP64;
            if((ux & POS_BITSET_F32) >= QNANBITPATT_SP32)
                return (long)__alm_handle_errorf(SIGNBIT_DP64, AMD_F_NONE);
            else
                return (long)__alm_handle_errorf(SIGNBIT_DP64, AMD_F_INVALID);
        #endif
    }

    ux_temp &= POS_BITSET_F32;
    intexp = (ux & POS_INF_F32) >> 23;
    sign = ux & SIGNBIT_SP32;
    intexp -= 0x7F;

    /* 1.0 x 2^-1 is the smallest number which can be rounded to 1 */
    if (intexp < -1)
        return (0);


#ifdef WIN64
    /* 1.0 x 2^31 is already too large */
    if (intexp >= 31)
    {
        result = NEG_ZERO_F32;
        return (long)__alm_handle_errorf(result, AMD_F_NONE);
    }

#else
    /* 1.0 x 2^31 (or 2^63) is already too large */
    if (intexp >= 63)
    {
        result = (long)NEG_ZERO_F64;
        return (long)__alm_handle_errorf((unsigned long long)result, AMD_F_NONE);
    }
 #endif

    r = asfloat(ux_temp);

    /* >= 2^23 is already an exact integer */
    if (intexp < 23)
    {
        /* add 0.5, extraction below will truncate */
        r = asfloat(ux_temp) + 0.5F;
    }
    uresult = asuint32(r);
    intexp = (uresult & POS_INF_F32) >> 23;
    intexp -= 0x7F;
    uresult &= MANTBITS_SP32; // store all mantissa bits ONLY of the result
    uresult |= IMPBIT_SP32; // set the last bit of exp as 1

    result = uresult; // All original bits of mantissa and last bit of exp is set!

    #ifdef WIN64
    shift = intexp - 23;
    #else

    /*Since float is only 32 bit for higher accuracy we shift the result by 32 bits
     * In the next step we shift an extra 32 bits in the reverse direction based
     * on the value of intexp*/
    result = result << 32;
    shift = intexp - 55; /* 55= 23 +32 */
    #endif

    if(shift < 0)
        result = result >> (-shift);
    if(shift > 0)
        result = result << (shift);

    if (sign)
        result = -result;

    return result;
}
