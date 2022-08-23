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

/* Implementation notes
    double amd_trunc (double x);

*/

#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/typehelper.h>

double ALM_PROTO_OPT(trunc)(double x)
{
    uint64_t ux, ux_temp, sign;
    int intexp; // Needs to be signed

    ux = asuint64(x);
    sign = ux & SIGNBIT_DP64;

    /*  if NAN or INF */
    if (unlikely((ux & EXPBITS_DP64) == EXPBITS_DP64)) {
        #ifdef WINDOWS
            return __alm_handle_error(ux |= QNAN_MASK_64, 0);
        #else
            /* If NaN, raise exception, no exception for Infinity */
            if (x != x) {
                return __alm_handle_error(ux, AMD_F_INVALID);
            }
            return x;
        #endif
        return __alm_handle_error(ux, 0);
    }

    /*Get the exponent of the input*/
    intexp = (ux & EXPBITS_DP64) >> 52;
    intexp -= 0x3FF;

    /*If exponent > 51 then the number is already truncated*/
    if (intexp > 51) {
        return x;
    }

    else if (intexp < 0) {
        return asdouble(sign);
    }

    ux_temp = ux & (uint64_t)~(MANTBITS_DP64 >> intexp);

    return asdouble(ux_temp);
}