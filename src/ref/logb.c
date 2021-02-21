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


double FN_PROTOTYPE_REF(logb)(double x)
{

  unsigned long long ux;
  long long u;
  GET_BITS_DP64(x, ux);
  u = ((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
  if ((ux & ~SIGNBIT_DP64) == 0)
    /* x is +/-zero. Return -infinity with div-by-zero flag. */
	return __amd_handle_error("logb", __amd_logb, NINFBITPATT_DP64, _SING, AMD_F_DIVBYZERO, ERANGE, x, 0.0, 1);
  else if (EMIN_DP64 <= u && u <= EMAX_DP64)
    /* x is a normal number */
    return (double)u;
  else if (u > EMAX_DP64)
    {
      /* x is infinity or NaN */
      if ((ux & MANTBITS_DP64) == 0)
      {
        /* x is +/-infinity. For VC++, return infinity of same sign. */
#ifdef WINDOWS 
        PUT_BITS_DP64(ux,x);
#else              
        PUT_BITS_DP64(ux&(~SIGNBIT_DP64),x);
#endif
        return x;
      }
      else
        /* x is NaN, result is NaN */
#ifdef WINDOWS
          return __amd_handle_error("logb", __amd_logb, ux|0x0008000000000000, DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
#else
          return x+x;
#endif
    }
  else
    {
      /* x is denormalized. */
#ifdef FOLLOW_IEEE754_LOGB
      /* Return the value of the minimum exponent to ensure that
         the relationship between logb and scalb, defined in
         IEEE 754, holds. */
      return EMIN_DP64;
#else
      /* Follow the rule set by IEEE 854 for logb */
      ux &= MANTBITS_DP64;
      u = EMIN_DP64;
      while (ux < IMPBIT_DP64)
        {
          ux <<= 1;
          u--;
        }
      return (double)u;
#endif
    }

}


