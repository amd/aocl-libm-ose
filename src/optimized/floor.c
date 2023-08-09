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

double ALM_PROTO_OPT(floor)(double x) {
    double r;
    uint64_t ux, ax, ur, mask, rexp, xneg;

    ux = asuint64(x);
    ax = ux & (~SIGNBIT_DP64);
    xneg = (ux != ax);

    if (ax >= TWOPOW53_DP64) {
        /* abs(x) is either NaN, infinity, or >= 2^53 */
        if (ax > EXPBITS_DP64) {
            /* x is NaN */
#ifdef WINDOWS
    return __alm_handle_error(ux | QNAN_MASK_64, 0);
#else
    if (unlikely(!(ax & QNAN_MASK_64))) // x is snan
        return __alm_handle_error(ux | QNAN_MASK_64, AMD_F_INVALID);
    else // x is qnan or inf
        return x;
#endif
        }
        else
            return x;
    }
    else if (ax < POS_ONE_F64) {
        /* abs(x) < 1.0 */
        if (ax == POS_ZERO_F64)
            /* x is +zero or -zero; return the same zero */
            return x;
        else if (xneg) /* x < 0.0 */
            return -1.0;
        else
            return 0.0;
    }
    else {
        r = x;
        rexp = ((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
        /* Mask out the bits of r that we don't want */
        mask = 1;
        mask = (mask << (EXPSHIFTBITS_DP64 - rexp)) - 1;
        ur = (ux & ~mask);
        r = asdouble(ur);
        if (xneg && (ur != ux))
            /* We threw some bits away and x was negative */
            return r - 1.0;
        return r;
    }
}
