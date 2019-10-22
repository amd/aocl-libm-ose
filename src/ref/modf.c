#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"
#include "libm_errno_amd.h"

double FN_PROTOTYPE(modf)(double x, double *iptr)
{
  /* modf splits the argument x into integer and fraction parts,
     each with the same sign as x. */


  long long xexp;
  unsigned long long ux, ax, mask;

  GET_BITS_DP64(x, ux);
  ax = ux & (~SIGNBIT_DP64);

  if (ax >= 0x4340000000000000)
    {
      /* abs(x) is either NaN, infinity, or >= 2^53 */
      if (ax > 0x7ff0000000000000)
        {
          /* x is NaN */
          *iptr = x;
#ifdef WINDOWS
          return __amd_handle_error("modf", __amd_modf, ux|0x0008000000000000, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
#else
          return x+x;
#endif
        }
      else
        {
          /* x is infinity or large. Return zero with the sign of x */
          *iptr = x;
          PUT_BITS_DP64(ux & SIGNBIT_DP64, x);
          return x;
        }
    }
  else if (ax < 0x3ff0000000000000)
    {
      /* abs(x) < 1.0. Set iptr to zero with the sign of x
         and return x. */
      PUT_BITS_DP64(ux & SIGNBIT_DP64, *iptr);
      return x;
    }
  else
    {
      double r;
      unsigned long long ur;
      xexp = ((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
      /* Mask out the bits of x that we don't want */
      mask = 1;
      mask = (mask << (EXPSHIFTBITS_DP64 - xexp)) - 1;
      PUT_BITS_DP64(ux & ~mask, *iptr);
      r = x - *iptr;
      GET_BITS_DP64(r, ur);
      PUT_BITS_DP64(((ux & SIGNBIT_DP64)|ur), r);
      return r;
    }

}


