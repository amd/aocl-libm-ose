/* Contains implementation of double pow(double x, double y)
 * x^y = 2^(y*log2(x))
 */
#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"

#define L__exp_mask  0x7ff0000000000000
#define L__exp_bias  0x00000000000003ff // 1023
#define L__mant_mask 0x000fffffffffffff




double FN_PROTOTYPE_REF(zen_pow)(double x, double y)
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



