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
* Function signature:
 * int ilogbf(float x)
 *
 * Extracts the value of the unbiased exponent from the floating-point argument
 * and returns it as a signed integer value.
 *
 * Algorithm:
 * 1. Get the binary representation of the input floating point argument
 * 2. exponent = Left shift by 1 and right shift by 24.
 * 3. Subtract bias from exponent
 *
 * IEEE SPEC:
 * 1. If the correct result is greater than INT_MAX or smaller than INT_MIN, FE_INVALID is raised.
 * 2. If arg is +/-0, +/-infinity, or NaN, FE_INVALID is raised.
 * 3. In all other cases, the result is exact (FE_INEXACT is never raised).
 */

#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/typehelper.h>

int ALM_PROTO_OPT(ilogbf)(float x) {
    uint32_t ux = asuint32(x);
    uint32_t mant   = ux & MANTBITS_SP32;
    int32_t expbits = (int32_t) (( ux << 1) >> 24);
    uint32_t sign = ux & SIGNBIT_SP32;

    /* if x is 0 */
    if (x * 2 == 0) {
        __alm_handle_errorf((unsigned int)INT_MIN,
                                  AMD_F_DIVBYZERO);
        return INT_MIN;
    }

    /* either NAN or inf */
    if ((ux & EXPBITS_SP32) == EXPBITS_SP32) {
        if (x != x) {    /* if NaN */
             __alm_handle_errorf((unsigned int)INT_MIN,
                                              AMD_F_INVALID);
            return INT_MIN;
        }
        else {    /* inf */
            if (sign) {    /* -INF */
                __alm_handle_errorf((unsigned int)INT_MAX,
                                          AMD_F_INVALID);
            }
            else {     /* + INF */
                __alm_handle_errorf((unsigned int)INT_MAX,
                                          AMD_F_INVALID);
            }
            return INT_MAX;
        }
    }

    if(expbits == 0 && (mant)!= 0) {
        /* the value is denormalized */
        expbits = EMIN_SP32;
        while (mant < IMPBIT_SP32) {
            mant <<= 1;
            expbits --;
        }
    }

    else {
        expbits -= EXPBIAS_SP32;
    }

    return expbits;
}

