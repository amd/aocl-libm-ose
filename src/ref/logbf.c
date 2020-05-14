/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"


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


