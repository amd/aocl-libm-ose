/*
 * Copyright (C) 2025 Advanced Micro Devices, Inc. All rights reserved.
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
 * Signature:
 *    v_f64x8_t amd_vrd8_round(v_f64x8_t x)
 *
 * Computes round() of 8 64-bit double values packed in a vector
 *
 * *************************************************************
 * Implementation Notes
 * ---------------------
 *   Implements "round to nearest, ties away from zero" semantics
 *   for all eight elements simultaneously using vector operations.
 *
 *   Algorithm:
 *   1. For typical values with |x| < 2^52:
 *      a. Extract sign bits from all eight input elements
 *      b. Create ±0.5 by ORing sign bits with +0.5 constant
 *      c. Add x + (±0.5) in floating-point
 *      d. Round result toward zero to complete rounding
 *   2. For special cases (|x| >= 2^52, NaN, Inf):
 *      Use vector blend to return x unchanged (round(x) = x for these)
 *
 *   This correctly implements: sign(x) * trunc(|x| + 0.5)
 *   which rounds halfway cases away from zero as required by round().
 *
 * Requires: AVX-512F (Zen4 or later)
 * *************************************************************
 */

/* Threshold: 2^52 = 0x4330000000000000 */
#define ROUND_ARG_MAX _mm512_set1_pd(0x1.0p52)

v_f64x8_t
ALM_PROTO_ARCH_ZN4(vrd8_round)(v_f64x8_t x)
{
    /* Convert to integer representation for bit operations */
    v_u64x8_t ux = as_v8_u64_f64(x);

    /* Extract sign bits and magnitude from all eight input elements */
    v_u64x8_t sign = ux & SIGNBIT_DP64;
    v_u64x8_t abs_ux = ux & ~SIGNBIT_DP64;
    v_f64x8_t abs_x = as_v8_f64_u64(abs_ux);

    /* Check if |x| >= 2^52 (already an integer) */
    __mmask8 is_large = _mm512_cmp_pd_mask(abs_x, ROUND_ARG_MAX, _CMP_GE_OQ);

    /* Create ±0.5 for all eight elements by ORing sign bits with +0.5 */
    v_u64x8_t signed_half = HALFEXPBITS_DP64 | sign;

    /* Add x + (±0.5) for all eight elements */
    v_f64x8_t sum = x + as_v8_f64_u64(signed_half);

    /* Round toward zero using AVX-512 roundscale to complete "round half away from zero" */
    v_f64x8_t rounded = _mm512_roundscale_pd(sum, _MM_FROUND_TO_ZERO);

    /* Select result: use x for large values, rounded for small values */
    return _mm512_mask_blend_pd(is_large, rounded, x);
}
