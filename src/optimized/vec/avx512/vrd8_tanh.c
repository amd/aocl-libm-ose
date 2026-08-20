/*
 * Copyright (C) 2026, Advanced Micro Devices, Inc. All rights reserved.
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
 * v_f64x8_t vrd8_tanh(v_f64x8_t x)  -- AVX-512, 8 x double
 *
 * ---------------------
 * Implementation Notes
 * ---------------------
 * tanh(x) = sign(x) * q / (q + 2),  q = e^{2|x|} - 1.
 * The argument is folded to |x| so q >= 0 (avoids cancellation in q + 2)
 * and the sign is reapplied at the end. q = e^{2|x|} - 1 uses an inline expm1:
 *   1. Argument reduction: 2|x| = n*ln2 + f, n = round(2|x|/ln2), |f| <= ln2/2
 *      (ln2 split into head/tail; n obtained via the "huge" round-to-int trick).
 *   2. Polynomial: expm1(f) = f + f^2 * P(f), deg-9 minimax, Horner via FMA.
 *   3. Reconstruction: t = 2^n (integer-shift); q = expm1(f)*t + (t - 1).
 * For |x| > 18.8 the result saturates to sign*1.0; Inf/NaN use a scalar
 * fallback. Max ULP: <3.
 */

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>

#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly-vec.h>

static struct {
    v_f64x8_t two_by_ln2, ln2_head, ln2_tail;
    v_f64x8_t huge, one, two, bound;
    v_i64x8_t exp_bias;
    v_u64x8_t one_bits, arg_max;
    v_f64x8_t poly[10];
} tanh_v8_data = {
    .two_by_ln2 = _MM512_SET1_PD8(0x1.71547652b82fep+1),   /* 2/ln2            */
    .ln2_head   = _MM512_SET1_PD8(0x1.62e42fefa39efp-1),   /* ln2 head         */
    .ln2_tail   = _MM512_SET1_PD8(0x1.abc9e3b39803fp-56),  /* ln2 tail         */
    .huge       = _MM512_SET1_PD8(0x1.8000000000000p+52),  /* round-to-int aid */
    .one        = _MM512_SET1_PD8(1.0),
    .two        = _MM512_SET1_PD8(2.0),
    .bound      = _MM512_SET1_PD8(0x1.2cccccccccccdp+4),   /* 18.8 saturation  */
    .exp_bias   = _MM512_SET1_I64x8((int64_t)EXPBIAS_DP64),
    .one_bits   = _MM512_SET1_U64x8(0x3ff0000000000000UL), /* bit pattern 1.0  */
    .arg_max    = _MM512_SET1_U64x8(0x7ff0000000000000UL), /* Inf/NaN threshold*/
    .poly = {
        _MM512_SET1_PD8(0x1.0000000000001p-1),
        _MM512_SET1_PD8(0x1.555555555554cp-3),
        _MM512_SET1_PD8(0x1.5555555553ce8p-5),
        _MM512_SET1_PD8(0x1.11111111149cap-7),
        _MM512_SET1_PD8(0x1.6c16c178a5883p-10),
        _MM512_SET1_PD8(0x1.a01a018a45851p-13),
        _MM512_SET1_PD8(0x1.a019b8cc9a7a7p-16),
        _MM512_SET1_PD8(0x1.71de5ebeb00cfp-19),
        _MM512_SET1_PD8(0x1.2891bbf4ad67fp-22),
        _MM512_SET1_PD8(0x1.aea39a22839cp-26),
    },
};

#define ALM_TANH_V8_TWO_BY_LN2   tanh_v8_data.two_by_ln2
#define ALM_TANH_V8_LN2_HEAD     tanh_v8_data.ln2_head
#define ALM_TANH_V8_LN2_TAIL     tanh_v8_data.ln2_tail
#define ALM_TANH_V8_HUGE_VAL     tanh_v8_data.huge
#define ALM_TANH_V8_ONE          tanh_v8_data.one
#define ALM_TANH_V8_TWO          tanh_v8_data.two
#define ALM_TANH_V8_BOUND        tanh_v8_data.bound
#define ALM_TANH_V8_EXP_BIAS     tanh_v8_data.exp_bias
#define ALM_TANH_V8_ONE_BITS     tanh_v8_data.one_bits
#define ALM_TANH_V8_ARG_MAX      tanh_v8_data.arg_max

#define C0 tanh_v8_data.poly[0]
#define C1 tanh_v8_data.poly[1]
#define C2 tanh_v8_data.poly[2]
#define C3 tanh_v8_data.poly[3]
#define C4 tanh_v8_data.poly[4]
#define C5 tanh_v8_data.poly[5]
#define C6 tanh_v8_data.poly[6]
#define C7 tanh_v8_data.poly[7]
#define C8 tanh_v8_data.poly[8]
#define C9 tanh_v8_data.poly[9]

#define SCALAR_TANH ALM_PROTO_OPT(tanh)

static inline v_f64x8_t
vrd8_tanh_specialcase(v_f64x8_t _x, v_f64x8_t result, v_u64x8_t cond)
{
    return call_v8_f64(SCALAR_TANH, _x, result, cond);
}

v_f64x8_t
ALM_PROTO_OPT(vrd8_tanh)(v_f64x8_t x)
{
    v_u64x8_t ux   = as_v8_u64_f64(x);
    v_u64x8_t sign = ux & SIGNBIT_DP64;
    v_u64x8_t axu  = ux & ~SIGNBIT_DP64;
    v_f64x8_t ax   = as_v8_f64_u64(axu);

    v_u64x8_t cond = axu >= ALM_TANH_V8_ARG_MAX;

    v_f64x8_t axr = _mm512_min_pd(ax, ALM_TANH_V8_BOUND);

    v_f64x8_t w  = axr + axr;
    v_f64x8_t dn = axr * ALM_TANH_V8_TWO_BY_LN2 + ALM_TANH_V8_HUGE_VAL;
    v_i64x8_t n  = as_v8_i64_f64(dn);
    dn = dn - ALM_TANH_V8_HUGE_VAL;
    v_f64x8_t f = w - dn * ALM_TANH_V8_LN2_HEAD;
    f = f - dn * ALM_TANH_V8_LN2_TAIL;

    v_f64x8_t P = POLY_EVAL_HORNER_10_1(f, C0, C1, C2, C3, C4,
                                        C5, C6, C7, C8, C9);
    v_f64x8_t f2 = f * f;
    v_f64x8_t p  = mul_add(f2, P, f);

    v_i64x8_t m = (n + ALM_TANH_V8_EXP_BIAS) << 52;
    v_f64x8_t t = as_v8_f64_i64(m);
    v_f64x8_t q = mul_add(p, t, t - ALM_TANH_V8_ONE);

    v_f64x8_t y = q / (q + ALM_TANH_V8_TWO);

    v_u64x8_t big = (v_u64x8_t)(ax > ALM_TANH_V8_BOUND);
    v_u64x8_t mag = (ALM_TANH_V8_ONE_BITS & big) |
                    (as_v8_u64_f64(y) & ~big);
    v_f64x8_t result = as_v8_f64_u64(mag | sign);

    if (unlikely(any_v8_u64_avx512(cond)))
        result = vrd8_tanh_specialcase(x, result, cond);

    return result;
}
