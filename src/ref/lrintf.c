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

long ALM_PROTO_REF(lrintf)(float x)
{
    long result = 0;

#if defined(__SSE2__) && (defined(__x86_64__) || defined(_M_X64))
#if LONG_MAX > 0x7fffffffL
    /* CVTSS2SI raises FE_INVALID for -2^63 (= LONG_MIN) on some x86 CPUs, even
     * though it is exactly representable as long. */
    if (unlikely(x == -0x1p63f)) {
        result = LONG_MIN;
    } else {
        result = (long)_mm_cvtss_si64(_mm_set_ss(x));
    }
#else
    result = (long)_mm_cvtss_si32(_mm_set_ss(x));
#endif
#else
    /* Threshold: 2^(CHAR_BIT*sizeof(long)-1), the long overflow boundary as a float bit-pattern. */
    static const uint32_t OvfThreshold =
        (uint32_t)(CHAR_BIT * sizeof(long) - 1 + 127) << 23;

    UT32 checkbits   = { .f32 = x };
    uint32_t absbits = checkbits.u32 & POS_BITSET_F32;

    if ((absbits > OvfThreshold) ||
        ((absbits == OvfThreshold) && ((checkbits.u32 & SIGNBIT_SP32) == 0))) {
        /* NaN, Inf, x > LONG_MAX, or x < LONG_MIN: out of long range.
           x = -2^(N-1) (LONG_MIN) has absbits == OvfThreshold with sign set,
           so it is excluded here and handled by the else-if branch below. */
        __alm_handle_errorf(INDEFBITPATT_SP32, AMD_F_INVALID);
        result = LONG_MIN;
    } else if (absbits > EXP_VAL_23_F32) {
        /* 2^23 < |x| < overflow threshold: already integral in float, cast directly. */
        result = (long)x;
    } else {
        UT32 val_2p23 = { .u32 = (checkbits.u32 & SIGNBIT_SP32) | EXP_VAL_23_F32 };
        result = (long)((x + val_2p23.f32) - val_2p23.f32);
    }
#endif

    return result;
}
