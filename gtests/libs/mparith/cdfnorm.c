/*
 * Copyright (C) 2025 Advanced Micro Devices, Inc. All rights reserved.
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
#define FUNC_CDFNORM alm_mp_cdfnormf

#elif defined(DOUBLE)
#define FUNC_CDFNORM alm_mp_cdfnorm

#else
#error
#endif

#include <mpfr.h>

REAL_L FUNC_CDFNORM(REAL x)
{
    REAL_L y;

    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t mpx, mpx_div, one, sqrt2, half, erf, add, mp_rop;

    mpfr_inits2(ALM_MP_PRECI_BITS, mpx, mp_rop, mpx_div, one, sqrt2, half, erf, add, (mpfr_ptr) 0);

#if defined(FLOAT)
    mpfr_set_d(mpx, x, rnd);
    mpfr_set_ui(one, 1u, rnd);
    mpfr_set_d(half, 0.5, rnd);
    mpfr_sqrt_ui(sqrt2, 2u, rnd);                 /* sqrt2 = sqrt(2) */
#elif defined(DOUBLE)
    mpfr_set_ld(mpx, x, rnd);
    mpfr_set_ui(one, 1u, rnd);
    mpfr_set_d(half, 0.5, rnd);
    mpfr_sqrt_ui(sqrt2, 2u, rnd);                 /* sqrt2 = sqrt(2) */
#endif

    /* mpx_div = x / sqrt(2) */
    mpfr_div(mpx_div, mpx, sqrt2, rnd);

    if (mpfr_sgn(mpx) < 0) {
        /* Φ(x) = 0.5 * erfc(-x / √2) */
        mpfr_neg(mpx_div, mpx_div, rnd);
        mpfr_erfc(mp_rop, mpx_div, rnd);
        mpfr_mul(mp_rop, mp_rop, half, rnd);
    } else {
        /* Φ(x) = 0.5 * (1 + erf(x / √2)) */
        mpfr_erf(erf, mpx_div, rnd);
        mpfr_add(mp_rop, erf, one, rnd);
        mpfr_mul(mp_rop, mp_rop, half, rnd);
    }

#if defined(FLOAT)
    y = mpfr_get_d(mp_rop, rnd);
#elif defined(DOUBLE)
    /* Return a correctly-rounded double directly to avoid double rounding via long double */
    y = mpfr_get_ld(mp_rop, rnd);
#endif

    mpfr_clears(mpx, mp_rop, mpx_div, one, sqrt2, half, erf, add, (mpfr_ptr) 0);
    return y;
}