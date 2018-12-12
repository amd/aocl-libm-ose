#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"


float FN_PROTOTYPE(frexpf)(float value, int *exp)
{
    UT32 val;
    unsigned int sign;
    int exponent;
    val.f32 = value;
    sign = val.u32 & SIGNBIT_SP32;
    val.u32 = val.u32 & ~SIGNBIT_SP32; /* remove the sign bit */
    *exp = 0;
    if((val.u32 == 0x00000000) || (val.u32 == 0x7f800000)) 
        return value; /* value= +-0 or value= nan or value = +-inf return value */

    if(val.u32 > 0x7f800000)
     {
#ifdef WINDOWS
         return __amd_handle_errorf("frexpf", __amd_frexp, val.u32|QNANBITPATT_DP64, DOMAIN, AMD_F_NONE, EDOM, value, 0.0, 1);
#else
         return value+value;
#endif
     }
    exponent = val.u32 >> 23; /* get the exponent */

	if(exponent == 0)/*x is denormal*/
	{
		val.f32 = val.f32 * VAL_2PMULTIPLIER_SP;/*multiply by 2^24 to bring it to the normal range*/
		exponent = (val.u32 >> 23); /* get the exponent */
		exponent = exponent - MULTIPLIER_SP;
	}

    exponent -= 126; /* remove bias(127)-1 */
    *exp = exponent; /* set the integral power of two */
    val.u32 = sign | 0x3f000000 | (val.u32 & 0x007fffff);/* make the fractional part(divide by 2) */                                              
    return val.f32;
}
weak_alias (frexpf, FN_PROTOTYPE(frexpf))

