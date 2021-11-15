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
#include "libm_errno_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>



float ALM_PROTO_REF(rintf)(float x)
{

    UT32 checkbits,sign,val_2p23;
    checkbits.f32=x;

    /* Clear the sign bit and check if the value can be rounded */
    if( (checkbits.u32 & 0x7FFFFFFF) > 0x4B000000)
    {
      /* Number exceeds the representable range could be nan or inf also*/
      /* take care of nan or inf */
	if(	(checkbits.u32 & EXPBITS_SP32) == EXPBITS_SP32)
	{
        if((checkbits.u32 & MANTBITS_SP32) == 0x0)
        {
            // x is Inf
#ifdef WINDOWS
            return  __alm_handle_errorf(checkbits.u32, AMD_F_INVALID);
#else
            return  __alm_handle_errorf(checkbits.u32, AMD_F_NONE);
#endif
		}
		else {
			// x is NaN
#ifdef WINDOWS
		return  __alm_handle_errorf(checkbits.u32 | QNAN_MASK_32, AMD_F_NONE);
#else
		if (checkbits.u32 & QNAN_MASK_32)
		    return  __alm_handle_errorf(checkbits.u32 | QNAN_MASK_32, AMD_F_NONE);
		else
		    return  __alm_handle_errorf(checkbits.u32 | QNAN_MASK_32, AMD_F_INVALID);
#endif
		}
	}
	else
		return x;

    }

    sign.u32 =  checkbits.u32 & 0x80000000;
    val_2p23.u32 = (checkbits.u32 & 0x80000000) | 0x4B000000;

   /* Add and sub 2^23 to round the number according to the current rounding direction */
    val_2p23.f32  = ((x + val_2p23.f32) - val_2p23.f32);

    /*This extra line is to take care of denormals and various rounding modes*/
    val_2p23.u32 = ((val_2p23.u32 << 1) >> 1) | sign.u32;

    if (val_2p23.f32 != x)
    {
        /* Raise floating-point inexact exception if the result differs in value from the argument */
         checkbits.u32 = 0xFFC00000;
         checkbits.f32 = checkbits.f32 +  checkbits.f32;        /* raise inexact exception by adding two nan numbers.*/
    }


    return val_2p23.f32;
}


