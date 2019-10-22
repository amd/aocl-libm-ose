#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"

#ifdef WINDOWS
/*In windows llong long int is 64 bit and long int is 32 bit.
  In Linux long long int and long int both are of size 64 bit*/
long long int FN_PROTOTYPE_REF(llround)(double d)
{
    UT64 u64d;
    UT64 u64Temp,u64result;
    int intexp, shift;
    U64 sign;
    long long int result;

    u64d.f64 = u64Temp.f64 = d;

    if ((u64d.u32[1] & 0X7FF00000) == 0x7FF00000)
    {
        /*the number is infinity*/
        //Got to raise range or domain error
		__amd_handle_error("llround", __amd_lround, u64d.u64, _DOMAIN, AMD_F_NONE, EDOM, d, 0.0, 1);
		return SIGNBIT_DP64; /*GCC returns this when the number is out of range*/
    }

    u64Temp.u32[1] &= 0x7FFFFFFF;
    intexp = (u64d.u32[1] & 0x7FF00000) >> 20;
    sign = u64d.u64 & 0x8000000000000000;
    intexp -= 0x3FF;

    /* 1.0 x 2^-1 is the smallest number which can be rounded to 1 */
    if (intexp < -1)
        return (0);

    /* 1.0 x 2^31 (or 2^63) is already too large */
    if (intexp >= 63)
    {
        /*Based on the sign of the input value return the MAX and MIN*/
        result = 0x8000000000000000; /*Return LONG MIN*/
		__amd_handle_error("llround", __amd_lround, result, _DOMAIN, AMD_F_NONE, EDOM, d, 0.0, 1);
        return result;
    }

    u64result.f64 = u64Temp.f64;
    /* >= 2^52 is already an exact integer */
    if (intexp < 52)
    {
        /* add 0.5, extraction below will truncate */
        u64result.f64 = u64Temp.f64 + 0.5;
    }

    intexp = ((u64result.u32[1] >> 20) & 0x7ff) - 0x3FF;

    u64result.u32[1] &= 0xfffff;
    u64result.u32[1] |= 0x00100000; /*Mask the last exp bit to 1*/
    shift = intexp - 52;

    if(shift < 0)
        u64result.u64 = u64result.u64 >> (-shift);
    if(shift > 0)
        u64result.u64 = u64result.u64 << (shift);

    result = u64result.u64;

    if (sign)
        result = -result;

    return result;
}

#else //WINDOWS 
/*llroundf is equivalent to the linux implementation of 
  lroundf. Both long int and long long int are of the same size*/
long long int FN_PROTOTYPE_REF(llround)(double d)
{
    long long int result;
    result = FN_PROTOTYPE(lround)(d);
    return result;
}

#endif
