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


int ALM_PROTO_REF(ilogb)(double x)
{


    /* Check for input range */
    UT64 checkbits;
    int expbits;
    U64 manbits;
    U64  zerovalue;
        /* Clear the sign bit and check if the value is zero nan or inf.*/
    checkbits.f64=x;
    zerovalue = (checkbits.u64 & ~SIGNBIT_DP64);

    if(zerovalue == 0)
    {
        /* Raise exception as the number zero*/
        __alm_handle_error((unsigned long long)INT_MIN, AMD_F_DIVBYZERO);
        return INT_MIN;
    }

    if( zerovalue == EXPBITS_DP64 )
    {
        /* Raise exception as the number is inf */
        if (x<0.0)
                __alm_handle_error((unsigned int)INT_MAX, AMD_F_INVALID);
        else
                __alm_handle_error((unsigned int)INT_MAX, AMD_F_NONE);
        return INT_MAX;
    }

    if( zerovalue > EXPBITS_DP64 )
    {
        /* Raise exception as the number is nan */
#ifdef WINDOWS
        __alm_handle_error((unsigned long long)INT_MIN, AMD_F_NONE);
        return INT_MIN;
#else
        //x = x+x;
        //x+x is not sufficient here since we return an integer and in 
        //optimization mode the compiler tends to optimize out the 
        //x+x operation if done.
        if (zerovalue >= 0x7ff8000000000000)
                __alm_handle_error((unsigned long long)INT_MIN, AMD_F_NONE);
        else
                __alm_handle_error((unsigned long long)INT_MIN, AMD_F_INVALID);
        return INT_MIN;
#endif        
    }

    expbits = (int) (( checkbits.u64 << 1) >> 53);

    if(expbits == 0 && (checkbits.u64 & MANTBITS_DP64 )!= 0)
    {
        /* the value is denormalized */
      manbits = checkbits.u64 & MANTBITS_DP64;
      expbits = EMIN_DP64;
      while (manbits < IMPBIT_DP64)
        {
          manbits <<= 1;
          expbits--;
        }
    }
    else
    {
        expbits-=EXPBIAS_DP64;
    }


    return expbits;
}

