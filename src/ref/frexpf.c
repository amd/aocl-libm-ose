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


float FN_PROTOTYPE_REF(frexpf)(float value, int *exp)
{
    UT32 val;
    unsigned int sign;
    int exponent;
    val.f32 = value;
    sign = val.u32 & SIGNBIT_SP32;
    val.u32 = val.u32 & ~SIGNBIT_SP32; /* remove the sign bit */
    *exp = 0;
    if((val.u32 == 0x00000000) || (val.u32 == 0x7f800000)) 
        return value; /* value= +-0 or value= nan or value = +-inf return value */

    if(val.u32 > 0x7f800000)
     {
#ifdef WINDOWS
         return __amd_handle_errorf("frexpf", __amd_frexp, val.u32|QNANBITPATT_DP64, DOMAIN, AMD_F_NONE, EDOM, value, 0.0, 1);
#else
         return value+value;
#endif
     }
    exponent = val.u32 >> 23; /* get the exponent */

	if(exponent == 0)/*x is denormal*/
	{
		val.f32 = val.f32 * VAL_2PMULTIPLIER_SP;/*multiply by 2^24 to bring it to the normal range*/
		exponent = (val.u32 >> 23); /* get the exponent */
		exponent = exponent - MULTIPLIER_SP;
	}

    exponent -= 126; /* remove bias(127)-1 */
    *exp = exponent; /* set the integral power of two */
    val.u32 = sign | 0x3f000000 | (val.u32 & 0x007fffff);/* make the fractional part(divide by 2) */                                              
    return val.f32;
}


