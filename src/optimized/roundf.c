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
 *   float roundf(float x)
 *
 * Mathematical Definition:
 *   roundf(x) = sign(x) × floor( |x| + 0.5 )
 *
 *   Where:
 *   - sign(x) = { +1 if x ≥ 0, -1 if x < 0 }
 *   - Ties ( x = n + 0.5 ),  round away from zero
 *
 * Special Values:
 *   roundf(±0)   = ±0
 *   roundf(±∞)   = ±∞
 *   roundf(qNaN) = qNaN (no exception)
 *   roundf(sNaN) = qNaN (raises FE_INVALID)
 *
 ******************************************
 * Implementation Using Bit Operations
 * -----------------------------------
 * IEEE 754 single-precision format:
 *   x = (-1)^s × 2^(e-127) × 1.f
 *   Where: s = sign bit, e = 8-bit exponent, f = 23-bit fraction
 *
 * Algorithm by Value Range:
 * ------------------------
 * Case 1: |x| ≥ 2^23
 *   → x is integer (no fractional bits in representation)
 *   → return x
 *
 * Case 2: 0.5 ≤ |x| < 1.0  (e = 126, exponent = -1)
 *   → roundf(x) = sign(x) × 1.0
 *   → Construct: s | 0x3F800000  (exponent for 1.0)
 *
 * Case 3: |x| < 0.5  (e < 126, exponent < -1)
 *   → roundf(x) = sign(x) × 0.0
 *   → Construct: s | 0x0 (signed zero)
 *
 * Case 4: 1.0 ≤ |x| < 2^23  (0 ≤ exponent < 23)
 *   → Add 0.5 to |x|, then truncate fractional bits
 *   → Operations:
 *     Step 1: ux = ux + (0x00400000 >> exponent)  // Add 0.5 ULP
 *     Step 2: ux = ux & ~(0x007FFFFF >> exponent) // Clear fraction
 *   → Implements: x + sign(x) * 0.5, then mask off (23-exponent) LSBs
 *
 *******************************************
 */

#define INTEGERBITS_SP32        23
#define INF_NAN_SP32            0x7f800000
#define HALF_MANTISSA_BIT_SP32  0x00400000

float ALM_PROTO_OPT(roundf)(float x)
{
    uint32_t ux = asuint32(x);         /* Bit representation of x */
    uint32_t uax = ux & ~SIGNBIT_SP32; /* Absolute value of x */

    /* Fast path: Check for NaN or Infinity using bit pattern */
    if (unlikely(uax >= INF_NAN_SP32)) {
        /* Check if NaN (mantissa != 0) using bit operations - faster than x != x */
        if (uax > INF_NAN_SP32) {
            /* Check if it's a signaling NaN (quiet bit not set) */
            if (!(ux & QNAN_MASK_32)) {
                /* Signaling NaN - raise FE_INVALID and return quiet NaN */
                return __alm_handle_errorf(ux | QNAN_MASK_32, AMD_F_INVALID);
            }
            /* Quiet NaN - return as is, no exception */
            return x;
        }
        /* Infinity case - return as is */
        return x;
    }

    /* Extract biased exponent */
    int32_t intexp = (int32_t)(uax >> EXPSHIFTBITS_SP32) - EXPBIAS_SP32;

    /* Fast path: Already an integer (exp >= 23) or large value */
    if (intexp >= INTEGERBITS_SP32) {
        return x;
    }

    /* Handle small values: |x| < 1.0 */
    if (intexp < 0) {
        /* Extract sign bit, clear magnitude */
        ux = ux & SIGNBIT_SP32;

        /* Special case: 0.5 <= |x| < 1.0 rounds to ±1.0 */
        if (intexp == -1) {
            ux = ux | ONEEXPBITS_SP32;  /* Set exponent for 1.0 */
        }
        /* Otherwise: |x| < 0.5 rounds to ±0.0 (already in ux) */
        return asfloat(ux);
    }

    /* Handle medium values: 1.0 <= |x| < 2^23 */
    /* Calculate fractional bit mask */
    uint32_t mantissa = MANTBITS_SP32 >> intexp;

    /* Check if already integral - avoid unnecessary computation */
    if (unlikely((ux & mantissa) == 0)) {
        return x;
    }

    /* Add 0.5 ULP at the current exponent for rounding */
    ux = ux + (HALF_MANTISSA_BIT_SP32 >> intexp);

    /* Clear fractional bits */
    ux = ux & ~mantissa;

    return asfloat(ux);
}
