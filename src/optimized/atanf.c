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
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * Signature: float atanf(float y)
 *
 * Algorithm: specialization of atan2f(y, 1.0f) -- see atan2f.c.
 *   With denominator fixed at 1.0, all computation in double for accuracy:
 *   - ratio = yabs  (no division needed for table index)
 *   - r = (yabs - c) / (1.0 + c*yabs)  -- one vdivsd, no overflow possible
 *   - No quadrant adjustment (x = 1.0 > 0 always)
 *   - Sign applied via copysignf(fresult, fy)
 */

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/amd_funcs_internal.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>

#include "atan2_data.h"

float ALM_PROTO_OPT(atanf)(float fy)
{
    uint32_t  uy  = asuint32(fy);
    uint32_t  fay = uy & POS_BITSET_F32;
    uint32_t yexp = fay >> EXPSHIFTBITS_SP32;
    float fresult;

    // Single unlikely gate: catches zero, subnormal, Inf, and NaN for y.
    // yexp-1 wraps to ~0 for zero or subnormal; >= BIASEDEMAX_SP32 for Inf or NaN.
    if (unlikely(yexp - 1 >= BIASEDEMAX_SP32)) {
        if (yexp == 0) {
            // y zero or subnormal: atan(y) ~= y; raises FE_INEXACT for subnormal
            fresult = (fy + 1.0f) * fy;
        } else if ((fay << (32 - EXPSHIFTBITS_SP32)) != 0) {
            // y is NaN; raises FE_INVALID for sNaN
            fresult = fy + fy;
        } else {
            // |y|==Inf: atanf(+/-Inf) = +/-pi/2
            fresult = copysignf((float)atan2_piby2, fy);
            ALM_RAISE_FE_INEXACT();
        }
    } else {
        // ratio = |y| / 1.0 = |y|: table index from |y| bits directly.
        double   yabs = (double)asfloat(fay);
        uint64_t   rb = asuint64(yabs);
        int         b = (int)(rb >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64 + ATAN2_LOG2SIZE - 1;
        double result = 0.0;

        if (likely((unsigned)b < 8)) {
            int k = (int)((rb >> (EXPSHIFTBITS_DP64 - ATAN2_LOG2SIZE)) & ((1u << ATAN2_LOG2SIZE) - 1));
            double tbl_head = atan2_log_table[b * (1 << ATAN2_LOG2SIZE) + k].head;

            // c = 2^(b-4)*(2k+65)/64, exact from bit pattern
            double c = asdouble(((uint64_t)(EXPBIAS_DP64 - ATAN2_LOG2SIZE + 1 + b) << EXPSHIFTBITS_DP64) |
                                ((uint64_t)(2*k + 1) << (EXPSHIFTBITS_DP64 - ATAN2_LOG2SIZE - 1)));

            // r = (|y| - c) / (1 + c*|y|): x=1.0, no overflow possible.
            double r = (yabs - c) / (1.0 + c * yabs);
            double s = r * r;
            result = tbl_head + r * (1.0 + s * (ATAN2_A0 + s * ATAN2_A1));
        } else if (unlikely((unsigned)(b - ATAN2_FALLBACK_EMIN_SP32 - ATAN2_LOG2SIZE + 1) >
                            (unsigned)(ATAN2_FALLBACK_EMAX_SP32 - ATAN2_FALLBACK_EMIN_SP32))) {
            // |y| < 2^-53: atanf(y) rounds to y
            // |y| >= 2^25: atanf(y) rounds to +/-pi/2 in float.
            result = (b < 0) ? yabs : atan2_piby2;
            ALM_RAISE_FE_INEXACT();
        } else {
            // |y| < 1/16 (b < 0): atanf(y) ~ y via polynomial
            // |y| > 16  (b > 7): atanf(y) = pi/2 - atanf(1/y)
            double z = (b >= 0) ? 1.0 / yabs : yabs;
            double s = z * z;
            result = z * (1.0 + s * (ATAN2_B0 + s * (ATAN2_B1 +
                  s * (ATAN2_B2 + s * (ATAN2_B3 + s * (ATAN2_B4 + s * ATAN2_B5))))));
            if (b >= 0) result = atan2_piby2 - result;
        }

        // Round result to float and apply the sign of y
        fresult = copysignf((float)result, fy);
    }

    return fresult;
}
