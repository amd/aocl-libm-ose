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

#include <emmintrin.h>
#include "fn_macros.h"
#include "libm_util_amd.h"
#include "libm_special.h"

/*SSE2 contains an instruction SQRTSD. This instruction Computes the square root
  of the low-order double-precision floating-point value in an XMM register
  or in a 64-bit memory location and writes the result in the low-order quadword
  of another XMM register. The corresponding intrinsic is _mm_sqrt_sd()*/
double FN_PROTOTYPE_REF(sqrt)(double x)
{
  __m128d X128;
  double result;
  unsigned long long ux, ax;
  long long xneg;

  GET_BITS_DP64(x, ux);
  ax = ux & (~SIGNBIT_DP64);
  xneg = (ux != ax);

  if (ax > 0x7ff0000000000000)     /* x is NaN */
     #ifdef WINDOWS
           return __amd_handle_error("sqrt", __amd_squareroot, ux|0x0008000000000000, _DOMAIN, 0, EDOM, x, 0.0, 1);
     #else
        {
        if(!(ax & 0x0008000000000000))// x is snan
           return __amd_handle_error("sqrt", __amd_squareroot, ux|0x0008000000000000, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
        else // x is qnan
           return x;
	    }
     #endif

  if(xneg)
  {
       if(ax == 0x0) /* x == -0*/
	      return -0.0;
       return __amd_handle_error("sqrt", __amd_squareroot, 0xfff8000000000000, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
  }
    /*Load x into an XMM register*/
    X128 = _mm_load_sd(&x);
    /*Calculate sqrt using SQRTSD instrunction*/
    X128 = _mm_sqrt_sd(X128, X128);
    /*Store back the result into a double precision floating point number*/
    _mm_store_sd(&result, X128);
    return result;
}



