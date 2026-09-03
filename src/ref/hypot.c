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
#include <limits.h>
#include <stdbool.h>

// Avoid FMA contraction so that error correction works
ALM_FP_CONTRACT_OFF

// Returns sqrt(x*x + y*y) with no overflow or underflow unless the result warrants it
double ALM_PROTO_REF(hypot)(double x, double y)
{
    uint64_t ux = asuint64(x) & POS_BITSET_DP64;
    uint64_t uy = asuint64(y) & POS_BITSET_DP64;
    double result;

    // If x or y is +/-Zero, +/-Inf, NaN
    // ux - 1 and uy - 1 wrap Zero around so >= POS_INF_F64 - 1
    if (unlikely((ux - 1 >= POS_INF_F64 - 1) || (uy - 1 >= POS_INF_F64 - 1))) {
        // Compute result = |x| + |y|
        // If x or y is sNaN, raise FE_INVALID and return qNaN
        result = asdouble(ux) + asdouble(uy);

        // Keep result calculation alive so FE_INVALID is generated for sNaN
        // even if result is replaced below for hypot(sNaN, Inf) = Inf
        ALM_KEEP_ALIVE_DP64(result);

        // If |x| == Inf || |y| == Inf, return Inf; hypot(Inf, NaN) = Inf.
        if (ux == POS_INF_F64 || uy == POS_INF_F64) {
            result = asdouble(POS_INF_F64);
        }
    } else {
        // Make |x| >= |y|
        if (ux < uy) {
            uint64_t t = ux;
            ux = uy;
            uy = t;
        }

        // Biased exponents
        uint64_t xe = ux >> EXPSHIFTBITS_DP64;
        uint64_t ye = uy >> EXPSHIFTBITS_DP64;

        // Whether y is subnormal. If x is subnormal, then y is subnormal.
        bool subnormal = (ye == 0);

        // Return |x| if |x| > 2^(EMIN+52) and y subnormal, preventing overflow
        // in the ux calculation below and short-circuiting because |y| is small
        if (unlikely(subnormal && (xe > BIASEDEMIN_DP64 + EXPSHIFTBITS_DP64))) {
            // |x|+|y| raises FE_INEXACT but returns |x|
            result = asdouble(ux) + asdouble(uy);
        } else {
            if (unlikely(subnormal)) {
                // y is subnormal

                // Scale x and y by 2^52 to normalize y (and possibly x)
                ux = asuint64(asdouble(ux) * 0x1p52);
                uy = asuint64(asdouble(uy) * 0x1p52);
                xe = ux >> EXPSHIFTBITS_DP64;
                ye = uy >> EXPSHIFTBITS_DP64;
            }

            // At this point, x and y are normal and nonzero
            uint64_t expdiff = xe - ye;  // |x| >= |y|, so xe >= ye

            // Return |x| if |x| > |y| * 2^52, preventing underflow in yscale
            if (unlikely(expdiff > EXPSHIFTBITS_DP64)) {
                // |x|+|y| raises FE_INEXACT but returns |x|
                result = asdouble(ux) + asdouble(uy);
            } else {
                // xscale is x scaled to [1,2)
                double xscale = asdouble((ux & MANTBITS_DP64) | ONEEXPBITS_DP64);

                // yscale is y scaled down 2^-expdiff
                double yscale = asdouble((uy & MANTBITS_DP64) | (ONEEXPBITS_DP64 -
                                                                 (expdiff << EXPSHIFTBITS_DP64)));

                // Compute xscale^2 + yscale^2 as double-double (sumHi, sumLo).
                // #pragma float_control(precise, on) ensures xscale * xscale
                // and yscale * yscale are rounded, so fma(xyscale, xyscale,
                // -xyscale2) is exact.
                double xscale2 = xscale * xscale;
                double yscale2 = yscale * yscale;
                double sumHi   = xscale2 + yscale2;
                double sumLo   = (xscale2 - sumHi) + yscale2
                    + fma(xscale, xscale, -xscale2)
                    + fma(yscale, yscale, -yscale2);
                double approx  = sqrt(sumHi);
                double error   = sumLo + fma(-approx, approx, sumHi);

                // Newton correction step
                result = fma(error / approx, 0.5, approx);

                // Scale the result by 2^(xe - bias)
                result *= asdouble(xe << EXPSHIFTBITS_DP64);
            }

            // If |x| and |y| were scaled by 2^52 for subnormal, scale by 2^-52
            if (unlikely(subnormal)) {
                result *= 0x1p-52;
                if (result < DBL_MIN) {
                    // Raise FE_UNDERFLOW for subnormal result (we assume that
                    // the result is inexact even if, in rare cases, it is
                    // exact; testing for result exactness is expensive).
                    ALM_RAISE_FE_UNDERFLOW();
                }
            }
        }
    }

    return result;
}
