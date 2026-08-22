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
 * double remainder(double x, double y)
 *
 * Algorithm:
 * remainder(x, y) = x - n*y, where n = roundTiesToEven(x/y).
 *
 * This is identical to the fmod integer algorithm except that fmod uses
 * n = trunc(x/y) while remainder uses n = roundTiesToEven(x/y).
 *
 * Main path (|x| >= |y|):
 *   Compute rem = Mx * 2^shift mod My via Rem128P (same as fmod), but
 *   Rem128P also returns the integer quotient.  The sum of all quotients
 *   across all reduction steps gives the parity of n.
 *   After the reduction:
 *     2*rem < My: keep rem; sign(result) = sign(x).
 *     2*rem > My: use rem = My-rem (round up); sign(result) = -sign(x).
 *     2*rem == My: round up iff n is odd (tie-to-even); tracked via n_quot.
 *   MAXSHIFT is 63 for divq/_udiv128 and 75 for __uint128_t (same as fmod).
 *
 * Small-x path (0 < |x| < |y|):
 *   n is 0 or 1.  The comparison 2|x| vs |y| is done in float arithmetic,
 *   which is exact here: doubling is exact for finite non-max doubles, and
 *   the Sterbenz subtraction |y|-|x| is exact when |y|/2 < |x| < |y|.
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

#define CLZ64(x) __builtin_clzll(x)

#elif defined(_MSC_VER)

#include <intrin.h>
static inline int alm_clz64(uint64_t x)
{
    unsigned long idx;
    _BitScanReverse64(&idx, x);
    return 63 - (int)idx;
}
#define CLZ64(x) alm_clz64(x)

#else

#error "Intrinsic for counting leading zeroes not found"

#endif

typedef struct
{
    uint64_t m;  // 53-bit significand (including implicit 1)
    int e;       // biased exponent
} F64ExpMan;

// Extract a double precision value into a mantissa and biased exponent
// Handles subnormal values by shifting value and adjusting biased exponent
static inline F64ExpMan F64Extract(uint64_t fax)
{
    int lz;
    return unlikely(fax < POS_LNORMAL_F64) ?
        lz = CLZ64(fax),
        (F64ExpMan) {  // Subnormal values; shift leftmost 1 into implied bit
            .m = fax << (lz - (64 - MANTLENGTH_DP64)),
            .e = (64 - MANTLENGTH_DP64) + 1 - lz
        } :
        (F64ExpMan) {  // Normal values
            .m = (fax & MANTBITS_DP64) | IMPBIT_DP64,
            .e = (int)(fax >> EXPSHIFTBITS_DP64)
        };
}

typedef struct
{
    uint64_t quot;
    uint64_t rem;
} QuotRem64;

/* Rem128P: same as Rem128 but also returns the quotient.
   The sum of quot across all steps gives the parity of n. */
#if defined(__x86_64__) && (defined(__GNUC__) || defined(__clang__))

/* hi = Mx >> (64-d) < 2^52 <= My for d in [0,63] when My has implicit bit */
#define MAXSHIFT 63

static inline QuotRem64 Rem128P(uint64_t Mx, uint64_t My, int d)
{
    QuotRem64 qr;
    uint64_t hi = (d != 0) ? Mx >> (64 - d) : 0;
    uint64_t lo = Mx << d;
    __asm__ volatile("divq %[divisor]"
            : "=a"(qr.quot), "=d"(qr.rem)
            : "0"(lo), "1"(hi), [divisor] "r"(My)
            : "cc");
    return qr;
}

#elif defined(__SIZEOF_INT128__)

/* (Mx << d) < 2^128 for d <= 75 since Mx < 2^53; remainder fits in uint64_t */
#define MAXSHIFT (128 - MANTLENGTH_DP64)

static inline QuotRem64 Rem128P(uint64_t Mx, uint64_t My, int d)
{
    __uint128_t dividend = (__uint128_t)Mx << d;
    return (QuotRem64) {
        .quot = (uint64_t)(dividend / My),
        .rem = (uint64_t)(dividend % My)
    };
}

#elif defined(_MSC_VER) && defined(_M_X64)

/* hi = Mx >> (64-d) < 2^52 <= My for d in [0,63] when My has implicit bit.
 * Note: _udiv128 is unresolved at link time with clang-cl + lld-link due to
 * a known open bug (https://github.com/llvm/llvm-project/issues/59168).
 * This branch is only reachable with MSVC link.exe. */
#define MAXSHIFT 63

#include <intrin.h>
static inline QuotRem64 Rem128P(uint64_t Mx, uint64_t My, int d)
{
    QuotRem64 qr;
    uint64_t hi = (d != 0) ? Mx >> (64 - d) : 0;
    uint64_t lo = Mx << d;
    qr.quot = _udiv128(hi, lo, My, &qr.rem);
    return qr;
}

#else

#error "128-bit integer division not available"

#endif

double ALM_PROTO_OPT(remainder)(double x, double y)
{
    uint64_t fax = asuint64(x) & POS_BITSET_DP64;
    uint64_t fay = asuint64(y) & POS_BITSET_DP64;
    double result = x;

    // Single predicted-untaken branch for all error cases
    // All cases of x NaN, Inf and all cases of y Zero, NaN, Inf
    if (unlikely(((fay - 1) | fax) >= POS_INF_F64))
    {
        if (fay > POS_INF_F64)
        {   // |y| NaN: raise FE_INVALID if either operand is sNaN
            result = x * y;
        }
        else if (fax > POS_INF_F64)
        {   // |x| NaN: propagate NaN; raise FE_INVALID if x is sNaN
            result = x + x;
        }
        else if ((fax == POS_INF_F64) || (fay == 0))
        {   // |x| == Inf || y == 0
            ALM_RAISE_FE_INVALID(); // Raise FE_INVALID exception
            result = asdouble(INDEFBITPATT_DP64); // Return Indefinite NaN
        }
        else
        {   // False positive ((fay-1) | fax) >= POS_INF_F64; continue normally
            // goto may be considered harmful, but this is much simpler and
            // faster than the alternatives, and mimics assembly code branch
            // optimization.
            goto noerror;
        }
    }
    else noerror: if (likely(fax >= fay))
    {   // |x| >= |y|
        int xe = (int)(fax >> EXPSHIFTBITS_DP64);
        int ye = (int)(fay >> EXPSHIFTBITS_DP64);
        int shift = xe - ye;
        uint64_t qSum = 0;
        QuotRem64 qr;
        F64ExpMan fpy;

        // The (xe != 0) test is logically redundant since fax >= fay,
        // so (ye != 0) implies (xe != 0). But Clang fuses consecutive
        // side-effect-free equality tests into parallelizable setX
        // instructions, and if you remove (xe != 0), it falls back on
        // using multiple branches instead, and loses 11 Mcalls/sec.
        if (likely((ye != 0) && (xe != 0) && (shift <= MAXSHIFT)))
        {
            // Fast path: both normal, small shift
            qr.rem = (fax & MANTBITS_DP64) | IMPBIT_DP64;
            fpy = (F64ExpMan) { .m = (fay & MANTBITS_DP64) | IMPBIT_DP64, .e = ye };
        }
        else
        {
            // Slow path: subnormals or large shift
            F64ExpMan fpx = F64Extract(fax);
            fpy = F64Extract(fay);
            qr.rem = fpx.m;
            shift = fpx.e - fpy.e;
            while (shift > MAXSHIFT)
            {
                qr = Rem128P(qr.rem, fpy.m, MAXSHIFT);
                qSum += qr.quot;
                shift -= MAXSHIFT;
            }
        }

        // Compute (x * 2^shift) mod y
        qr = Rem128P(qr.rem, fpy.m, shift);

        // Nearest-even rounding: round up if 2*rem > My, or on a tie (2*rem == My)
        // when n is odd. Sum of all quotient LSBs gives parity of the total n.
        if (qr.rem*2 > fpy.m || ((qr.rem*2 == fpy.m) && (((qSum + qr.quot) & 1) != 0)))
        {
            qr.rem = fpy.m - qr.rem;
            result = -result;
        }

        if (likely(qr.rem != 0))
        {
            // k = number of bits to shift rem left to position implied bit
            int k = CLZ64(qr.rem) - (64 - MANTLENGTH_DP64);

            // k < fpy.e for normals, where fpy.e - k is biased result exponent
            // For k >= fpy.e, result is subnormal and shifted in mantissa bits
            qr.rem = (k < fpy.e) ? ((uint64_t)(fpy.e - k) << EXPSHIFTBITS_DP64)
                | ((qr.rem << k) & MANTBITS_DP64) :
                likely(fpy.e > 0) ? qr.rem << (fpy.e - 1) : qr.rem >> (1 - fpy.e);
        }

        // Result is same sign as original x with exponent and mantissa in rem
        result = asdouble(qr.rem | (asuint64(result) & SIGNBIT_DP64));
    }
    else if (fax < POS_LNORMAL_F64 ? fay < fax * 2 :
             fay < POS_INF_F64 && fay < fax + POS_LNORMAL_F64)
    {
        // 2*|x| > |y|
        // n=1: |result| = |y|-|x|, sign = -sign(x)
        // exact by Sterbenz (adx > ady/2)
        result = asdouble(asuint64(asdouble(fay) - asdouble(fax))
                          | (~asuint64(result) & SIGNBIT_DP64));
    }
    // else 2|x| <= |y|: n=0 (includes tie 2|x|==|y|: rounds to 0); result=x
    // |y| == Inf also lands here (2|x| is finite, never > Inf), returning x.

    return result;
}
