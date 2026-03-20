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

/*
 * C implementation of round double precision 256-bit vector version (vrd4)
 *
 * Signature:
 *   v_f64x4_t amd_vrd4_round(v_f64x4_t x)
 *
 * Computes round() of 4 64-bit double values packed in a vector.
 *
 * SPECIFICATION:
 *    round(x) rounds x to the nearest integer value
 *    round(±∞) = ±∞
 *    round(NaN) = NaN
 *    round(±0) = ±0
 *    Ties (fractional part = 0.5) round away from zero
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 * Implements "round to nearest, ties away from zero" semantics
 * for 4 packed double precision values using AVX vector operations.
 *
 * The implementation uses a two-phase approach to correctly handle tie cases:
 *   1. Round using IEEE "round to nearest-even"
 *   2. Detect exact ties (remainder == 0.5) and round away from zero
 *
 * Algorithm:
 *   1. Extract sign bits and compute absolute values
 *   2. Apply round-to-nearest-even using roundpd instruction
 *      - For |x| >= 2^52: Returns input unchanged (already integer)
 *   3. Calculate fractional part: remainder = abs_x - rounded_even
 *   4. Detect ties via exact floating-point comparison (remainder == 0.5)
 *   5. Use vector mask operations to conditionally add 1.0 for ties
 *   6. Restore original sign to final result
 *
 * Special cases:
 *   - Special values (±∞, NaN, ±0) preserved through the algorithm
 *
 ******************************************
 */

static struct {
    v_f64x4_t half;
    v_f64x4_t one;
    v_f64x4_t sign;
} v_rnd4_data  = {
    .half = _MM_SET1_PD4(0.5),
    .one  = _MM_SET1_PD4(1.0),
    .sign = _MM_SET1_PD4(-0.0),
};
#define HALF4_DP64      v_rnd4_data.half
#define SIGN4_DP64      v_rnd4_data.sign
#define ONE4_DP64       v_rnd4_data.one

v_f64x4_t
ALM_PROTO_OPT(vrd4_round)(v_f64x4_t x)
{
    /* Extract sign bits and compute absolute values */
    v_f64x4_t sign_bits = _mm256_and_pd(x, SIGN4_DP64);
    v_f64x4_t abs_x = _mm256_andnot_pd(SIGN4_DP64, x);

    /* Round to nearest-even (for |x| >= 2^52, returns input unchanged) */
    v_f64x4_t rounded_even = _mm256_round_pd(abs_x, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);

    /* Calculate fractional part (ties yield exactly 0.5) */
    v_f64x4_t remainder = _mm256_sub_pd(abs_x, rounded_even);

    /* Detect ties (remainder == 0.5) */
    v_f64x4_t is_tie = _mm256_cmp_pd(remainder, HALF4_DP64, _CMP_EQ_OQ);

    /* Create adjustment vector: 1.0 for ties, 0.0 for non-ties */
    v_f64x4_t adjustment = _mm256_and_pd(is_tie, ONE4_DP64);

    /* Apply adjustment (adds 1.0 for ties to round away from zero) */
    v_f64x4_t result_abs = _mm256_add_pd(rounded_even, adjustment);

    /* Restore original sign */
    v_f64x4_t result = _mm256_or_pd(result_abs, sign_bits);

    return result;
}
