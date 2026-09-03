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
#include <libm/amd_funcs_internal.h>
#include <libm/typehelper.h>
#include <limits.h>

#define MAXEXP ((int)(sizeof(llint_t) * CHAR_BIT - 1))

llint_t ALM_PROTO_REF(llround)(double x)
{
    uint64_t ux = asuint64(x);
    int     exp = (int)((ux >> EXPSHIFTBITS_DP64) &
                        (EXPBITS_DP64 >> EXPSHIFTBITS_DP64)) - EXPBIAS_DP64;
    llint_t result;

    if (unlikely(exp >= MAXEXP)) {
        // Cold path: |x| too large for 'long long', or x is NaN / +-Inf.
        // The result is unspecified and FE_INVALID is raised -- *except*
        // negative inputs that still round to exactly -2^(w-1) == LLONG_MIN,
        // which are in range and must not raise.
        if (unlikely((ux ^ SIGNBIT_DP64) >
                     ((uint64_t)(EXPBIAS_DP64 + MAXEXP) << EXPSHIFTBITS_DP64))) {
            ALM_RAISE_FE_INVALID();
        }
        result = LLONG_MIN;
    } else {
        if (likely((unsigned)exp < EXPSHIFTBITS_DP64)) {
            uint64_t mant = (ux & MANTBITS_DP64) | IMPBIT_DP64;
            // Add 0.5 ULP at this exponent, then truncate (ties away from zero)
            result = (llint_t)((mant + (QNAN_MASK_64 >> exp)) >> (EXPSHIFTBITS_DP64 - exp));
        } else if (likely(exp >= 0)) {
            uint64_t mant = (ux & MANTBITS_DP64) | IMPBIT_DP64;
            // |x| >= 2^52 is already an exact integer: shift into place only
            result = (llint_t)mant << (exp - EXPSHIFTBITS_DP64);
        } else {
            // 0 if |x|<0.5, 1 if 0.5<=|x|<1
            result = (exp == -1);
        }
        if ((ux >> 63) != 0) result = -result;
    }
    return result;
}
