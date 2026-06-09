/*
 * Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
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
 * vrs8_exp2f inline kernel — fast-path only, no special-case handling.
 *
 * Prerequisites (must be satisfied by the including .c file):
 *   #include <libm_macros.h>
 *   #include <libm/types.h>
 *   #include <libm/typehelper-vec.h>     // _MM256_SET1_*, as_v8_*
 *   #include <libm/compiler.h>           // ALM_ALWAYS_INLINE
 *   #define  AMD_LIBM_FMA_USABLE 1
 *   #include <libm/poly-vec.h>           // POLY_EVAL_9  (10-arg version)
 *
 * Precondition on inputs:
 *   _x must be in the wrapper's fast-path range where exp2f does not
 *   overflow or underflow.  The caller must
 *   handle larger or smaller inputs separately.
 */

#ifndef VRS8_EXP2F_DATA_H
#define VRS8_EXP2F_DATA_H

static const struct {
    v_f32x8_t huge;
    v_f32x8_t ln2;
    v_i32x8_t bias;
    v_f32x8_t poly[9];
} v8_exp2f_data = {
    .huge  = _MM256_SET1_PS8(0x1.8p+23f),
    .ln2   = _MM256_SET1_PS8(0x1.62e42fefa39efp-1f),
    .bias  = _MM256_SET1_I32(127),
    .poly = {
        _MM256_SET1_PS8(0x1p0f),
        _MM256_SET1_PS8(0x1p-1f),
        _MM256_SET1_PS8(0x1.5555555555555p-3f),
        _MM256_SET1_PS8(0x1.5555555555556p-5f),
        _MM256_SET1_PS8(0x1.111111111110fp-7f),
        _MM256_SET1_PS8(0x1.6c16c16bee4d5p-10f),
        _MM256_SET1_PS8(0x1.a01a01a093e4ep-13f),
        _MM256_SET1_PS8(0x1.a01aba8ed59e3p-16f),
        _MM256_SET1_PS8(0x1.71ddb3d62728ep-19f),
    },
};

#define V8_EXP2F_HUGE   v8_exp2f_data.huge
#define V8_EXP2F_LN2    v8_exp2f_data.ln2
#define V8_EXP2F_BIAS   v8_exp2f_data.bias
#define V8_EXP2F_C1     v8_exp2f_data.poly[0]
#define V8_EXP2F_C2     v8_exp2f_data.poly[1]
#define V8_EXP2F_C3     v8_exp2f_data.poly[2]
#define V8_EXP2F_C4     v8_exp2f_data.poly[3]
#define V8_EXP2F_C5     v8_exp2f_data.poly[4]
#define V8_EXP2F_C6     v8_exp2f_data.poly[5]
#define V8_EXP2F_C7     v8_exp2f_data.poly[6]
#define V8_EXP2F_C8     v8_exp2f_data.poly[7]
#define V8_EXP2F_C9     v8_exp2f_data.poly[8]

static inline ALM_ALWAYS_INLINE v_f32x8_t
vrs8_exp2f_fastpath(v_f32x8_t _x)
{
    v_f32x8_t dn = _x + V8_EXP2F_HUGE;

    v_u32x8_t n = as_v8_u32_f32(dn);

    dn = dn - V8_EXP2F_HUGE;

    v_f32x8_t r = _x - dn;
    r *= V8_EXP2F_LN2;

    /*
     * Equivalent to original vrs8_exp2f.c:
     *   C1 + POLY_EVAL_9(r, C1, C2, C3, C4, C5, C6, C7, C8, C9)
     *
     * Using poly-vec.h's 10-arg POLY_EVAL_9 with c0=0 so that
     * C1 (= 1.0) is added externally at the end, preserving
     * precision by keeping the large constant out of the early FMA steps.
     */
    v_f32x8_t zero = _MM256_SET1_PS8(0.0f);

    v_f32x8_t poly = V8_EXP2F_C1 + POLY_EVAL_9(r,
        zero,        V8_EXP2F_C1,
        V8_EXP2F_C2, V8_EXP2F_C3,
        V8_EXP2F_C4, V8_EXP2F_C5,
        V8_EXP2F_C6, V8_EXP2F_C7,
        V8_EXP2F_C8, V8_EXP2F_C9);

    /*
     * result = poly * 2^n.
     * Since 2^n is exactly representable, this is equivalent to adding n
     * directly into poly's biased exponent, which avoids the trailing
     * multiply on the critical path. Same idiom as the f32 / f64 sibling
     * kernels (vrs4_expf.c, vrs4_exp2f.c). Safe because the fast-path
     * contract above already excludes overflow / underflow inputs.
     */
    return as_v8_f32_u32(as_v8_u32_f32(poly) + (n << 23));
}

#endif /* VRS8_EXP2F_DATA_H */
