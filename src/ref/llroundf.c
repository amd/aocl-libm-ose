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

llint_t ALM_PROTO_REF(llroundf)(float x)
{
    uint32_t ux = asuint32(x);
    int     exp = (int)((ux >> EXPSHIFTBITS_SP32) &
                        (EXPBITS_SP32 >> EXPSHIFTBITS_SP32)) - EXPBIAS_SP32;
    llint_t result;

    if (unlikely(exp >= MAXEXP)) {
        // Cold path: |x| too large for 'long long', or x is NaN / +-Inf.
        // The result is unspecified and FE_INVALID is raised -- *except*
        // negative inputs that still round to exactly -2^(w-1) == LLONG_MIN,
        // which are in range and must not raise.
        if (unlikely((ux ^ SIGNBIT_SP32) >
                     ((uint32_t)(EXPBIAS_SP32 + MAXEXP) << EXPSHIFTBITS_SP32))) {
            ALM_RAISE_FE_INVALID();
        }
        result = LLONG_MIN;
    } else {
        if (likely((unsigned)exp <= 64 - MANTLENGTH_SP32)) {
            uint64_t mant = (uint64_t)((ux & MANTBITS_SP32) | IMPBIT_SP32);
            // Unified rounding: (mant<<exp) fits in uint64_t, add 0.5 ULP then truncate
            result = (llint_t)(((mant << exp) + QNAN_MASK_32) >> EXPSHIFTBITS_SP32);
        } else if (likely(exp >= 0)) {
            uint64_t mant = (uint64_t)((ux & MANTBITS_SP32) | IMPBIT_SP32);
            // |x| >= 2^41: already an exact integer, shift left only
            result = (llint_t)(mant << (exp - EXPSHIFTBITS_SP32));
        } else {
            //  0 if |x|<0.5, 1 if 0.5<=|x|<1
            result = (exp == -1);
        }

        if ((ux >> 31) != 0) result = -result;
    }
    return result;
}
