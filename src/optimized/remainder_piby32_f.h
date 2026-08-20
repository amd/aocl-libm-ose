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

#ifndef LIBM_REMAINDER_PIBY32_F_H
#define LIBM_REMAINDER_PIBY32_F_H

/*
 * Float Payne-Hanek reduction: |x| = m*(pi/32) + r, m in [0,31], |r| <~ pi/64.
 * Single-double residual (no low part) -- enough for a float result.
 */

#include <stdint.h>

#ifndef LIBM_TWO_BY_PI_TAIL_P32F_DEFINED
#define LIBM_TWO_BY_PI_TAIL_P32F_DEFINED
static const uint8_t two_by_pi_tail_P32F[40] = {
     65, 14,241,100, 86,138,109,  3,119,211,212, 71, 95,157,240,167,
     84, 16, 57,185, 13,230,139,  2,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0
};
#endif

/* pi/32 * 2^(i-50), indexed by (xexp & 7): leading fraction-word scale. */
static const double sph_P32F[8] = {
    0x1.921fb54442d18p-54, 0x1.921fb54442d18p-53, 0x1.921fb54442d18p-52,
    0x1.921fb54442d18p-51, 0x1.921fb54442d18p-50, 0x1.921fb54442d18p-49,
    0x1.921fb54442d18p-48, 0x1.921fb54442d18p-47
};
/* pi/32 * 2^(i-114), indexed by (xexp & 7): leftover fraction-word scale. */
static const double slp_P32F[8] = {
    0x1.921fb54442d18p-118, 0x1.921fb54442d18p-117, 0x1.921fb54442d18p-116,
    0x1.921fb54442d18p-115, 0x1.921fb54442d18p-114, 0x1.921fb54442d18p-113,
    0x1.921fb54442d18p-112, 0x1.921fb54442d18p-111
};

static inline void
remainder_piby32_f(uint32_t axf, int *m_out, double *r)
{
    int      xexp = (int)(axf >> 23) - 127;
    uint64_t mant = (uint64_t)((axf & 0x7FFFFFu) | 0x800000u) << 29;

    int      idx = 15 - (xexp >> 3);
    uint64_t w1, w2;
    __builtin_memcpy(&w1, &two_by_pi_tail_P32F[idx],     8);
    __builtin_memcpy(&w2, &two_by_pi_tail_P32F[idx + 8], 8);

    __uint128_t p1 = (__uint128_t)mant * w1;
    __uint128_t p2 = (__uint128_t)mant * w2 + (uint64_t)(p1 >> 64);
    uint64_t    res0 = (uint64_t)p2;
    uint64_t    res1 = (uint64_t)p1;

    int      resexp = xexp & 7;
    int      shift  = 50 - resexp;
    uint64_t ltb    = res0 >> shift;
    uint64_t pf     = (res0 >> (shift - 1)) & 1ULL;        /* round-up carry */

    uint64_t neg_mask = -pf;
    res0 ^= neg_mask;                         /* |1 - frac| on round-up */
    res1 ^= neg_mask;

    int ib = resexp + 14;
    res0 = (res0 << ib) >> ib;                /* fraction of one pi/32 segment */

    /* res1 term is ~2^-64 below the head -> plain multiply+add suffices */
    double rh = (double)res0 * sph_P32F[resexp] + (double)res1 * slp_P32F[resexp];
    if (pf) rh = -rh;

    *r     = rh;
    *m_out = (int)((ltb + pf) & 0x1FULL);     /* m in [0,31] */
}

#endif /* LIBM_REMAINDER_PIBY32_F_H */
