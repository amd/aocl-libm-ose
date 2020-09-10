/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include <libm_macros.h>
#include <libm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/constants.h>

fc32_t
ALM_PROTO_REF(cexpf)(fc32_t z)
{
    flt32u_t    re, im;
    f32_t       zy_re, zy_im;

    re.f = creal(z);
    im.f = cimag(z);

    if((re.i & ALM_F32_SIGN_MASK) == 0) {
        if((im.i & ALM_F32_SIGN_MASK) == 0) {
            zy_re = 1.0f;
            zy_im = 0.0f;
        } else {
            ALM_PROTO(sincosf)(im.f, &zy_im, &zy_re);
        }
    } else {
        if((im.i & ALM_F32_SIGN_MASK) == 0) {
            zy_re = ALM_PROTO(expf)(re.f);
            zy_im = 0.0f;
        } else {
            f32_t t = ALM_PROTO(expf)(re.f);
            ALM_PROTO(sincosf)(im.f, &zy_im, &zy_re);
            zy_re *= t;
            zy_im *= t;
        }

    }

    return zy_re + I*zy_im;
}

