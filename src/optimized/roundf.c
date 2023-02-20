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
    float amd_roundf (float x);
    when x = NAN, return x, raise FE_INVALID
    when x is QNAN, return x
    when x is INF, return x.
*/

#include "fn_macros.h"
#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/typehelper.h>

float ALM_PROTO_OPT(roundf)(float f) {
    uint32_t ux, ux_temp, sign, exponent, mantissa;
    int intexp;            /*Needs to be signed */
    float r, temp;

    ux = asuint32(f);
    sign = ux & SIGNBIT_SP32;

    /*  if NAN or INF */
    if (unlikely((ux & EXPBITS_SP32) == EXPBITS_SP32)) {
    #ifdef WINDOWS
        return __alm_handle_errorf(ux |= QNAN_MASK_32, 0);
    #else
        /* If NaN, raise exception, no exception for Infinity */
        if (f != f) {
            return __alm_handle_errorf(ux, AMD_F_INVALID);
        }
        return f;
    #endif
        return __alm_handle_errorf(ux, 0);
    }

    /*Get the exponent of the input*/
    intexp = (ux & EXPBITS_SP32) >> 23;
    intexp -= 0x7F;
    
    /*If exponent > 22 then the number is already rounded*/
    if (unlikely(intexp > 22)) {
        return f;
    }

    if (intexp < 0) {
        temp = f;
        ux_temp = asuint32(temp);
        ux_temp &= 0x7FFFFFFF;

        /*Add with a large number (2^23 +1) = 8388609.0F
        to force an overflow*/
        temp = asfloat(ux_temp) + 8388609.0F;

        /*Subtract back with the large number*/
        temp -= 8388609;
        ux_temp = asuint32(temp);

        if (sign) {
            ux_temp |= SIGNBIT_SP32;
        }
        return asfloat(ux_temp);
    }
    else {
        ux &= 0x7FFFFFFF;
        r = asfloat(ux);
        r += 0.5f;
        exponent = asuint32(r) & EXPBITS_SP32;

        /*right shift & left shift to discard the decimal places*/
        mantissa = (asuint32(r) & MANTBITS_SP32) >> (23 - intexp);
        mantissa = mantissa << (23 - intexp);
        ux_temp = sign | exponent | mantissa;

        return asfloat(ux_temp);
    }
}
