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
 * Constants for vrs8_exp10f (8-wide exp10f).
 *
 * Include before this file (order matters for macros and vector types):
 *   libm_util_amd.h
 *   libm_macros.h
 *   libm/types.h
 *   libm/typehelper-vec.h
 */

#ifndef VRS8_EXP10F_DATA_H
#define VRS8_EXP10F_DATA_H

static const struct {
    v_f32x8_t neg_arg_max;    /* ~37.93: scalar when x < -neg_arg_max */
    v_f32x8_t pos_arg_max;    /* ~38.53: scalar when x >= pos_arg_max */
    v_f32x8_t log2_10_hi;     /* high part of log2(10) */
    v_f32x8_t log2_10_lo;     /* tail   part of log2(10) */
    v_f32x8_t huge;
    v_f32x8_t poly[8];        /* 2^f coefficients, D0..D7  (Dk = ln2^k / k!) */
} v8_exp10f_data = {
    /*
     * Vector fast-path domain: -neg_arg_max <= x < pos_arg_max
     * (pos_arg_max matches scalar EXP10F_FARG_MAX / log10(FLT_MAX)).
     */
    .neg_arg_max = _MM256_SET1_PS8(0x1.2f703p+5f),     /* ~37.93 */
    .pos_arg_max = _MM256_SET1_PS8(0x1.344136p5f),    /* ~38.53 = log10(FLT_MAX) */
    .log2_10_hi  = _MM256_SET1_PS8(0x1.a934fp+1f),    /* 3.321928024  */
    .log2_10_lo  = _MM256_SET1_PS8(0x1.2f346ep-24f),  /* 7.05954e-08  */
    .huge        = _MM256_SET1_PS8(0x1.8p+23f),
    /*
     * 2^f Remez/Taylor coefficients evaluated directly in f (no separate
     * r = f*ln2 multiply): Dk = ln2^k / k!, so the ln2 scaling is folded
     * into the constants and incurs no extra runtime rounding.
     */
    .poly = {
        _MM256_SET1_PS8(0x1p+0f),           /* 1            */
        _MM256_SET1_PS8(0x1.62e43p-1f),     /* ln2          */
        _MM256_SET1_PS8(0x1.ebfbep-3f),     /* ln2^2 / 2    */
        _MM256_SET1_PS8(0x1.c6b08ep-5f),    /* ln2^3 / 6    */
        _MM256_SET1_PS8(0x1.3b2ab6p-7f),    /* ln2^4 / 24   */
        _MM256_SET1_PS8(0x1.5d87fep-10f),   /* ln2^5 / 120  */
        _MM256_SET1_PS8(0x1.430912p-13f),   /* ln2^6 / 720  */
        _MM256_SET1_PS8(0x1.ffcbfcp-17f),   /* ln2^7 / 5040 */
    },
};

#define V8_EXP10F_NEG_ARG_MAX  v8_exp10f_data.neg_arg_max
#define V8_EXP10F_POS_ARG_MAX  v8_exp10f_data.pos_arg_max
#define V8_EXP10F_LOG2_10_HI   v8_exp10f_data.log2_10_hi
#define V8_EXP10F_LOG2_10_LO   v8_exp10f_data.log2_10_lo
#define V8_EXP10F_HUGE         v8_exp10f_data.huge
#define V8_EXP10F_E0           v8_exp10f_data.poly[0]
#define V8_EXP10F_E1           v8_exp10f_data.poly[1]
#define V8_EXP10F_E2           v8_exp10f_data.poly[2]
#define V8_EXP10F_E3           v8_exp10f_data.poly[3]
#define V8_EXP10F_E4           v8_exp10f_data.poly[4]
#define V8_EXP10F_E5           v8_exp10f_data.poly[5]
#define V8_EXP10F_E6           v8_exp10f_data.poly[6]
#define V8_EXP10F_E7           v8_exp10f_data.poly[7]

#endif /* VRS8_EXP10F_DATA_H */

