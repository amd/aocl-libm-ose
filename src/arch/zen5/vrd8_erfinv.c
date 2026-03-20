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
 * C implementation of erfinv double precision 512-bit vector version (vrd8)
 *
 * Signature:
 *    v_f64x8_t amd_vrd8_erfinv(v_f64x8_t x)
 *
 * Computes the inverse error function erfinv(x) for a given input vector x.
 *
 * SPEC:
 *    erfinv(±0)   = ±0
 *    erfinv(±1)   = ±Inf, DIVBYZERO Exception
 *    erfinv(±Inf) =  QNaN, INVALID Exception
 *    erfinv(QNaN) =  QNaN
 *    erfinv(SNaN) =  QNaN, INVALID Exception
 *    erfinv(|x|>1) = QNaN, INVALID Exception
 *
 * IMPLEMENTATION NOTES
 * ====================
 *
 * This AVX-512 implementation uses mask registers (__mmask8) for efficient
 * conditional execution and blending, minimizing scalar fallback.
 *
 * The function uses rational Chebyshev approximations for different ranges
 * of x, as described in Blair et al. (1976).
 *
 * The function uses polynomial approximations for different ranges of |x|:
 *
 * Range 1: |x| <= 0.75
 *    erfinv(x) = x * P1(x^2 - offset1) / Q1(x^2 - offset1)
 *    Uses degree-7 rational approximation
 *
 * Range 2: 0.75 < |x| <= 0.9375
 *    erfinv(x) = x * P2(x^2 - offset2) / Q2(x^2 - offset2)
 *    Uses degree-8 rational approximation
 *
 * Range 3: 0.9375 < |x| < 1.0
 *    erfinv(x) = sign(x) * s^-1 * P3(s) / Q3(s)
 *    where s = sqrt(-log1p(-|x|))
 *    Uses degree-11/9 rational approximation
 *
 * When all vector elements fall within the same region, the fast vector path
 * is used. When elements span two adjacent regions (Range 1+2 or Range 2+3),
 * both polynomial paths are computed and blended using _mm512_mask_blend_pd
 * intrinsics, avoiding expensive scalar fallback. Only when elements span all
 * three regions does the implementation fall back to scalar processing.
 *
 * Reference:
 *   J.M. Blair, C.A. Edwards, J.H. Johnson, "Rational Chebyshev Approximations
 *   for the Inverse of the Error Function", Mathematics of Computation,
 *   Vol. 30, No. 136 (Oct. 1976), pp. 827-830.
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
#include "../../optimized/erfinv_data.h"

static const struct {
  v_u64x8_t  bound1, bound2;
  v_u64x8_t  sign_mask, abs_mask;
  v_u64x8_t  one, inf, zero;
  v_f64x8_t  offset1, offset2;
  v_f64x8_t  poly_range1_p[7];  /* P100-P106 */
  v_f64x8_t  poly_range1_q[7];  /* Q100-Q106 */
  v_f64x8_t  poly_range2_p[8];  /* P200-P207 */
  v_f64x8_t  poly_range2_q[8];  /* Q200-Q207 */
  v_f64x8_t  poly_range3_p[11]; /* P300-P310 */
  v_f64x8_t  poly_range3_q[9];  /* Q300-Q308 */
} v_erfinv_data = {
  .bound1    = _MM512_SET1_U64x8((uint64_t)ERFINV_BOUND1),
  .bound2    = _MM512_SET1_U64x8((uint64_t)ERFINV_BOUND2),
  .sign_mask = _MM512_SET1_U64x8((uint64_t)ERFINV_SIGN_MASK),
  .abs_mask  = _MM512_SET1_U64x8((uint64_t)ERFINV_ABS_MASK),
  .one       = _MM512_SET1_U64x8((uint64_t)ERFINV_ONE),
  .inf       = _MM512_SET1_U64x8((uint64_t)ERFINV_INF),
  .zero      = _MM512_SET1_U64x8((uint64_t)ERFINV_ZERO),
  .offset1   = _MM512_SET1_PD8(ERFINV_OFFSET1),
  .offset2   = _MM512_SET1_PD8(ERFINV_OFFSET2),

  /* Range 1: Table 17 by Blair et al */
  .poly_range1_p = {
    _MM512_SET1_PD8(ERFINV_P100),
    _MM512_SET1_PD8(ERFINV_P101),
    _MM512_SET1_PD8(ERFINV_P102),
    _MM512_SET1_PD8(ERFINV_P103),
    _MM512_SET1_PD8(ERFINV_P104),
    _MM512_SET1_PD8(ERFINV_P105),
    _MM512_SET1_PD8(ERFINV_P106),
  },

  .poly_range1_q = {
    _MM512_SET1_PD8(ERFINV_Q100),
    _MM512_SET1_PD8(ERFINV_Q101),
    _MM512_SET1_PD8(ERFINV_Q102),
    _MM512_SET1_PD8(ERFINV_Q103),
    _MM512_SET1_PD8(ERFINV_Q104),
    _MM512_SET1_PD8(ERFINV_Q105),
    _MM512_SET1_PD8(ERFINV_Q106),
  },

  /* Range 2: Table 37 by Blair et al */
  .poly_range2_p = {
    _MM512_SET1_PD8(ERFINV_P200),
    _MM512_SET1_PD8(ERFINV_P201),
    _MM512_SET1_PD8(ERFINV_P202),
    _MM512_SET1_PD8(ERFINV_P203),
    _MM512_SET1_PD8(ERFINV_P204),
    _MM512_SET1_PD8(ERFINV_P205),
    _MM512_SET1_PD8(ERFINV_P206),
    _MM512_SET1_PD8(ERFINV_P207),
  },

  .poly_range2_q = {
    _MM512_SET1_PD8(ERFINV_Q200),
    _MM512_SET1_PD8(ERFINV_Q201),
    _MM512_SET1_PD8(ERFINV_Q202),
    _MM512_SET1_PD8(ERFINV_Q203),
    _MM512_SET1_PD8(ERFINV_Q204),
    _MM512_SET1_PD8(ERFINV_Q205),
    _MM512_SET1_PD8(ERFINV_Q206),
    _MM512_SET1_PD8(ERFINV_Q207),
  },

  /* Range 3: Table 58 by Blair et al */
  .poly_range3_p = {
    _MM512_SET1_PD8(ERFINV_P300),
    _MM512_SET1_PD8(ERFINV_P301),
    _MM512_SET1_PD8(ERFINV_P302),
    _MM512_SET1_PD8(ERFINV_P303),
    _MM512_SET1_PD8(ERFINV_P304),
    _MM512_SET1_PD8(ERFINV_P305),
    _MM512_SET1_PD8(ERFINV_P306),
    _MM512_SET1_PD8(ERFINV_P307),
    _MM512_SET1_PD8(ERFINV_P308),
    _MM512_SET1_PD8(ERFINV_P309),
    _MM512_SET1_PD8(ERFINV_P310),
  },

  .poly_range3_q = {
    _MM512_SET1_PD8(ERFINV_Q300),
    _MM512_SET1_PD8(ERFINV_Q301),
    _MM512_SET1_PD8(ERFINV_Q302),
    _MM512_SET1_PD8(ERFINV_Q303),
    _MM512_SET1_PD8(ERFINV_Q304),
    _MM512_SET1_PD8(ERFINV_Q305),
    _MM512_SET1_PD8(ERFINV_Q306),
    _MM512_SET1_PD8(ERFINV_Q307),
    _MM512_SET1_PD8(ERFINV_Q308),
  },
};


#define OFFSET1   v_erfinv_data.offset1
#define OFFSET2   v_erfinv_data.offset2


#define P100 v_erfinv_data.poly_range1_p[0]
#define P101 v_erfinv_data.poly_range1_p[1]
#define P102 v_erfinv_data.poly_range1_p[2]
#define P103 v_erfinv_data.poly_range1_p[3]
#define P104 v_erfinv_data.poly_range1_p[4]
#define P105 v_erfinv_data.poly_range1_p[5]
#define P106 v_erfinv_data.poly_range1_p[6]

#define Q100 v_erfinv_data.poly_range1_q[0]
#define Q101 v_erfinv_data.poly_range1_q[1]
#define Q102 v_erfinv_data.poly_range1_q[2]
#define Q103 v_erfinv_data.poly_range1_q[3]
#define Q104 v_erfinv_data.poly_range1_q[4]
#define Q105 v_erfinv_data.poly_range1_q[5]
#define Q106 v_erfinv_data.poly_range1_q[6]

#define P200 v_erfinv_data.poly_range2_p[0]
#define P201 v_erfinv_data.poly_range2_p[1]
#define P202 v_erfinv_data.poly_range2_p[2]
#define P203 v_erfinv_data.poly_range2_p[3]
#define P204 v_erfinv_data.poly_range2_p[4]
#define P205 v_erfinv_data.poly_range2_p[5]
#define P206 v_erfinv_data.poly_range2_p[6]
#define P207 v_erfinv_data.poly_range2_p[7]

#define Q200 v_erfinv_data.poly_range2_q[0]
#define Q201 v_erfinv_data.poly_range2_q[1]
#define Q202 v_erfinv_data.poly_range2_q[2]
#define Q203 v_erfinv_data.poly_range2_q[3]
#define Q204 v_erfinv_data.poly_range2_q[4]
#define Q205 v_erfinv_data.poly_range2_q[5]
#define Q206 v_erfinv_data.poly_range2_q[6]
#define Q207 v_erfinv_data.poly_range2_q[7]

#define P300 v_erfinv_data.poly_range3_p[0]
#define P301 v_erfinv_data.poly_range3_p[1]
#define P302 v_erfinv_data.poly_range3_p[2]
#define P303 v_erfinv_data.poly_range3_p[3]
#define P304 v_erfinv_data.poly_range3_p[4]
#define P305 v_erfinv_data.poly_range3_p[5]
#define P306 v_erfinv_data.poly_range3_p[6]
#define P307 v_erfinv_data.poly_range3_p[7]
#define P308 v_erfinv_data.poly_range3_p[8]
#define P309 v_erfinv_data.poly_range3_p[9]
#define P310 v_erfinv_data.poly_range3_p[10]

#define Q300 v_erfinv_data.poly_range3_q[0]
#define Q301 v_erfinv_data.poly_range3_q[1]
#define Q302 v_erfinv_data.poly_range3_q[2]
#define Q303 v_erfinv_data.poly_range3_q[3]
#define Q304 v_erfinv_data.poly_range3_q[4]
#define Q305 v_erfinv_data.poly_range3_q[5]
#define Q306 v_erfinv_data.poly_range3_q[6]
#define Q307 v_erfinv_data.poly_range3_q[7]
#define Q308 v_erfinv_data.poly_range3_q[8]


#define SIGN_MASK v_erfinv_data.sign_mask
#define ABS_MASK  v_erfinv_data.abs_mask

#define ZERO      v_erfinv_data.zero
#define ONE       v_erfinv_data.one
#define INF       v_erfinv_data.inf

#define BOUND1    v_erfinv_data.bound1
#define BOUND2    v_erfinv_data.bound2


#define SCALAR_ERFINV ALM_PROTO_OPT(erfinv)

static inline int test_condition_for_all(v_u64x8_t cond)
{
  return (_mm512_movepi64_mask((__m512i)cond) == 0xFF);
}

static inline int test_condition_any(v_u64x8_t cond)
{
  return (_mm512_movepi64_mask((__m512i)cond) != 0);
}


v_f64x8_t ALM_PROTO_ARCH_ZN5(vrd8_erfinv)(v_f64x8_t x)
{
  v_f64x8_t result;
  v_u64x8_t ux = as_v8_u64_f64(x);
  v_u64x8_t ux_abs = ux & ABS_MASK;
  v_u64x8_t sign = ux & SIGN_MASK;

  /* Check for special values: |x| >= 1.0 */
  /* This will cover Inf and NaN and Zero also. */
  v_u64x8_t special_cond = ux_abs >= ONE;
  v_u64x8_t zero_cond = ux_abs == ZERO;

  int any_special = test_condition_any(special_cond);
  int zero_available = test_condition_any(zero_cond);

  if (unlikely(any_special || zero_available)) {
    /* Fall back to scalar for special values */
    for (uint64_t i = 0; i < 8; i++) {
      result[i] = SCALAR_ERFINV(x[i]);
    }

    return result;
  }

  /* Mixed ranges: compute multiple paths and blend */
  /* Check if we can handle mixed Range 1 & Range 2 */

  /* Range 1: |x| <= 0.75 */
  /* Range 2: 0.75 < |x| <= 0.9375 */
  v_u64x8_t cond12 = ux_abs <= BOUND2; /* Covers both Range 1 and Range 2 */

  if (test_condition_for_all(cond12)) {
    /* All elements are in Range 1 or Range 2 - compute both and blend */
    v_f64x8_t _x2 = x * x;

    /* Compute Range 1 result */
    v_f64x8_t z1 = _x2 - OFFSET1;
    v_f64x8_t P1 = POLY_EVAL_6(z1, P100, P101, P102, P103, P104, P105, P106);
    v_f64x8_t Q1 = POLY_EVAL_6(z1, Q100, Q101, Q102, Q103, Q104, Q105, Q106);
    v_f64x8_t result1 = x * (P1 / Q1);

    /* Compute Range 2 result */
    v_f64x8_t z2 = _x2 - OFFSET2;
    v_f64x8_t P2 = POLY_EVAL_7(z2, P200, P201, P202, P203, P204, P205, P206, P207);
    v_f64x8_t Q2 = POLY_EVAL_7(z2, Q200, Q201, Q202, Q203, Q204, Q205, Q206, Q207);
    v_f64x8_t result2 = x * (P2 / Q2);

    /* Blend: use result1 if cond1, otherwise result2 */
    /* Range 1: |x| <= 0.75 */
    v_u64x8_t cond1 = ux_abs <= BOUND1;
    __mmask8 mask1 = _mm512_movepi64_mask((__m512i)cond1);
    result = _mm512_mask_blend_pd(mask1, result2, result1);

    return result;
  }

  /* Check if we can handle mixed Range 2 & Range 3 */

  /* Range 2: 0.75 < |x| <= 0.9375 */
  /* Range 3: 0.9375 < |x| < 1.0 */
  v_u64x8_t cond23a = ux_abs > BOUND1; /* Covers both Range 2 and Range 3 */

  if (test_condition_for_all(cond23a)) {
    /* All elements are in Range 2 or Range 3 - compute both and blend */

    /* Compute Range 2 result */
    v_f64x8_t _x2 = x * x;
    v_f64x8_t z2 = _x2 - OFFSET2;
    v_f64x8_t P2 = POLY_EVAL_7(z2, P200, P201, P202, P203, P204, P205, P206, P207);
    v_f64x8_t Q2 = POLY_EVAL_7(z2, Q200, Q201, Q202, Q203, Q204, Q205, Q206, Q207);
    v_f64x8_t result2 = x * (P2 / Q2);

    /* Compute Range 3 result */
    v_f64x8_t neg_absx = as_v8_f64_u64(SIGN_MASK | ux_abs);
    v_f64x8_t log_val;
    for (uint64_t i = 0; i < 8; i++) {
      log_val[i] = ALM_PROTO(log1p)(neg_absx[i]);
    }
    v_f64x8_t neg_log = as_v8_f64_u64(as_v8_u64_f64(log_val) ^ SIGN_MASK);
    v_f64x8_t y = _mm512_sqrt_pd(neg_log);
    const v_f64x8_t one_ro = _MM512_SET1_PD8((double)1.0);
    v_f64x8_t z3 = one_ro / y;
    v_f64x8_t P3 = POLY_EVAL_10(z3, P300, P301, P302, P303, P304, P305,
                                        P306, P307, P308, P309, P310);
    v_f64x8_t Q3 = POLY_EVAL_8(z3, Q300, Q301, Q302, Q303, Q304, Q305,
                                                   Q306, Q307, Q308);
    v_f64x8_t abs_result3 = y * (P3 / Q3);
    __mmask8 sign_mask = _mm512_movepi64_mask((__m512i)sign);
    v_f64x8_t result3 = _mm512_mask_blend_pd(sign_mask, abs_result3, -abs_result3);

    /* Blend: use result2 if cond2, otherwise result3 */
    /* Range 2: 0.75 < |x| <= 0.9375 */
    v_u64x8_t cond2 = ux_abs <= BOUND2;
    __mmask8 mask2 = _mm512_movepi64_mask((__m512i)cond2);
    result = _mm512_mask_blend_pd(mask2, result3, result2);

    return result;
  }

  /* For the remaining cases where the vector spans all three regions,
   * call the scalar function.
   */
  for (uint64_t i = 0; i < 8; i++) {
    result[i] = SCALAR_ERFINV(x[i]);
  }

  return result;
}
