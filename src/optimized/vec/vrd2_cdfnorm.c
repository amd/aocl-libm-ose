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
 * C implementation of cdfnorm double precision 128-bit vector version (vrd2)
 *
 * Signature:
 *    v_f64x2_t amd_vrd2_cdfnorm(v_f64x2_t a)
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
 * The implementation uses a hybrid vectorized/scalar approach:
 *
 * Region 1 [a > -0.5]: Positive and small negative arguments
 *   Use transformation Φ(a) = (1/2)[1 + erf(a/√2)] with vector erf.
 *   Fully vectorized using amd_vrd2_erf.
 *   For small |a|, erf accurately represents small deviations from 0.5,
 *   while erfc(small_x) ≈ 1 loses precision in the deviation from 1.
 *
 * Region 2 [-1.5 < a ≤ -0.5]: Moderate negative arguments
 *   Use transformation Φ(a) = (1/2)erfc(|a|/√2) with vector erfc.
 *   Fully vectorized using amd_vrd2_erfc.
 *
 * Region 3 [-39 < a ≤ -1.5]: Large negative arguments
 *   Falls back to optimized scalar implementation for high accuracy.
 *   The scalar version uses long double precision and interval-specific
 *   polynomials/rational approximations.
 *
 * Mixed regions:
 *   When vector elements span multiple regions (including mixed special
 *   and normal values), falls back to scalar processing for all elements.
 *
 * Special cases:
 *   - a ≥ 37.5: Returns 1.0 (saturation)
 *   - a ≤ -39.0: Returns 0.0 (underflow)
 *   - Φ(+∞) = 1
 *   - Φ(-∞) = 0
 *   - Φ(NaN) = NaN
 *
 */

#include <stdint.h>
#include <emmintrin.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly-vec.h>

/* Mask constants for vector comparison operations */
#define VEC_ALL_ONES_U64    0xFFFFFFFFFFFFFFFFULL

static const struct {
    v_u64x2_t   sign_mask;
    v_u64x2_t   sign_zero;
    v_u64x2_t   inf_mask;
    v_u64x2_t   inf;
    v_f64x2_t   zero;
    v_f64x2_t   half;
    v_f64x2_t   one;
    v_f64x2_t   sqrth;
    v_f64x2_t   hi_cut;                    /* 37.5: saturation to 1.0 */
    v_f64x2_t   lo_cut;                    /* -39.0: underflow to 0.0 */
    v_f64x2_t   highprec_erfc_threshold;   /* -1.5: transition to Region 3 */
    v_f64x2_t   small_neg_threshold;       /* -0.5: Region 1/Region 2 boundary */
} v_cdfnorm_data = {
    .sign_mask                = _MM_SET1_I64x2(0x7FFFFFFFFFFFFFFFULL),
    .sign_zero                = _MM_SET1_I64x2(0x0000000000000000ULL),
    .inf_mask                 = _MM_SET1_I64x2(0x7FF0000000000000ULL),
    .inf                      = _MM_SET1_I64x2(0x7ff0000000000000ULL),
    .zero                     = _MM_SET1_PD2(0x0p+0),
    .half                     = _MM_SET1_PD2(0x1p-1),
    .one                      = _MM_SET1_PD2(0x1p+0),
    .sqrth                    = _MM_SET1_PD2(0x1.6a09e667f3bcdp-1),  /* 1/√2 */
    .hi_cut                   = _MM_SET1_PD2(0x1.2cp+5),             /* 37.5 */
    .lo_cut                   = _MM_SET1_PD2(-0x1.38p+5),            /* -39.0 */
    .highprec_erfc_threshold  = _MM_SET1_PD2(-0x1.8p+0),            /* -1.5 */
    .small_neg_threshold      = _MM_SET1_PD2(-0x1p-1),               /* -0.5 */
};

#define SIGN_MASK            v_cdfnorm_data.sign_mask
#define SIGN_ZERO            v_cdfnorm_data.sign_zero
#define INF_MASK             v_cdfnorm_data.inf_mask
#define INF                  v_cdfnorm_data.inf
#define ZERO                 v_cdfnorm_data.zero
#define HALF                 v_cdfnorm_data.half
#define ONE                  v_cdfnorm_data.one
#define SQRTH                v_cdfnorm_data.sqrth
#define HI_CUT               v_cdfnorm_data.hi_cut
#define LO_CUT               v_cdfnorm_data.lo_cut
#define HIGHPREC_THRESHOLD   v_cdfnorm_data.highprec_erfc_threshold
#define SMALL_NEG_THRESHOLD  v_cdfnorm_data.small_neg_threshold

#define SCALAR_CDFNORM ALM_PROTO_OPT(cdfnorm)

/*
 * Helper function to test if a condition is true for all vector elements
 */
static inline int 
test_condition_all(v_u64x2_t cond) {
    return (cond[0] == VEC_ALL_ONES_U64) && 
           (cond[1] == VEC_ALL_ONES_U64);
}

/*
 * Helper function to test if a condition is true for any vector element
 */
static inline int 
test_condition_any(v_u64x2_t cond) {
    return (cond[0] == VEC_ALL_ONES_U64) || 
           (cond[1] == VEC_ALL_ONES_U64);
}

v_f64x2_t
ALM_PROTO_OPT(vrd2_cdfnorm)(v_f64x2_t a) {
    v_f64x2_t result;

    /* Extract absolute value and sign */
    v_u64x2_t ua = as_v2_u64_f64(a);
    v_u64x2_t ua_abs = ua & SIGN_MASK;
    v_f64x2_t a_abs = as_v2_f64_u64(ua_abs);
    v_u64x2_t sign = ua & ~SIGN_MASK;
    v_u64x2_t is_positive = (sign == SIGN_ZERO);

    /*
     * Handle special values: infinities and NaN
     * 
     * If ALL elements are special (Inf or NaN), handle them vectorially.
     * If mixed with normal values, fall back to scalar to ensure
     * normal values are computed correctly.
     */
    v_u64x2_t inf_nan_mask = (ua_abs >= INF_MASK);
    
    if (unlikely(test_condition_any(inf_nan_mask))) {
        if (test_condition_all(inf_nan_mask)) {
            /* All elements are Inf or NaN - fast vector path */
            v_u64x2_t is_inf = (ua_abs == INF);
            
            /* For infinity: return 0 if -∞, 1 if +∞ */
            v_f64x2_t inf_result = _mm_blendv_pd(ZERO, ONE, as_v2_f64_u64(is_positive));
            
            /* For NaN: return NaN (a + a) */
            v_f64x2_t nan_result = _mm_add_pd(a, a);
            
            /* Select between infinity result and NaN result */
            result = _mm_blendv_pd(nan_result, inf_result, as_v2_f64_u64(is_inf));
            return result;
        }
        /* Mixed special + normal values: use scalar fallback */
        for (uint64_t i = 0; i < 2; i++) {
            result[i] = SCALAR_CDFNORM(a[i]);
        }
        return result;
    }

    /*
     * Check for saturation to 0 or 1 for extreme values
     */
    v_u64x2_t sat_one_mask = a >= HI_CUT;
    v_u64x2_t sat_zero_mask = a <= LO_CUT;
    
    /* Fast path: all saturate to 1 */
    if (test_condition_all(sat_one_mask)) {
        return ONE;
    }
    
    /* Fast path: all saturate to 0 */
    if (test_condition_all(sat_zero_mask)) {
        return ZERO;
    }

    /*
     * Region 1 [a > -0.5]: Positive and small negative arguments
     * Use Φ(a) = (1/2)[1 + erf(a/√2)]
     * 
     * For small |a|, erf accurately represents small deviations from 0,
     * while erfc(small_x) ≈ 1 loses precision in the deviation from 1.
     */
    v_u64x2_t region1_mask = (a > SMALL_NEG_THRESHOLD);
    
    if (test_condition_all(region1_mask)) {
        v_f64x2_t x = _mm_mul_pd(a, SQRTH);
        v_f64x2_t erf_x = ALM_PROTO_OPT(vrd2_erf)(x);
        result = _mm_add_pd(HALF, _mm_mul_pd(HALF, erf_x));
        return result;
    }

    /*
     * Region 2 [-1.5 < a ≤ -0.5]: Moderate negative arguments
     * Use Φ(a) = (1/2)erfc(-a/√2)
     * Fully vectorized path
     */
    v_u64x2_t region2_mask = (~region1_mask) & (a > HIGHPREC_THRESHOLD);
    
    if (test_condition_all(region2_mask)) {
        /* x = |a|/√2, computed as a_abs * (1/√2) */
        v_f64x2_t x = _mm_mul_pd(a_abs, SQRTH);
        v_f64x2_t erfc_x = ALM_PROTO_OPT(vrd2_erfc)(x);
        result = _mm_mul_pd(HALF, erfc_x);
        return result;
    }
    /*
     * Region 3 [a ≤ -1.5]: Large negative arguments
     * OR mixed regions where elements span different regions
     * Fall back to scalar implementation for high accuracy
     */
    for (uint64_t i = 0; i < 2; i++) {
        result[i] = SCALAR_CDFNORM(a[i]);
    }
    
    return result;
}