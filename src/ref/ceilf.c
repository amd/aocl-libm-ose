/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include "fn_macros.h"
#include "libm_util_amd.h"
#include "libm_special.h"

float FN_PROTOTYPE_REF(ceilf)(float x)
{
  float r;
  int rexp, xneg;
  unsigned int ux, ax, ur, mask;

  GET_BITS_SP32(x, ux);
  /*ax is |x|*/
  ax = ux & (~SIGNBIT_SP32);
  /*xneg stores the sign of the input x*/
  xneg = (ux != ax);
  /*The range is divided into
    > 2^24. ceil will either the number itself or Nan
            always returns a QNan. Raises exception if input is a SNan
    < 1.0   If 0.0 then return with the appropriate sign
            If input is less than -0.0 and greater than -1.0 then return -0.0
            If input is greater than 0.0 and less than 1.0 then return 1.0
    1.0 < |x| < 2^24
            appropriately check the exponent and set the return Value by shifting
            */
  if (ax >= 0x4b800000) /* abs(x) > 2^24*/
    {
      /* abs(x) is either NaN, infinity, or >= 2^24 */
      if (ax > 0x7f800000)
        /* x is NaN */
            #ifdef WINDOWS
                return __amd_handle_errorf("ceilf", __amd_ceil, ux|0x00400000, _DOMAIN, 0, EDOM, x, 0.0, 1);
            #else
                if(!(ax & 0x00400000)) //x is snan
                    return __amd_handle_errorf("ceilf", __amd_ceil, ux|0x00400000, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
		else
			return x;
            #endif

      else
        return x;
    }
  else if (ax < 0x3f800000) /* abs(x) < 1.0 */
    {
      if (ax == 0x00000000)
        /* x is +zero or -zero; return the same zero */
        return x;
      else if (xneg) /* x < 0.0 */
        return -0.0F;
      else
        return 1.0F;
    }
  else
    {
      rexp = ((ux & EXPBITS_SP32) >> EXPSHIFTBITS_SP32) - EXPBIAS_SP32;
      /* Mask out the bits of r that we don't want */
      mask = (1 << (EXPSHIFTBITS_SP32 - rexp)) - 1;
      /*Keeps the exponent part and the required mantissa.*/
      ur = (ux & ~mask);
      PUT_BITS_SP32(ur, r);

      if (xneg || (ux == ur)) return r;
      else
        /* We threw some bits away and x was positive */
        return r + 1.0F;
    }
}


