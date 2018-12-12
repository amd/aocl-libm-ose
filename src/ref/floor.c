#include "fn_macros.h"
#include "libm_util_amd.h"
#include "libm_special.h"

double FN_PROTOTYPE(floor)(double x)
{
  double r;
  long long rexp, xneg;


  unsigned long long ux, ax, ur, mask;

  GET_BITS_DP64(x, ux);
  ax = ux & (~SIGNBIT_DP64);
  xneg = (ux != ax);

  if (ax >= 0x4340000000000000)
    {
      /* abs(x) is either NaN, infinity, or >= 2^53 */
      if (ax > 0x7ff0000000000000)
      {
        /* x is NaN */
       #ifdef WINDOWS
            return __amd_handle_error("floor", __amd_floor, ux|0x0008000000000000, _DOMAIN, 0, EDOM, x, 0.0, 1);
       #else
            if(!(ax & 0x0008000000000000)) //x is snan
                  return __amd_handle_error("floor", __amd_floor, ux|0x0008000000000000, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
            else // x is qnan or inf
                  return x;
       #endif
      }
      else
        return x;
    }
  else if (ax < 0x3ff0000000000000) /* abs(x) < 1.0 */
    {
      if (ax == 0x0000000000000000)
        /* x is +zero or -zero; return the same zero */
        return x;
      else if (xneg) /* x < 0.0 */
        return -1.0;
      else
        return 0.0;
    }
  else
    {
      r = x;
      rexp = ((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
      /* Mask out the bits of r that we don't want */
      mask = 1;
      mask = (mask << (EXPSHIFTBITS_DP64 - rexp)) - 1;
      ur = (ux & ~mask);
      PUT_BITS_DP64(ur, r);
      if (xneg && (ur != ux))
        /* We threw some bits away and x was negative */
        return r - 1.0;
      else
        return r;
    }

}
weak_alias (floor, FN_PROTOTYPE(floor))

