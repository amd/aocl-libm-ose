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


#include "precision.h"


#if defined(FLOAT)
#define FUNC_CPOW alm_mpc_cpowf

#elif defined(DOUBLE)
#define FUNC_CPOW alm_mpc_cpow

#else
#error
#endif

#include <mpfr.h>
#include <complex.h>
#include <mpc.h>
#include <stdio.h>
#include <string.h>

COMPLEX_L FUNC_CPOW(COMPLEX x, COMPLEX y)
{
    COMPLEX_L rop;

    mpc_rnd_t rnd = MPC_RNDNN;
    mpfr_rnd_t mpfr_rnd = MPFR_RNDN;
    mpc_t mpc_x, mpc_y, mpc_rop;
    mpfr_t mpfr_real, mpfr_imag;

    mpc_init2(mpc_x, ALM_MP_PRECI_BITS);
    mpc_init2(mpc_y, ALM_MP_PRECI_BITS);
    mpc_init2(mpc_rop, ALM_MP_PRECI_BITS);
    mpfr_init2(mpfr_real, ALM_MP_PRECI_BITS);
    mpfr_init2(mpfr_imag, ALM_MP_PRECI_BITS);

#if defined(FLOAT)
    { float px[2], py[2]; memcpy(px, &x, sizeof(px)); memcpy(py, &y, sizeof(py));
      mpc_set_d_d(mpc_x, px[0], px[1], rnd);
      mpc_set_d_d(mpc_y, py[0], py[1], rnd); }
#elif defined(DOUBLE)
    { double px[2], py[2]; memcpy(px, &x, sizeof(px)); memcpy(py, &y, sizeof(py));
      mpc_set_d_d(mpc_x, px[0], px[1], rnd);
      mpc_set_d_d(mpc_y, py[0], py[1], rnd); }
#endif

    mpc_pow(mpc_rop, mpc_x, mpc_y, rnd);

    mpc_real(mpfr_real, mpc_rop, mpfr_rnd);
    mpc_imag(mpfr_imag, mpc_rop, mpfr_rnd);

#if defined(FLOAT)
    {
        double real_val = mpfr_get_d(mpfr_real, mpfr_rnd);
        double imag_val = mpfr_get_d(mpfr_imag, mpfr_rnd);
         #if (defined _WIN32 || defined _WIN64)
            double temp[2] = {real_val, imag_val};
            memcpy(&rop, temp, sizeof(temp));
        #else
            __real__ rop = real_val;
            __imag__ rop = imag_val;
        #endif
    }
#elif defined(DOUBLE)
    {
        #if (defined _WIN32 || defined _WIN64)
            double real_val = mpfr_get_d(mpfr_real, mpfr_rnd);
            double imag_val = mpfr_get_d(mpfr_imag, mpfr_rnd);
            double temp[2] = {real_val, imag_val};
            memcpy(&rop, temp, sizeof(temp));
        #else
            long double real_val = ALM_MPFR_GET_REAL(mpfr_real, mpfr_rnd);
            long double imag_val = ALM_MPFR_GET_REAL(mpfr_imag, mpfr_rnd);
            __real__ rop = real_val;
            __imag__ rop = imag_val;
        #endif
    }
#endif

    mpfr_clear(mpfr_real);
    mpfr_clear(mpfr_imag);
    mpc_clear(mpc_x);
    mpc_clear(mpc_y);
    mpc_clear(mpc_rop);

    return rop;
}
