/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_errno_amd.h"
#include "libm_special.h"



float FN_PROTOTYPE_REF(rintf)(float x)
{

    UT32 checkbits,sign,val_2p23;
    checkbits.f32=x;

    /* Clear the sign bit and check if the value can be rounded */
    if( (checkbits.u32 & 0x7FFFFFFF) > 0x4B000000)
    {
      /* Number exceeds the representable range could be nan or inf also*/
      /* take care of nan or inf */
	if(	(checkbits.u32 & EXPBITS_SP32) == EXPBITS_SP32)
	{
        if((checkbits.u32 & MANTBITS_SP32) == 0x0)
        {
            // x is Inf
#ifdef WINDOWS
            return  __amd_handle_errorf("rintf", __amd_rint, checkbits.u32, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
#else
            return  __amd_handle_errorf("rintf", __amd_rint, checkbits.u32, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
#endif
		}
		else {
			// x is NaN
#ifdef WINDOWS
		return  __amd_handle_errorf("rintf", __amd_rint, checkbits.u32 | QNAN_MASK_32, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
#else
		if (checkbits.u32 & QNAN_MASK_32)
		return  __amd_handle_errorf("rintf", __amd_rint, checkbits.u32 | QNAN_MASK_32, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
		else
		return  __amd_handle_errorf("rintf", __amd_rint, checkbits.u32 | QNAN_MASK_32, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
#endif
		}
	}
	else
		return x;

    }

    sign.u32 =  checkbits.u32 & 0x80000000;
    val_2p23.u32 = (checkbits.u32 & 0x80000000) | 0x4B000000;

   /* Add and sub 2^23 to round the number according to the current rounding direction */
    val_2p23.f32  = ((x + val_2p23.f32) - val_2p23.f32);

    /*This extra line is to take care of denormals and various rounding modes*/
    val_2p23.u32 = ((val_2p23.u32 << 1) >> 1) | sign.u32;

    if (val_2p23.f32 != x)
    {
        /* Raise floating-point inexact exception if the result differs in value from the argument */
         checkbits.u32 = 0xFFC00000;
         checkbits.f32 = checkbits.f32 +  checkbits.f32;        /* raise inexact exception by adding two nan numbers.*/
    }


    return val_2p23.f32;
}


