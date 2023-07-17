/*
 * Copyright (C) 2008-2023 Advanced Micro Devices, Inc. All rights reserved.
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
#define FUNC_CEXP alm_mpc_cexpf

#elif defined(DOUBLE)
#define FUNC_CEXP alm_mpc_cexp

#else
#error
#endif

#include <external/amdlibm.h>
#include <mpfr.h>
#include <complex.h>
#include <mpc.h>
#include <stdio.h>

COMPLEX_L FUNC_CEXP(COMPLEX x)
{
    COMPLEX_L y;

    mpc_rnd_t rnd = MPC_RNDNN;
    mpc_t mpc_x, mpc_rop;

    mpc_init2(mpc_x, ALM_MP_PRECI_BITS);
    mpc_init2(mpc_rop, ALM_MP_PRECI_BITS);

#if defined(FLOAT)
    mpc_set_dc(mpc_x, x, rnd);
#elif defined(DOUBLE)
    mpc_set_ldc(mpc_x, x, rnd);
#endif

    // Note: To print MPC variable as string, below printf statement is useful:
    // printf("%s\n", mpc_get_str(10, 2, mpc_x, rnd));

    mpc_exp(mpc_rop, mpc_x, rnd);

#if defined(FLOAT)
    y = mpc_get_dc(mpc_rop, rnd);
#elif defined(DOUBLE)
    y = mpc_get_ldc(mpc_rop, rnd);
#endif

    mpc_clear(mpc_x);
    mpc_clear(mpc_rop);

    return y;
}

