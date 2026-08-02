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
 *
 * Signature: double atan(double y)
 *
 * Algorithm: specialization of atan2(y, 1.0) -- see atan2.c.
 *   With denominator fixed at 1.0:
 *   - ratio = yabs  (no division needed for table index)
 *   - r = (yabs - c) / (1.0 + c*yabs)  -- one vdivsd, no overflow possible
 *   - No quadrant adjustment (x = 1.0 > 0 always)
 *   - Sign applied via copysign(result, y)
 */

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>

#include "atan2_data.h"

double ALM_PROTO_OPT(atan)(double y)
{
    uint64_t  uy  = asuint64(y);
    uint64_t  fay = uy & POS_BITSET_DP64;
    uint64_t yexp = fay >> EXPSHIFTBITS_DP64;
    double result;

    // Single unlikely gate: catches zero, subnormal, Inf, and NaN for y.
    // yexp-1 wraps to ~0 for zero or subnormal; >= BIASEDEMAX_DP64 for Inf or NaN.
    if (unlikely(yexp - 1 >= BIASEDEMAX_DP64)) {
        if (fay > POS_INF_F64) {
            // y is NaN; raises FE_INVALID for sNaN
            result = y + y;
        } else if (yexp != 0) {
            // |y|==Inf: atan(+/-Inf) = +/-pi/2
            result = copysign(atan2_piby2, y);
        } else {
            // y==0 or y subnormal: atan(y ~= 0) = y
            result = y;
        }
    } else {
        // ratio = |y| / 1.0 = |y|: table index from |y| bits directly.
        double yabs = asdouble(fay);
        uint64_t rb = fay;
        int b = (int)(rb >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64 + ATAN2_LOG2SIZE - 1;

        if (likely((unsigned)b < 8)) {
            int k = (int)((rb >> (EXPSHIFTBITS_DP64 - ATAN2_LOG2SIZE)) & ((1u << ATAN2_LOG2SIZE) - 1));
            Atan2LogData tbl = atan2_log_table[b * (1u << ATAN2_LOG2SIZE) + k];

            // c = 2^(b-4)*(2k+65)/64, exact from bit pattern
            double c = asdouble(((uint64_t)(EXPBIAS_DP64 - ATAN2_LOG2SIZE + 1 + b) << EXPSHIFTBITS_DP64) |
                                ((uint64_t)(2*k + 1) << (EXPSHIFTBITS_DP64 - ATAN2_LOG2SIZE - 1)));

            // r = (|y| - c) / (1 + c*|y|): x=1.0, no overflow possible.
            double r = (yabs - c) / (1.0 + c * yabs);
            double s = r * r;
            double poly = tbl.tail + r * (1.0 + s * (ATAN2_A0 + s * (ATAN2_A1 + s * ATAN2_A2)));
            result = tbl.head + poly;
        } else {
            // |y| < 1/16 (b < 0): atan(y) ~ y via polynomial
            // |y| > 16  (b > 7): atan(y) = pi/2 - atan(1/y)
            double z = (b >= 0) ? 1.0 / yabs : yabs;
            double s = z * z;
            result = z * (1.0 + s * (ATAN2_B0 + s * (ATAN2_B1 +
                  s * (ATAN2_B2 + s * (ATAN2_B3 + s * (ATAN2_B4 + s * ATAN2_B5))))));
            if (b >= 0) result = atan2_piby2 - result;
        }

        // Apply the sign of y
        result = copysign(result, y);
    }

    return result;
}
