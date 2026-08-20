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

#ifndef LIBM_REMAINDER_PIBY32_H
#define LIBM_REMAINDER_PIBY32_H

/*
 * Payne-Hanek reduction of a double modulo pi/32 (large-argument path).
 *
 *     |x| = m * (pi/32) + (r + rr),   m in [0, 64),   |r| <~ pi/64.
 *
 * Inputs / outputs:
 *   - input  : the magnitude bit-pattern  ax = bits(x) & 0x7FFF...FFF.
 *   - outputs: index m = (k & 63) and a double-double residual (r, rr) for |x|;
 *              the caller applies the input sign.
 *
 * Method: a cheap single-limb residual is computed on the common path.  Right
 * at a function zero the residual |r| collapses to a tiny value and a single
 * limb loses its low bits, so when |r| < 2^-49 the residual is recomputed with
 * an exact double-double (32-bit-split) reconstruction that keeps every
 * fraction bit (~64+ relative bits).  That accurate branch is cold and fires
 * only on the rare near-zero inputs, so the common path stays fast.
 */

#include <stdint.h>
#include <math.h>
#include <libm/typehelper.h>   /* asdouble / asuint64 */

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

/* Near-zero gate: when the cheap residual |r| is below this threshold the input
 * is in the deep-cancellation zone (right at a function zero) where a single
 * limb loses its low bits, so the residual is recomputed accurately.  2^-49
 * lies above the largest such |r| while staying below well-resolved residuals. */
#ifndef RP32_NEAR_ZERO_THRESH
#define RP32_NEAR_ZERO_THRESH 0x1p-49
#endif

/* 2/pi packed into 158 bytes (most significant byte first). */
#ifndef LIBM_TWO_BY_PI_BYTES_NEW_DEFINED
#define LIBM_TWO_BY_PI_BYTES_NEW_DEFINED
static const uint8_t two_by_pi_bytes_new[158] = {
    224,241, 27,193, 12, 88, 33,116, 53,126,196,126,237,175,169, 75,
     74, 41,222,231, 28,244,236,197,151,175, 31,235,158,212,181,168,
    127,121,154,253, 24, 61,221, 38, 44,159, 60,251,217,180,125,180,
     41,104, 45, 70,188,188, 63, 96, 22,120,255, 95,226,127,236,160,
    228,247, 46,126, 17,114,210,231, 76, 13,230, 88, 71,230,  4,249,
    125,209,154,192,113,166, 19, 18,237,186,212,215,  8,162,251,156,
    166,196,114,172,119,248,115, 72, 70, 39,168,187, 36, 25,128, 75,
     55,  9,233,184,145,220,134, 21,239,122,175,142, 69,249,  7, 65,
     14,241,100, 86,138,109,  3,119,211,212, 71, 95,157,240,167, 84,
     16, 57,185, 13,230,139,  2,  0,  0,  0,  0,  0,  0,  0
};
#endif

/* Triple-double pi/2, scaled by 2^(i-54) for i = 0..7 (indexed by xexp & 7). */
#ifndef LIBM_SPH_N_DEFINED
#define LIBM_SPH_N_DEFINED
static const double sph_n[8] = {
    0x1.921fb54442d18p-54, 0x1.921fb54442d18p-53, 0x1.921fb54442d18p-52,
    0x1.921fb54442d18p-51, 0x1.921fb54442d18p-50, 0x1.921fb54442d18p-49,
    0x1.921fb54442d18p-48, 0x1.921fb54442d18p-47
};
static const double spl_n[8] = {
    0x1.1a62633145c07p-108, 0x1.1a62633145c07p-107, 0x1.1a62633145c07p-106,
    0x1.1a62633145c07p-105, 0x1.1a62633145c07p-104, 0x1.1a62633145c07p-103,
    0x1.1a62633145c07p-102, 0x1.1a62633145c07p-101
};
static const double slp_n[8] = {
    0x1.921fb54442d18p-118, 0x1.921fb54442d18p-117, 0x1.921fb54442d18p-116,
    0x1.921fb54442d18p-115, 0x1.921fb54442d18p-114, 0x1.921fb54442d18p-113,
    0x1.921fb54442d18p-112, 0x1.921fb54442d18p-111
};
#endif

/* one 64-bit word lower than slp_n (= slp_n * 2^-64): scales the next fraction
 * word so the near-zero double-double residual keeps its low bit. */
#ifndef LIBM_SLP2_N_DEFINED
#define LIBM_SLP2_N_DEFINED
static const double slp2_n[8] = {
    0x1.921fb54442d18p-182, 0x1.921fb54442d18p-181, 0x1.921fb54442d18p-180,
    0x1.921fb54442d18p-179, 0x1.921fb54442d18p-178, 0x1.921fb54442d18p-177,
    0x1.921fb54442d18p-176, 0x1.921fb54442d18p-175
};
#endif

/* Cold near-zero rescue: an exact double-double reconstruction of the residual,
 * re-deriving the fraction words from `ax` so the common path need not keep them
 * live.  noinline; reached only for the rare |r| < 2^-49 (near-zero) inputs. */
typedef struct { double r, rr; } rp32_dd_t;

static __attribute__((noinline)) rp32_dd_t
rp32_near_zero(uint64_t ax, int resexp_ph)
{
    int      xexp = (int)(ax >> 52) - 1023;
    uint64_t mant = (ax & 0xFFFFFFFFFFFFFULL) | (1ULL << 52);
    int last = 134 - (xexp >> 3); if (last < 0) last = 0;
    uint64_t w0, w1, w2;
    __builtin_memcpy(&w0, &two_by_pi_bytes_new[last],      8);
    __builtin_memcpy(&w1, &two_by_pi_bytes_new[last +  8], 8);
    __builtin_memcpy(&w2, &two_by_pi_bytes_new[last + 16], 8);
    __uint128_t q0 = (__uint128_t)mant * w0;
    __uint128_t q1 = (__uint128_t)mant * w1 + (uint64_t)(q0 >> 64);
    __uint128_t q2 = (__uint128_t)mant * w2 + (uint64_t)(q1 >> 64);
    uint64_t pr2 = (uint64_t)q0, pr1 = (uint64_t)q1, pr0 = (uint64_t)q2;
    int      shift = 50 - resexp_ph;
    uint64_t pf    = (pr0 >> (shift - 1)) & 1ULL;
    uint64_t mask_ph = (uint64_t)0 - pf;
    pr0 ^= mask_ph; pr1 ^= mask_ph; pr2 ^= mask_ph;
    int ib = resexp_ph + 14;
    pr0 = (ib < 64) ? ((pr0 << ib) >> ib) : 0;

    uint64_t res0 = pr0, res1 = pr1, res2 = pr2;
    uint64_t sgn  = mask_ph & 0x8000000000000000ULL;
    int rexp = (resexp_ph + 14) - 64;

    int hsb;
    if (res0 != 0) {
        hsb = 63 - __builtin_clzll(res0);
    } else {
        res0 = res1; res1 = res2; res2 = 0; rexp -= 64;
        hsb = (res0 != 0) ? (63 - __builtin_clzll(res0)) : 0;
    }
    rexp += hsb;
    int sh = hsb - 52;
    if (sh > 0) {
        uint64_t old0 = res0;
        uint64_t hi = res0 >> sh;
        uint64_t r1 = res1 >> sh;
        uint64_t fill = old0 << (64 - sh);
        res1 = r1 | fill; res0 = hi;
    } else if (sh < 0) {
        int s = -sh;
        uint64_t save_r1 = res1;
        uint64_t hi = (res0 << s) | (save_r1 >> (64 - s));
        uint64_t r1 = (res1 << s) | (res2 >> (64 - s));
        res0 = hi; res1 = r1;
    }

    uint64_t xbits = ((uint64_t)(rexp + 1023) << 52)
                   | (res0 & ~(1ULL << 52)) | sgn;
    double xf = asdouble(xbits);
    double xx;
    {
        int hsb2 = (res1 != 0) ? (63 - __builtin_clzll(res1)) : 0;
        int cnt  = 64 - hsb2;
        uint64_t m2 = (cnt < 64) ? (res1 << cnt) : 0;
        m2 >>= 12;
        int e2b = (rexp + 1023) - (cnt + 52);
        uint64_t xxbits = ((uint64_t)((unsigned)e2b & 0x7FF) << 52) | m2 | sgn;
        xx = asdouble(xxbits);
    }

    const double LEAD  = asdouble(0x3fb921fb54442d18ULL);
    const double PART1 = asdouble(0x3fb921fb50000000ULL);
    const double PART2 = asdouble(0x3e1110b460000000ULL);
    uint64_t xb = asuint64(xf);
    double hx = asdouble((xb >> 27) << 27);
    double tx = xf - hx;
    double c   = LEAD * xf;
    double p1h = PART1 * hx;
    double p1t = PART1 * tx;
    double p2  = PART2 * xf;
    double lxx = LEAD * xx;
    double cc = (p1h - c) + p1t + p2 + lxx;
    rp32_dd_t out;
    out.r  = c + cc;
    out.rr = (c - out.r) + cc;
    return out;
}

static inline void
remainder_piby32(uint64_t ax, int *m_out, double *r_out, double *rr_out)
{
    /* ax = magnitude bit-pattern; caller applies the input sign. */
    int      xexp_ph = (int)(ax >> 52) - 1023;
    uint64_t mant_ph = (ax & 0xFFFFFFFFFFFFFULL) | (1ULL << 52);

    int last_ph = 134 - (xexp_ph >> 3);
    if (last_ph < 0) last_ph = 0;

    uint64_t pw0, pw1, pw2;
    __builtin_memcpy(&pw0, &two_by_pi_bytes_new[last_ph],      8);
    __builtin_memcpy(&pw1, &two_by_pi_bytes_new[last_ph +  8], 8);
    __builtin_memcpy(&pw2, &two_by_pi_bytes_new[last_ph + 16], 8);

    /* native 128-bit carry chain (compiles to mulx/adcx on AOCC/GCC) */
    __uint128_t pp0 = (__uint128_t)mant_ph * pw0;
    __uint128_t pp1 = (__uint128_t)mant_ph * pw1 + (uint64_t)(pp0 >> 64);
    __uint128_t pp2 = (__uint128_t)mant_ph * pw2 + (uint64_t)(pp1 >> 64);
    uint64_t pr1 = (uint64_t)pp1, pr0 = (uint64_t)pp2;

    int      resexp_ph = xexp_ph & 7;
    int      shift_ph  = 50 - resexp_ph;
    uint64_t ltb_ph    = pr0 >> shift_ph;
    uint64_t pf        = (pr0 >> (shift_ph - 1)) & 1ULL;
    uint64_t mask_ph   = (uint64_t)0 - pf;

    pr0 ^= mask_ph;
    pr1 ^= mask_ph;
    int m = (int)((ltb_ph + pf) & 0x3FULL);

    int ib = resexp_ph + 14;
    pr0 = (ib < 64) ? ((pr0 << ib) >> ib) : 0;

    double dpr0 = (double)pr0;
    double dpr1 = (double)pr1;
    if (pf != 0) { dpr0 = -dpr0; dpr1 = -dpr1; }

    double sph = sph_n[resexp_ph];

    /* cheap single-limb residual (the common, fast path) */
    double hh = dpr0 * sph;
    double rl = _AMD_FMA(dpr0, sph, -hh) + dpr0 * spl_n[resexp_ph]
              + dpr1 * slp_n[resexp_ph];
    double r  = hh + rl;
    double rr = (hh - r) + rl;

    /* accurate re-reduction only for tiny (near-zero) residuals.
     * Compare |r| in the integer domain (clear the sign bit and compare the
     * bit-pattern against the threshold): for finite doubles this is exactly
     * equivalent to fabs(r) < RP32_NEAR_ZERO_THRESH but avoids the fabs. */
    if (__builtin_expect((asuint64(r) & 0x7FFFFFFFFFFFFFFFULL)
                         < asuint64((double)RP32_NEAR_ZERO_THRESH), 0)) {
        rp32_dd_t dd = rp32_near_zero(ax, resexp_ph);
        r  = dd.r;
        rr = dd.rr;
    }

    *m_out  = m;
    *r_out  = r;
    *rr_out = rr;
}

#endif /* LIBM_REMAINDER_PIBY32_H */
