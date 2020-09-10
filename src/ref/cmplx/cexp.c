/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include <libm_macros.h>
#include <libm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/constants.h>

fc64_t
ALM_PROTO_REF(cexp)(fc64_t z)
{
    flt64u_t re, im;
    f64_t zy_re, zy_im;

    re.d = creal(z);
    im.d = cimag(z);

    if((re.i & ALM_F64_SIGN_MASK) == 0) {
        if((im.i & ALM_F64_SIGN_MASK) == 0) {
            zy_re = 1.0;
            zy_im = 0.0;
				} else {
            ALM_PROTO(sincos)(im.d, &zy_im, &zy_re);
        }
    } else {
        if((im.i & ALM_F64_SIGN_MASK) == 0) {
            zy_re = ALM_PROTO(exp)(re.d);
            zy_im = 0.0;
        } else {
            f64_t t = ALM_PROTO(exp)(re.d);
            ALM_PROTO(sincos)(im.d, &zy_im, &zy_re);
            zy_re *= t;
            zy_im *= t;
        }

    }

    return (zy_re +I*zy_im);
}


