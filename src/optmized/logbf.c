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
 *
 * Signature:
 * float logbf(float x);
 * Extracts the value of the unbiased radix-independent exponent from the floating-point argument arg, and returns it as a floating-point value.
 *
 * if x is +/- zero, return infinity with FE_DIV_BY_ZERO exception
 * if x is +/- infinity, infinity is returned
 * if x is NaN, NaN is returned
 * In all other cases, result is exact.
 *
 * Algorithm:
 * 1. Obtain bit pattern of x.
 *    Let ux = asuint64(x)
 *
 * 2. logbf(x) = ((ux & 0x7f800000) >> 23) - 127
 */

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>

float ALM_PROTO_OPT(logbf)(float x) {

    uint32_t ux;
    int32_t u;

    ux = asuint32(x);

    u = (int32_t)(((ux & EXPBITS_SP32) >> EXPSHIFTBITS_SP32) - EXPBIAS_SP32);

    if (unlikely((ux & ~SIGNBIT_SP32) == 0)) {

        return __alm_handle_errorf(NINFBITPATT_SP32, AMD_F_DIVBYZERO);

    }
    if ((EMIN_SP32 <= u) && (u <= EMAX_SP32)) {

        return (float)u;

    }
    if (unlikely(u > EMAX_SP32)) {
        /* x is infinity or NaN */
        if ((ux & MANTBITS_SP32) == 0) {

        #ifdef WINDOWS
            x = asfloat(ux);;
        #else
            x = asfloat(ux & (~SIGNBIT_SP32));
        #endif
            return x;
        }
        else {

        #ifdef WINDOWS
            return __alm_handle_errorf(ux|0x00400000, AMD_F_NONE);
        #else
            return x + x;
        #endif
        }
    }
    else {
        /* x is denormalized. */
    #ifdef FOLLOW_IEEE754_LOGB
      /* Return the value of the minimum exponent to ensure that
         the relationship between logb and scalb, defined in IEEE 754, holds. */
         return EMIN_SP32;
    #else
      /* Follow the rule set by IEEE 854 for logb */
        ux &= MANTBITS_SP32;

        u = EMIN_SP32;

        while (ux < IMPBIT_SP32) {

            ux <<= 1;

            u--;

        }

        return (float)u;
    #endif
    }
}


