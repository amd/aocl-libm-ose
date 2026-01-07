/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
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
 * C implementation of cdfnorm double precision 512-bit vector version (vrd8)
 *
 * Signature:
 *    v_f64x8_t amd_vrd8_cdfnorm(v_f64x8_t a)
 *
 * Computes the standard normal cumulative distribution function:
 *    Φ(a) = (1/√(2π)) ∫_{-∞}^{a} exp(-t²/2) dt
 *
 * This can be expressed in terms of the error function:
 *    Φ(a) = (1/2)[1 + erf(a/√2)]
 *         = (1/2) erfc(-a/√2)
 *
 * SPECIFICATION:
 *    Φ(+∞) = 1
 *    Φ(-∞) = 0
 *    Φ(NaN) = NaN
 *    Φ is monotonically increasing
 *
 * IMPLEMENTATION NOTES
 * ====================
 *
 * This AVX-512 implementation uses mask registers (__mmask8) for efficient
 * conditional execution and blending, minimizing scalar fallback.
 *
 * Region 1 [a > -0.5]: Positive and small negative arguments
 *   Use transformation Φ(a) = (1/2)[1 + erf(a/√2)] with vector erf.
 *   Fully vectorized using amd_vrd8_erf.
 *
 * Region 2 [-1.5 < a ≤ -0.5]: Moderate negative arguments
 *   Use transformation Φ(a) = (1/2)erfc(|a|/√2) with vector erfc.
 *   Fully vectorized using amd_vrd8_erfc.
 *
 * Region 3 [-39 < a ≤ -1.5]: Large negative arguments
 *   Falls back to optimized scalar implementation for high accuracy.
 *   The scalar cdfnorm internally handles both rational approximations
 *   (for -10 < a ≤ -1.5) and asymptotic expansion (for a ≤ -10).
 *
 * Mixed Region 1 + Region 2 + Saturation:
 *   When elements span Region 1, Region 2, and/or saturation zones (but
 *   no special values or Region 3), both erf and erfc paths are computed
 *   and blended using AVX-512 mask operations. This avoids scalar fallback
 *   for this common mixed case.
 *
 * Special values and Region 3:
 *   When ANY element is Inf/NaN or in Region 3, ALL elements are processed
 *   via the scalar implementation to ensure correct handling.
 *
 * Saturation:
 *   - a ≥ 37.5: Returns 1.0 (applied via mask blend after main computation)
 *   - a ≤ -39.0: Returns 0.0 (applied via mask blend after main computation)
 *
 * Special cases:
 *   - Φ(+∞) = 1
 *   - Φ(-∞) = 0
 *   - Φ(NaN) = NaN
 *
 */

#include <stdint.h>
#include <immintrin.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly-vec.h>
#include <libm/arch/zen5.h>

static const struct {
    v_u64x8_t   sign_mask;
    v_u64x8_t   sign_zero;
    v_u64x8_t   inf_mask;
    v_u64x8_t   inf;
    v_f64x8_t   zero;
    v_f64x8_t   half;
    v_f64x8_t   one;
    v_f64x8_t   sqrth;
    v_f64x8_t   hi_cut;                    /* 37.5: saturation to 1.0 */
    v_f64x8_t   lo_cut;                    /* -39.0: underflow to 0.0 */
    v_f64x8_t   highprec_erfc_threshold;   /* -1.5: transition to Region 3 */
    v_f64x8_t   small_neg_threshold;       /* -0.5: Region 1/Region 2 boundary */
} v8_cdfnorm_data = {
    .sign_mask                = _MM512_SET1_U64x8((uint64_t)0x7FFFFFFFFFFFFFFFULL),
    .sign_zero                = _MM512_SET1_U64x8((uint64_t)0x0000000000000000ULL),
    .inf_mask                 = _MM512_SET1_U64x8((uint64_t)0x7FF0000000000000ULL),
    .inf                      = _MM512_SET1_U64x8((uint64_t)0x7ff0000000000000ULL),
    .zero                     = _MM512_SET1_PD8(0x0p+0),
    .half                     = _MM512_SET1_PD8(0x1p-1),
    .one                      = _MM512_SET1_PD8(0x1p+0),
    .sqrth                    = _MM512_SET1_PD8(0x1.6a09e667f3bcdp-1),  /* 1/√2 */
    .hi_cut                   = _MM512_SET1_PD8(0x1.2cp+5),             /* 37.5 */
    .lo_cut                   = _MM512_SET1_PD8(-0x1.38p+5),            /* -39.0 */
    .highprec_erfc_threshold  = _MM512_SET1_PD8(-0x1.8p+0),             /* -1.5 */
    .small_neg_threshold      = _MM512_SET1_PD8(-0x1p-1),            /* -0.5 */
};

#define SIGN_MASK            v8_cdfnorm_data.sign_mask
#define SIGN_ZERO            v8_cdfnorm_data.sign_zero
#define INF_MASK             v8_cdfnorm_data.inf_mask
#define INF                  v8_cdfnorm_data.inf
#define ZERO                 v8_cdfnorm_data.zero
#define HALF                 v8_cdfnorm_data.half
#define ONE                  v8_cdfnorm_data.one
#define SQRTH                v8_cdfnorm_data.sqrth
#define HI_CUT               v8_cdfnorm_data.hi_cut
#define LO_CUT               v8_cdfnorm_data.lo_cut
#define HIGHPREC_THRESHOLD   v8_cdfnorm_data.highprec_erfc_threshold
#define SMALL_NEG_THRESHOLD  v8_cdfnorm_data.small_neg_threshold

#define SCALAR_CDFNORM ALM_PROTO_OPT(cdfnorm)

v_f64x8_t
ALM_PROTO_ARCH_ZN5(vrd8_cdfnorm)(v_f64x8_t a) {
    v_f64x8_t result;

    /* Extract absolute value */
    v_u64x8_t ua = as_v8_u64_f64(a);
    v_u64x8_t ua_abs = ua & SIGN_MASK;
    v_f64x8_t a_abs = as_v8_f64_u64(ua_abs);

    /*
     * Create masks using AVX-512 mask registers directly.
     * This is more efficient than vector comparisons + helper functions.
     */
    __mmask8 k_inf_nan = _mm512_cmp_epu64_mask(ua_abs, INF_MASK, _MM_CMPINT_NLT);
    __mmask8 k_sat_one = _mm512_cmp_pd_mask(a, HI_CUT, _CMP_GE_OQ);
    __mmask8 k_sat_zero = _mm512_cmp_pd_mask(a, LO_CUT, _CMP_LE_OQ);
    
    /*
     * Region 1: a > -0.5 (positive and small negative, uses erf)
     * Region 2: -1.5 < a <= -0.5 (moderate negative, uses erfc)
     * Region 3: a <= -1.5 (large negative, needs scalar for high precision)
     */
    __mmask8 k_region1 = _mm512_cmp_pd_mask(a, SMALL_NEG_THRESHOLD, _CMP_GT_OQ);
    __mmask8 k_region2 = ((__mmask8)(~k_region1)) & _mm512_cmp_pd_mask(a, HIGHPREC_THRESHOLD, _CMP_GT_OQ);
    
    /*
     * Region 3: negative values that are NOT in region1, NOT in region2, AND NOT saturating to zero.
     * This covers -39 < a <= -1.5, which the scalar cdfnorm handles with
     * high-precision rational approximations and asymptotic expansion.
     */
    __mmask8 k_region3 = ((__mmask8)(~k_region1)) & ((__mmask8)(~k_region2)) & ((__mmask8)(~k_sat_zero));

    /*
     * Handle special values (Inf/NaN) and Region 3 via scalar.
     * Region 3 needs scalar for long double precision accuracy.
     * If ANY element needs scalar, process ALL elements via scalar
     * to ensure correct results.
     */
    __mmask8 k_needs_scalar = k_inf_nan | k_region3;
    
    if (unlikely(k_needs_scalar)) {
        for (uint64_t i = 0; i < 8; i++) {
            result[i] = SCALAR_CDFNORM(a[i]);
        }
        return result;
    }

    /* Fast path: all elements saturate to 1 */
    if (k_sat_one == 0xFF) {
        return ONE;
    }
    
    /* Fast path: all elements saturate to 0 */
    if (k_sat_zero == 0xFF) {
        return ZERO;
    }

    /* Fast path: all in Region 1 (a > -0.5) */
    if (k_region1 == 0xFF) {
        v_f64x8_t x = _mm512_mul_pd(a, SQRTH);
        v_f64x8_t erf_x = ALM_PROTO(vrd8_erf)(x);
        return _mm512_add_pd(HALF, _mm512_mul_pd(HALF, erf_x));
    }

    /* Fast path: all in Region 2 (-1.5 < a <= -0.5) */
    if (k_region2 == 0xFF) {
        v_f64x8_t x = _mm512_mul_pd(a_abs, SQRTH);
        v_f64x8_t erfc_x = ALM_PROTO(vrd8_erfc)(x);
        return _mm512_mul_pd(HALF, erfc_x);
    }

    /*
     * Mixed case: combination of Region 1, Region 2, and saturation.
     * Compute both erf and erfc paths, then blend using masks.
     */
    
    /* Region 1: Φ(a) = 0.5 * (1 + erf(a/√2)) for a > -0.5 */
    v_f64x8_t x_r1 = _mm512_mul_pd(a, SQRTH);
    v_f64x8_t erf_x = ALM_PROTO(vrd8_erf)(x_r1);
    v_f64x8_t result_r1 = _mm512_add_pd(HALF, _mm512_mul_pd(HALF, erf_x));
    
    /* Region 2: Φ(a) = 0.5 * erfc(|a|/√2) for -1.5 < a <= -0.5 */
    v_f64x8_t x_r2 = _mm512_mul_pd(a_abs, SQRTH);
    v_f64x8_t erfc_x = ALM_PROTO(vrd8_erfc)(x_r2);
    v_f64x8_t result_r2 = _mm512_mul_pd(HALF, erfc_x);
    
    /* Blend Region 1 and Region 2 */
    result = _mm512_mask_blend_pd(k_region1, result_r2, result_r1);
    
    /* Apply saturation overrides */
    result = _mm512_mask_blend_pd(k_sat_one, result, ONE);
    result = _mm512_mask_blend_pd(k_sat_zero, result, ZERO);
    
    return result;
}