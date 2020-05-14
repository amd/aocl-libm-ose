/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include "fn_macros.h"
#include "libm_util_amd.h"
#include "libm_special.h"

/* Returns 0 if x is infinite or NaN, otherwise returns 1 */

int FN_PROTOTYPE_REF(finite)(double x)
{

  unsigned long long ax,ux;

  GET_BITS_DP64(x, ux);
  ax = ux & (~SIGNBIT_DP64);
  if (ax > 0x7ff0000000000000)
     /* x is NaN */
     #ifdef WINDOWS
        return (int)__amd_handle_error("finite", __amd_finite, 0x0, _DOMAIN, 0, EDOM, x, 0.0, 1);
     #else
        if(!(ax & 0x0008000000000000)) //x is snan
             return (int)__amd_handle_error("finite", __amd_finite, 0x0, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
     #endif

  return (int)((ax - PINFBITPATT_DP64) >> 63);


}


