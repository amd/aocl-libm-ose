/*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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
*   double asinh(double x)
*
* Spec:
*   asinh(+/-inf)    = +/-inf
*   asinh(+/-0)    = +/-0
*
*
********************************************
* Implementation Notes
* ---------------------
* To compute asinh(double x)
* Let y = x
* Let x = |x|
*
* Based on the value of x, asinh(x) is calculated as,
*
* 1. If ax < 1.0,
*      asinh(x) = x - (1/2*3)x^3 + (1*3/2*4*5)x^5 - (1*3*5/2*4*6*7)x^7 ....
*               = x - (-x^3)((1/2*3) - (1*3/2*4*5)x^2 + (1*3*5/2*4*6*5)x^4 ....)
*               = x + x^3 * poly
*
*
* 2. If ax >= 1.0 and ax <= 32.0
*        asinh(x) = ln (2x) + 1/2*2x^2 - 1*3/2*4*4x^4 ......
*
*
* 3. If ax > 32
*        asinh(x) =  ln(2) + ln(abs(x))
*
*
* 4. If 32.0 <= ax <= 1/sqrt(epsilon)
*         asinh(x) = ln(abs(x) + sqrt(x*x+1))
*
*/

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>
#include "libm_inlines_amd.h"


static struct {
                uint64_t rteps,recrteps;
                const double log2_lead, log2_tail;
                uint64_t ranges[10];
                double poly_asinh_A[10], poly_asinh_B[10], poly_asinh_C[10], poly_asinh_D[11];
                double poly_asinh_E[8], poly_asinh_F[9], poly_asinh_G[11], poly_asinh_H[11], poly_asinh_J[12];
} asinh_data = {
                  .rteps            = 0x3e46a09e667f3bcd,
                  .recrteps         = 0x4196a09e667f3bcd,
                  .log2_lead        = 6.93147122859954833984e-01,
                  .log2_tail        = 5.76999904754328540596e-08,
                  .ranges = {
                            0x0000000000000000,  //0.0
                            0x3ff0000000000000,  // 1.0
                            0x3fd0000000000000,  // 0.25
                            0x3fe0000000000000,  // 0.5
                            0x3fe8000000000000,  // 0.75
                            0x4040000000000000,  // 32.0
                            0x4020000000000000,  // 8.0
                            0x4010000000000000,  // 4.0
                            0x4000000000000000,  // 2.0
                            0x3ff8000000000000,  // 1.5
                            },
                  .poly_asinh_A = {
                                    -0x1.0712c83a32494p-3,
                                    -0x1.af52a99a8d965p-3,
                                    -0x1.a156e7495bdeap-4,
                                    -0x1.c73493ef3344bp-7,
                                    -0x1.b10b99c58a7cfp-14,
                                    0x1.8a9c2c574b6dfp-1,
                                    0x1.9c47892df4bb9p0,
                                    0x1.212db144831d3p0,
                                    0x1.3403376356f73p-2,
                                    0x1.81644a95b0515p-6,
                                  },
                  .poly_asinh_B = {
                                    -0x1.f329d10cd6fc4p-4,
                                    -0x1.950d914d4af74p-3,
                                    -0x1.8293a155d158p-4,
                                    -0x1.9d896b36d9727p-7,
                                    -0x1.7ae97e02b87fdp-14,
                                    0x1.765f5cc9a1504p-1,
                                    0x1.84060840ece8ap0,
                                    0x1.0d700c04959bep0,
                                    0x1.1b46c39aa2b54p-2,
                                    0x1.5c618da6da602p-6,
                                  },
                  .poly_asinh_C = {
                                    -0x1.4ca2e272f5482p-4,
                                    -0x1.f8edafd2d3066p-4,
                                    -0x1.b7f36426aa4b5p-5,
                                    -0x1.9d938a0971be4p-8,
                                    -0x1.2857ee8fdf067p-15,
                                    0x1.f2f453adf95fbp-2,
                                    0x1.eaf609d0b1c1cp-1,
                                    0x1.3f16fc14a77efp-1,
                                    0x1.33c9974fca6c2p-3,
                                    0x1.5194e117f1658p-7,
                                  },
                  .poly_asinh_D = {
                                    -0x1.7bf5aafeb6e5fp-5,
                                    -0x1.2562a995b0cf9p-4,
                                    -0x1.0a0f3378e76ap-5,
                                    -0x1.14f0ea2f32605p-8,
                                    -0x1.3f8552ef6499cp-15,
                                    0x1.aec1a6eeb9991p-21,
                                    0x1.1cf84053a1d47p-2,
                                    0x1.1c2840384a501p-1,
                                    0x1.7af47b7af6b66p-2,
                                    0x1.80d9ee1e01be9p-4,
                                    0x1.d7dced875024bp-8,
                                  },
                  .poly_asinh_E = {
                                    -0x1.d93bac43ec438p-35,
                                    -0x1.2cef5283494a7p-32,
                                    -0x1.26cfd638648eep-32,
                                    -0x1.4635196a31a02p-97,
                                    0x1.05c68347fb4c9p-32,
                                    0x1.17cc1d362637ap-29,
                                    0x1.35d014925611cp-28,
                                    0x1.89151da08613ep-29,
                                  },
                  .poly_asinh_F = {
                                    -0x1.7edcc59b2c48ap-23,
                                    -0x1.f29c41318bb02p-21,
                                    -0x1.efb3fca23fc2p-21,
                                    -0x1.6ea70f1d0b1a8p-64,
                                    0x1.5f9c5e037dc9cp-72,
                                    0x1.a65a7ef307b54p-21,
                                    0x1.cbfcf360ce1fp-18,
                                    0x1.02001519f68bp-16,
                                    0x1.4a77fdc18d549p-17,
                                  },
                  .poly_asinh_G = {
                                    -0x1.c24dfec27281p-23,
                                    -0x1.2646895d97377p-19,
                                    -0x1.7233550498cf2p-18,
                                    -0x1.008eaf3d3fd82p-18,
                                    -0x1.36c70dad71a45p-58,
                                    0x1.d0e75de58f1b4p-65,
                                    0x1.dd5fc13df322cp-21,
                                    0x1.b812db8e664a3p-17,
                                    0x1.c34523a01e2d9p-15,
                                    0x1.55d253a567c3dp-14,
                                    0x1.56139451e79e5p-15,

                                  },
                  .poly_asinh_H = {
                                    -0x1.47e36287fd195p-16,
                                    -0x1.e94c5039d8c62p-13,
                                    -0x1.525d90d9e8a77p-11,
                                    -0x1.f636db4fd8e6bp-12,
                                    -0x1.c54e772cdc564p-41,
                                    0x1.bbed21d03b101p-46,
                                    0x1.58d778035f6d3p-14,
                                    0x1.62cd57610735cp-10,
                                    0x1.8b02dd5c6f528p-8,
                                    0x1.3e9460ee0e2a7p-7,
                                    0x1.4ecf3d6ca8bfap-8,
                                  },
                  .poly_asinh_J = {
                                    -0x1.96c2ca30a93d8p-17,
                                    -0x1.1e69eccc39524p-12,
                                    -0x1.90bb4ba126416p-10,
                                    -0x1.7f08b3b7870e3p-9,
                                    -0x1.c9e3a829bef98p-10,
                                    -0x1.f603535c00237p-41,
                                    0x1.a2f30d76d07d5p-15,
                                    0x1.6dee13569bf4ap-10,
                                    0x1.60cc2453952ffp-7,
                                    0x1.0ae74573fd0bcp-5,
                                    0x1.542679a826017p-5,
                                    0x1.3142704edaddap-6,
                                  },


              };


#define rteps         asinh_data.rteps
#define recrteps      asinh_data.recrteps
#define log2_lead     asinh_data.log2_lead
#define log2_tail     asinh_data.log2_tail
#define RANGES        asinh_data.ranges

#define A  asinh_data.poly_asinh_A
#define B  asinh_data.poly_asinh_B
#define C  asinh_data.poly_asinh_C
#define D  asinh_data.poly_asinh_D
#define E  asinh_data.poly_asinh_E
#define F  asinh_data.poly_asinh_F
#define G  asinh_data.poly_asinh_G
#define H  asinh_data.poly_asinh_H
#define J  asinh_data.poly_asinh_J

double
ALM_PROTO_OPT(asinh)(double x)
{
    double poly, absx, t, m1, m2, v2, m, rarg;
    uint64_t ax;
    int32_t xexp;
    uint8_t sign = 0;
    uint64_t ux = asuint64(x);

    /* Get sign of input value */
    if(ux & SIGNBIT_DP64)
        sign = 1;

    /* Get absolute value of input */
    ax = ux & (~SIGNBIT_DP64);
    absx = asdouble(ax);

    if (unlikely((ux & EXPBITS_DP64) == EXPBITS_DP64)){
        /* x is either NaN or infinity */
        if (unlikely(ux & MANTBITS_DP64)){
            // printf("1 : NAN case : %0.12f\n",x);
            if (ux & QNAN_MASK_64)
                return __alm_handle_error(ux|0x0008000000000000, AMD_F_NONE);
            else
                return __alm_handle_error(ux|0x0008000000000000, AMD_F_INVALID);
        }
        else {
            /* x is infinity. Return the same infinity. */
            return x;
        }
    }
    else if (ax < rteps){
        if (ax == RANGES[0]) {
        /* x is +/-zero. Return the same zero. */
            return x;
        }
        else{
            /* Tiny arguments approximated by asinh(x) = x
              - avoid slow operations on denormalized numbers */
#ifdef WINDOWS
            return x; //return val_with_flags(x,AMD_F_INEXACT);
#else
            return __alm_handle_error(ux, AMD_F_UNDERFLOW|AMD_F_INEXACT);
#endif
        }
    }

    if (unlikely(ax <= RANGES[1])) {
        /* abs(x) <= 1.0 */
        /* Arguments less than 1.0 in magnitude are
            approximated by [4,4] or [5,4] minimax polynomials
            fitted to asinh series 4.6.31 (x < 1) from Abramowitz and Stegun
        */
          t = x * x;
          if (ax < RANGES[2]) {
              /* [4,4] for 0 < abs(x) < 0.25 */
              poly =
                    POLY_EVAL_EVEN_8(x, A[0], A[1], A[2], A[3], A[4]) /
                    POLY_EVAL_EVEN_8(x, A[5], A[6], A[7], A[8], A[9]);
          }
          else if (ax < RANGES[3]) {
              /* [4,4] for 0.25 <= abs(x) < 0.5 */
              poly =
                    POLY_EVAL_EVEN_8(x, B[0], B[1], B[2], B[3], B[4]) /
                    POLY_EVAL_EVEN_8(x, B[5], B[6], B[7], B[8], B[9]);
          }
          else if (ax < RANGES[4]) {
              /* [4,4] for 0.5 <= abs(x) < 0.75 */
              poly =
                    POLY_EVAL_EVEN_8(x, C[0], C[1], C[2], C[3], C[4]) /
                    POLY_EVAL_EVEN_8(x, C[5], C[6], C[7], C[8], C[9]);
          }
          else {
              /* [5,4] for 0.75 <= abs(x) <= 1.0 */
              poly =
                    POLY_EVAL_EVEN_10(x, D[0], D[1], D[2], D[3], D[4], D[5]) /
                    POLY_EVAL_EVEN_8(x, D[6], D[7], D[8], D[9], D[10]);
          }

        return x + x * t * poly;
    }
    else if (unlikely(ax < RANGES[5])) {
        /* 1.0 <= abs(x) <= 32.0 */
        /* Arguments in this region are approximated by various
            minimax polynomials fitted to asinh series 4.6.31
            in Abramowitz and Stegun.
        */
        t = x * x;
        if (ax >= RANGES[6]) {
            /* [3,3] for 8.0 <= abs(x) <= 32.0 */
            poly =
                  POLY_EVAL_EVEN_6(x, E[0], E[1], E[2], E[3]) /
                  POLY_EVAL_EVEN_6(x, E[4], E[5], E[6], E[7]);
        }
        else if (ax >= RANGES[7]){
            /* [4,3] for 4.0 <= abs(x) <= 8.0 */
            poly =
                  POLY_EVAL_EVEN_8(x, F[0], F[1], F[2], F[3], F[4]) /
                  POLY_EVAL_EVEN_6(x, F[5], F[6], F[7], F[8]);
        }
        else if (ax >= RANGES[8]) {
            /* [5,4] for 2.0 <= abs(x) <= 4.0 */
            poly =
                  POLY_EVAL_EVEN_10(x, G[0], G[1], G[2], G[3], G[4], G[5]) /
                  POLY_EVAL_EVEN_8(x, G[6], G[7], G[8], G[9], G[10]);
        }
        else if (ax >= RANGES[9]) {
            /* [5,4] for 1.5 <= abs(x) <= 2.0 */
            poly =
                  POLY_EVAL_EVEN_10(x, H[0], H[1], H[2], H[3], H[4], H[5]) /
                  POLY_EVAL_EVEN_8(x, H[6], H[7], H[8], H[9], H[10]);
        }
        else{
            /* [5,5] for 1.0 <= abs(x) <= 1.5 */
            poly =
                  POLY_EVAL_EVEN_10(x, J[0], J[1], J[2], J[3], J[4], J[5]) /
                  POLY_EVAL_EVEN_10(x, J[6], J[7], J[8], J[9], J[10], J[11]);
        }

        log_kernel_amd64(absx, ax, &xexp, &m1, &m2);
        m1 = ((xexp + 1) * log2_lead + m1);
        m2 = ((xexp + 1) * log2_tail + m2);
        v2 = (poly + 0.25) / t;
        m = m1 + m2 + v2;

        /* Now (r1,r2) sum to log(2x). Add the term
            1/(2.2.x^2) = 0.25/t, and add poly/t, carefully
            to maintain precision. (Note that we add poly/t
            rather than poly because of the *x factor used
            when generating the minimax polynomial) */

        if (sign)
            return -m;
        else
            return m;

    }
    else {
        /* abs(x) > 32.0 */
        if (likely(ax > recrteps)) {
            /* Arguments greater than 1/sqrt(epsilon) in magnitude are
                approximated by asinh(x) = ln(2) + ln(abs(x)), with sign of x */
            /* log_kernel_amd(x) returns xexp, r1, r2 such that
                log(x) = xexp*log(2) + r1 + r2 */
            log_kernel_amd64(absx, ax, &xexp, &m1, &m2);
              /* Add (xexp+1) * log(2) to z1,z2 to get the result asinh(x).
                The computed r1 is not subject to rounding error because
                (xexp+1) has at most 10 significant bits, log(2) has 24 significant
                bits, and r1 has up to 24 bits; and the exponents of r1
                and r2 differ by at most 6. */
            m1 = ((xexp + 1) * log2_lead + m1);
            m2 = ((xexp + 1) * log2_tail + m2);

        }
        else {
            rarg = absx * absx + 1.0;
            /* Arguments such that 32.0 <= abs(x) <= 1/sqrt(epsilon) are
                approximated by
                  asinh(x) = ln(abs(x) + sqrt(x*x+1))
                with the sign of x (see Abramowitz and Stegun 4.6.20) */
            /* Use assembly instruction to compute r = sqrt(rarg); */
            m = sqrt(rarg);
            m += absx;
            ax = asuint64(m);
            log_kernel_amd64(m, ax, &xexp, &m1, &m2);
            m1 = (xexp * log2_lead + m1);
            m2 = (xexp * log2_tail + m2);

        }
            if (sign)
                return -(m1 + m2);
            else
                return m1 + m2;
    }
}