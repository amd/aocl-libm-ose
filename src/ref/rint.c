#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_errno_amd.h"
#include "libm_special.h"


double FN_PROTOTYPE(rint)(double x)
{

    UT64 checkbits,val_2p52;
	UT32 sign;
    checkbits.f64=x;

    /* Clear the sign bit and check if the value can be rounded(i.e check if exponent less than 52) */
    if( (checkbits.u64 & 0x7FFFFFFFFFFFFFFF) > 0x4330000000000000)
    {
      /* take care of nan or inf */
	if(	(checkbits.u64 & EXPBITS_DP64) == EXPBITS_DP64)
	{
        if((checkbits.u64 & MANTBITS_DP64) == 0x0)
        {
            // x is Inf	
#ifdef WINDOWS
            return  _amd_handle_error("rint", _FpCodeRint, checkbits.u64, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
#else
            return  _amd_handle_error("rint", _FpCodeRint, checkbits.u64, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
#endif
		}
		else {
			// x is NaN
			// QNAN_MASK_32
#ifdef WINDOWS
		return  _amd_handle_error("rint", _FpCodeRint, checkbits.u64 | QNAN_MASK_64, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
#else
		if (checkbits.u64 & QNAN_MASK_64)
		return  _amd_handle_error("rint", _FpCodeRint, checkbits.u64 | QNAN_MASK_64, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
		else
		return  _amd_handle_error("rint", _FpCodeRint, checkbits.u64 | QNAN_MASK_64, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
#endif
		}
	}
	else
		return x;
    }

    sign.u32 =  checkbits.u32[1] & 0x80000000;
    val_2p52.u32[1] = sign.u32 | 0x43300000;
    val_2p52.u32[0] = 0;

	/* Add and sub 2^52 to round the number according to the current rounding direction */
    val_2p52.f64 = (x + val_2p52.f64) - val_2p52.f64;

    /*This extra line is to take care of denormals and various rounding modes*/
    val_2p52.u32[1] = ((val_2p52.u32[1] << 1) >> 1) | sign.u32;

     if(x!=val_2p52.f64)
	{
   	     /* Raise floating-point inexact exception if the result differs in value from the argument */
      	    checkbits.u64 = QNANBITPATT_DP64;
     	    checkbits.f64 = checkbits.f64 +  checkbits.f64;        /* raise inexact exception by adding two nan numbers.*/
	}


    return (val_2p52.f64);
}

weak_alias (rint, FN_PROTOTYPE(rint))



