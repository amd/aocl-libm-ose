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


double FN_PROTOTYPE_REF(nextafter)(double x, double y)
{


    UT64 checkbits,checkbitsy;
    double dy = y;
    checkbits.f64=x;
	checkbitsy.f64 = y;




    /* check if the number is nan */
    if(((checkbits.u64 & ~SIGNBIT_DP64) > EXPBITS_DP64 ))
    {
#ifdef WINDOWS
		return  __amd_handle_error("nextafter", __amd_nextafter, checkbits.u64 | QNAN_MASK_64 , _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
#else
	    if (checkbits.u64 & QNAN_MASK_64)

		return  __amd_handle_error("nextafter", __amd_nextafter, checkbits.u64 | QNAN_MASK_64 , _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
	    else
		return  __amd_handle_error("nextafter", __amd_nextafter, checkbits.u64 | QNAN_MASK_64 , _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
#endif
    }


    /* check if y is nan */
    if(((checkbitsy.u64 & ~SIGNBIT_DP64) > EXPBITS_DP64 ))
    {
#ifdef WINDOWS
		return  __amd_handle_error("nextafter", __amd_nextafter, checkbitsy.u64 | QNAN_MASK_64, _DOMAIN, AMD_F_NONE, EDOM, y, 0.0, 1);
#else
	    if (checkbitsy.u64 & QNAN_MASK_64)
		return  __amd_handle_error("nextafter", __amd_nextafter, checkbitsy.u64 | QNAN_MASK_64, _DOMAIN, AMD_F_NONE, EDOM, y, 0.0, 1);
	    else
		return  __amd_handle_error("nextafter", __amd_nextafter, checkbits.u64 | QNAN_MASK_64 , _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
#endif
    }
	
	
	/* if x == y return y in the type of x */
    if( x == dy )
    {
        return dy;
    }

    if( x == 0.0)
    {
        checkbits.u64 = 1;
        if( dy > 0.0 )
             return checkbits.f64;
        else
            return -checkbits.f64;
    }


    /* compute the next heigher or lower value */

    if(((x>0.0) ^ (dy>x)) == 0)
    {
        checkbits.u64++;
    }
    else
    {
        checkbits.u64--;
    }

    /* check if the result is nan or inf */
    if(((checkbits.u64 & ~SIGNBIT_DP64) >= EXPBITS_DP64 ))
    {
		return  __amd_handle_error("nextafter", __amd_nextafter, checkbits.u64 | QNAN_MASK_64, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
    }

    return checkbits.f64;
}

