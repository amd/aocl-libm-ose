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

/*
 * Signature:
 *    v_f64x2_t amd_vrd2_round(v_f64x2_t x)
 *
 * Computes round() of 2 64-bit double values packed in a vector
 *
 * *************************************************************
 * Implementation Notes
 * ---------------------
 *   Implements "round to nearest, ties away from zero" semantics
 *   for both elements simultaneously using vector operations.
 *
 *   Algorithm:
 *   1. For typical values with |x| < 2^52:
 *      a. Extract sign bits from both input elements
 *      b. Create ±0.5 by ORing sign bits with +0.5 constant
 *      c. Add x + (±0.5) in floating-point
 *      d. Round result toward zero to complete rounding
 *   2. For special cases (|x| >= 2^52, NaN, Inf):
 *      Use vector blend to return x unchanged (round(x) = x for these)
 *
 *   This correctly implements: sign(x) * trunc(|x| + 0.5)
 *   which rounds halfway cases away from zero as required by round().
 *
 *  *************************************************************
 */

/* Threshold: 2^52 = 0x4330000000000000 */
#define ROUND_ARG_MAX _MM_SET1_PD2(0x1.0p52)

v_f64x2_t
ALM_PROTO_OPT(vrd2_round)(v_f64x2_t x)
{
    /* Convert to integer representation for bit operations */
    v_u64x2_t ux = as_v2_u64_f64(x);

    /* Extract sign bits and magnitude from both input elements */
    v_u64x2_t sign = ux & SIGNBIT_DP64;
    v_u64x2_t abs_ux = ux & ~SIGNBIT_DP64;
    v_f64x2_t abs_x = as_v2_f64_u64(abs_ux);

    /* Check if |x| >= 2^52 (already an integer) */
    v_f64x2_t is_large = _mm_cmpge_pd(abs_x, ROUND_ARG_MAX);

    /* Create ±0.5 for both elements by ORing sign bits with +0.5 */
    v_u64x2_t signed_half = HALFEXPBITS_DP64 | sign;

    /* Add x + (±0.5) for both elements */
    v_f64x2_t sum = x + as_v2_f64_u64(signed_half);

    /* Round toward zero using SSE4.1 roundpd to complete "round half away from zero" */
    v_f64x2_t rounded = _mm_round_pd(sum, _MM_FROUND_TO_ZERO);

    /* Select result: use x for large values, rounded for small values */
    return _mm_blendv_pd(rounded, x, is_large);
}
