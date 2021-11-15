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

#include "libm_util_amd.h"
#include "libm_inlines_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>


double ALM_PROTO_REF(hypot)(double x, double y)
{
  /* Returns sqrt(x*x + y*y) with no overflow or underflow unless
     the result warrants it */

  const double large = 1.79769313486231570815e+308; /* 0x7fefffffffffffff */
  double u, r, retval, hx, tx, x2, hy, ty, y2, hs, ts;
  unsigned long long xexp, yexp, ux, uy, ut;
  int dexp, expadjust,x_is_nan,y_is_nan;
  UT64 val;

  GET_BITS_DP64(x, ux);
  ux &= ~SIGNBIT_DP64;
  GET_BITS_DP64(y, uy);
  uy &= ~SIGNBIT_DP64;
  xexp = (ux >> EXPSHIFTBITS_DP64);
  yexp = (uy >> EXPSHIFTBITS_DP64);
  x_is_nan = (ux > 0x7ff0000000000000);
  y_is_nan = (uy > 0x7ff0000000000000);
  val.u64 = PINFBITPATT_DP64;

  if (xexp == BIASEDEMAX_DP64 + 1 || yexp == BIASEDEMAX_DP64 + 1)
    {
      /* One or both of the arguments are NaN or infinity. The
         result will also be NaN or infinity. */
      if (((xexp == BIASEDEMAX_DP64 + 1) && !(ux & MANTBITS_DP64)) ||((yexp == BIASEDEMAX_DP64 + 1) && !(uy & MANTBITS_DP64)))
        /* x or y is infinity. ISO C99 defines that we must
           return +infinity, even if the other argument is NaN.
           Note that the computation of x*x + y*y above will already
           have raised invalid if either x or y is a signalling NaN. */
           {
			 if(x_is_nan)
			     {
			     #ifdef WINDOWS
			        return __alm_handle_error(val.u64, 0);
			     #else
			        if(!(ux & 0x0008000000000000)) //x is snan
			              return __alm_handle_error(val.u64, AMD_F_INVALID);
			     #endif
				    }
			 if(y_is_nan)
			     {
			     #ifdef WINDOWS
			        return __alm_handle_error(val.u64, 0);
			     #else
			        if(!(uy & 0x0008000000000000)) //y is snan
			              return __alm_handle_error(val.u64, AMD_F_INVALID);
			     #endif
				 }
				 return val.f64;
		   }

      if(x_is_nan || y_is_nan)
	  {
        /* One or both of x or y is NaN, and neither is infinity.
           Raise invalid if it's a signalling NaN */
        if(x_is_nan)
             {
             val.f64 = x;
             #ifdef WINDOWS
                return __alm_handle_error(val.u64|0x0008000000000000, 0);
             #else
                if(!(ux & 0x0008000000000000)) //x is snan
                      return __alm_handle_error(val.u64|0x0008000000000000, AMD_F_INVALID);
             #endif
			 }
        if(y_is_nan)
             {
			 val.f64 = y;
             #ifdef WINDOWS
                return __alm_handle_error(val.u64|0x0008000000000000, 0);
             #else
                if(!(uy & 0x0008000000000000)) //y is snan
                      return __alm_handle_error(val.u64|0x0008000000000000, AMD_F_INVALID);
             #endif
		     }

        // x or y is qnan
        if(x_is_nan)
            return x;
        if(y_is_nan)
            return y;
	  }

    }

  /* Set x = abs(x) and y = abs(y) */
  PUT_BITS_DP64(ux, x);
  PUT_BITS_DP64(uy, y);

  /* The difference in exponents between x and y */
  dexp = (int)(xexp - yexp);
  expadjust = 0;

  if (ux == 0)
    /* x is zero */
    return y;
  else if (uy == 0)
    /* y is zero */
    return x;
  else if (dexp > MANTLENGTH_DP64 + 1 || dexp < -MANTLENGTH_DP64 - 1)
    /* One of x and y is insignificant compared to the other */
    return x + y; /* Raise inexact */
  else if (xexp > EXPBIAS_DP64 + 500 || yexp > EXPBIAS_DP64 + 500)
    {
      /* Danger of overflow; scale down by 2**600. */
      expadjust = 600;
      ux -= 0x2580000000000000;
      PUT_BITS_DP64(ux, x);
      uy -= 0x2580000000000000;
      PUT_BITS_DP64(uy, y);
    }
  else if (xexp < EXPBIAS_DP64 - 500 || yexp < EXPBIAS_DP64 - 500)
    {
      /* Danger of underflow; scale up by 2**600. */
      expadjust = -600;
      if (xexp == 0)
        {
          /* x is denormal - handle by adding 601 to the exponent
           and then subtracting a correction for the implicit bit */
          PUT_BITS_DP64(ux + 0x2590000000000000, x);
          x -= 9.23297861778573578076e-128; /* 0x2590000000000000 */
          GET_BITS_DP64(x, ux);
        }
      else
        {
          /* x is normal - just increase the exponent by 600 */
          ux += 0x2580000000000000;
          PUT_BITS_DP64(ux, x);
        }
      if (yexp == 0)
        {
          PUT_BITS_DP64(uy + 0x2590000000000000, y);
          y -= 9.23297861778573578076e-128; /* 0x2590000000000000 */
          GET_BITS_DP64(y, uy);
        }
      else
        {
          uy += 0x2580000000000000;
          PUT_BITS_DP64(uy, y);
        }
    }


#ifdef FAST_BUT_GREATER_THAN_ONE_ULP
  /* Not awful, but results in accuracy loss larger than 1 ulp */
  r = x*x + y*y
#else
  /* Slower but more accurate */

  /* Sort so that x is greater than y */
  if (x < y)
    {
      u = y;
      y = x;
      x = u;
      ut = ux;
      ux = uy;
      uy = ut;
    }

  /* Split x into hx and tx, head and tail */
  PUT_BITS_DP64(ux & 0xfffffffff8000000, hx);
  tx = x - hx;

  PUT_BITS_DP64(uy & 0xfffffffff8000000, hy);
  ty = y - hy;

  /* Compute r = x*x + y*y with extra precision */
  x2 = x*x;
  y2 = y*y;
  hs = x2 + y2;

  if (dexp == 0)
    /* We take most care when x and y have equal exponents,
       i.e. are almost the same size */
    ts = (((x2 - hs) + y2) +
          ((hx * hx - x2) + 2 * hx * tx) + tx * tx) +
      ((hy * hy - y2) + 2 * hy * ty) + ty * ty;
  else
    ts = (((x2 - hs) + y2) +
          ((hx * hx - x2) + 2 * hx * tx) + tx * tx);

  r = hs + ts;
#endif

#ifdef WINDOWS
  /* VC++ intrinsic call */
  _mm_store_sd(&retval, _mm_sqrt_sd(_mm_setzero_pd(), _mm_load_sd(&r)));
#else
  /* Hammer sqrt instruction */
  asm volatile ("sqrtsd %1, %0" : "=x" (retval) : "x" (r));
#endif

  /* If necessary scale the result back. This may lead to
     overflow but if so that's the correct result. */
  retval = scaleDouble_1(retval, expadjust);

  if (retval > large)
    /* The result overflowed. Deal with errno. */
    return __alm_handle_error(PINFBITPATT_DP64, AMD_F_INEXACT|AMD_F_OVERFLOW);
  else if((x !=0.0) && (y!=0))
		{
		 val.f64 = retval;
         val.u64 >>= EXPSHIFTBITS_DP64;
		 if(val.u64 == 0x0)
		 {
			 val.f64 = retval;
             return __alm_handle_error(val.u64, AMD_F_INEXACT|AMD_F_UNDERFLOW);
	     }
		 else
			 return retval;
	}

  return retval;
}


