/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
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
 * long long int llround(double x)
 *
 * Special values:
 *   - FE_INEXACT is never raised; current rounding mode has no effect.
 *   - x = +-0                                  -> 0
 *   - x = NaN / +-Inf / result out of 'long long'   -> FE_INVALID raised, value
 *                                                 unspecified.  errno is not set.
 *
 * long long is always 64-bit on both Windows and Linux (range exponent < 63).
 *
 * IEEE 754 double precision: x = (-1)^s * 2^(exp) * 1.f
 *   - |x| >= 2^52 : already an integer (no fractional bits) -> shift only.
 *   - |x| <  2^52 : add 0.5 ULP (0x0008000000000000 >> exp) to the significand,
 *                   then truncate -> ties away from zero.  The addition cannot
 *                   overflow the 53-bit significand.
 */

#define INTEGERBITS_DP64        52
#define HALF_MANTISSA_BIT_DP64  0x0008000000000000ULL
#define LLROUND_MAXEXP_DP64     63     /* ((int32_t)(8 * sizeof(llint_t)) - 1)  */
#define LLONG_MIN_AS_DOUBLE     asdouble(0xC3E0000000000000ULL)  /* -2^63       */

llint_t ALM_PROTO_OPT(llround)(double x)
{
    uint64_t ux   = asuint64(x);            /* Bit representation of x   */
    uint64_t uax  = ux & ~SIGNBIT_DP64;     /* |x| bits                  */
    int32_t  exp  = (int32_t)(uax >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
    llint_t  sign = (llint_t)((int64_t)ux >> 63); /* 0 (pos) / -1 (neg) */
    uint64_t i, mag;

    /*
     * Cold path: |x| too large for 'long long', or x is NaN / +-Inf (exp == 1024).
     * The result is unspecified and FE_INVALID is raised -- *except* negative
     * inputs that still round to exactly -2^(w-1) == LLONG_MIN, which are in
     * range and must not raise.
     */
    if (unlikely(exp >= LLROUND_MAXEXP_DP64)) {
         if (!sign || !(x >= LLONG_MIN_AS_DOUBLE))
            __alm_handle_error(ux, AMD_F_INVALID);
        return (llint_t)LLONG_MIN;
    }

    /* |x| < 1.0 : 0.5 <= |x| < 1 rounds away to +-1, |x| < 0.5 -> 0. */
    if (exp < 0)
        return (llint_t)((exp < -1) ? 0 : (sign | 1));  /* -1 or +1 */

    i = (uax & MANTBITS_DP64) | IMPBIT_DP64;    /* 53-bit significand */

    if (exp >= INTEGERBITS_DP64) {
        /* |x| >= 2^52 is already an exact integer: shift into place only. */
        mag = i << (exp - INTEGERBITS_DP64);
    } else {
        /* Add 0.5 ULP at this exponent, then truncate (ties away from zero). */
        i += HALF_MANTISSA_BIT_DP64 >> exp;
        mag = i >> (INTEGERBITS_DP64 - exp);
    }

    /*
     * Branchless conditional negate via XOR-negate idiom:
     *   sign =  0 :  mag          (positive)
     *   sign = -1 : -mag          (two's complement negate)
     */
    return ((llint_t)mag ^ sign) - sign;
}
