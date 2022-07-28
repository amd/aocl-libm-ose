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
    long int amd_lround (double x);
    when x = NAN, return x, raise FE_INVALID
    when x is QNAN, return x
    when x is INF, return x.
*/

#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/typehelper.h>

long int ALM_PROTO_OPT(lround)(double x)
{
    uint64_t ux, ux_temp, sign, exponent, mantissa;
    int intexp;            /*Needs to be signed */
    double temp, r;
    long int result;

    ux = asuint64(x);
    sign = ux & SIGNBIT_DP64;

    /*  if NAN or INF */
    if (unlikely((ux & EXPBITS_DP64) == EXPBITS_DP64)) {
        #ifdef WINDOWS
            return (long)__alm_handle_error(ux |= QNAN_MASK_64, 0);
        #else
            /* If NaN, raise exception, no exception for Infinity */
            if (unlikely(x != x)) {
                return (long)__alm_handle_error(ux, AMD_F_INVALID);
            }
        #endif
        return (long)__alm_handle_error(ux, 0);
    }

    /*Get the exponent of the input*/
    intexp = (ux & EXPBITS_DP64) >> 52;
    intexp -= 0x3FF;
    /*If exponent > 51 then the number is already rounded*/
    if (intexp > 51) {
        return (long)x;
    }

    if (intexp < 0) {
        temp = x;
        ux_temp = asuint64(temp);
        ux_temp &= POS_BITSET_DP64;
        /*Add with a large number (2^52 +1) = 4503599627370497.0
        to force an overflow*/
        temp = asdouble(ux_temp) + asdouble(0x4330000000000001);
        /*Subtract back with the large number*/
        temp -= asdouble(0x4330000000000001);
        ux_temp = asuint64(temp);

        if (sign) {
            ux_temp |= SIGNBIT_DP64;
        }

        return (long)ux_temp;
    }
    ux &= POS_BITSET_DP64;
    r = asdouble(ux);
    r += 0.5;
    exponent = asuint64(r) & EXPBITS_DP64;
    /*right shift then left shift to discard the decimal places*/
    mantissa = (asuint64(r) & MANTBITS_DP64) >> (52 - intexp);
    mantissa = mantissa << (52 - intexp);
    ux_temp = sign | exponent | mantissa;

    result = (long)ux_temp;
    if (sign)
        result = -result;

    return result;
}
