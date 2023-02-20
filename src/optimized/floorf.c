/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "fn_macros.h"
#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/typehelper.h>

float ALM_PROTO_OPT(floorf)(float x) {
    float r;
    uint32_t ux, ax, ur, mask, rexp, xneg;

    ux = asuint32(x);
    ax = ux & (~SIGNBIT_SP32);
    xneg = (ux != ax);

    if (ax >= TWOPOW24_F32) {
        /* abs(x) is either NaN, infinity, or >= 2^24 */
        if (ax > EXPBITS_SP32) {
            /* x is NaN */
#ifdef WINDOWS
    return __alm_handle_errorf(ux|QNAN_MASK_32, 0);
#else
    if (unlikely(!(ax & QNAN_MASK_32))) //x is snan
        return __alm_handle_errorf(ux | QNAN_MASK_32, AMD_F_INVALID);
    // x is qnan or inf
    return x;
#endif
	    }
        return x;
    }
    else if (ax < POS_ONE_F32) {
        /* abs(x) < 1.0 */
        if (ax == POS_ZERO_F32)
            /* x is +zero or -zero; return the same zero */
            return x;
        if (xneg) /* x < 0.0 */
            return -1.0F;
        return 0.0F;
    }
    rexp = ((ux & EXPBITS_SP32) >> EXPSHIFTBITS_SP32) - EXPBIAS_SP32;
    /* Mask out the bits of r that we don't want */
    mask = (uint32_t)((1 << (EXPSHIFTBITS_SP32 - rexp)) - 1);
    ur = (ux & ~mask);
    r = asfloat(ur);
    if (xneg && (ux != ur)) /* We threw some bits away and x was negative */
        return r - 1.0F;
    return r;
}
