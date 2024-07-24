/*
 * Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
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

#include <libm_macros.h>
#include <libm/compiler.h>
#include <libm_util_amd.h>
#include <libm/types.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly-vec.h>

#include <libm/arch/zen5.h>

/* Contains implementation of double log(double x)
 * Reduce x into the form:
 *        x = (-1)^s*2^n*m
 * s will be always zero, as log is defined for posititve numbers
 * n is an integer known as the exponent
 * m is mantissa
 *
 * x is reduced such that the mantissa, m lies in [2/3,4/3]
 *      x = 2^n*m where m is in [2/3,4/3]
 *      log(x) = log(2^n*m)         		We have log(a*b) = log(a)+log(b)
 *             = log(2^n) + log(m)   		We have log(a^n) = n*log(a)
 *             = n*log(2) + log(m)
 *             = n*log(2) + log(1+(m-1))
 *             = n*log(2) + log(1+f)        Where f = m-1
 *             = n*log(2) + log1p(f)        f lies in [-1/3,+1/3]
 *
 * Thus we have :
 * log(x) = n*log(2) + log1p(f)
 * In the above, the first term n*log(2), n can be calculated by using right-shift
 * and the value of log(2) is a compile time constant
 * The second term log1p(F) is approximated by using a polynomial
 */

#define VECTOR_SIZE 8

static struct {
    v_f64x8_t poly_log[20];
    v_f64x8_t ln2, ln2_head, ln2_tail;
    v_u64x8_t inf, v_max, v_min;
    v_u64x8_t two_by_three;
} log_data = {
    .two_by_three = _MM512_SET1_U64x8(0x3fe5555555555555UL),
    .inf          = _MM512_SET1_U64x8(0xfff0000000000000UL),
    .ln2          = _MM512_SET1_PD8(0x1.62e42fefa39efp-1),
    .ln2_head     = _MM512_SET1_PD8(0x1.63p-1),
    .ln2_tail     = _MM512_SET1_PD8(-0x1.bd0105c610ca8p-13),
    .v_max        = _MM512_SET1_U64x8(0x7ff0000000000000UL),
    .v_min        = _MM512_SET1_U64x8(0x0010000000000000UL),
    /* Polynomial coefficients obtained using fpminimax algorithm from Sollya */
    .poly_log     = {
        _MM512_SET1_PD8(0x1.0p0),
        _MM512_SET1_PD8(-0x1.ffffffffffff8p-2),
        _MM512_SET1_PD8(0x1.5555555555b1p-2),
        _MM512_SET1_PD8(-0x1.00000000014eep-2),
        _MM512_SET1_PD8(0x1.99999998c65d2p-3),
        _MM512_SET1_PD8(-0x1.5555555359624p-3),
        _MM512_SET1_PD8(0x1.24924982d3265p-3),
        _MM512_SET1_PD8(-0x1.000000b030e18p-3),
        _MM512_SET1_PD8(0x1.c71c47299f643p-4),
        _MM512_SET1_PD8(-0x1.9999569fc809fp-4),
        _MM512_SET1_PD8(0x1.74629af3a9782p-4),
        _MM512_SET1_PD8(-0x1.555cf41b1e0bfp-4),
        _MM512_SET1_PD8(0x1.3aa51860d5cd6p-4),
        _MM512_SET1_PD8(-0x1.24080742fb868p-4),
        _MM512_SET1_PD8(0x1.1660dbc68088p-4),
        _MM512_SET1_PD8(-0x1.061fa86db3d64p-4),
        _MM512_SET1_PD8(0x1.9471a01ce7ab9p-5),
        _MM512_SET1_PD8(-0x1.73ac251462367p-5),
        _MM512_SET1_PD8(0x1.6c6c793f08f4dp-4),
        _MM512_SET1_PD8(-0x1.63f7ba7a7111cp-4),
    },
};

static inline v_f64x8_t
v8_log_specialcase(v_f64x8_t _x,
                   v_f64x8_t result,
                   v_u64x8_t cond)
{
    return call_v8_f64(ALM_PROTO(log), _x, result, cond);
}

#define EXPSHIFTBITS_SP64 52
#define V8_LN2          log_data.ln2
#define V8_LN2_HEAD     log_data.ln2_head
#define V8_LN2_TAIL     log_data.ln2_tail
#define V8_TWO_BY_THREE log_data.two_by_three
#define V8_INF log_data.inf
#define C0  _MM512_SET1_PD8(0.0)
#define C1  log_data.poly_log[0]
#define C2  log_data.poly_log[1]
#define C3  log_data.poly_log[2]
#define C4  log_data.poly_log[3]
#define C5  log_data.poly_log[4]
#define C6  log_data.poly_log[5]
#define C7  log_data.poly_log[6]
#define C8  log_data.poly_log[7]
#define C9  log_data.poly_log[8]
#define C10 log_data.poly_log[9]
#define C11 log_data.poly_log[10]
#define C12 log_data.poly_log[11]
#define C13 log_data.poly_log[12]
#define C14 log_data.poly_log[13]
#define C15 log_data.poly_log[14]
#define C16 log_data.poly_log[15]
#define C17 log_data.poly_log[16]
#define C18 log_data.poly_log[17]
#define C19 log_data.poly_log[18]
#define C20 log_data.poly_log[19]
#define V_MIN log_data.v_min
#define V_MAX log_data.v_max


v_f64x8_t
ALM_PROTO_ARCH_ZN5(vrd8_log) (v_f64x8_t x)
{
    v_f64x8_t m, r, n, f;
    v_u64x8_t ix;

    ix = as_v8_i64_f64(x);

    v_u64x8_t condition = (as_v8_u64_f64(x) - V_MIN >= V_MAX - V_MIN);

    ix = (ix - V8_TWO_BY_THREE) & V8_INF;

    v_i64x8_t int_exponent = (v_i64x8_t)ix;

    v_i32x8_t int32_exponent;

    for(int i = 0; i < VECTOR_SIZE; i++) {

       int32_exponent[i] = (int32_t)(int_exponent[i]  >> EXPSHIFTBITS_SP64);

    }

    n = (v_f64x8_t)_mm512_cvtepi32_pd((__m256i)int32_exponent);

	/* Reduce the mantissa, m to [2/3, 4/3] */

    m = as_v8_f64_u64(as_v8_u64_f64(x) - ix);

    f = m - C1;                         /* f is in [-1/3,+1/3] */

    /* Compute log1p(f) using Polynomial approximation
     *
     * r = C0 + f*C1 + f^2*C2 + f^3*C3 + .... + f^20*C20
     *
     */

    r =  POLY_EVAL_20(f,
                      C0,  C1,  C2,  C3,  C4,  C5,  C6, C7,
                      C8,  C9,  C10, C11, C12, C13, C14,
                      C15, C16, C17, C18, C19, C20);

	/* Addition by using head and tail */

    r = n * V8_LN2_HEAD + (n * V8_LN2_TAIL + r);

    if (unlikely(any_v8_u64_loop(condition))) {
           return v8_log_specialcase(x, r, condition);
    }

    return r;
}


