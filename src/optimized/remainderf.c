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
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/******************************************
 * Implementation Notes:
 *
 * Prototype:
 * float remainderf(float x, float y)
 *
 * Algorithm:
 * remainderf(x, y) = x - n*y, where n = roundTiesToEven(x/y).
 *
 * This is identical to the fmodf integer algorithm except that fmodf uses
 * n = trunc(x/y) while remainderf uses n = roundTiesToEven(x/y).
 *
 * Main path (|x| >= |y|):
 *   Compute rem = Mx * 2^shift mod My using 64-bit integer division (same
 *   as fmodf).  The quotient from each step is added into qSum; the LSB
 *   of qSum gives the parity of the total n.  After the reduction:
 *     2*rem < My: keep rem; sign(result) = sign(x).
 *     2*rem > My: use rem = My-rem (round up); sign(result) = -sign(x).
 *     2*rem == My: round up iff n is odd (tie-to-even); tracked via qSum.
 *   MAXSHIFT = 64 - MANTLENGTH_SP32 = 40 (same as fmodf).
 *
 * Small-x path (0 < |x| < |y|):
 *   n is 0 or 1.  The comparison 2|x| vs |y| is done in float arithmetic,
 *   which is exact (doubling is exact for finite non-max floats, and the
 *   Sterbenz subtraction |y|-|x| is exact when |y|/2 < |x| < |y|).
 *
 */

#include <stdint.h>
#include <math.h>

#include "libm_macros.h"
#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>

#if defined(__GNUC__) || defined(__clang__)

#define CLZ32(x) __builtin_clz(x)

#elif defined(_MSC_VER)

#include <intrin.h>
static inline int alm_clz32(uint32_t x)
{
    unsigned long idx;
    _BitScanReverse(&idx, x);
    return 31 - (int)idx;
}
#define CLZ32(x) alm_clz32(x)

#else

#error "Intrinsic for counting leading zeroes not found"

#endif

typedef struct
{
    uint32_t m;  // 24-bit significand (including implicit 1)
    int e;       // biased exponent
} F32ExpMan;

// Extract a single precision value into a mantissa and biased exponent
// Handles subnormal values by shifting value and adjusting biased exponent
static inline F32ExpMan F32Extract(uint32_t fax)
{
    int lz;
    return unlikely(fax < POS_LNORMAL_F32) ?
        lz = CLZ32(fax),
        (F32ExpMan) {
            .m = fax << (lz - (32 - MANTLENGTH_SP32)),
            .e = (32 - MANTLENGTH_SP32 + 1) - lz
        } :
        (F32ExpMan) {
            .m = (fax & MANTBITS_SP32) | IMPBIT_SP32,
            .e = (int)(fax >> EXPSHIFTBITS_SP32)
        };
}

typedef struct
{
    uint32_t quot;
    uint32_t rem;
} QuotRem32;

/* (Mx << d) < 2^64 for d <= 40 since Mx < 2^24; remainder fits in uint32_t */
#define MAXSHIFT (64 - MANTLENGTH_SP32)  // 40

/* Rem64P: Returns the { quotient, remainder }.
   The sum of quot across all steps gives the parity of n. */
static inline QuotRem32 Rem64P(uint32_t Mx, uint32_t My, int d)
{
    uint64_t dividend = (uint64_t)Mx << d;
    return (QuotRem32) {
        .quot = (uint32_t)(dividend / My),
        .rem = (uint32_t)(dividend % My)
    };
}

float ALM_PROTO_OPT(remainderf)(float x, float y)
{
    uint32_t fax = asuint32(x) & POS_BITSET_F32;  // |x| bit pattern
    uint32_t fay = asuint32(y) & POS_BITSET_F32;  // |y| bit pattern
    float result = x;  // Default result value = x; saves sign bit for later

    // All error conditions are caught by one predicted-untaken branch
    if (unlikely(((fay - 1) | fax) >= POS_INF_F32))
    {
        if (fay > POS_INF_F32)
        {   // |y| NaN: raise FE_INVALID if either operand is sNaN
            result = x * y;
        }
        else if (fax > POS_INF_F32)
        {   // |x| NaN: propagate x; raise FE_INVALID if x is sNaN
            result = x + x;
        }
        else if ((fax == POS_INF_F32) || (fay == 0u))
        {   // |x| == Inf || y == 0
            result = __alm_handle_errorf(INDEFBITPATT_SP32, AMD_F_INVALID);
        }
        else
        {   // False positive ((fay-1) | fax) >= POS_INF_F32; continue normally
            goto noerror;
        }
    }
    else noerror: if (likely(fax >= fay))
    {   // |x| >= |y|
        F32ExpMan fpx = F32Extract(fax);
        F32ExpMan fpy = F32Extract(fay);
        int shift = fpx.e - fpy.e;
        uint32_t qSum = 0;
        QuotRem32 qr = { .rem = fpx.m };

        while (unlikely(shift > MAXSHIFT)) {
            qr = Rem64P(qr.rem, fpy.m, MAXSHIFT);
            qSum += qr.quot;
            shift -= MAXSHIFT;
        }

        // Compute (x * 2^shift) mod y
        qr = Rem64P(qr.rem, fpy.m, shift);

        // Nearest-even rounding: round up if 2*rem > My, or on a tie (2*rem == My)
        // when n is odd. Sum of all quotient LSBs gives parity of the total n.
        if ((qr.rem*2 > fpy.m) || ((qr.rem*2 == fpy.m) && (((qSum + qr.quot) & 1) != 0)))
        {
            qr.rem = fpy.m - qr.rem;
            result = -result;
        }

        if (likely(qr.rem != 0)) {
            // k = number of bits to shift rem left to position implied bit
            int k = CLZ32(qr.rem) - (32 - MANTLENGTH_SP32);

            // k < fpy.e for normals, where fpy.e - k is biased result exponent
            // For k >= fpy.e, result is subnormal and shifted in mantissa bits
            qr.rem = (k < fpy.e) ? ((uint32_t)(fpy.e - k) << EXPSHIFTBITS_SP32)
                | ((qr.rem << k) & MANTBITS_SP32) :
                likely(fpy.e > 0) ? qr.rem << (fpy.e - 1) : qr.rem >> (1 - fpy.e);
        }

        // Result is same sign as original x with exponent and mantissa in rem
        result = asfloat(qr.rem | (asuint32(result) & SIGNBIT_SP32));
    }
    else if (fax < POS_LNORMAL_F32 ? fay < fax * 2 :
             fay < POS_INF_F32 && fay < fax + POS_LNORMAL_F32)
    {
        // 2*|x| > |y|
        // n=1: |result| = |y|-|x|, sign = -sign(x)
        // exact by Sterbenz (adx > ady/2)
        result = asfloat(asuint32(asfloat(fay) - asfloat(fax))
                         | (~asuint32(result) & SIGNBIT_SP32));
    }
    // else 2|x| <= |y|: n=0 (includes tie 2|x|==|y|: rounds to 0), result=x
    // |y| == Inf also lands here (2|x| is finite, never > Inf), returning x.

    return result;
}
