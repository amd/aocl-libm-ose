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


double ALM_PROTO_REF(frexp)(double value, int *exp)
{
    UT64 val;
    unsigned int sign;
    int exponent;
    val.f64 = value;
    sign = val.u32[1] & SIGNBIT_SP32;
    val.u32[1] = val.u32[1] & ~SIGNBIT_SP32; /* remove the sign bit */
    *exp = 0;
    if((val.u64 == 0x0000000000000000) || (val.u64 == 0x7ff0000000000000))
        return value; /* value= +-0 or value= nan or value = +-inf return value */
    
    if(val.u64 > 0x7ff0000000000000)
    {
#ifdef WINDOWS
         return __alm_handle_error("frexp", __amd_frexp, val.u64|QNANBITPATT_DP64, DOMAIN, AMD_F_NONE, EDOM,value, 0.0, 1);
#else
         return value+value;
#endif
    }
    exponent = (int)(val.u32[1] >> 20); /* get the exponent */

    if(exponent == 0)/*x is denormal*/
    {
		val.f64 = val.f64 * VAL_2PMULTIPLIER_DP;/*multiply by 2^53 to bring it to the normal range*/
        exponent = (int)(val.u32[1] >> 20); /* get the exponent */
		exponent = exponent - MULTIPLIER_DP;
    }

	exponent -= 1022; /* remove bias(1023)-1 */
    *exp = exponent; /* set the integral power of two */
    val.u32[1] = sign | 0x3fe00000 | (val.u32[1] & 0x000fffff);/* make the fractional part(divide by 2) */                                              
    return val.f64;
}


