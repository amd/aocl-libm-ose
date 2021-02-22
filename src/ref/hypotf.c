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
#include "libm_special.h"
#include <libm/amd_funcs_internal.h>


float FN_PROTOTYPE_REF(hypotf)(float x, float y)
{
  /* Returns sqrt(x*x + y*y) with no overflow or underflow unless
     the result warrants it */

    /* Do intermediate computations in double precision
       and use sqrt instruction from chip if available. */
    double dx, dy, dr, retval;

    /* The largest finite float, stored as a double */
    double large;

  int x_is_nan,y_is_nan;
  unsigned int ux, uy, avx, avy;
  UT32 val;
  GET_BITS_SP32(x, avx);
  avx &= ~SIGNBIT_SP32;
  GET_BITS_SP32(y, avy);
  avy &= ~SIGNBIT_SP32;
  ux = (avx >> EXPSHIFTBITS_SP32);
  uy = (avy >> EXPSHIFTBITS_SP32);
  x_is_nan = (avx > 0x7f800000);
  y_is_nan = (avy > 0x7f800000);
  val.u32 = PINFBITPATT_SP32;

  if (ux == BIASEDEMAX_SP32 + 1 || uy == BIASEDEMAX_SP32 + 1)
    {
      /* One or both of the arguments are NaN or infinity. The
         result will also be NaN or infinity. */
      if (((ux == BIASEDEMAX_SP32 + 1) && !(avx & MANTBITS_SP32)) ||
          ((uy == BIASEDEMAX_SP32 + 1) && !(avy & MANTBITS_SP32)))
        /* x or y is infinity. ISO C99 defines that we must
           return +infinity, even if the other argument is NaN.
           Note that the computation of x*x + y*y above will already
           have raised invalid if either x or y is a signalling NaN. */
           {
			  if(x_is_nan)
			      {
			        #ifdef WINDOWS
					  return  val.f32 ;// __amd_handle_errorf("hypotf", __amd_hypot, val.u32, _DOMAIN, 0, EDOM, x, y, 2);
			        #else
			              if(!(avx & 0x00400000)) //x is snan
			                  return __amd_handle_errorf("hypotf", __amd_hypot, val.u32, _DOMAIN, AMD_F_INVALID, EDOM, x, y, 2);
			        #endif
			   }
			   if(y_is_nan)
			   {
			        #ifdef WINDOWS
				   return val.f32 ; //__amd_handle_errorf("hypotf", __amd_hypot, val.u32, _DOMAIN, 0, EDOM, x, y, 2);
			        #else
			              if(!(avy & 0x00400000)) //y is snan
			                  return __amd_handle_errorf("hypotf", __amd_hypot, val.u32, _DOMAIN, AMD_F_INVALID, EDOM, x, y, 2);
			        #endif
			   }
			  return val.f32;
		   }

      if(x_is_nan || y_is_nan)
	  {
        /* One or both of x or y is NaN, and neither is infinity.
           Raise invalid if it's a signalling NaN */
        if(x_is_nan)
           {
             val.f32 = x;
             #ifdef WINDOWS
                   return __amd_handle_errorf("hypotf", __amd_hypot, val.u32|0x00400000, _DOMAIN, 0, EDOM, x, y, 2);
             #else
                   if(!(avx & 0x00400000)) //x is snan
                       return __amd_handle_errorf("hypotf", __amd_hypot, val.u32|0x00400000, _DOMAIN, AMD_F_INVALID, EDOM, x, y, 2);
             #endif
		   }
        if(y_is_nan)
		   {
			 val.f32 = y;
             #ifdef WINDOWS
                   return __amd_handle_errorf("hypotf", __amd_hypot, val.u32|0x00400000, _DOMAIN, 0, EDOM, x, y, 2);
             #else
                   if(!(avy & 0x00400000)) //y is snan
                       return __amd_handle_errorf("hypotf", __amd_hypot, val.u32|0x00400000, _DOMAIN, AMD_F_INVALID, EDOM, x, y, 2);
             #endif
		   }
	  }
    }
    large = 3.40282346638528859812e+38; /* 0x47efffffe0000000 */
    dx = x, dy = y;
    if(avx == 0x0) // to prevent underflow exception when x is small
      {
       val.u32 = avy;
       return val.f32;
      }
    if(avy == 0x0)// to prevent underflow exception when y is small
      {
       val.u32 = avx;
       return val.f32;
      }
    dr = (dx*dx + dy*dy);

#ifdef WINDOWS
  /* VC++ intrinsic call */
  _mm_store_sd(&retval, _mm_sqrt_sd(_mm_setzero_pd(), _mm_load_sd(&dr)));
#else
    /* Hammer sqrt instruction */
    asm volatile ("sqrtsd %1, %0" : "=x" (retval) : "x" (dr));
#endif

    if (retval > large)
         return __amd_handle_errorf("hypotf", __amd_hypot, PINFBITPATT_SP32, _OVERFLOW, AMD_F_INEXACT|AMD_F_OVERFLOW, ERANGE, x, y, 2);
	else if((x !=0.0f) && (y!=0))
		{
		 val.f32 = (float)(retval);
         val.u32 >>= EXPSHIFTBITS_SP32;
		 if(val.u32 == 0x0)
		 {
			 val.f32 = (float)retval;
             return __amd_handle_errorf("hypotf", __amd_hypot, val.u32, _UNDERFLOW, AMD_F_INEXACT|AMD_F_UNDERFLOW, ERANGE, x, y, 2);
	     }
		 else
			 return (float)retval;
	}
    else
         return (float)retval;
  }


