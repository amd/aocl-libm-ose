#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"

double FN_PROTOTYPE(logb)(double x)
{

  unsigned long long ux;
  long long u;
  GET_BITS_DP64(x, ux);
  u = ((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
  if ((ux & ~SIGNBIT_DP64) == 0)
    /* x is +/-zero. Return -infinity with div-by-zero flag. */
	return _amd_handle_error("logb", _FpCodeLogb, NINFBITPATT_DP64, SING, AMD_F_DIVBYZERO, ERANGE, x, 0.0, 1);
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
          return _amd_handle_error("logb", _FpCodeLogb, ux|0x0008000000000000, DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
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
weak_alias (logb, FN_PROTOTYPE(logb))

