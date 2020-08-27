/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include <libm_special.h>
#include <libm/compiler.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>

#define  ALM_TANF_SIGN_MASK   ~(1UL<<63)
#define  ALM_TANF_SIGN_MASK32 ~(1U<<31)

float tanf_specialcase(float x)
{
    uint32_t   ux = asuint32(x);

    if(unlikely((ux - asuint32(0x1p-126)) >
                (asuint32(0x1p+127) - asuint32(0x1p-126)))) {

        if((ux  & ALM_TANF_SIGN_MASK32) >= 0x7f800000) {
            /*  infinity or NaN */
            return _tanf_special(x);
        }
    }

    if (ux == 0)
        return 0;

    /* Result is already calculated */
    return x;

    /* return ALM_PROTO(tanf)(x); */
}
