#include "fn_macros.h"
#include "libm_util_amd.h"
#include "libm_special.h"

float FN_PROTOTYPE(floorf)(float x)
{
  float r;
  int rexp, xneg;
  unsigned int ux, ax, ur, mask;

  GET_BITS_SP32(x, ux);
  ax = ux & (~SIGNBIT_SP32);
  xneg = (ux != ax);

  if (ax >= 0x4b800000)
    {
      /* abs(x) is either NaN, infinity, or >= 2^24 */
      if (ax > 0x7f800000)
      {
        /* x is NaN */
        #ifdef WINDOWS
               return __amd_handle_errorf("floorf", __amd_floor, ux|0x00400000, _DOMAIN, 0, EDOM, x, 0.0, 1);
        #else
               if(!(ax & 0x00400000)) //x is snan
                     return __amd_handle_errorf("floorf", __amd_floor, ux|0x00400000, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
               else // x is qnan or inf
                     return x;
        #endif
	  }
      else
        return x;
    }
  else if (ax < 0x3f800000) /* abs(x) < 1.0 */
    {
      if (ax == 0x00000000)
        /* x is +zero or -zero; return the same zero */
        return x;
      else if (xneg) /* x < 0.0 */
        return -1.0F;
      else
        return 0.0F;
    }
  else
    {
      rexp = ((ux & EXPBITS_SP32) >> EXPSHIFTBITS_SP32) - EXPBIAS_SP32;
      /* Mask out the bits of r that we don't want */
      mask = (1 << (EXPSHIFTBITS_SP32 - rexp)) - 1;
      ur = (ux & ~mask);
      PUT_BITS_SP32(ur, r);
      if (xneg && (ux != ur))
        /* We threw some bits away and x was negative */
        return r - 1.0F;
      else
        return r;
    }
}


