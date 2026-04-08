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
 *
 */

/* Contains implementation of double atan2(double y, double x)
 *
 * Table-driven atan2(y, x) with per-interval minimax polynomials.
 *
 *   1. Compute |x|, |y|; scale subnormals to normal range
 *   2. Swap to ensure v <= u, compute ratio r = v/u in [0, 1]
 *   3. Three-path polynomial evaluation:
 *      a. Tiny   (r < 2^-26):  atan(r) ≈ r
 *      b. Fast   (r < 0.125):  degree-7 minimax in r^2
 *      c. Table  (r >= 0.125): 64-interval degree-7 Estrin
 *   4. Reconstruct quadrant from signs and swap flag
 * 
 * Special cases handled here (y=0, x=0):
 *  atan2(+0,  +x)  = +0        atan2(-0,  +x)  = -0
 *  atan2(+0,  -x)  = +pi       atan2(-0,  -x)  = -pi
 *  atan2(+0,  +0)  = +0        atan2(-0,  +0)  = -0
 *  atan2(+0,  -0)  = +pi       atan2(-0,  -0)  = -pi
 *  atan2(+y,  +0)  = +pi/2     atan2(-y,  +0)  = -pi/2
 *  atan2(+y,  -0)  = +pi/2     atan2(-y,  -0)  = -pi/2
 * 
 * Not handled (NaN/Inf inputs)
 */

#include <stdint.h>
#include <immintrin.h> // intrinsics for bit manipulation and min/max
#include <libm/alm_special.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/poly.h>
#include <libm/amd_funcs_internal.h>
#include "atan_data.h"

#if defined(__GNUC__) || defined(__clang__)
#define ALM_COPYSIGN(x, y) __builtin_copysign((x), (y)) /* Copy sign of y onto x */
#define ALM_FLOOR(x) __builtin_floor(x)
#else
#include <math.h>
static inline double
ALM_COPYSIGN(double x, double y)
{
    return asdouble((asuint64(x) & ~0x8000000000000000ULL) |
                    (asuint64(y) &  0x8000000000000000ULL));
}
#define ALM_FLOOR(x) floor(x)
#endif

/*
 * fabs / fmax / fmin implementations without handling special cases (NaN, Inf).
 * Maps to : vpsllq, vpsrlq, vmaxsd, vminsd
 */
//* _asm__("vpsllq $1, %1, %0\n\tvpsrlq $1, %0, %0" : "=x"(ax) : "x"(x));
#define FABS_NON_SPECIAL(x) \
    _mm_cvtsd_f64(_mm_castsi128_pd( \
        _mm_srli_epi64(_mm_slli_epi64( \
            _mm_castpd_si128(_mm_set_sd(x)), 1), 1)))

//_asm__("vmaxsd %1, %2, %0" : "=x"(denom) : "x"(ay), "x"(ax));
#define FMAX_NON_SPECIAL(a, b) \
    _mm_cvtsd_f64(_mm_max_sd(_mm_set_sd(a), _mm_set_sd(b)))

//__asm__("vminsd %1, %2, %0" : "=x"(numer) : "x"(ay), "x"(ax));
#define FMIN_NON_SPECIAL(a, b) \
    _mm_cvtsd_f64(_mm_min_sd(_mm_set_sd(a), _mm_set_sd(b)))

/*
 * min_by_max(va, vb) -> FMIN_NON_SPECIAL(va,vb) / FMAX_NON_SPECIAL(va,vb)
 *
 * Using inline asm, as compilers translate '/' or even '_mm_div_sd'
 * into vdivpd instead of vdivsd. With vdivpd, the unused [127:64] bits
 * end up computing 0/0 and throw undesired exceptions.
 */
static inline double
min_by_max(double va, double vb)
{
#if defined(__GNUC__) || defined(__clang__)
    double hi, lo, ratio;
    __asm__ ("vmaxsd %[vb], %[va], %[hi]\n\t"
             "vminsd %[vb], %[va], %[lo]\n\t"
             "vdivsd %[hi], %[lo], %[ratio]"
             : [hi]    "=&x" (hi),
               [lo]    "=&x" (lo),
               [ratio] "=&x" (ratio)
             : [va]    "x"   (va),
               [vb]    "x"   (vb));
    return ratio;
#else
    /* Fallback for compilers without GCC extended asm */
    return fmin(va, vb) / fmax(va, vb);
#endif
}

double
ALM_PROTO_FAST(atan2)(double y, double x)
{
    uint64_t ux = asuint64(x);
    uint64_t uy = asuint64(y);

    uint64_t aux = (ux << 1) >> 1;
    uint64_t auy = (uy << 1) >> 1;

    double ax = FABS_NON_SPECIAL(x);
    double ay = FABS_NON_SPECIAL(y);

    if (unlikely((aux | auy) < ATAN_FAST_SUBNORM_LIM)) {
        ax *= ATAN_FAST_SCALE_UP;
        ay *= ATAN_FAST_SCALE_UP;
        if (unlikely((aux | auy) == 0)) {
              ax = 1.0; /* avoid divide by 0 */
        }
    }

    double r = min_by_max(ax, ay);

    uint64_t swapped = (auy > aux);
    uint64_t xneg = ux >> 63;
    unsigned ridx = (unsigned)((swapped << 1) | xneg);
    double sign   = ATAN_FAST_RECON_TBL[ridx][0];
    double offset = ATAN_FAST_RECON_TBL[ridx][1];

    double poly;

    if (unlikely(r < ATAN_FAST_TINY_R)) {
        /*
         * TINY PATH: r < 2^-26.
         * atan(r) = r within 0.5 ULP. Skips polynomial entirely,
         * avoiding underflow penalties in r^2, r^4, r^8 intermediates.
         */
        poly = r;
    } else if (r < ATAN_FAST_FAST_R) {
        /*
         * FAST PATH: 2^-26 <= r < 0.125.
         * atan(r) = r * P(r^2)  where P(t) = 1 + t*g(t),
         * g = degree-6 minimax in t. Estrin on r^2.
         */
        const double *fc = ATAN_FAST_FAST_POLY;
        double t = r * r;

        poly = r * POLY_EVAL_ESTRIN_8(t, 1.0, fc[0], fc[1], fc[2],
                                         fc[3], fc[4], fc[5], fc[6]);
    } else {
        /*
         * TABLE PATH: r >= 0.125.
         * 64-interval degree-7 Estrin with floor()-based d.
         */
        double rs = r * ATAN_FAST_TBL_N;
        /* Guard against NaN/Inf resulting in invalid 'int j' below.
         * Which will result in segfault if j > ATAN_FAST_TBL_N.
         * vminsd returns the rhs when lhs is NaN clamping rs to [0, 64].
         * No-op for valid inputs. */
        rs = FMIN_NON_SPECIAL(rs, ATAN_FAST_TBL_N);

        double jf = ALM_FLOOR(rs);
        double d = jf * ATAN_FAST_INV_N + r;

        int j = (int)rs;

        const double *c = ATAN_FAST_TBL[j];

        poly = POLY_EVAL_ESTRIN_8(d, c[0], c[1], c[2], c[3],
                                     c[4], c[5], c[6], c[7]);
    }

    double result = sign * poly + offset;
    /* Sign of result always matches sign of y, since atan2 returns
     * values in (-pi, pi] and sign(y) determines the half-plane. */
    return ALM_COPYSIGN(result, y);
}

strong_alias (__atan2_finite, ALM_PROTO_FAST(atan2))
weak_alias (amd_atan2, ALM_PROTO_FAST(atan2))
weak_alias (atan2, ALM_PROTO_FAST(atan2))
