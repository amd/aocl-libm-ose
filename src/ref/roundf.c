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

#include "fn_macros.h"
#include "libm_util_amd.h"
#include "libm_special.h"

float FN_PROTOTYPE_REF(roundf)(float f)
{
    UT32 u32f, u32Temp;
    U32 u32sign, u32exp, u32mantissa;
    int intexp;            /*Needs to be signed */
    u32f.f32 = f;
    u32sign = u32f.u32 & SIGNBIT_SP32;
    if ((u32f.u32 & 0X7F800000) == 0x7F800000)
    {
        //u32f.f32 = f;
        /*Return Quiet Nan.
         * Quiet the signalling nan*/
        if(!((u32f.u32 & MANTBITS_SP32) == 0))
        {
            #ifdef WINDOWS
			return __amd_handle_errorf("roundf", __amd_round, u32f.u32 |= QNAN_MASK_32, _DOMAIN, 0, EDOM, f, 0.0, 1);
            #else
                if(!(u32f.u32 & 0x00400000)) //x is snan
			return __amd_handle_errorf("roundf", __amd_round, u32f.u32, _DOMAIN, AMD_F_INVALID, EDOM, f, 0.0, 1);
		else
			return u32f.f32;
            #endif
		}
        /*else the number is infinity*/
        //Raise range or domain error
		return __amd_handle_errorf("roundf", __amd_round, u32f.u32, _DOMAIN, 0, EDOM, f, 0.0, 1);
    }
    /*Get the exponent of the input*/
    intexp = (u32f.u32 & 0x7f800000) >> 23;
    intexp -= 0x7F;
    /*If exponent is greater than 22 then the number is already
      rounded*/
    if (intexp > 22)
        return f;
    if (intexp < 0)
    {
        u32Temp.f32 = f;
        u32Temp.u32 &= 0x7FFFFFFF;
        /*Add with a large number (2^23 +1) = 8388609.0F
        to force an overflow*/
        u32Temp.f32 = (u32Temp.f32 + 8388609.0F);
        /*Substract back with t he large number*/
        u32Temp.f32 -= 8388609;
        if (u32sign)
            u32Temp.u32 |= 0x80000000;
        return u32Temp.f32;
    }
    else
    {
        /*if(intexp == -1)
            u32exp = 0x3F800000;       */
        u32f.u32 &= 0x7FFFFFFF;
        u32f.f32 += 0.5;
        u32exp = u32f.u32 & 0x7F800000;
        /*right shift then left shift to discard the decimal
          places*/
        u32mantissa = (u32f.u32 & MANTBITS_SP32) >> (23 - intexp);
        u32mantissa = u32mantissa << (23 - intexp);
        u32Temp.u32 = u32sign | u32exp | u32mantissa;
        return (u32Temp.f32);
    }
}


