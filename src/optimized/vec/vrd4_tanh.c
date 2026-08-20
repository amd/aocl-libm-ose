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
 * v_f64x4_t vrd4_tanh(v_f64x4_t x)  -- AVX2, 4 x double
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
 * For |x| > 18.8 the result saturates to sign*1.0; Inf/NaN use a per-lane
 * scalar fallback. Uses no AVX-512-only ops. Max ULP: <3.
 */

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly-vec.h>

#define DOUBLE_PRECISION_BIAS 1023

static const struct {
    v_f64x4_t two_by_ln2, ln2_head, ln2_tail;
    v_f64x4_t huge, one, two, bound;
    v_i64x4_t exp_bias, sign_mask, abs_mask, one_bits;
    v_f64x4_t poly[10];
} tanh_v4_data = {
    .two_by_ln2 = _MM_SET1_PD4(0x1.71547652b82fep+1),   /* 2/ln2 */
    .ln2_head   = _MM_SET1_PD4(0x1.62e42fefa39efp-1),
    .ln2_tail   = _MM_SET1_PD4(0x1.abc9e3b39803fp-56),
    .huge       = _MM_SET1_PD4(0x1.8000000000000p+52),
    .one        = _MM_SET1_PD4(1.0),
    .two        = _MM_SET1_PD4(2.0),
    .bound      = _MM_SET1_PD4(0x1.2cccccccccccdp+4),   /* 18.8 */
    .exp_bias   = _MM_SET1_I64(DOUBLE_PRECISION_BIAS),
    .sign_mask  = _MM_SET1_I64((int64_t)0x8000000000000000ULL),
    .abs_mask   = _MM_SET1_I64((int64_t)0x7FFFFFFFFFFFFFFFULL),
    .one_bits   = _MM_SET1_I64((int64_t)0x3FF0000000000000ULL),
    .poly = {
        _MM_SET1_PD4(0x1.0000000000001p-1),
        _MM_SET1_PD4(0x1.555555555554cp-3),
        _MM_SET1_PD4(0x1.5555555553ce8p-5),
        _MM_SET1_PD4(0x1.11111111149cap-7),
        _MM_SET1_PD4(0x1.6c16c178a5883p-10),
        _MM_SET1_PD4(0x1.a01a018a45851p-13),
        _MM_SET1_PD4(0x1.a019b8cc9a7a7p-16),
        _MM_SET1_PD4(0x1.71de5ebeb00cfp-19),
        _MM_SET1_PD4(0x1.2891bbf4ad67fp-22),
        _MM_SET1_PD4(0x1.aea39a22839cp-26),
    },
};

#define TWO_BY_LN2 tanh_v4_data.two_by_ln2
#define LN2_HEAD   tanh_v4_data.ln2_head
#define LN2_TAIL   tanh_v4_data.ln2_tail
#define HUGE_RND   tanh_v4_data.huge
#define ONE        tanh_v4_data.one
#define TWO        tanh_v4_data.two
#define BOUND      tanh_v4_data.bound
#define EXP_BIAS   tanh_v4_data.exp_bias
#define SIGN_MASK  tanh_v4_data.sign_mask
#define ABS_MASK   tanh_v4_data.abs_mask
#define ONE_BITS   tanh_v4_data.one_bits

#define C0 tanh_v4_data.poly[0]
#define C1 tanh_v4_data.poly[1]
#define C2 tanh_v4_data.poly[2]
#define C3 tanh_v4_data.poly[3]
#define C4 tanh_v4_data.poly[4]
#define C5 tanh_v4_data.poly[5]
#define C6 tanh_v4_data.poly[6]
#define C7 tanh_v4_data.poly[7]
#define C8 tanh_v4_data.poly[8]
#define C9 tanh_v4_data.poly[9]

#define ARG_MAX     0x7ff0000000000000ULL
#define SCALAR_TANH ALM_PROTO_OPT(tanh)

v_f64x4_t
ALM_PROTO_OPT(vrd4_tanh)(v_f64x4_t x)
{
    v_i64x4_t ux   = as_v4_i64_f64(x);
    v_i64x4_t sign = ux & SIGN_MASK;
    v_i64x4_t axi  = ux & ABS_MASK;
    v_f64x4_t ax   = as_v4_f64_i64(axi);

    v_f64x4_t axr = _mm256_min_pd(ax, BOUND);

    v_f64x4_t w  = axr + axr;
    v_f64x4_t dn = axr * TWO_BY_LN2 + HUGE_RND;
    v_i64x4_t n  = as_v4_i64_f64(dn);
    dn = dn - HUGE_RND;
    v_f64x4_t f = w - dn * LN2_HEAD;
    f = f - dn * LN2_TAIL;

    v_f64x4_t P  = POLY_EVAL_HORNER_10_1(f, C0, C1, C2, C3, C4,
                                         C5, C6, C7, C8, C9);
    v_f64x4_t f2 = f * f;
    v_f64x4_t p  = mul_add(f2, P, f);

    v_i64x4_t m = (n + EXP_BIAS) << 52;
    v_f64x4_t t = as_v4_f64_i64(m);
    v_f64x4_t q = mul_add(p, t, t - ONE);

    v_f64x4_t y = q / (q + TWO);

    v_i64x4_t big = (v_i64x4_t)(ax > BOUND);
    v_i64x4_t mag = (ONE_BITS & big) | (as_v4_i64_f64(y) & ~big);
    v_f64x4_t result = as_v4_f64_i64(mag | sign);

    for (int i = 0; i < 4; i++) {
        if (unlikely((uint64_t)axi[i] >= ARG_MAX))
            result[i] = SCALAR_TANH(x[i]);
    }

    return result;
}
