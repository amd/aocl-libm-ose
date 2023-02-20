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
 * Signature:
 * double logb(double x);
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
 * 2. logb(x) = ((ux & 0x7ff0000000000000) >> 52) - 1023
 *
 */

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>

double ALM_PROTO_OPT(logb)(double x) {

    uint64_t ux;
    int64_t u;

    ux = asuint64(x);

    u = (int64_t)(((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64);

    if(unlikely((ux & ~SIGNBIT_DP64) == 0)) {

        return __alm_handle_error(NINFBITPATT_DP64, AMD_F_DIVBYZERO);

    }
    if ((EMIN_DP64 <= u) && (u <= EMAX_DP64)) {

        return (double)u;

    }
    if (unlikely(u > EMAX_DP64)) {

        if ((ux & MANTBITS_DP64) == 0) {
        /* x is +/-infinity. For VC++, return infinity of same sign. */
        #ifdef WINDOWS
            return x;
        #endif
            return asdouble(ux & (~SIGNBIT_DP64));

        }
        else {
        /* x is NaN, result is NaN */
        #ifdef WINDOWS
            return __alm_handle_error(ux|0x0008000000000000, AMD_F_NONE);
        #endif
            return x+x;

        }
    }

    /* x is denormalized. */
#ifdef FOLLOW_IEEE754_LOGB
      /* Return the value of the minimum exponent to ensure that
         the relationship between logb and scalb, defined in
         IEEE 754, holds. */
    return EMIN_DP64;
#else
    /* Follow the rule set by IEEE 854 for logb */
    ux &= MANTBITS_DP64;

    u = EMIN_DP64;

    while (ux < IMPBIT_DP64) {

        ux <<= 1;
        u--;

    }

     return (double)u;
#endif

}


