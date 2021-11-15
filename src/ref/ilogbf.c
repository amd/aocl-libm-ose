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

int ALM_PROTO_REF(ilogbf)(float x)
{

    /* Check for input range */
    UT32 checkbits;
    int expbits;
    U32 manbits;
    U32 zerovalue;
    checkbits.f32=x;

    /* Clear the sign bit and check if the value is zero nan or inf.*/
    zerovalue = (checkbits.u32 & ~SIGNBIT_SP32);

    if(zerovalue == 0)
    {
        /* Raise domain error as the number zero*/
        __alm_handle_errorf((unsigned int)INT_MIN,
                                  AMD_F_DIVBYZERO);
        return INT_MIN;
    }

    if( zerovalue == EXPBITS_SP32 )
    {
        /* Raise domain error as the number is inf */
        //if negative inf raise an exception
        //if positive inf don't raise and exception
        if (x<0.0f)
                __alm_handle_errorf((unsigned int)INT_MAX,
                                          AMD_F_INVALID);
        else
                __alm_handle_errorf((unsigned int)INT_MAX,
                                          AMD_F_NONE);
        return INT_MAX;
    }

    if( zerovalue > EXPBITS_SP32 )
    {
        /* Raise exception as the number is inf */
#ifdef WINDOWS
        __alm_handle_errorf((unsigned int)INT_MIN,
                                           AMD_F_NONE);
        return INT_MIN;
#else
        //x = x+x;
        //x+x is not sufficient here since we return an integer and in
        //optimization mode the compiler tends to optimize out the
        //x+x operation if done.
        if (zerovalue >= 0x7fC00000)
                __alm_handle_errorf((unsigned int)INT_MIN,
                                              AMD_F_NONE);
        else
                __alm_handle_errorf((unsigned int)INT_MIN,
                                             AMD_F_INVALID);
        return INT_MIN;
#endif
    }

    expbits = (int) (( checkbits.u32 << 1) >> 24);

    if(expbits == 0 && (checkbits.u32 & MANTBITS_SP32 )!= 0)
    {
        /* the value is denormalized */
      manbits = checkbits.u32 & MANTBITS_SP32;
      expbits = EMIN_SP32;
      while (manbits < IMPBIT_SP32)
        {
          manbits <<= 1;
          expbits--;
        }
    }
    else
    {
        expbits-=EXPBIAS_SP32;
    }

    return expbits;
}

