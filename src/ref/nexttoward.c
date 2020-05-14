/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"




double FN_PROTOTYPE_REF(nexttoward)(double x, long double y)
{


    UT64 checkbits;
    long double dy = (long double) y;
    checkbits.f64=x;


    /* check if the number is nan */
    if(((checkbits.u64 & ~SIGNBIT_DP64) > EXPBITS_DP64 ))
    {
#ifdef WINDOWS
	return  __amd_handle_error("nexttoward", __amd_nexttoward, checkbits.u64 | QNAN_MASK_64 , _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
#else
	if (checkbits.u64 & QNAN_MASK_64)
	return  __amd_handle_error("nexttoward", __amd_nexttoward, checkbits.u64 | QNAN_MASK_64 , _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
	else
	return  __amd_handle_error("nexttoward", __amd_nexttoward, checkbits.u64 | QNAN_MASK_64 , _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0, 1);
#endif
    }

	   /* if x == y return y in the type of x */
    if( x == dy )
    {
        return (double) dy;
    }

    if( x == 0.0)
    {
        checkbits.u64 = 1;
        if( dy > 0.0 )
             return checkbits.f64;
        else
            return -checkbits.f64;
    }


    /* compute the next heigher or lower value */

    if(((x>0.0) ^ (dy>x)) == 0)
    {
        checkbits.u64++;
    }
    else
    {
        checkbits.u64--;
    }

    /* check if the result is nan or inf */
    if(((checkbits.u64 & ~SIGNBIT_DP64) >= EXPBITS_DP64 ))
    {
		return  __amd_handle_error("nexttoward", __amd_nexttoward, checkbits.u64 | QNAN_MASK_64, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0, 1);
    }

    return checkbits.f64;
}

