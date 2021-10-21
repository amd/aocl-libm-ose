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


double ALM_PROTO_REF(rint)(double x)
{

    UT64 checkbits,val_2p52;
	UT32 sign;
    checkbits.f64=x;

    /* Clear the sign bit and check if the value can be rounded(i.e check if exponent less than 52) */
    if( (checkbits.u64 & 0x7FFFFFFFFFFFFFFF) > 0x4330000000000000)
    {
      /* take care of nan or inf */
	if(	(checkbits.u64 & EXPBITS_DP64) == EXPBITS_DP64)
	{
        if((checkbits.u64 & MANTBITS_DP64) == 0x0)
        {
            // x is Inf
#ifdef WINDOWS
            return  __alm_handle_error(checkbits.u64, AMD_F_INVALID);
#else
            return  __alm_handle_error(checkbits.u64, AMD_F_NONE);
#endif
		}
		else {
			// x is NaN
			// QNAN_MASK_32
#ifdef WINDOWS
		return  __alm_handle_error(checkbits.u64 | QNAN_MASK_64, AMD_F_NONE);
#else
		if (checkbits.u64 & QNAN_MASK_64)
		return  __alm_handle_error(checkbits.u64 | QNAN_MASK_64, AMD_F_NONE);
		else
		return  __alm_handle_error(checkbits.u64 | QNAN_MASK_64, AMD_F_INVALID);
#endif
		}
	}
	else
		return x;
    }

    sign.u32 =  checkbits.u32[1] & 0x80000000;
    val_2p52.u32[1] = sign.u32 | 0x43300000;
    val_2p52.u32[0] = 0;

	/* Add and sub 2^52 to round the number according to the current rounding direction */
    val_2p52.f64 = (x + val_2p52.f64) - val_2p52.f64;

    /*This extra line is to take care of denormals and various rounding modes*/
    val_2p52.u32[1] = ((val_2p52.u32[1] << 1) >> 1) | sign.u32;

     if(x!=val_2p52.f64)
	{
   	     /* Raise floating-point inexact exception if the result differs in value from the argument */
      	    checkbits.u64 = QNANBITPATT_DP64;
     	    checkbits.f64 = checkbits.f64 +  checkbits.f64;        /* raise inexact exception by adding two nan numbers.*/
	}


    return (val_2p52.f64);
}





