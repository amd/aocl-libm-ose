#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"


double FN_PROTOTYPE(frexp)(double value, int *exp)
{
    UT64 val;
    unsigned int sign;
    int exponent;
    val.f64 = value;
    sign = val.u32[1] & SIGNBIT_SP32;
    val.u32[1] = val.u32[1] & ~SIGNBIT_SP32; /* remove the sign bit */
    *exp = 0;
    if((val.u64 == 0x0000000000000000) || (val.u64 == 0x7ff0000000000000))
        return value; /* value= +-0 or value= nan or value = +-inf return value */
    
    if(val.u64 > 0x7ff0000000000000)
    {
#ifdef WINDOWS
         return __amd_handle_error("frexp", __amd_frexp, val.u64|QNANBITPATT_DP64, DOMAIN, AMD_F_NONE, EDOM,value, 0.0, 1);
#else
         return value+value;
#endif
    }
    exponent = val.u32[1] >> 20; /* get the exponent */

    if(exponent == 0)/*x is denormal*/
    {
		val.f64 = val.f64 * VAL_2PMULTIPLIER_DP;/*multiply by 2^53 to bring it to the normal range*/
        exponent = val.u32[1] >> 20; /* get the exponent */
		exponent = exponent - MULTIPLIER_DP;
    }

	exponent -= 1022; /* remove bias(1023)-1 */
    *exp = exponent; /* set the integral power of two */
    val.u32[1] = sign | 0x3fe00000 | (val.u32[1] & 0x000fffff);/* make the fractional part(divide by 2) */                                              
    return val.f64;
}


