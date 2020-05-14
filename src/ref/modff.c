/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"

float FN_PROTOTYPE_REF(modff)(float x, float *iptr)
{
  /* modff splits the argument x into integer and fraction parts,
     each with the same sign as x. */

  unsigned int ux, mask;
  int xexp;

  GET_BITS_SP32(x, ux);
  xexp = ((ux & (~SIGNBIT_SP32)) >> EXPSHIFTBITS_SP32) - EXPBIAS_SP32;

  if (xexp < 0)
    {
      /* abs(x) < 1.0. Set iptr to zero with the sign of x
         and return x. */
      PUT_BITS_SP32(ux & SIGNBIT_SP32, *iptr);
      return x;
    }
  else if (xexp < EXPSHIFTBITS_SP32)
    {
      float r;
      unsigned int ur;
      /* x lies between 1.0 and 2**(24) */
      /* Mask out the bits of x that we don't want */
      mask = (1 << (EXPSHIFTBITS_SP32 - xexp)) - 1;
      PUT_BITS_SP32(ux & ~mask, *iptr);
      r = x - *iptr;
      GET_BITS_SP32(r, ur);
      PUT_BITS_SP32(((ux & SIGNBIT_SP32)|ur), r);
      return r;
    }
  else if ((ux & (~SIGNBIT_SP32)) > 0x7f800000)
    {
      /* x is NaN */
      *iptr = x;
#ifdef WINDOWS
      return __amd_handle_errorf("modff", __amd_logb, ux|0x0008000000000000, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1); /* Raise invalid if it is a signalling NaN */
#else
      return x+x;
#endif
    }
  else
    {
      /* x is infinity or large. Set iptr to x and return zero
         with the sign of x. */
      *iptr = x;
      PUT_BITS_SP32(ux & SIGNBIT_SP32, x);
      return x;
    }
}


