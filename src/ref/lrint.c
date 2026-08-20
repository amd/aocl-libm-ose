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
#include <libm/compiler.h>
#include <limits.h>
#if defined(__SSE2__) && (defined(__x86_64__) || defined(_M_X64))
#include <emmintrin.h>
#endif

long ALM_PROTO_REF(lrint)(double x)
{
    long result = 0;

#if defined(__SSE2__) && (defined(__x86_64__) || defined(_M_X64))
#if LONG_MAX > 0x7fffffffL
    /* CVTSD2SI raises FE_INVALID for -2^63 (= LONG_MIN) on some x86 CPUs, even
     * though it is exactly representable as long. */
    if (unlikely(x == -0x1p63)) {
        result = LONG_MIN;
    } else {
        result = (long)_mm_cvtsd_si64(_mm_set_sd(x));
    }
#else
    result = (long)_mm_cvtsd_si32(_mm_set_sd(x));
#endif
#else
    UT64 checkbits   = { .f64 = x };
    uint64_t absbits = checkbits.u64 & POS_BITSET_DP64;

    if (unlikely(absbits >= EXP_VAL_52_DP64)) {
        /* NaN, Inf, or |x| >= 2^52. */
#if LONG_MAX > 0x7fffffffL
        /* LP64: exact integers in [2^52, 2^63) are in-range; cast directly.
           Biased-exponent bit-pattern for 2^63: */
        static const uint64_t Ovf64 = (uint64_t)(63 + EXPBIAS_DP64) << EXPSHIFTBITS_DP64;
        if ((absbits >= 0x7FF0000000000000ULL) ||    /* NaN or Inf */
            (absbits > Ovf64) ||                     /* |x| > 2^63 */
            ((absbits == Ovf64) && !(checkbits.u64 & SIGNBIT_DP64))) { /* x = +2^63 */
            __alm_handle_error(INDEFBITPATT_DP64, AMD_F_INVALID);
            result = LONG_MIN;
        } else {
            result = (long)x;  /* exact integer in [-2^63, 2^63), fits in LP64 long */
        }
#else
        /* ILP32: |x| >= 2^52 >> 2^31 = LONG_MAX+1, always out of range. */
        __alm_handle_error(INDEFBITPATT_DP64, AMD_F_INVALID);
        result = LONG_MIN;
#endif
    } else {
        /* |x| < 2^52: round to the nearest integer using the 2^52 add/subtract
           trick (respects the current rounding mode), then range-check.
           Checking the *rounded* value rather than the raw exponent is essential
           on ILP32: a value like -(2^31 + epsilon) rounds to LONG_MIN under
           FE_TONEAREST / FE_TOWARDZERO / FE_UPWARD and must NOT raise FE_INVALID. */
        UT64 val_2p52 = { .u64 = (checkbits.u64 & SIGNBIT_DP64) | EXP_VAL_52_DP64 };
        double rx = (x + val_2p52.f64) - val_2p52.f64;
        if (unlikely((rx > (double)LONG_MAX) || (rx < (double)LONG_MIN))) {
            __alm_handle_error(INDEFBITPATT_DP64, AMD_F_INVALID);
            result = LONG_MIN;
        } else {
            result = (long)rx;
        }
    }
#endif

    return result;
}
