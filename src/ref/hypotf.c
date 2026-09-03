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
#include "libm_inlines_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>

// Returns sqrt(x*x + y*y) with no overflow or underflow unless the result warrants it
float ALM_PROTO_REF(hypotf)(float x, float y)
{
    uint32_t ux = asuint32(x) & POS_BITSET_F32;
    uint32_t uy = asuint32(y) & POS_BITSET_F32;
    float result;

    // If x or y is +/-Zero, +/-Inf, NaN
    // ux - 1 and uy - 1 wrap Zero around so >= POS_INF_F32 - 1
    if (unlikely((ux - 1 >= POS_INF_F32 - 1) || (uy - 1 >= POS_INF_F32 - 1))) {
        // Compute result = |x| + |y|
        // If x or y is sNaN, raise FE_INVALID and return qNaN
        result = asfloat(ux) + asfloat(uy);

        // Keep result calculation alive so FE_INVALID is generated for sNaN
        // even if result is replaced below for hypot(sNaN, Inf) = Inf
        ALM_KEEP_ALIVE_SP32(result);

        // If |x| == Inf || |y| == Inf, return Inf; hypotf(Inf, NaN) = Inf.
        if (ux == POS_INF_F32 || uy == POS_INF_F32) {
            result = asfloat(POS_INF_F32);
        }
    } else {
        // Compute result in double intermediate precision
        result = (float)sqrt((double)x * (double)x + (double)y * (double)y);
    }

    return result;
}
