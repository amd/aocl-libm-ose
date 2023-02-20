/*
 * Copyright (C) 2018-2020, Advanced Micro Devices. All rights reserved.
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
 *
 */

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/poly-vec.h>
#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>

/* Contains implementation of double log(double x)
 * Reduce x into the form:
 *        x = (-1)^s*2^n*m
 * s will be always zero, as log is defined for posititve numbers
 * n is an integer known as the exponent
 * m is mantissa
 *
 * x is reduced such that the mantissa, m lies in [2/3,4/3]
 *      x = 2^n*m where m is in [2/3,4/3]
 *      log2(x) = log2(2^n*m)              We have log(a*b) = log(a)+log(b)
 *             = log2(2^n) + log2(m)       We have log(a^n) = n*log(a)
 *             = n + log2(m)
 *             = n + log2(1+(m-1))
 *             = n + ln(1+f) * log2(e)          Where f = m-1
 *             = n + log1p(f) * log2(e)         f lies in [-1/3,+1/3]
 *
 * Thus we have :
 * log(x) = n + log1p(f) * log2(e)
 * The second term log1p(F) is approximated by using a polynomial
 */

#define VECTOR_SIZE 2

static struct {
    double poly_log[20];
    v_f64x2_t ln2, ln2_head, ln2_tail;
    v_u64x2_t inf, v_max, v_min;
    v_u64x2_t two_by_three;
} vrd2_log2_data = {
    .two_by_three = _MM_SET1_I64x2(0x3fe5555555555555),
    .inf = _MM_SET1_I64x2(0xfff0000000000000),
    .ln2 = _MM_SET1_PD2(0x1.62e42fefa39efp-1),
    .ln2_head = _MM_SET1_PD2(1.44269180297851562500E+00),
    .ln2_tail = _MM_SET1_PD2(3.23791044778235969970E-06),
    .v_max = _MM_SET1_I64x2(0x7ff0000000000000),
    .v_min = _MM_SET1_I64x2(0x0010000000000000),
    /* Polynomial coefficients obtained using fpminimax algorithm from Sollya */
    .poly_log = {
        0x1p0,
        -0x1.ffffffffffff8p-2,
        0x1.5555555555b1p-2,
        -0x1.00000000014eep-2,
        0x1.99999998c65d2p-3,
        -0x1.5555555359624p-3,
        0x1.24924982d3265p-3,
        -0x1.000000b030e18p-3,
        0x1.c71c47299f643p-4,
        -0x1.9999569fc809fp-4,
        0x1.74629af3a9782p-4,
        -0x1.555cf41b1e0bfp-4,
        0x1.3aa51860d5cd6p-4,
        -0x1.24080742fb868p-4,
        0x1.1660dbc68088p-4,
        -0x1.061fa86db3d64p-4,
        0x1.9471a01ce7ab9p-5,
        -0x1.73ac251462367p-5,
        0x1.6c6c793f08f4dp-4,
        -0x1.63f7ba7a7111cp-4,
    },
};

#define SCALAR_LOG2 ALM_PROTO(log2)

#define EXPSHIFTBITS_SP64 52
#define ln2 vrd2_log2_data.ln2
#define ln2_head vrd2_log2_data.ln2_head
#define ln2_tail vrd2_log2_data.ln2_tail
#define TWO_BY_THREE vrd2_log2_data.two_by_three
#define INF vrd2_log2_data.inf
#define C0  _MM_SET1_PD2(0.0)
#define C1 _MM_SET1_PD2(vrd2_log2_data.poly_log[0])
#define C2 _MM_SET1_PD2(vrd2_log2_data.poly_log[1])
#define C3 _MM_SET1_PD2(vrd2_log2_data.poly_log[2])
#define C4 _MM_SET1_PD2(vrd2_log2_data.poly_log[3])
#define C5 _MM_SET1_PD2(vrd2_log2_data.poly_log[4])
#define C6 _MM_SET1_PD2(vrd2_log2_data.poly_log[5])
#define C7 _MM_SET1_PD2(vrd2_log2_data.poly_log[6])
#define C8 _MM_SET1_PD2(vrd2_log2_data.poly_log[7])
#define C9 _MM_SET1_PD2(vrd2_log2_data.poly_log[8])
#define C10 _MM_SET1_PD2(vrd2_log2_data.poly_log[9])
#define C11 _MM_SET1_PD2(vrd2_log2_data.poly_log[10])
#define C12 _MM_SET1_PD2(vrd2_log2_data.poly_log[11])
#define C13 _MM_SET1_PD2(vrd2_log2_data.poly_log[12])
#define C14 _MM_SET1_PD2(vrd2_log2_data.poly_log[13])
#define C15 _MM_SET1_PD2(vrd2_log2_data.poly_log[14])
#define C16 _MM_SET1_PD2(vrd2_log2_data.poly_log[15])
#define C17 _MM_SET1_PD2(vrd2_log2_data.poly_log[16])
#define C18 _MM_SET1_PD2(vrd2_log2_data.poly_log[17])
#define C19 _MM_SET1_PD2(vrd2_log2_data.poly_log[18])
#define C20 _MM_SET1_PD2(vrd2_log2_data.poly_log[19])
#define V_MIN vrd2_log2_data.v_min
#define V_MAX vrd2_log2_data.v_max


__m128d
ALM_PROTO_OPT(vrd2_log2) (__m128d x)
{

    v_f64x2_t m, r, n, f;

    v_i64x2_t ix;

    ix = as_v2_i64_f64(x);

    v_u64x2_t condition = (as_v2_u64_f64(x) - V_MIN >= V_MAX - V_MIN);

    ix = (ix - TWO_BY_THREE) & INF;

    for(int i = 0; i < VECTOR_SIZE; i++) {

       n[i] = (double)(ix[i] >> EXPSHIFTBITS_SP64);

    }

    /* Reduce the mantissa, m to [2/3, 4/3] */

    m = as_v2_f64_u64(as_v2_u64_f64(x) - ix);

    /* f is in [-1/3,+1/3] */
    
    f = m - C1;

    /* Compute log1p(f) using Polynomial approximation
     *
     * r = C0 + f*C1 + f^2*C2 + f^3*C3 + .... + f^20*C20
     *
     */

    r =  POLY_EVAL_20(f, C0, C1, C2, C3, C4, C5, C6, C7,
                         C8, C9, C10, C11, C12, C13, C14,
                         C15, C16, C17, C18, C19, C20);

    /* Addition by using head and tail */

    r = (n + ln2_tail * r) + r * ln2_head ;

    if (unlikely(any_v2_u64_loop(condition))) {

        return (v_f64x2_t) {

           condition[0] ? SCALAR_LOG2(x[0]) : r[0],

           condition[1] ? SCALAR_LOG2(x[1]) : r[1],

        };

    }

    return r;
}


