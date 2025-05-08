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
#define FUNC_SINCOS alm_mp_sincosf

#elif defined(DOUBLE)
#define FUNC_SINCOS alm_mp_sincos

#else
#error
#endif

#include <mpfr.h>

void FUNC_SINCOS(REAL x, REAL_L *sin, REAL_L *cos) {
    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t mpx, mp_sin, mp_cos;

    // Initialize MPFR variables with specified precision
    mpfr_inits2(ALM_MP_PRECI_BITS, mpx, mp_sin, mp_cos, (mpfr_ptr) 0);

    // Convert input to MPFR format based on defined type
    #if defined(FLOAT)
    mpfr_set_d(mpx, x, rnd);
    #elif defined(DOUBLE)
    mpfr_set_ld(mpx, x, rnd);
    #endif

    // Compute sine and cosine
    mpfr_sin_cos(mp_sin, mp_cos, mpx, rnd);

    // Convert results back to appropriate type
    #if defined(FLOAT)
    *sin = mpfr_get_d(mp_sin, rnd);
    *cos = mpfr_get_d(mp_cos, rnd);
    #elif defined(DOUBLE)
    *sin = mpfr_get_ld(mp_sin, rnd);
    *cos = mpfr_get_ld(mp_cos, rnd);
    #endif

    // Clear MPFR variables
    mpfr_clears(mpx, mp_sin, mp_cos, (mpfr_ptr) 0);
}
