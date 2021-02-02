/*
 * Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_special.h>               /* for __amd_handle_error() */
#include <libm/types.h>


inline double
__alm_handle_error(char *fname, int opcode, unsigned long long value,
                   int type, int flags, int error, double arg1, double arg2,
                   int nargs)
{
    return __amd_handle_error(fname, opcode, value, type, flags, error, arg1, arg2, nargs);
}

inline float
__alm_handle_errorf(char *fname, int opcode, unsigned long long value,
                          int type, int flags, int error, float arg1, float arg2,
                          int nargs)
{
    return __amd_handle_errorf(fname, opcode, value, type, flags, error, arg1, arg2, nargs);
}



/*
 * alm_exp2_special:
 *    'x' is input, 'y' is value to return from __amd_handle_error
 *
 *    - output is NaN  (when input is NaN)
 *    - output is INF  (when 'x' much larger than LN2(DBL_MAX))
 *    - output is ZERO (when 'x' is -INF)
 */
double
alm_exp2_special(double x, double y, uint32_t code)
{
    flt64u_t ym = {.d = y};

    switch (code) {
    case ALM_E_IN_X_NAN:
        __alm_handle_error("exp2", __amd_exp2, ym.i,
                           ALM_ERR_DOMAIN,
                           0, EDOM, x, 0.0, 1);
        break;

    case ALM_E_OUT_ZERO:
        __alm_handle_error("exp2", __amd_exp2, ym.i,
                           ALM_ERR_UNDERFLOW,
                           AMD_F_INEXACT | AMD_F_UNDERFLOW,
                           ERANGE, x, 0.0, 1);
        break;

    case ALM_E_OUT_INF:
        __alm_handle_error("exp2", __amd_exp2, ym.i,
                           ALM_ERR_OVERFLOW,
                           AMD_F_INEXACT | AMD_F_OVERFLOW,
                           ERANGE, x, 0.0, 1);
        break;
    default:
        break;
    }

    return y;
}

float
alm_exp2f_special(float x, float y, uint32_t code)
{
    flt32u_t ym = {.f = y};

    switch (code) {
    case ALM_E_IN_X_NAN:
        __amd_handle_errorf("exp2f", __amd_exp2, ym.i,
                            ALM_ERR_DOMAIN, 0, EDOM, x, 0.0, 1);
        break;

    case ALM_E_OUT_ZERO:
        __amd_handle_errorf("exp2f", __amd_exp2, ym.i,
                            ALM_ERR_UNDERFLOW,
                            AMD_F_INEXACT | AMD_F_UNDERFLOW,
                            ERANGE, x, 0.0, 1);
        break;

    case ALM_E_OUT_INF:
        __amd_handle_errorf("exp2f", __amd_exp2, ym.i,
                            ALM_ERR_OVERFLOW,
                            AMD_F_INEXACT | AMD_F_OVERFLOW,
                            ERANGE, x, 0.0, 1);
        break;

    default:
        break;
    }

    return y;
}
