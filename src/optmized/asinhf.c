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
*   float asinhf(float x)
*
* Spec:
*   asinhf(+/-inf)    = +/-inf
*   asinhf(+/-0)    = +/-0
*
*
********************************************
* Implementation Notes
* ---------------------
* To compute asinhf(float x)
* Let y = x
* Let x = |x|
*
* Based on the value of x, asinhf(x) is calculated as,
*
* 1. If ax <= 4.0,
*      asinhf(x) = x - (1/2*3)x^3 + (1*3/2*4*5)x^5 - (1*3*5/2*4*6*7)x^7 ....
*               = x - (-x^3)((1/2*3) - (1*3/2*4*5)x^2 + (1*3*5/2*4*6*5)x^4 ....)
*               = x + x^3 * poly
*
*
* 2. If ax > 4.0 and ax <= 1/sqrt(epsilon)
*        asinhf(x) = ln(abs(x) + sqrt(x*x+1))
*
*
* 3. If ax > 1/sqrt(epsilon)
*        asinhf(x) =  ln(2) + ln(abs(x))
*/
#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>
#include "libm_inlines_amd.h"
#include "kern/sqrt_pos.c"

static struct {
    const uint32_t rteps, recrteps;
    const double log2;
    double poly_asinhf_A[10], poly_asinhf_B[10];
} asinhf_data = {
    .rteps     = 0x39800000,
    .recrteps  = 0x46000000,
    .log2      = 6.93147180559945286227e-01,
    // Polynomial coefficients obtained using fpminimax algorithm from Sollya
    .poly_asinhf_A = {
        -0x1.79cdc8cad8ecfp-7,
            -0x1.e50886dc8d955p-7,
            -0x1.4bd26af2415c3p-8,
            -0x1.b47e5f01aed5dp-12,
            -0x1.3c007e573c526p-20,
            0x1.1b5a569f8dd8ap-4,
            0x1.eb48a2b8008fbp-4,
            0x1.0d9d12c211cd8p-4,
            0x1.9ce28e60bc4f5p-7,
            0x1.497b89f55e0fap-11,
        },
    .poly_asinhf_B = {
        -0x1.e62da2ed59b74p-10,
            -0x1.29fc588dcceedp-10,
            -0x1.2a3b8becef9dbp-13,
            -0x1.c7bb1f54fd677p-19,
            -0x1.9f3745642df74p-30,
            0x1.6d515e40bda61p-7,
            0x1.81f311f55d6cfp-7,
            0x1.ab2b28fab47dcp-9,
            0x1.0c552ef7695ep-12,
            0x1.235a8989d067ap-18,
        },
};

#define rteps         asinhf_data.rteps
#define recrteps      asinhf_data.recrteps
#define log2          asinhf_data.log2
#define A             asinhf_data.poly_asinhf_A
#define B             asinhf_data.poly_asinhf_B

float
ALM_PROTO_OPT(asinhf)(float x) {
    double dx = x;
    uint32_t ux, ax;
    double r, rarg, t, poly;
    uint8_t sign = 0;
    double absx = dx;
    ux = asuint32(x);
    ax = ux & ~SIGNBIT_SP32;

    if (ux & SIGNBIT_SP32) {
        sign = 1;
        absx = -dx;
    }

    if (unlikely((ux & EXPBITS_SP32) == EXPBITS_SP32)) {
        /* x is either NaN or infinity */
        if (ux & MANTBITS_SP32) {
            /* x is NaN */
#ifdef WINDOWS
            return __alm_handle_errorf(ux | 0x00400000, AMD_F_NONE);
#else

            if (ux & QNAN_MASK_32)
            { return __alm_handle_errorf(ux | 0x00400000, AMD_F_NONE); }
            else
            { return __alm_handle_errorf(ux | 0x00400000, AMD_F_INVALID); }

#endif
        } else {
            /* x is infinity. Return the same infinity. */
            return x;
        }
    } else if (ax < rteps) { /* abs(x) < sqrt(epsilon) */
        if (ax == 0x00000000) {
            /* x is +/-zero. Return the same zero. */
            return x;
        } else {
            /* Tiny arguments approximated by asinhf(x) = x
               - avoid slow operations on denormalized numbers */
#ifdef WINDOWS
            return x; //return valf_with_flags(x,AMD_F_INEXACT);
#else
            return __alm_handle_errorf(ux, AMD_F_UNDERFLOW | AMD_F_INEXACT);
#endif
        }
    }

    if (ax <= 0x40800000) { /* abs(x) <= 4.0 */
        /* Arguments less than 4.0 in magnitude are
           approximated by [4,4] minimax polynomials
        */
        t = dx * dx;

        if (ax <= 0x40000000) /* abs(x) <= 2 */
            poly =
                    POLY_EVAL_EVEN_8(dx, A[0], A[1], A[2], A[3], A[4]) /
                    POLY_EVAL_EVEN_8(dx, A[5], A[6], A[7], A[8], A[9]);
        else
            poly =
                    POLY_EVAL_EVEN_8(dx, B[0], B[1], B[2], B[3], B[4]) /
                    POLY_EVAL_EVEN_8(dx, B[5], B[6], B[7], B[8], B[9]);

        return (float)(dx + dx * t * poly);
    } else {
        /* abs(x) > 4.0 */
        if (ax > recrteps) {
            /* Arguments greater than 1/sqrt(epsilon) in magnitude are
               approximated by asinhf(x) = ln(2) + ln(abs(x)), with sign of x */
            r = ALM_PROTO_FMA3(log)(absx) + log2;
        } else {
            rarg = absx * absx + 1.0;
            /* Arguments such that 4.0 <= abs(x) <= 1/sqrt(epsilon) are
               approximated by
                 asinhf(x) = ln(abs(x) + sqrt(x*x+1))
               with the sign of x (see Abramowitz and Stegun 4.6.20) */
            /* Use assembly instruction to compute r = sqrt(rarg); */
            r = ALM_PROTO_KERN(sqrt)(rarg);
            r += absx;
            r = ALM_PROTO_FMA3(log)(r);
        }

        if (sign)
            return (float)(-r);
        else
            return (float)r;
    }
}
