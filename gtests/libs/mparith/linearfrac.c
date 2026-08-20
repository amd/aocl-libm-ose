/*
 * Copyright (C) 2024 Advanced Micro Devices, Inc. All rights reserved.
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
#define FUNC_LINEARFRAC alm_mp_linearfracf

#elif defined(DOUBLE)
#define FUNC_LINEARFRAC alm_mp_linearfrac

#else
#error
#endif

#include <mpfr.h>

void FUNC_LINEARFRAC(REAL x, REAL y, REAL scale_x, REAL shift_x, REAL scale_y, REAL shift_y, mpfr_t result){

     REAL_L y1;

     mpfr_rnd_t rnd = MPFR_RNDN;
     mpfr_t mpx, mpy;
     mpfr_t mpscale_x, mpshift_x, mpscale_y, mpshift_y;
     mpfr_t mp_rop, mp_numerator, mp_denominator;

     mpfr_inits2(ALM_MP_PRECI_BITS, mpx, mpy, mpscale_x, mpshift_x, mpscale_y, mpshift_y, mp_rop, mp_numerator, mp_denominator, (mpfr_ptr) 0);

     #if defined(FLOAT)
#if defined(FLOAT)
    mpfr_set_flt(mpx, x, rnd);
#else
    mpfr_set_d(mpx, x, rnd);
#endif
#if defined(FLOAT)
    mpfr_set_flt(mpy, y, rnd);
#else
    mpfr_set_d(mpy, y, rnd);
#endif
#if defined(FLOAT)
    mpfr_set_flt(mpscale_x, scale_x, rnd);
#else
    mpfr_set_d(mpscale_x, scale_x, rnd);
#endif
#if defined(FLOAT)
    mpfr_set_flt(mpshift_x, shift_x, rnd);
#else
    mpfr_set_d(mpshift_x, shift_x, rnd);
#endif
#if defined(FLOAT)
    mpfr_set_flt(mpscale_y, scale_y, rnd);
#else
    mpfr_set_d(mpscale_y, scale_y, rnd);
#endif
#if defined(FLOAT)
    mpfr_set_flt(mpshift_y, shift_y, rnd);
#else
    mpfr_set_d(mpshift_y, shift_y, rnd);
#endif
#elif defined(DOUBLE)
#if defined(FLOAT)
    mpfr_set_flt(mpx, x, rnd);
#else
    mpfr_set_d(mpx, x, rnd);
#endif
#if defined(FLOAT)
    mpfr_set_flt(mpy, y, rnd);
#else
    mpfr_set_d(mpy, y, rnd);
#endif
#if defined(FLOAT)
    mpfr_set_flt(mpscale_x, scale_x, rnd);
#else
    mpfr_set_d(mpscale_x, scale_x, rnd);
#endif
#if defined(FLOAT)
    mpfr_set_flt(mpshift_x, shift_x, rnd);
#else
    mpfr_set_d(mpshift_x, shift_x, rnd);
#endif
#if defined(FLOAT)
    mpfr_set_flt(mpscale_y, scale_y, rnd);
#else
    mpfr_set_d(mpscale_y, scale_y, rnd);
#endif
#if defined(FLOAT)
    mpfr_set_flt(mpshift_y, shift_y, rnd);
#else
    mpfr_set_d(mpshift_y, shift_y, rnd);
#endif
#endif

    mpfr_fma(mp_numerator, mpx, mpscale_x, mpshift_x, rnd);
    mpfr_fma(mp_denominator, mpy, mpscale_y, mpshift_y, rnd);
    mpfr_div(mp_rop, mp_numerator, mp_denominator, rnd);

    mpfr_set(result, mp_rop, rnd);

    mpfr_clears(mpx, mpy, mpscale_x, mpshift_x, mpscale_y, mpshift_y, mp_rop, mp_numerator, mp_denominator, (mpfr_ptr) 0);


}