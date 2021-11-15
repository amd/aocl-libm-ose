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
 
float ALM_PROTO_REF(nexttowardf)(float x, long double y) 
{ 
 
 
    UT32 checkbits; 
    long double dy = (long double) y; 
    checkbits.f32=x; 
 
 
 
	/* check if the number is nan */
    if(((checkbits.u32 & ~SIGNBIT_SP32) > EXPBITS_SP32 ))
    {
#ifdef WINDOWS
	return  __alm_handle_errorf(checkbits.u32| QNAN_MASK_32,
                                          AMD_F_NONE);
#else
	if (checkbits.u32 & QNAN_MASK_32)
	    return  __alm_handle_errorf(checkbits.u32| QNAN_MASK_32,
                                            AMD_F_NONE);
	else
	    return  __alm_handle_errorf(checkbits.u32| QNAN_MASK_32,
                                            AMD_F_INVALID);
#endif
    }
	 
 
    /* if x == y return y in the type of x */ 
    if( x == dy ) 
    { 
        return (float) dy; 
    } 
 
    if(x == 0.0f)
    { 
        checkbits.u32 = 1; 
        if( dy > 0.0 ) 
             return checkbits.f32; 
        else 
            return -checkbits.f32; 
    } 
 
 
    /* compute the next heigher or lower value */ 
    if(((x>0.0F) ^ (dy>x)) == 0) 
    { 
        checkbits.u32++; 
    } 
    else 
    { 
        checkbits.u32--; 
    } 
 
    /* check if the result is nan or inf */ 
    if(((checkbits.u32 & ~SIGNBIT_SP32) >= EXPBITS_SP32 )) 
    { 
        return  __alm_handle_errorf(checkbits.u32 | QNAN_MASK_32,
                                                0);
    } 
 
    return checkbits.f32; 
} 

