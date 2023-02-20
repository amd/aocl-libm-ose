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
*   double atanh(double x)
*
*
*
* Spec:
*   Valid range : -1 < x > 1
*
*   atanh(x)    x is +/-1         = +/-inf
*   atanh(x)   -1 < x > 1         = NAN
*   atanh(x)    x is 0            = 0
*
*
********************************************
* Implementation Notes
* ---------------------
* To compute atanh(double x)
* Let ax = |x|
*
* Based on the value of x, atanh(x) is calculated as,
*
* 1. If ax < 3.72e^-9 (0x3e30000000000000),
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
#include <libm/typehelper.h>
#include <libm/poly.h>

static struct {
    double poly_atanh_A[12];
} atanh_data = {
    // Polynomial coefficients obtained using fpminimax algorithm from Sollya
    .poly_atanh_A = {
       0x1.e638b7bbea45ep-2,
       -0x1.1a53706989746p0,
       0x1.c4f4f6baa48ffp-1,
       -0x1.2090bb7302592p-2,
       0x1.d6b0a4cfde8fcp-6,
       -0x1.b711000f5a53bp-14,
       0x1.6caa89ccefb46p0,
       -0x1.0a71c2944b0bfp2,
       0x1.22a7720caaa5dp2,
       -0x1.2164ca4f0c6f3p1,
       0x1.fb81b3fe42b33p-2,
       -0x1.25c7216683ecap-5,
    },
};


#define A  atanh_data.poly_atanh_A


double
ALM_PROTO_OPT(atanh)(double x) {
    uint64_t ax;
    double r, absx, t, poly;
    uint64_t ux = asuint64(x);
    ax = ux & ~SIGNBIT_DP64;
    absx = asdouble(ax);

    if (unlikely((ux & EXPBITS_DP64) == EXPBITS_DP64)) {
        /* x is either NaN or infinity */
        if (ux & MANTBITS_DP64) {
            /* x is NaN */
#ifdef WINDOWS
            return __alm_handle_error(ux | 0x0008000000000000, AMD_F_NONE);
#else

            if (ux & QNAN_MASK_64)
                return __alm_handle_error(ux | 0x0008000000000000, AMD_F_NONE);
            else
                return __alm_handle_error(ux | 0x0008000000000000, AMD_F_INVALID);

#endif
        } else {
            /* x is infinity; return a NaN */
              return __alm_handle_error(INDEFBITPATT_DP64, AMD_F_INVALID);
        }
    } else if (ax >= POS_ONE_F64) {
        if (ax > POS_ONE_F64) {
            /* abs(x) > 1.0; return NaN */
            return __alm_handle_error(INDEFBITPATT_DP64, AMD_F_INVALID);
        } else if (ux == POS_ONE_F64) {
              /* x = +1.0; return infinity with the same sign as x
               and set the divbyzero status flag */
              return __alm_handle_error(PINFBITPATT_DP64, AMD_F_DIVBYZERO);
        } else {
              /* x = -1.0; return infinity with the same sign as x */
              return __alm_handle_error(NINFBITPATT_DP64, AMD_F_DIVBYZERO);
        }
    }

    if (ax < 0x3e30000000000000) {
        if (ax == POS_ZERO_F64) {
            /* x is +/-zero. Return the same zero. */
            return x;
        } else {
               /* Arguments smaller than 2^(-28) in magnitude are
               approximated by atanh(x) = x, raising inexact flag. */
#ifdef WINDOWS
              return x;//return val_with_flags(x, AMD_F_INEXACT);
#else
              return __alm_handle_error(ux, AMD_F_UNDERFLOW | AMD_F_INEXACT);
#endif
        }
    } else {
        if (ax < 0x3fe0000000000000) {
            /* Arguments up to 0.5 in magnitude are
               approximated by a [5,5] minimax polynomial */
            t = x * x * x;
            poly =
                    POLY_EVAL_EVEN_10(x, A[0], A[1], A[2], A[3], A[4], A[5]) /
                    POLY_EVAL_EVEN_10(x, A[6], A[7], A[8], A[9], A[10], A[11]);
            return x + t * poly;
        } else {
            /* abs(x) >= 0.5 */
            /* Note that
                 atanh(x) = 0.5 * ln((1+x)/(1-x))
               (see Abramowitz and Stegun 4.6.22).
               For greater accuracy we use the variant formula
               atanh(x) = log(1 + 2x/(1-x)) = log1p(2x/(1-x)).
            */
            r = (2.0 * absx) / (1.0 - absx);
            r = 0.5 * log1p(r);

            if (ux & SIGNBIT_DP64){
                return -r;
            }

            return r;
        }
    }
}
