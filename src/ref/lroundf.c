/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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

#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"

long int FN_PROTOTYPE_REF(lroundf)(float f)
{
    UT32 u32d;
    UT32 u32Temp,u32result;
    int intexp, shift;
    U32 sign;
    long int  result;

    u32d.f32 = u32Temp.f32 = f;
    if ((u32d.u32 & 0X7F800000) == 0x7F800000)
    {
        /*else the number is infinity*/
		//Raise range or domain error
        {
		#ifdef WIN64
			__amd_handle_errorf("lroundf", __amd_lround, SIGNBIT_SP32, _DOMAIN, AMD_F_NONE, EDOM, f, 0.0, 1);
			return (long int)SIGNBIT_SP32;
		#else
         if((u32d.u32 & 0x7fffffff) == 0x7f800000)
            return SIGNBIT_DP64;
         if((u32d.u32 & 0x7fffffff) >= 0x7fc00000)
                __amd_handle_errorf("lround", __amd_lround, (unsigned int)SIGNBIT_DP64, _DOMAIN, AMD_F_NONE, EDOM, f, 0.0, 1);
         else    
                __amd_handle_errorf("lround", __amd_lround, (unsigned int)SIGNBIT_DP64, _DOMAIN, AMD_F_INVALID, EDOM, f, 0.0, 1);
            
		 return SIGNBIT_DP64; /*GCC returns this when the number is out of range*/
		#endif
        }

    }

    u32Temp.u32 &= 0x7FFFFFFF;
    intexp = (u32d.u32 & 0x7F800000) >> 23;
    sign = u32d.u32 & 0x80000000;
    intexp -= 0x7F;


    /* 1.0 x 2^-1 is the smallest number which can be rounded to 1 */
    if (intexp < -1)
        return (0);


#ifdef WIN64
    /* 1.0 x 2^31 is already too large */
    if (intexp >= 31)
    {
        result = 0x80000000;
		__amd_handle_errorf("lroundf", __amd_lround, result, _DOMAIN, AMD_F_NONE, EDOM, f, 0.0, 1);
        return result;
	}

#else
    /* 1.0 x 2^31 (or 2^63) is already too large */
    if (intexp >= 63)
    {
        result = 0x8000000000000000;
        __amd_handle_errorf("lroundf", __amd_lround, result, _DOMAIN, AMD_F_NONE, EDOM, f, 0.0, 1);
		return result;
    }
 #endif

    u32result.f32 = u32Temp.f32;

    /* >= 2^23 is already an exact integer */
    if (intexp < 23)
    {
        /* add 0.5, extraction below will truncate */
        u32result.f32 = u32Temp.f32 + 0.5F;
    }
    intexp = (u32result.u32 & 0x7f800000) >> 23;
    intexp -= 0x7f;
    u32result.u32 &= 0x7fffff;
    u32result.u32 |= 0x00800000;

    result = u32result.u32;

    #ifdef WIN64
    shift = intexp - 23;
    #else

    /*Since float is only 32 bit for higher accuracy we shift the result by 32 bits
     * In the next step we shift an extra 32 bits in the reverse direction based
     * on the value of intexp*/
    result = result << 32;
    shift = intexp - 55; /*55= 23 +32*/
    #endif


	if(shift < 0)
		result = result >> (-shift);
	if(shift > 0)
        result = result << (shift);

    if (sign)
        result = -result;
    return result;

}




