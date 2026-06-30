/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
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
 */

#ifndef LIBM_REMAINDER_PIBY2_F_H
#define LIBM_REMAINDER_PIBY2_F_H

/*
 * Payne-Hanek reduction of a float modulo pi/2 (large-argument path).
 *
 *     |x| = k * (pi/2) + r,    region = k & 3,    |r| <~ pi/4.
 *
 * Inputs / outputs:
 *   - input  : the magnitude bit-pattern of x.
 *   - outputs: region = k & 3 and the reduced residual r for |x|; the input
 *              sign is NOT applied here (the caller folds it once after the
 *              quadrant select).
 *
 * Method: operate on the integer fields of x.  Multiply the 24-bit significand
 * by a 64-bit window of the 2/pi bit stream; the bits above the pi/2 unit give
 * the quadrant index, and the fraction below is scaled by pi/2.  The residual r
 * is reconstructed from two fraction words (head + leftover) with one FMA,
 * giving near-full-double accuracy.
 */

#include <stdint.h>

/*
 * Explicit fma primitive (allows keeping rounding errors in check)
 */
#if !defined(_AMD_FMA)
#if defined(__GNUC__) || defined(__clang__)
#  define _AMD_FMA(a, b, c)  __builtin_fma((a), (b), (c))
#else
#  include <math.h>
#  define _AMD_FMA(a, b, c)  fma((a), (b), (c))
#endif
#endif

/* 2/pi byte stream; a few extra trailing bytes keep a single 8-byte window
 * read valid for every float exponent. */
static const uint8_t two_by_pi_tail_P2F[40] = {
     65, 14,241,100, 86,138,109,  3,119,211,212, 71, 95,157,240,167,
     84, 16, 57,185, 13,230,139,  2,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0
};

/* pi/2 * 2^(i-54), indexed by (xexp & 7); scales the leading fraction. */
static const double sph_P2F[8] = {
    0x1.921fb54442d18p-54, 0x1.921fb54442d18p-53, 0x1.921fb54442d18p-52,
    0x1.921fb54442d18p-51, 0x1.921fb54442d18p-50, 0x1.921fb54442d18p-49,
    0x1.921fb54442d18p-48, 0x1.921fb54442d18p-47
};
/* pi/2 * 2^(i-118), indexed by (xexp & 7); scales the leftover fraction word. */
static const double slp_P2F[8] = {
    0x1.921fb54442d18p-118, 0x1.921fb54442d18p-117, 0x1.921fb54442d18p-116,
    0x1.921fb54442d18p-115, 0x1.921fb54442d18p-114, 0x1.921fb54442d18p-113,
    0x1.921fb54442d18p-112, 0x1.921fb54442d18p-111
};

static inline void
remainder_piby2_f(uint32_t axf, int *region, double *r)
{
    int      xexp = (int)(axf >> 23) - 127;
    uint64_t mant = (uint64_t)((axf & 0x7FFFFFu) | 0x800000u) << 29;

    /* significand * two 64-bit 2/pi windows -> head + leftover fraction words */
    int      idx = 15 - (xexp >> 3);
    uint64_t w1, w2;
    __builtin_memcpy(&w1, &two_by_pi_tail_P2F[idx],     8);
    __builtin_memcpy(&w2, &two_by_pi_tail_P2F[idx + 8], 8);

    __uint128_t p1 = (__uint128_t)mant * w1;
    __uint128_t p2 = (__uint128_t)mant * w2 + (uint64_t)(p1 >> 64);
    uint64_t    res0 = (uint64_t)p2;
    uint64_t    res1 = (uint64_t)p1;

    int      resexp = xexp & 7;
    int      shift  = 54 - resexp;            /* pi/2 unit bit position */
    uint64_t ltb    = res0 >> shift;
    uint64_t pf     = (res0 >> (shift - 1)) & 1ULL;        /* round-up carry */

    uint64_t neg_mask = -pf;
    res0 ^= neg_mask;                         /* |1 - frac| on round-up */
    res1 ^= neg_mask;

    int ib = resexp + 10;
    res0 = (res0 << ib) >> ib;                /* fraction of one pi/2 segment */

    /* r = res0 * (pi/2 * 2^(i-54)) + res1 * (pi/2 * 2^(i-118)) */
    double rh = _AMD_FMA((double)res1, slp_P2F[resexp],
                    (double)res0 * sph_P2F[resexp]);
    if (pf) rh = -rh;

    *r      = rh;
    *region = (int)((ltb + pf) & 3ULL);
}

#endif /* LIBM_REMAINDER_PIBY2_F_H */
