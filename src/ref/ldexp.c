#include "fn_macros.h"
#include "libm_util_amd.h"
#include "libm_special.h"

double FN_PROTOTYPE_REF(ldexp)(double x, int n)
{
    UT64 val,val_x;
    unsigned int sign;
    int exponent;
    val.f64 = x;
	val_x.f64 = x;
    sign = val.u32[1] & 0x80000000;
    val.u32[1] = val.u32[1] & 0x7fffffff; /* remove the sign bit */

    if (val.u64 > 0x7ff0000000000000)     /* x is NaN */
        #ifdef WINDOWS
        return __amd_handle_error("ldexp", __amd_ldexp, val_x.u64|0x0008000000000000, _DOMAIN, 0, EDOM, x, n, 2);
        #else
        {
        if(!(val.u64 & 0x0008000000000000))// x is snan
           return __amd_handle_error("ldexp", __amd_ldexp, val_x.u64|0x0008000000000000, _DOMAIN, AMD_F_INVALID, EDOM, x, n, 2);
        else
           return x;
	    }
        #endif

    if(val.u64 == 0x7ff0000000000000)/* x = +-inf*/
        return x;

    if((val.u64 == 0x0000000000000000) || (n==0))
        return x; /* x= +-0 or n= 0*/

    exponent = val.u32[1] >> 20; /* get the exponent */

    if(exponent == 0)/*x is denormal*/
    {
		val.f64 = val.f64 * VAL_2PMULTIPLIER_DP;/*multiply by 2^53 to bring it to the normal range*/
        exponent = val.u32[1] >> 20; /* get the exponent */
		exponent = exponent + n - MULTIPLIER_DP;
		if(exponent < -MULTIPLIER_DP)/*underflow*/
		{
			val.u32[1] = sign | 0x00000000;
			val.u32[0] = 0x00000000;
            return __amd_handle_error("ldexp", __amd_ldexp, val.u64, _UNDERFLOW, AMD_F_INEXACT|AMD_F_UNDERFLOW, ERANGE, x, (double)n, 2);
		}
		if(exponent > 2046)/*overflow*/
		{
			val.u32[1] = sign | 0x7ff00000;
			val.u32[0] = 0x00000000;
			return __amd_handle_error("ldexp", __amd_ldexp, val.u64, _OVERFLOW, AMD_F_INEXACT|AMD_F_OVERFLOW, ERANGE ,x, (double)n, 2);
		}

		exponent += MULTIPLIER_DP;
		val.u32[1] = sign | (exponent << 20) | (val.u32[1] & 0x000fffff);
		val.f64 = val.f64 * VAL_2PMMULTIPLIER_DP;
        return val.f64;
    }

    exponent += n;

    if(exponent < -MULTIPLIER_DP)/*underflow*/
	{
		val.u32[1] = sign | 0x00000000;
		val.u32[0] = 0x00000000;
        return __amd_handle_error("ldexp", __amd_ldexp, val.u64, _UNDERFLOW, AMD_F_INEXACT|AMD_F_UNDERFLOW, ERANGE, x, (double)n, 2);
	}

    if(exponent < 1)/*x is normal but output is debnormal*/
    {
		exponent += MULTIPLIER_DP;
		val.u32[1] = sign | (exponent << 20) | (val.u32[1] & 0x000fffff);
		val.f64 = val.f64 * VAL_2PMMULTIPLIER_DP;
        return val.f64;
    }

    if(exponent > 2046)/*overflow*/
	{
		val.u32[1] = sign | 0x7ff00000;
		val.u32[0] = 0x00000000;
	return __amd_handle_error("ldexp", __amd_ldexp, val.u64, _OVERFLOW, AMD_F_INEXACT|AMD_F_OVERFLOW, ERANGE ,x, (double)n, 2);
	}

    val.u32[1] = sign | (exponent << 20) | (val.u32[1] & 0x000fffff);
    return val.f64;
}




