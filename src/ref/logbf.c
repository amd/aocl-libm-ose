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


float FN_PROTOTYPE_REF(logbf)(float x)
{
  unsigned int ux;
  int u;
  GET_BITS_SP32(x, ux);
  u = ((ux & EXPBITS_SP32) >> EXPSHIFTBITS_SP32) - EXPBIAS_SP32;
  if ((ux & ~SIGNBIT_SP32) == 0)
    /* x is +/-zero. Return -infinity with div-by-zero flag. */
	return __amd_handle_errorf("logbf", __amd_logb, NINFBITPATT_SP32, _SING, AMD_F_DIVBYZERO, ERANGE, x, 0.0, 1);
  else if (EMIN_SP32 <= u && u <= EMAX_SP32)
    /* x is a normal number */
    return (float)u;
  else if (u > EMAX_SP32)
    {
      /* x is infinity or NaN */
      if ((ux & MANTBITS_SP32) == 0)
      {
        /* x is +/-infinity. return +ve inf. */
#ifdef WINDOWS 
        PUT_BITS_SP32(ux,x);
#else              
        PUT_BITS_SP32(ux & (~SIGNBIT_SP32), x);
#endif
        return x;
      }
      else
      {
        /* x is NaN, result is NaN */
#ifdef WINDOWS
          return __amd_handle_errorf("logbf", __amd_logb, ux|0x00400000, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
#else
          return x+x;
#endif
      }
    }
  else
    {
      /* x is denormalized. */
#ifdef FOLLOW_IEEE754_LOGB
      /* Return the value of the minimum exponent to ensure that
	         the relationship between logb and scalb, defined in
         IEEE 754, holds. */
      return EMIN_SP32;
#else
      /* Follow the rule set by IEEE 854 for logb */
      ux &= MANTBITS_SP32;
      u = EMIN_SP32;
      while (ux < IMPBIT_SP32)
        {
          ux <<= 1;
          u--;
        }
      return (float)u;
#endif
    }
}


