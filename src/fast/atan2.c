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

/* Contains implementation of double atan2(double y, double x)
 *
 * Table-driven atan2(y, x) with per-interval minimax polynomials.
 * 64-interval degree-7 minimax with branchless special case handling.
 * Each interval: 8 coefficients x 8 bytes = 64 bytes = 1 cache line.
 *
 *   1. Extract |x|, |y|, track signs
 *   2. Branchless special-case on integer ALU (parallel to FP pipeline)
 *   3. Scale subnormals to normal range
 *   4. Swap to ensure v <= u  (ratio r = v/u in [0, 1])
 *   5. Single division:  r = v / u
 *   6. Table index:  j = floor(r * 64),  j in {0, ..., 63}
 *   7. Degree-7 minimax polynomial in Estrin form
 *   8. Branchless blend of special or polynomial result
 */

#include <stdint.h>
#include <libm/alm_special.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>
#include "atan_data.h"

double
ALM_PROTO_FAST(atan2)(double y, double x)
{
    uint64_t ux = asuint64(x);
    uint64_t uy = asuint64(y);

    uint64_t xneg = ux & (~ATAN_FAST_SIGN_MASK);
    uint64_t yneg = uy & (~ATAN_FAST_SIGN_MASK);

    uint64_t aux = ux & ATAN_FAST_SIGN_MASK;
    uint64_t auy = uy & ATAN_FAST_SIGN_MASK;

    /* Branchless special-case on integer ALU (parallel to FP pipeline)
     *
     * Special cases handled here (y=0, x=0):
     *  atan2(+0,  +x)  = +0        atan2(-0,  +x)  = -0
     *  atan2(+0,  -x)  = +pi       atan2(-0,  -x)  = -pi
     *  atan2(+0,  +0)  = +0        atan2(-0,  +0)  = -0
     *  atan2(+0,  -0)  = +pi       atan2(-0,  -0)  = -pi
     *  atan2(+y,  +0)  = +pi/2     atan2(-y,  +0)  = -pi/2
     *  atan2(+y,  -0)  = +pi/2     atan2(-y,  -0)  = -pi/2
     *
     * Not handled (NaN/Inf inputs)
     * Sign of result always matches sign of y, since atan2 returns
     * values in (-pi, pi] and sign(y) determines the half-plane.
     */
    uint64_t pi_bits    = asuint64(ATAN_FAST_PI);
    uint64_t piby2_bits = asuint64(ATAN_FAST_PIBY2);

    uint64_t y_zero_mask = -(uint64_t)(auy == 0);
    uint64_t x_zero_mask = -(uint64_t)(aux == 0);
    uint64_t is_special  = y_zero_mask | x_zero_mask;

    uint64_t x_neg_mask    = (uint64_t)((int64_t)xneg >> 63);
    uint64_t y_zero_result = (x_neg_mask & pi_bits) | yneg;
    uint64_t x_zero_result = piby2_bits | yneg;

    uint64_t special_result = (y_zero_mask & y_zero_result)
                            | (~y_zero_mask & x_zero_result);

    /* FP path proceeds unconditionally */
    double u = asdouble(aux);
    double v = asdouble(auy);

    if (unlikely((aux | auy) < ATAN_FAST_SUBNORM_LIM)) {
        u *= ATAN_FAST_SCALE_UP;
        v *= ATAN_FAST_SCALE_UP;
        if (unlikely(u==0.0)) 
            u = 1.0; /* avoid FE_INVALID exception from divide by 0 */
    }

    uint32_t swapped = (v > u);
    if (swapped) {
        double tmp = u;
        u = v;
        v = tmp;
    }

    double r = v / u;

    double poly;

    if (unlikely(!(r >= ATAN_FAST_TINY_R))) {
        poly = r; /* atan is linear for tiny r*/
    } else {
        unsigned j = (unsigned)(int)(r * ATAN_FAST_TBL_N); /* safe as r is always +ve */
        j = j > 63 ? 63 : j;

        double a_j = (double)j * ATAN_FAST_INV_N;
        double d = r - a_j; /* delta within interval */

        const double *c = ATAN_FAST_TBL[j];

        /* Estrin evaluation of degree-7 polynomial:
         * p(d) = c0 + c1*d + c2*d^2 + c3*d^3 + c4*d^4 + c5*d^5 + c6*d^6 + c7*d^7
         * 3-level tree: depth = 3 FMA latencies
         */
        double d2 = d * d;
        double d4 = d2 * d2;

        double p1 = c[0] + c[1] * d;
        double p2 = c[2] + c[3] * d;
        double p3 = c[4] + c[5] * d;
        double p4 = c[6] + c[7] * d;

        double q1 = p1 + p2 * d2;
        double q2 = p3 + p4 * d2;

        poly = q1 + q2 * d4;
    }

    /* Map atan(|y|/|x|) result in [0, pi/2] to full circle */
    if (swapped) poly = ATAN_FAST_PIBY2 - poly;
    if (xneg)    poly = ATAN_FAST_PI - poly;

    /* Branchless blend: select special or polynomial result */
    uint64_t poly_result  = asuint64(poly) | yneg;
    uint64_t final_result = (is_special & special_result)
                          | (~is_special & poly_result);

    return asdouble(final_result);
}

strong_alias (__atan2_finite, ALM_PROTO_FAST(atan2))
weak_alias (amd_atan2, ALM_PROTO_FAST(atan2))
weak_alias (atan2, ALM_PROTO_FAST(atan2))
