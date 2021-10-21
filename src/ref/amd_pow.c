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

/* Contains implementation of double pow(double x, double y)
 * x^y = 2^(y*log2(x))
 */
#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>

#define L__exp_mask  0x7ff0000000000000
#define L__exp_bias  0x00000000000003ff // 1023
#define L__mant_mask 0x000fffffffffffff




double ALM_PROTO_REF(zen_pow)(double x, double y)
{
  // x = 2^a, y = Integer
/*
  unsigned long ux;
  unsigned long uy;
  
  unsigned long exp_x;
  unsigned long exp_y;

  int yexp;
  int xexp;
  int inty;
  
  GET_BITS_DP64(x, ux);
  GET_BITS_DP64(y, uy);
 
  exp_x = (ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64;
  exp_y = (uy & EXPBITS_DP64) >> EXPSHIFTBITS_DP64;

  xexp = (int)(exp_y - EXPBIAS_DP64);
  yexp = (int)(exp_y - EXPBIAS_DP64);
  if(xexp > 8 || yexp > 7)
	 return amd_pow(x,y); 

  if (ux & L__mant_mask)
    {
      // x is not a power of 2
      return amd_pow(x,y);
    }
   */
  /* See whether y is an integer.
	inty = 0 means not an integer.
	inty = 1 means odd integer.
	inty = 2 means even integer.
  
 // yexp = (int)(exp_y - EXPBIAS_DP64);
    
  if (yexp < 1)
    inty = 0;
  else if (yexp > 53)
    inty = 2;
  else // 1 <= yexp <= 53 
    {
      // Mask out the bits of r that we don't want 
      unsigned long mask = 1;
      mask = (mask << (53 - yexp)) - 1;
      if ((uy & mask) != 0)
	inty = 0;
      else if (((uy & ~mask) >> (53 - yexp)) & 0x0000000000000001)
	inty = 1;
      else	
	inty = 2;
    }
  

  if (inty == 0)
    {
      // y is not integer
      return amd_pow(x,y);
    }

  // y is integer and x is powers of 2 

//  exp_x = (ux & L__exp_mask) >> 52;
  exp_x = exp_x - L__exp_bias; // remove bias

  double z = 0.0;
 
  z = exp_x * y + 1023.0; // Get exponent of x^y

  unsigned long uz = (unsigned long)z;
  uz = uz << 52;

  PUT_BITS_DP64(uz, z);

  unsigned long ax;
  int xpos;

  ax = ux & (~SIGNBIT_DP64);
  xpos = (ax == ux);

  if (xpos) return z;
  else if (inty == 1)
    {
      // x is -ve
      z = -z;
    }
 
  return z;
 */
 return amd_pow(x,y);
}// End of function



