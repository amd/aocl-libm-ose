/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include "fn_macros.h"
#include "libm_util_amd.h"
#include "libm_special.h"

long long int FN_PROTOTYPE_REF(llrint)(double x)
{


    UT64 checkbits,val_2p52;
    checkbits.f64=x;

    /* Clear the sign bit and check if the value can be rounded */

    if( (checkbits.u64 & 0x7FFFFFFFFFFFFFFF) > 0x4330000000000000)
    {
        /* number cant be rounded raise an exception */
        /* Number exceeds the representable range could be nan or inf also*/
       // __amd_handle_error(DOMAIN, EDOM, "llrint", x,0.0 ,(double)x);
		__amd_handle_error("llrint", __amd_lrint, (long long int)x, _DOMAIN, 0, EDOM, x, 0.0, 1);
		return (long long int) x;
    }

    val_2p52.u32[1] = (checkbits.u32[1] & 0x80000000) | 0x43300000;
    val_2p52.u32[0] = 0;


	/* Add and sub 2^52 to round the number according to the current rounding direction */

    return (long long int) ((x + val_2p52.f64) - val_2p52.f64);
}

