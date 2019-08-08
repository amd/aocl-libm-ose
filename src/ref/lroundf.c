#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"

long int FN_PROTOTYPE(lroundf)(float f)
{
    UT32 u32d;
    UT32 u32Temp,u32result;
    int intexp, shift;
    U32 sign;
    long int  result;

    u32d.f32 = u32Temp.f32 = f;
    if ((u32d.u32 & 0X7F800000) == 0x7F800000)
    {
        /*else the number is infinity*/
		//Raise range or domain error
        {
		#ifdef WIN64
			_amd_handle_errorf("lroundf", _FpCodeLround, SIGNBIT_SP32, DOMAIN, AMD_F_NONE, EDOM, f, 0.0, 1);
			return (long int)SIGNBIT_SP32;
		#else
         if((u32d.u32 & 0x7fffffff) == 0x7f800000)
            return SIGNBIT_DP64;
         if((u32d.u32 & 0x7fffffff) >= 0x7fc00000)
                _amd_handle_errorf("lround", _FpCodeLround, (unsigned int)SIGNBIT_DP64, DOMAIN, AMD_F_NONE, EDOM, f, 0.0, 1);
         else    
                _amd_handle_errorf("lround", _FpCodeLround, (unsigned int)SIGNBIT_DP64, DOMAIN, AMD_F_INVALID, EDOM, f, 0.0, 1);
            
		 return SIGNBIT_DP64; /*GCC returns this when the number is out of range*/
		#endif
        }

    }

    u32Temp.u32 &= 0x7FFFFFFF;
    intexp = (u32d.u32 & 0x7F800000) >> 23;
    sign = u32d.u32 & 0x80000000;
    intexp -= 0x7F;


    /* 1.0 x 2^-1 is the smallest number which can be rounded to 1 */
    if (intexp < -1)
        return (0);


#ifdef WIN64
    /* 1.0 x 2^31 is already too large */
    if (intexp >= 31)
    {
        result = 0x80000000;
		_amd_handle_errorf("lroundf", _FpCodeLround, result, DOMAIN, AMD_F_NONE, EDOM, f, 0.0, 1);
        return result;
	}

#else
    /* 1.0 x 2^31 (or 2^63) is already too large */
    if (intexp >= 63)
    {
        result = 0x8000000000000000;
        _amd_handle_errorf("lroundf", _FpCodeLround, result, DOMAIN, AMD_F_NONE, EDOM, f, 0.0, 1);
		return result;
    }
 #endif

    u32result.f32 = u32Temp.f32;

    /* >= 2^23 is already an exact integer */
    if (intexp < 23)
    {
        /* add 0.5, extraction below will truncate */
        u32result.f32 = u32Temp.f32 + 0.5F;
    }
    intexp = (u32result.u32 & 0x7f800000) >> 23;
    intexp -= 0x7f;
    u32result.u32 &= 0x7fffff;
    u32result.u32 |= 0x00800000;

    result = u32result.u32;

    #ifdef WIN64
    shift = intexp - 23;
    #else

    /*Since float is only 32 bit for higher accuracy we shift the result by 32 bits
     * In the next step we shift an extra 32 bits in the reverse direction based
     * on the value of intexp*/
    result = result << 32;
    shift = intexp - 55; /*55= 23 +32*/
    #endif


	if(shift < 0)
		result = result >> (-shift);
	if(shift > 0)
        result = result << (shift);

    if (sign)
        result = -result;
    return result;

}
weak_alias (lroundf, FN_PROTOTYPE(lroundf))



