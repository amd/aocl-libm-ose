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
    uint64_t ux, ax, uresult, sign;
    int intexp, shift;
    long int result;
    double r;

    ux = ax = asuint64(x);

    /*  if NAN or INF */
    if (unlikely((ux & EXPBITS_DP64) == EXPBITS_DP64)) {
        #ifdef WIN64
            return (long)__alm_handle_error(ux |= QNAN_MASK_64, 0);
        #else
            /* If NaN, raise exception, no exception for Infinity */
            if (x != x) {
                return (long)__alm_handle_error(ux, AMD_F_INVALID);
            }
            return (long)x;
        #endif
        return (long)__alm_handle_error(ux, 0);
    }

    ax &= ~SIGNBIT_DP64;

    sign = ux & SIGNBIT_DP64;
    intexp = (ux & EXPBITS_DP64) >> 52;
    intexp -= 0x3FF;

    /* 1.0 x 2^-1 is the smallest number which can be rounded to 1 */
    if (intexp < -1)
        return (0);

#ifdef WIN64
    /* 1.0 x 2^31 (or 2^63) is already too large */
    if (intexp >= 31) {
        /*Based on the sign of the input value return the MAX and MIN*/
        result = NEG_ZERO_F64; /*Return LONG MIN*/
        return (long)__alm_handle_error(result, AMD_F_NONE);
    }

#else
    /* 1.0 x 2^31 (or 2^63) is already too large */
    if (intexp >= 63) {
        /*Based on the sign of the input value return the MAX and MIN*/
        result = (long)NEG_ZERO_F64; /*Return LONG MIN*/
        return (long)__alm_handle_error((unsigned long long)result, AMD_F_NONE);
    }

#endif

    r = asdouble(ax);
    /* >= 2^52 is already an exact integer */
#ifdef WIN64
    if (intexp < 23)
#else
    if (intexp < 52)
#endif
    {
        /* add 0.5, extraction below will truncate */
        r = asdouble(ax) + 0.5;
    }

    uresult = asuint64(r);
    intexp = (uresult & EXPBITS_DP64) >> 52;
    intexp -= 0x3FF;
    uresult &= MANTBITS_DP64; // store all mantissa bits ONLY of the result
    uresult |= IMPBIT_DP64; // set the last bit of exp as 1
    shift = intexp - 52;

#ifdef WIN64
    /*The shift value will always be negative.*/
    uresult = uresult >> (-shift);
    /*Result will be stored in the lower word due to the shift being performed*/
    result = uresult;
#else
     if(shift < 0)
        uresult = uresult >> (-shift);
    if(shift > 0)
        uresult = uresult << (shift);

    result = (long)uresult;
#endif

    if (sign)
        result = -result;

    return result;
}
