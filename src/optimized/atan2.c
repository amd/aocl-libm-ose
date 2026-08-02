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
 * Signature: double atan2(double y, double x)
 *
 * Algorithm:
 *   1. Handle special cases (x==0, both-Inf, NaN) in one unlikely branch.
 *      Single-Inf and y==0 with x!=0 are handled correctly by the normal path.
 *   2. Compute ratio = yabs / xabs  (one vdivsd; no sort/swap needed).
 *   3. Extract b = biased_exp(ratio) - 1019, k = top-5 mantissa bits of ratio.
 *      Table covers b in [0,7]: ratio in [1/16, 16).
 *   4. Look up atan(c) = atan2_log_table[b*32+k].{head,tail}.
 *      Reconstruct c = 2^(b-4)*(2k+65)/64 from bit pattern.
 *   5. r = (yabs - c*xabs) / (xabs + c*yabs)  -- one vdivsd, no Dekker split.
 *   6. atan(yabs/xabs) = atan(c) + atan(r)  ~  head + tail + poly(r).
 *   7. Quadrant: if x < 0, result = pi - result  (using head/tail for precision).
 *   8. Sign: result = copysign(result, y).
 *   For ratio outside [1/16,16), a direct single-division fallback is used.
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

double ALM_PROTO_OPT(atan2)(double y, double x)
{
    uint64_t  ux  = asuint64(x);
    uint64_t  uy  = asuint64(y);
    uint64_t  fax = ux & POS_BITSET_DP64;
    uint64_t  fay = uy & POS_BITSET_DP64;
    double result = 0.0;

    // Single unlikely gate: catches zero, Inf, NaN for either argument.
    // fax-1 wraps to ~0 for zero; >= POS_INF_F64-1 for Inf or NaN.
    if (unlikely(((fax - 1) | (fay - 1)) >= (POS_INF_F64 - 1))) {
        if ((fax > POS_INF_F64) || (fay > POS_INF_F64)) {
            result = x + y;  // NaN; also raises FE_INVALID for sNaN
        } else {
            if (fax == 0) {
                // x==0:
                if (fay != 0) {
                    // result = +/-pi/2 for y!=0 and x==0
                    result = atan2_piby2;
                } else if ((ux & SIGNBIT_DP64) != 0) {
                    // result = +/-pi for y==0 and x -0.0
                    result = atan2_pi;
                }   // else result = +/-0 for y==0 and x +0.0 (result=0 above)
            } else if (fax == POS_INF_F64 && fay == POS_INF_F64) {
                // result = +/- pi/4 for y==+/-Inf and x==+Inf
                // result = +/-3pi/4 for y==+/-Inf and x==-Inf
                result = (ux & SIGNBIT_DP64) ? atan2_pi3by4 : atan2_piby4;
            } else {
                // x != 0 && (|x| != Inf || |y| != Inf) handled normally
                goto normal;
            }
            // Apply the sign of y
            result = copysign(result, y);
        }
    } else normal: {
        double xabs = asdouble(fax);
        double yabs = asdouble(fay);
        // Scale both inputs up if both are subnormal.
        if (unlikely(((fax | fay) >> EXPSHIFTBITS_DP64) == 0)) {
            xabs *= 0x1p512;
            yabs *= 0x1p512;
        }

        // ratio = |y|/|x| exactly. TODO: replace with vrcpss-based
        // approximation once the band-boundary problem is solved -- vrcpss
        // gives ~12-bit accuracy which is insufficient to distinguish octave
        // boundaries (e.g. ratio near 1.0 can land in b=3 instead of b=4).
        double ratio = yabs / xabs;
        uint64_t rb = asuint64(ratio);
        int b       = (int)(rb >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64 + ATAN2_LOG2SIZE - 1;  // 0..7 for ratio in [1/16,16)

        if (likely((unsigned) b < 8)) {
            // Main table path: b in [0,7], ratio in [1/16, 16]
            int k  = (int)((rb >> (EXPSHIFTBITS_DP64 - ATAN2_LOG2SIZE)) & ((1u << ATAN2_LOG2SIZE) - 1));
            Atan2LogData tbl = atan2_log_table[b * (1u << ATAN2_LOG2SIZE) + k];

            // c = 2^(b-4)*(2k+65)/64, exact from bit pattern
            double c = asdouble(((uint64_t)(EXPBIAS_DP64 - ATAN2_LOG2SIZE + 1 + b) << EXPSHIFTBITS_DP64) |
                                ((uint64_t)(2*k + 1) << (EXPSHIFTBITS_DP64 - ATAN2_LOG2SIZE - 1)));

            // One vdivsd for the residual. c*xabs may overflow for xabs near DBL_MAX
            // when c > 1 (b >= 4); in that case compute r from ratio directly.
            double cx = c * xabs;
            double r = unlikely(cx >= 0x1p1023) ? (ratio - c) / (1.0 + c * ratio) :
                (yabs - cx) / (xabs + c * yabs);
            double s  = r * r;
            double poly = tbl.tail + r * (1.0 + s * (ATAN2_A0 + s * (ATAN2_A1 + s * ATAN2_A2)));

            // Quadrant: if x < 0, atan2 = pi - atan(|y|/|x|).
            result = (ux & SIGNBIT_DP64) != 0 ? (atan2_pi_head - tbl.head) + (atan2_pi_tail - poly) : tbl.head + poly;
        } else {
            // ratio < 1/16 (b < 0): z = y/x, result = atan(z)
            // ratio > 16  (b > 7): z = x/y, result = pi/2 - atan(z)
            double z = (b >= 0) ? xabs / yabs : yabs / xabs;
            double s = z * z;
            result = z * (1.0 + s * (ATAN2_B0 + s * (ATAN2_B1 +
                  s * (ATAN2_B2 + s * (ATAN2_B3 + s * (ATAN2_B4 + s * ATAN2_B5))))));
            if (b >= 0) result = atan2_piby2 - result;
            if ((ux & SIGNBIT_DP64) != 0) result = atan2_pi - result;
        }

        // Apply the sign of y
        result = copysign(result, y);
    }

    return result;
}
