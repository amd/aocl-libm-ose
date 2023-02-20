/*
* Copyright (C) 2021-2022 Advanced Micro Devices, Inc. All rights reserved.
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

/*
* ISO-IEC-10967-2: Elementary Numerical Functions
* Signature:
*   float atanhf(float x)
*
*
*
* Spec:
*   Valid range : -1 < x > 1
*
*   atanhf(x)    x is +/-1         = +/-inf
*   atanhf(x)   -1 < x > 1         = NAN
*   atanhf(x)    x is 0            = 0
*
*
********************************************
* Implementation Notes
* ---------------------
* To compute atanhf(float x)
* Let ax = |x|
*
* Based on the value of x, atanhf(x) is calculated as,
*
* 1. If ax < 1.220e^-4 (0x39000000),
*      atanh(x) = x
*
*
* 2. If ax < 0.5 and ax > -0.5
*        atanh(x) = x + x^3 * [(a0 + a1*r^2 + a2*r^4 + a3*r^6+ a4*r^8 + a5*r^10) / (b0 + b1*r^2 + b2*r^4 + b3*r^6+ b4*r^8 + b5*r^10)
*                 = x + x^3 * poly
*
*        Here a0, a1, a2, a3, a4, a5  and
*             b0, b1, b2, b3, b4, b5  are  approximated by a [5,5] minimax polynomial
*
*
* 3. If ax >= 0.5 and ax <= -0.5
*        atanh(x) =  +/- (log(1 + 2x/(1-x)) = log1p(2x/(1-x)))
*
*
*/

#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>
#include "libm_inlines_amd.h"


static struct {
    double poly_atanhf_A[6];
} atanhf_data = {
    // Polynomial coefficients obtained using fpminimax algorithm from Sollya
    .poly_atanhf_A = {
        0x1.940152p-2,
        -0x1.1ff3cep-2,
        0x1.3032fcp-7,
        0x1.2f00fep0,
        -0x1.8dc42ap0,
        0x1.cfafc2p-2,
    },
};


#define A  atanhf_data.poly_atanhf_A

float
ALM_PROTO_OPT(atanhf)(float x) {
    double dx;
    uint32_t ax;
    double r, t, poly;
    uint32_t ux = asuint32(x);
    ax = ux & ~SIGNBIT_SP32;

    if (unlikely((ux & EXPBITS_SP32) == EXPBITS_SP32)) {
        /* x is either NaN or infinity */
        if (ux & MANTBITS_SP32) {
            /* x is NaN */
#ifdef WINDOWS
            return __alm_handle_errorf(ux | 0x00400000, AMD_F_INVALID);
#else

            if (ux & QNAN_MASK_32)
            { return __alm_handle_errorf(ux | 0x00400000, AMD_F_NONE); }
            else
            { return __alm_handle_errorf(ux | 0x00400000, AMD_F_INVALID); }

#endif
        } else {
            /* x is infinity; return a NaN */
            return __alm_handle_errorf(INDEFBITPATT_SP32, AMD_F_INVALID);
        }
    } else if (ax >= POS_ONE_F32) {
        if (ax > POS_ONE_F32) {
            /* abs(x) > 1.0; return NaN */
            return __alm_handle_errorf(INDEFBITPATT_SP32, AMD_F_INVALID);
        } else if (ux == POS_ONE_F32) {
            /* x = +1.0; return infinity with the same sign as x
               and set the divbyzero status flag */
            return __alm_handle_errorf(PINFBITPATT_SP32, AMD_F_DIVBYZERO);
        } else {
            /* x = -1.0; return infinity with the same sign as x */
            return __alm_handle_errorf(NINFBITPATT_SP32, AMD_F_DIVBYZERO);
        }
    }

    if (ax < 0x39000000) {
        if (ax == POS_ZERO_F32) {
            /* x is +/-zero. Return the same zero. */
            return x;
        } else {
            /* Arguments smaller than 2^(-13) in magnitude are
               approximated by atanhf(x) = x, raising inexact flag. */
#ifdef WINDOWS
            return x; // return valf_with_flags(x, AMD_F_INEXACT);
#else
            return __alm_handle_errorf(ux, AMD_F_UNDERFLOW | AMD_F_INEXACT);
#endif
        }
    } else {
        dx = x;

        if (ax < 0x3f000000) {
            /* Arguments up to 0.5 in magnitude are
               approximated by a [2,2] minimax polynomial */
            t = dx * dx * dx;
            poly =
                    POLY_EVAL_EVEN_4(dx, A[0], A[1], A[2]) /
                    POLY_EVAL_EVEN_4(dx, A[3], A[4], A[5]);
            return (float)(dx + t * poly);
        } else {
            /* abs(x) >= 0.5 */
            /* Note that
                 atanhf(x) = 0.5 * ln((1+x)/(1-x))
               (see Abramowitz and Stegun 4.6.22).
               For greater accuracy we use the variant formula
               atanhf(x) = log(1 + 2x/(1-x)) = log1p(2x/(1-x)).
            */
            if (ux & SIGNBIT_SP32) {
                /* Argument x is negative */
                r = (-2.0 * dx) / (1.0 + dx);
                r = 0.5 * ALM_PROTO_FMA3(log1p)(r);
                return (float) - r;
            }

            r = (2.0 * dx) / (1.0 - dx);
            r = 0.5 * ALM_PROTO_FMA3(log1p)(r);
            return (float)r;
        }
    }
}