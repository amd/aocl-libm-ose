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
 * Signature: float atan2f(float y, float x)
 *
 * Algorithm: identical structure to atan2() but with float inputs/output
 *   and double intermediates throughout (float needs only 24 bits; double
 *   delivers 53, so no head/tail splitting is needed).
 *
 *   1. Handle special cases (x==0, both-Inf, NaN) in one unlikely branch.
 *      Single-Inf and y==0 with x!=0 are handled correctly by the normal path.
 *   2. Promote yabs, xabs to double.  Compute ratio = yabs/xabs in double.
 *   3. Extract b = biased_exp(ratio) - 1019, k = top-5 mantissa bits of ratio.
 *      Table covers b in [0,7]: ratio in [1/16, 16).
 *   4. Look up tbl_head = atan2_log_table[b*32+k].head  (tail negligible for float).
 *      Reconstruct c from bit pattern.
 *   5. r = (yabs - c*xabs) / (xabs + c*yabs)  -- one vdivsd, no Dekker split.
 *   6. atan(yabs/xabs) = tbl_head + r + r*s*(A0 + s*A1),  s = r*r.
 *   7. Quadrant: if x < 0, result = pi - result.
 *   8. Sign: fresult = copysignf((float)result, fy).
 *   For ratio outside [1/16, 16), a direct single-division fallback is used.
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

float ALM_PROTO_OPT(atan2f)(float fy, float fx)
{
    uint32_t  ux  = asuint32(fx);
    uint32_t  uy  = asuint32(fy);
    uint32_t  fax = ux & POS_BITSET_F32;
    uint32_t  fay = uy & POS_BITSET_F32;
    float fresult = 0.0f;

    // Single unlikely gate: catches zero, Inf, NaN for either argument.
    // fax-1 wraps to ~0 for zero; >= POS_INF_F32-1 for Inf or NaN.
    if (unlikely(((fax - 1) | (fay - 1)) >= (POS_INF_F32 - 1))) {
        if (fax > POS_INF_F32 || fay > POS_INF_F32) {
            fresult = fx + fy;  // NaN; also raises FE_INVALID for sNaN
        } else {
            if (fax == 0) {
                // x==0:
                if (fay != 0) {
                    // result = +/-pi/2 for y!=0 and x==0
                    fresult = (float)atan2_piby2;
                } else if ((ux & SIGNBIT_SP32) != 0) {
                    // result = +/-pi for y==0 and x -0.0
                    fresult = (float)atan2_pi;
                }   // else result = +/-0 for y==0 and x +0.0 (result=0 above)
            } else if (fax == POS_INF_F32 && fay == POS_INF_F32) {
                // result = +/- pi/4 for y==+/-Inf and x==+Inf
                // result = +/-3pi/4 for y==+/-Inf and x==-Inf
                fresult = (ux & SIGNBIT_SP32) != 0 ? (float)atan2_pi3by4 : (float)atan2_piby4;
            } else {
                // x != 0 && (|x| != Inf || |y| != Inf) handled normally
                goto normal;
            }
            // Apply the sign of y
            fresult = copysignf(fresult, fy);
        }
    } else normal: {
        double result = 0.0;
        double xabs = (double)asfloat(fax);
        double yabs = (double)asfloat(fay);
        double ratio = yabs / xabs;
        uint64_t rb  = asuint64(ratio);
        int b = (int)(rb >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64 + ATAN2_LOG2SIZE - 1;

        if (likely((unsigned)b < 8)) {
            int k = (int)((rb >> (EXPSHIFTBITS_DP64 - ATAN2_LOG2SIZE)) & ((1u << ATAN2_LOG2SIZE) - 1));
            double tbl_head = atan2_log_table[b * (1u << ATAN2_LOG2SIZE) + k].head;

            // c = 2^(b-4)*(2k+65)/64, exact from bit pattern
            double c = asdouble(((uint64_t)(EXPBIAS_DP64 - ATAN2_LOG2SIZE + 1 + b) << EXPSHIFTBITS_DP64) |
                                ((uint64_t)(2*k + 1) << (EXPSHIFTBITS_DP64 - ATAN2_LOG2SIZE - 1)));

            double r = (yabs - c * xabs) / (xabs + c * yabs);
            double s  = r * r;
            result = tbl_head + r * (1.0 + s * (ATAN2_A0 + s * ATAN2_A1));
        } else {
            // ratio < 1/16 (b < 0): z = y/x, result = atan(z)
            // ratio > 16  (b > 7): z = x/y, result = pi/2 - atan(z)
            double z = (b >= 0) ? xabs / yabs : yabs / xabs;
            double s = z * z;
            result = z * (1.0 + s * (ATAN2_B0 + s * (ATAN2_B1 +
                  s * (ATAN2_B2 + s * (ATAN2_B3 + s * (ATAN2_B4 + s * ATAN2_B5))))));
            if (b >= 0) result = atan2_piby2 - result;
        }

        if ((ux & SIGNBIT_SP32) != 0) result = atan2_pi - result;

        // Round result to float and apply the sign of y
        fresult = copysignf((float)result, fy);
    }

    return fresult;
}
