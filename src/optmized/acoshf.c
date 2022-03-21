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

/*
* ISO-IEC-10967-2: Elementary Numerical Functions
* Signature:
*   float acoshf(float x)
*
* Spec:
*   acoshf(+inf) = +inf
*   acoshf(-inf) = NaN
*   acoshf(NaN)  = NaN
*
*
********************************************
* Implementation Notes
* ---------------------
* To compute acoshf(float x)
*
* Based on the value of x, acoshf(x) is calculated as,
*
* 1. If x < 1.0,
*      return a NaN
*
* 2. If x > 1/sqrt(epsilon) in magnitude are
*     approximated by acoshf(x) = ln(2) + ln(x)
* 3. If x between sqrt(epsilon) <= x <= 2.0,
*      t = x - 1.0;
*      rarg = 2.0 * t + t * t;
*      r = sqrt(rarg);
*      rarg = t + r;
*      r = log1p(rarg);
*      return (float)(r);
*/

#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include "libm_inlines_amd.h"
#include "kern/sqrt_pos.c"

static struct {
    const uint32_t recrteps;
    const double log2;
} acoshf_data = {
                  .recrteps = 0x46000000, /* 1/sqrt(eps) = 4.09600000000000000000e+03 */
                  .log2 = 6.93147180559945286227e-01, /* 0x3fe62e42fefa39ef */
};

#define recrteps acoshf_data.recrteps
#define log2     acoshf_data.log2

float ALM_PROTO_OPT(acoshf)(float x) {
    uint32_t ux;
    double dx, r, rarg, t;

    ux = asuint32(x);

    /* x is either NaN or infinity */
    if ((ux & EXPBITS_SP32) == EXPBITS_SP32) {
        /* x is NaN */
        if (x != x) {
#ifdef WINDOWS
            return __alm_handle_errorf(ux | 0x00400000, AMD_F_NONE);
#else
            if (ux & QNAN_MASK_32) {
                /* for qnan, raise no exceptions, for snan, raise invalid */
                return __alm_handle_errorf(ux | 0x00400000, AMD_F_NONE);
            }
            else {
                return __alm_handle_errorf(ux | 0x00400000, AMD_F_INVALID);
            }
#endif
        } else {
            /* x is infinity */
            if (ux & SIGNBIT_SP32) {
                /* x is negative infinity. Return a NaN. */
                return __alm_handle_errorf(INDEFBITPATT_SP32, AMD_F_INVALID);
            }
            else {
                /* Return positive infinity with no signal */
                return x;
            }
        }
    } else if ((ux & SIGNBIT_SP32) || (ux < 0x3f800000)) {
        /* x is less than 1.0. Return a NaN. */
        return __alm_handle_errorf(INDEFBITPATT_SP32, AMD_F_INVALID);
    }

    dx = x;

    if (ux > recrteps) {
        /* Arguments greater than 1/sqrt(epsilon) in magnitude are
           approximated by acoshf(x) = ln(2) + ln(x) */
        r = ALM_PROTO_OPT(log)(dx) + log2;
    } else if (ux > 0x40000000) {
        /* 2.0 <= x <= 1/sqrt(epsilon) */
        /* acoshf for these arguments is approximated by
           acoshf(x) = ln(x + sqrt(x*x-1)) */
        rarg = dx * dx - 1.0;
        /* Use assembly instruction to compute r = sqrt(rarg); */
        //ASMSQRT(rarg, r);
        r = ALM_PROTO_KERN(sqrt)(rarg);
        rarg = r + dx;
        r = ALM_PROTO_OPT(log)(rarg);
    } else {
        /* sqrt(epsilon) <= x <= 2.0 */
        t = dx - 1.0;
        rarg = 2.0 * t + t * t;
        r = ALM_PROTO_KERN(sqrt)(rarg);

        rarg = t + r;
        /* change this to ALM_PROTO_OPT when we have optimized c \
           variant for log1p */
        r = ALM_PROTO_FMA3(log1p)(rarg);
    }

    return (float)(r);
}
