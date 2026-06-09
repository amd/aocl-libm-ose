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
 * vrs8_log2f inline kernel — fast-path only, no special-case handling.
 *
 * Prerequisites (must be satisfied by the including .c file):
 *   #include <libm_util_amd.h>          // MANTBITS_SP32
 *   #include <libm_macros.h>
 *   #include <libm/types.h>
 *   #include <libm/typehelper-vec.h>     // _MM256_SET1_*, as_v8_*, cast_v8_*
 *   #include <libm/compiler.h>           // ALM_ALWAYS_INLINE
 *   #define  AMD_LIBM_FMA_USABLE 1
 *   #include <libm/poly-vec.h>           // POLY_EVAL_9  (10-arg version)
 *
 * Precondition on inputs:
 *   _x must contain only positive normal floats (no zero, subnormal,
 *   inf, or NaN).  The caller must handle those cases separately.
 */

#ifndef VRS8_LOG2F_DATA_H
#define VRS8_LOG2F_DATA_H

static const struct {
    v_u32x8_t v_mask, v_off;
    v_f32x8_t v_one;
    v_f32x8_t poly[10];
} v8_log2f_data = {
    .v_mask = _MM256_SET1_I32(MANTBITS_SP32),
    .v_off  = _MM256_SET1_I32(0x3f2aaaab),
    .v_one  = _MM256_SET1_PS8(1.0f),
    .poly = {
        _MM256_SET1_PS8(0.0f),
        _MM256_SET1_PS8(0x1.7154770a8161cp0f),
        _MM256_SET1_PS8(-0x1.715433cb77dfep-1f),
        _MM256_SET1_PS8(0x1.ec6e977351a82p-2f),
        _MM256_SET1_PS8(-0x1.71811bf7a93d7p-2f),
        _MM256_SET1_PS8(0x1.27d4a45f12fcp-2f),
        _MM256_SET1_PS8(-0x1.e335fcfb7fcb4p-3f),
        _MM256_SET1_PS8(0x1.9889eeabc6284p-3f),
        _MM256_SET1_PS8(-0x1.cf3075d14b775p-3f),
        _MM256_SET1_PS8(0x1.b2c4e715a67f9p-3f),
    },
};

#define V8_LOG2F_V_MASK  v8_log2f_data.v_mask
#define V8_LOG2F_V_OFF   v8_log2f_data.v_off
#define V8_LOG2F_V_ONE   v8_log2f_data.v_one
#define V8_LOG2F_C0      v8_log2f_data.poly[0]
#define V8_LOG2F_C1      v8_log2f_data.poly[1]
#define V8_LOG2F_C2      v8_log2f_data.poly[2]
#define V8_LOG2F_C3      v8_log2f_data.poly[3]
#define V8_LOG2F_C4      v8_log2f_data.poly[4]
#define V8_LOG2F_C5      v8_log2f_data.poly[5]
#define V8_LOG2F_C6      v8_log2f_data.poly[6]
#define V8_LOG2F_C7      v8_log2f_data.poly[7]
#define V8_LOG2F_C8      v8_log2f_data.poly[8]
#define V8_LOG2F_C9      v8_log2f_data.poly[9]

static inline ALM_ALWAYS_INLINE v_f32x8_t
vrs8_log2f_fastpath(v_f32x8_t _x)
{
    v_u32x8_t vx = as_v8_u32_f32(_x);

    vx -= V8_LOG2F_V_OFF;

    v_f32x8_t n = cast_v8_i32_to_f32(((v_i32x8_t)vx) >> 23);

    vx &= V8_LOG2F_V_MASK;
    vx += V8_LOG2F_V_OFF;

    v_f32x8_t r = as_v8_f32_u32(vx) - V8_LOG2F_V_ONE;

    v_f32x8_t poly = POLY_EVAL_9(r,
        V8_LOG2F_C0, V8_LOG2F_C1,
        V8_LOG2F_C2, V8_LOG2F_C3,
        V8_LOG2F_C4, V8_LOG2F_C5,
        V8_LOG2F_C6, V8_LOG2F_C7,
        V8_LOG2F_C8, V8_LOG2F_C9);

    return n + poly;
}

#endif /* VRS8_LOG2F_DATA_H */
