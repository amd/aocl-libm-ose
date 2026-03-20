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
#include <libm/typehelper.h>

/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   double round(double x)
 *
 * Mathematical Definition:
 *   round(x) = sign(x) × floor( |x| + 0.5 )
 *
 *   Where:
 *   - sign(x) = { +1 if x ≥ 0, -1 if x < 0 }
 *   - Ties ( x = n + 0.5 ),  round away from zero
 *
 * Special Values:
 *   round(±0)   = ±0
 *   round(±∞)   = ±∞
 *   round(qNaN) = qNaN (no exception)
 *   round(sNaN) = qNaN (raises FE_INVALID)
 *
 ******************************************
 * Implementation Using Bit Operations
 * -----------------------------------
 * IEEE 754 double-precision format:
 *   x = (-1)^s × 2^(e-1023) × 1.f
 *   Where: s = sign bit, e = 11-bit exponent, f = 52-bit fraction
 *
 * Algorithm by Value Range:
 * ------------------------
 * Case 1: |x| ≥ 2^52
 *   → x is integer (no fractional bits in representation)
 *   → return x
 *
 * Case 2: 0.5 ≤ |x| < 1.0  (e = 1022, exponent = -1)
 *   → round(x) = sign(x) × 1.0
 *   → Construct: s | 0x3FF0000000000000  (exponent for 1.0)
 *
 * Case 3: |x| < 0.5  (e < 1022, exponent < -1)
 *   → round(x) = sign(x) × 0.0
 *   → Construct: s | 0x0 (signed zero)
 *
 * Case 4: 1.0 ≤ |x| < 2^52  (0 ≤ exponent < 52)
 *   → Add 0.5 to |x|, then truncate fractional bits
 *   → Operations:
 *     Step 1: ux = ux + (0x0008000000000000 >> exponent)  // Add 0.5 ULP
 *     Step 2: ux = ux & ~(0x000FFFFFFFFFFFFF >> exponent) // Clear fraction
 *   → Implements: x + sign(x) * 0.5, then mask off (52-exponent) LSBs
 *
 *******************************************
 */

#define INTEGERBITS_DP64        52
#define INF_NAN_DP64            0x7ff0000000000000ULL
#define HALF_MANTISSA_BIT_DP64  0x0008000000000000ULL

double ALM_PROTO_OPT(round)(double x)
{
    uint64_t ux = asuint64(x);         /* Bit representation of x */
    uint64_t uax = ux & ~SIGNBIT_DP64; /* Absolute value of x */

    /* Fast path: Check for NaN or Infinity using bit pattern */
    if (unlikely(uax >= INF_NAN_DP64)) {
        /* Check if NaN (mantissa != 0) using bit operations - faster than x != x */
        if (uax > INF_NAN_DP64) {
            /* Check if it's a signaling NaN (quiet bit not set) */
            if (!(ux & QNAN_MASK_64)) {
                /* Signaling NaN - raise FE_INVALID and return quiet NaN */
                return __alm_handle_error(ux | QNAN_MASK_64, AMD_F_INVALID);
            }
            /* Quiet NaN - return as is, no exception */
            return x;
        }
        /* Infinity case - return as is */
        return x;
    }

    /* Extract biased exponent */
    int32_t intexp = (int32_t)(uax >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;

    /* Fast path: Already an integer (exp >= 52) or large value */
    if (intexp >= INTEGERBITS_DP64) {
        return x;
    }

    /* Handle small values: |x| < 1.0 */
    if (intexp < 0) {
        /* Extract sign bit, clear magnitude */
        ux = ux & SIGNBIT_DP64;

        /* Special case: 0.5 <= |x| < 1.0 rounds to ±1.0 */
        if (intexp == -1) {
            ux = ux | ONEEXPBITS_DP64;  /* Set exponent for 1.0 */
        }
        /* Otherwise: |x| < 0.5 rounds to ±0.0 (already in ux) */
        return asdouble(ux);
    }

    /* Handle medium values: 1.0 <= |x| < 2^52 */
    /* Calculate fractional bit mask */
    uint64_t mantissa = MANTBITS_DP64 >> intexp;

    /* Check if already integral - avoid unnecessary computation */
    if (unlikely((ux & mantissa) == 0)) {
        return x;
    }

    /* Add 0.5 ULP at the current exponent for rounding */
    ux = ux + (HALF_MANTISSA_BIT_DP64 >> intexp);

    /* Clear fractional bits */
    ux = ux & ~mantissa;

    return asdouble(ux);
}
