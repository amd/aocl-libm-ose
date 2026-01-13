/*
 * Copyright (C) 2025-2026 Advanced Micro Devices, Inc. All rights reserved.
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

#include <libm_util_amd.h>
#include <libm/alm_special.h>

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>

#include <libm/arch/zen4.h>

/*
 * C implementation of roundf single precision 512-bit vector version (vrs16)
 *
 * Signature:
 *   v_f32x16_t amd_vrs16_roundf(v_f32x16_t x)
 *
 * Computes roundf() of 16 32-bit float values packed in a vector.
 *
 * SPECIFICATION:
 *    roundf(x) rounds x to the nearest integer value
 *    roundf(±∞) = ±∞
 *    roundf(NaN) = NaN
 *    roundf(±0) = ±0
 *    Ties (fractional part = 0.5f) round away from zero
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 * Implements "round to nearest, ties away from zero" semantics
 * for 16 packed single precision values using AVX-512 vector operations.
 *
 * The implementation uses a two-phase approach to correctly handle tie cases:
 *   1. Round using IEEE "round to nearest-even"
 *   2. Detect exact ties (remainder == 0.5f) and round away from zero
 *
 * Algorithm:
 *   1. Extract sign bits and compute absolute values
 *   2. Apply round-to-nearest-even using roundscale instruction
 *      - For |x| >= 2^23: Returns input unchanged (already integer)
 *   3. Calculate fractional part: remainder = abs_x - rounded_even
 *   4. Detect ties via exact floating-point comparison (remainder == 0.5f)
 *   5. Use AVX-512 opmask operations to conditionally add 1.0f for ties
 *   6. Restore original sign to final result
 *
 * Special cases:
 *   - Special values (±∞, NaN, ±0) preserved through the algorithm
 *
 ******************************************
 */

static const struct {
    v_f32x16_t half;
    v_f32x16_t one;
    v_f32x16_t sign;
} v_rndf16_data  = {
    .half = _MM512_SET1_PS16(0.5f),
    .one  = _MM512_SET1_PS16(1.0f),
    .sign = _MM512_SET1_PS16(-0.0f),
};
#define HALF16_SP32      v_rndf16_data.half
#define SIGN16_SP32      v_rndf16_data.sign
#define ONE16_SP32       v_rndf16_data.one

v_f32x16_t
ALM_PROTO_ARCH_ZN4(vrs16_roundf)(v_f32x16_t x)
{
    /* Extract sign bits and compute absolute values */
    v_f32x16_t sign_bits = _mm512_and_ps(x, SIGN16_SP32);
    v_f32x16_t abs_x = _mm512_andnot_ps(SIGN16_SP32, x);

    /* Round to nearest-even (for |x| >= 2^23, returns input unchanged) */
    v_f32x16_t rounded_even = _mm512_roundscale_ps(abs_x, _MM_FROUND_TO_NEAREST_INT);

    /* Calculate fractional part (ties yield exactly 0.5f) */
    v_f32x16_t remainder = _mm512_sub_ps(abs_x, rounded_even);

    /* Detect ties (remainder == 0.5f) using AVX-512 opmask */
    __mmask16 is_tie = _mm512_cmp_ps_mask(remainder, HALF16_SP32, _CMP_EQ_OQ);

    /* Masked move: 1.0f for ties, 0.0f for non-ties */
    v_f32x16_t adjustment = _mm512_maskz_mov_ps(is_tie, ONE16_SP32);

    /* Apply adjustment (adds 1.0f for ties to round away from zero) */
    v_f32x16_t result_abs = _mm512_add_ps(rounded_even, adjustment);

    /* Restore original sign */
    v_f32x16_t result = _mm512_or_ps(result_abs, sign_bits);

    return result;
}
