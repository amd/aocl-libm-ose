/*
 * Copyright (C) 2024-2026 Advanced Micro Devices, Inc. All rights reserved.
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

#include "precision.h"

#if defined(FLOAT)
#define FUNC_NEXTAFTER alm_mp_nextafterf

#elif defined(DOUBLE)
#define FUNC_NEXTAFTER alm_mp_nextafter

#else
#error
#endif

#include <mpfr.h>

/* IEEE 754 binary32/binary64 exponent bounds for mpfr_subnormalize.  mpfr's
 * emin counts the binade of the smallest *normal*, then subnormalize collapses
 * anything smaller to the appropriate subnormal/zero per the format. */
#if defined(FLOAT)
#  define ALM_MP_NEXTAFTER_PREC   24
#  define ALM_MP_NEXTAFTER_EMIN  (-148)  /* binary32: emin = -126 - (p-1) = -149+1 */
#  define ALM_MP_NEXTAFTER_EMAX   128
#elif defined(DOUBLE)
#  define ALM_MP_NEXTAFTER_PREC   53
#  define ALM_MP_NEXTAFTER_EMIN  (-1073) /* binary64: emin = -1022 - (p-1) = -1074+1 */
#  define ALM_MP_NEXTAFTER_EMAX   1024
#endif

REAL_L FUNC_NEXTAFTER(REAL x, REAL y)
{
    REAL_L y1;

    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t mpx, mpy;
    mpfr_exp_t saved_emin = mpfr_get_emin();
    mpfr_exp_t saved_emax = mpfr_get_emax();

    /* Use the IEEE format's exact precision and exponent range so
     * mpfr_nexttoward + mpfr_subnormalize faithfully reproduces IEEE 754
     * nextAfter */
    mpfr_set_emin(ALM_MP_NEXTAFTER_EMIN);
    mpfr_set_emax(ALM_MP_NEXTAFTER_EMAX);

    mpfr_inits2(ALM_MP_NEXTAFTER_PREC, mpx, mpy, (mpfr_ptr) 0);

#if defined(FLOAT)
    mpfr_set_d(mpx, x, rnd);
    mpfr_set_d(mpy, y, rnd);
#elif defined(DOUBLE)
    mpfr_set_d(mpx, x, rnd);
    mpfr_set_d(mpy, y, rnd);
#endif

    mpfr_nexttoward(mpx, mpy);
    /* mpfr_nexttoward ignores the active emin/emax; collapse the result back
     * to the subnormal/zero range of the target IEEE format. */
    mpfr_subnormalize(mpx, 0, rnd);

#if defined(FLOAT)
    y1 = mpfr_get_d(mpx, rnd);
#elif defined(DOUBLE)
    y1 = mpfr_get_d(mpx, rnd);
#endif

    mpfr_clears(mpx, mpy, (mpfr_ptr) 0);
    mpfr_set_emin(saved_emin);
    mpfr_set_emax(saved_emax);

    return y1;
}
