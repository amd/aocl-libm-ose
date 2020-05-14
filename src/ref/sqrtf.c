/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include <emmintrin.h>
#include "fn_macros.h"
#include "libm_util_amd.h"
#include "libm_special.h"

/*SSE2 contains an instruction SQRTSS. This instruction Computes the square root
  of the low-order single-precision floating-point value in an XMM register
  or in a 32-bit memory location and writes the result in the low-order doubleword
  of another XMM register. The corresponding intrinsic is _mm_sqrt_ss()*/
/*SSE2 contains an instruction SQRTSS. This instruction Computes the square root
  of the low-order single-precision floating-point value in an XMM register
  or in a 32-bit memory location and writes the result in the low-order doubleword
  of another XMM register. The corresponding intrinsic is _mm_sqrt_ss()*/
float FN_PROTOTYPE_REF(sqrtf)(float x)
{
  __m128 X128;
  float result;
  int xneg;
  unsigned int ux, ax;
    //UT32 uresult;
  GET_BITS_SP32(x, ux);
  ax = ux & (~SIGNBIT_SP32);
  xneg = (ux != ax);

  if (ax > 0x7f800000)   /* x is NaN */
         #ifdef WINDOWS
         return __amd_handle_errorf("sqrtf", __amd_squareroot, ux|0x00400000, _DOMAIN, 0, EDOM, x, 0.0, 1);
         #else
         {
          if(!(ax & 0x00400000)) //x is snan
              return __amd_handle_errorf("sqrtf", __amd_squareroot, ux|0x00400000, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
          else
              return x;
		 }
         #endif

  if(xneg)
  {
       if(ax == 0x0) /* x == -0*/
	      return -0.0;
   return __amd_handle_errorf("sqrtf", __amd_squareroot, 0x00000000ffc00000, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
  }

    /*Load x into an XMM register*/
    X128 = _mm_load_ss(&x);
    /*Calculate sqrt using SQRTSS instrunction*/
    X128 = _mm_sqrt_ss(X128);
    /*Store back the result into a single precision floating point number*/
    _mm_store_ss(&result, X128);
    return result;
}



