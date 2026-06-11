/*
 * Copyright (C) 2008-2026 Advanced Micro Devices, Inc. All rights reserved.
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

#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/typehelper.h>
#include <libm/types.h>
#include <limits.h>

/*
 *  long int lroundf(float x)
 *
 * Special values:
 *   - FE_INEXACT is never raised; current rounding mode has no effect.
 *   - x = +-0                                  -> 0
 *   - x = NaN / +-Inf / result out of 'long'   -> FE_INVALID raised, value
 *                                                 unspecified.  errno is not set.
 *
 * The in-range threshold uses 8*sizeof(lint_t), so it adapts automatically:
 *   - Windows LLP64 : sizeof(long) == 4  (range exponent < 31)
 *   - Linux   LP64  : sizeof(long) == 8  (range exponent < 63)
 *
 * IEEE 754 single precision: x = (-1)^s * 2^(exp) * 1.f
 *   - |x| >= 2^23 : already an integer (no fractional bits) -> shift only.
 *   - |x| <  2^23 : add 0.5 ULP (0x00400000 >> exp) to the significand, then
 *                   truncate -> ties away from zero.  A float < 2^23 always
 *                   rounds to < 2^23, so this can never overflow 'long'.
 */

#define INTEGERBITS_SP32        23
#define HALF_MANTISSA_BIT_SP32  0x00400000U

#if LONG_MAX == 0x7FFFFFFF   /* 32-bit long */
#define LROUND_MAXEXP_SP32   31     /* ((int32_t)(8 * sizeof(lint_t)) - 1)  */
#define LONG_MIN_AS_FLOAT    asfloat(0xCF000000U)
#else                        /* 64-bit long */
#define LROUND_MAXEXP_SP32   63     /* ((int32_t)(8 * sizeof(lint_t)) - 1)  */
#define LONG_MIN_AS_FLOAT asfloat(0xDF000000U)
#endif

lint_t ALM_PROTO_OPT(lroundf)(float x)
{
    uint32_t ux   = asuint32(x);            /* Bit representation of x   */
    uint32_t uax  = ux & ~SIGNBIT_SP32;     /* |x| bits                  */
    int32_t  exp   = (int32_t)(uax >> EXPSHIFTBITS_SP32) - EXPBIAS_SP32;
    lint_t   sign = (lint_t)((int32_t)ux >> 31);  /* 0 (pos) / -1 (neg) */
    uint32_t i;
    lint_t   result;

    /*
     * Cold path: |x| too large for 'long', or x is NaN / +-Inf (exp == 128).
     * The result is unspecified and FE_INVALID is raised; exactly
     * -2^(w-1) == LONG_MIN is representable and returned without raising.
     */
    if (unlikely(exp >= LROUND_MAXEXP_SP32)) {
         if (!sign || x < LONG_MIN_AS_FLOAT)
             __alm_handle_errorf(ux, AMD_F_INVALID);
         return (lint_t)LONG_MIN;
     }

    /* |x| < 1.0 : 0.5 <= |x| < 1 rounds away to +-1, |x| < 0.5 -> 0. */
    if (exp < 0)
        return (lint_t)((exp < -1) ? 0 : (sign | 1));  /* -1 or +1 */

    i = (uax & MANTBITS_SP32) | IMPBIT_SP32;    /* 1 + 23 stored bits => 24-bit significand */

    if (exp >= INTEGERBITS_SP32) {
        /* |x| >= 2^23 is already an exact integer: shift into place only. */
        result = (lint_t)i << (exp - INTEGERBITS_SP32);
    } else {
        /* Add 0.5 ULP at this exponent, then truncate (ties away from zero). */
        i += HALF_MANTISSA_BIT_SP32 >> exp;
        result = (lint_t)(i >> (INTEGERBITS_SP32 - exp));
    }

    /*
     * Branchless conditional negate via XOR-negate idiom:
     *   sign =  0 :  result       (positive)
     *   sign = -1 : -result       (two's complement negate)
     */
    return (result ^ sign) - sign;
}
