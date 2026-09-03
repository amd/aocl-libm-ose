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

#include "fn_macros.h"
#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <limits.h>
#if defined(__SSE2__) && (defined(__x86_64__) || defined(_M_X64))
#include <emmintrin.h>
#endif

long long ALM_PROTO_REF(llrint)(double x)
{
    long long result = 0;

#if defined(__SSE2__) && (defined(__x86_64__) || defined(_M_X64))
    result = _mm_cvtsd_si64(_mm_set_sd(x));
#else
    /* Threshold: 2^63, the long long overflow boundary as a double bit-pattern (0x43E0000000000000). */
    static const uint64_t OvfThreshold = (uint64_t)(63 + EXPBIAS_DP64) << EXPSHIFTBITS_DP64;

    UT64 checkbits = { .f64 = x };
    uint64_t absbits = checkbits.u64 & POS_BITSET_DP64;

    if ((absbits > OvfThreshold) ||
        ((absbits == OvfThreshold) && ((checkbits.u64 & SIGNBIT_DP64) == 0))) {
        /* NaN, Inf, x > LLONG_MAX, or x < LLONG_MIN: out of long long range.
           x = -2^63 (LLONG_MIN) has absbits == OvfThreshold with sign set,
           so it is excluded here and handled by the else-if branch below. */
        __alm_handle_error(INDEFBITPATT_DP64, AMD_F_INVALID);
        result = LLONG_MIN;
    } else if (absbits > EXP_VAL_52_DP64) {
        /* 2^52 < |x| < 2^63: already integral in double, cast directly. */
        result = (long long)x;
    } else {
        UT64 val_2p52 = { .u64 = (checkbits.u64 & SIGNBIT_DP64) | EXP_VAL_52_DP64 };
        result = (long long)((x + val_2p52.f64) - val_2p52.f64);
    }
#endif

    return result;
}
