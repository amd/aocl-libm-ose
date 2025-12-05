/*
 * Copyright (C) 2025-2026, Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __LIBM_POLY_COMMON_H__
#define __LIBM_POLY_COMMON_H__
 
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
 
 /***********************************************************
  * Type-Generic High Precision Estrin Polynomial Evaluation
  ***********************************************************/

static inline void poly_eval_split_double(double a, double *aH, double *aL) {
    *aH = asdouble(asuint64(a) & 0xfffffffff8000000ULL);
    *aL = a - *aH;
}

static inline void poly_eval_split_float(float a, float *aH, float *aL) {
    *aH = asfloat(asuint32(a) & 0xfffff000U);
    *aL = a - *aH;
}

/* 128-bit vector (v_f64x2_t) */
static inline void poly_eval_split_v_f64x2(v_f64x2_t a, v_f64x2_t *aH, v_f64x2_t *aL) {
    v_u64x2_t mask = {0xfffffffff8000000ULL, 0xfffffffff8000000ULL};
    v_u64x2_t au = as_v2_u64_f64(a);
    *aH = as_v2_f64_u64(au & mask);
    *aL = a - *aH;
}

/* 256-bit vector (v_f64x4_t) */
static inline void poly_eval_split_v_f64x4(v_f64x4_t a, v_f64x4_t *aH, v_f64x4_t *aL) {
    v_u64x4_t mask = {0xfffffffff8000000ULL, 0xfffffffff8000000ULL,
                      0xfffffffff8000000ULL, 0xfffffffff8000000ULL};
    v_u64x4_t au = as_v4_u64_f64(a);
    *aH = as_v4_f64_u64(au & mask);
    *aL = a - *aH;
}

#ifdef __AVX512F__
/* 512-bit vector (v_f64x8_t) */
static inline void poly_eval_split_v_f64x8(v_f64x8_t a, v_f64x8_t *aH, v_f64x8_t *aL) {
    v_u64x8_t mask = {0xfffffffff8000000ULL, 0xfffffffff8000000ULL,
                      0xfffffffff8000000ULL, 0xfffffffff8000000ULL,
                      0xfffffffff8000000ULL, 0xfffffffff8000000ULL,
                      0xfffffffff8000000ULL, 0xfffffffff8000000ULL};
    v_u64x8_t au = as_v8_u64_f64(a);
    *aH = as_v8_f64_u64(au & mask);
    *aL = a - *aH;
}

#define POLY_EVAL_SPLIT(a, aH, aL) \
    _Generic((a), \
        double: poly_eval_split_double, \
        float:  poly_eval_split_float, \
        v_f64x2_t: poly_eval_split_v_f64x2, \
        v_f64x4_t: poly_eval_split_v_f64x4, \
        v_f64x8_t: poly_eval_split_v_f64x8)(a, &(aH), &(aL))
#else
#define POLY_EVAL_SPLIT(a, aH, aL) \
    _Generic((a), \
        double: poly_eval_split_double, \
        float:  poly_eval_split_float, \
        v_f64x2_t: poly_eval_split_v_f64x2, \
        v_f64x4_t: poly_eval_split_v_f64x4)(a, &(aH), &(aL))
#endif
 
 #define POLY_EVAL_ZERO(r) ((__typeof(r)){0})
 
 /* A_X_B_PLUS_C: <aH, aL> = <aH, aL> * <bH, bL> + <cH, cL> */
#define POLY_EVAL_A_X_B_PLUS_C(aH, aL, b, bH, bL, cH, cL) do { \
     __typeof(b) _wH, _wL, _xH, _xL, _yH, _yL, _z; \
     __typeof(b) _aHH, _aHL; \
     /* <_wH, _wL> = TwoProduct(aH, b) */ \
     _wH = aH * b; \
     POLY_EVAL_SPLIT(aH, _aHH, _aHL); \
     _wL = _aHL*bL-(((_wH-_aHH*bH)-_aHL*bH)-_aHH*bL); \
     /* <_xH, _xL> = FastTwoSum(_wH, (aL*b)) */ \
     _z = aL * b; \
     _xH = _wH + _z; \
     _xL = _z - (_xH-_wH); \
     /* <_yH, _yL> = FastTwoSum(_xH, (_xL+_wL)) */ \
     _z = _xL + _wL; \
     _yH = _xH + _z; \
     _yL = _z - (_yH-_xH); \
     /* <_wH, _wL> = TwoSum(_yH, cH) */ \
     _wH = _yH + cH; \
     _z = _wH - _yH; \
     _wL = cL + (_yH-(_wH-_z))+(cH-_z); \
     /* <aH, aL> = FastTwoSum(_wH, (_wL+_yL)) */ \
     _z = _wL + _yL; \
     aH = _wH + _z; \
     aL = _z - (aH-_wH); \
 } while(0)
 
 /* L0 = c1*r + c0 */
#define POLY_EVAL_ESTRIN_L0_SIMPLE(r, c0, c1, L0) L0 = (c1) * (r) + (c0)
 
 /* L0_TAIL_1: <L0_H, L0_L> = c1*r + <c0H, c0L> */
#define POLY_EVAL_ESTRIN_L0_TAIL_1(r, c0H, c0L, c1, L0_H, L0_L) do { \
     __typeof(r) _t0 = (c1) * (r); \
     __typeof(r) _t0H, _t0L; \
     POLY_EVAL_SPLIT(_t0, _t0H, _t0L); \
     /* <L0_H, L0_L> = TwoSum(t0H, c0H) + (t0L + c0L) */ \
     L0_H = _t0H + (c0H); \
     __typeof(r) _v = L0_H - _t0H; \
     L0_L = (_t0H - (L0_H - _v)) + ((c0H) - _v) + _t0L + (c0L); \
 } while(0)
 
 /* L0_TAIL_2: <L0_H, L0_L> = <c1H, c1L>*r + <c0H, c0L> */
#define POLY_EVAL_ESTRIN_L0_TAIL_2(r, c0H, c0L, c1H, c1L, L0_H, L0_L) do { \
     __typeof(r) _rH, _rL; \
     POLY_EVAL_SPLIT(r, _rH, _rL); \
     L0_H = (c1H); L0_L = (c1L); \
     POLY_EVAL_A_X_B_PLUS_C(L0_H, L0_L, r, _rH, _rL, c0H, c0L); \
 } while(0)
 
 
 /* Estrin-8 combiner:
  * L0: 4 pairs -> L0_0, L0_1, L0_2, L0_3
  * L1: L1_0 = L0_1*r^2 + L0_0, L1_1 = L0_3*r^2 + L0_2
  * L2: L2_0 = L1_1*r^4 + L1_0
  * Final: q = L2_0
  */
#define POLY_EVAL_ESTRIN_8_TAIL_COMMON(r, L0_0_H, L0_0_L, L0_1_H, L0_1_L, c4, c5, c6, c7) ({ \
     __typeof(r) _L0_2 = c5 * r + c4; \
     __typeof(r) _L0_3 = c7 * r + c6; \
     __typeof(r) _r2 = r * r; \
     __typeof(r) _r2H, _r2L; \
     POLY_EVAL_SPLIT(_r2, _r2H, _r2L); \
     \
     __typeof(r) _L1_0_H = L0_1_H; \
     __typeof(r) _L1_0_L = L0_1_L; \
     POLY_EVAL_A_X_B_PLUS_C(_L1_0_H, _L1_0_L, _r2, _r2H, _r2L, L0_0_H, L0_0_L); \
     \
     __typeof(r) _L1_1 = _L0_3 * _r2 + _L0_2; \
     __typeof(r) _r4 = _r2 * _r2; \
     __typeof(r) _r4H, _r4L; \
     POLY_EVAL_SPLIT(_r4, _r4H, _r4L); \
     \
     __typeof(r) _qH = _L1_1; \
     __typeof(r) _qL = POLY_EVAL_ZERO(r); \
     POLY_EVAL_A_X_B_PLUS_C(_qH, _qL, _r4, _r4H, _r4L, _L1_0_H, _L1_0_L); \
     _qH + _qL; \
 })
 
 /* Estrin-9 combiner:
  * L0: 5 pairs -> L0_0, L0_1, L0_2, L0_3, L0_4
  * L1: L1_0 = L0_1*r^2 + L0_0, L1_1 = L0_3*r^2 + L0_2, L1_2 = L0_4
  * L2: L2_0 = L1_1*r^4 + L1_0, L2_1 = L1_2
  * Final: q = L2_1*r^8 + L2_0
  */
#define POLY_EVAL_ESTRIN_9_TAIL_COMMON(r, L0_0_H, L0_0_L, L0_1_H, L0_1_L, c4, c5, c6, c7, c8) ({ \
     __typeof(r) _L0_2 = c5 * r + c4; \
     __typeof(r) _L0_3 = c7 * r + c6; \
     __typeof(r) _L0_4 = c8; \
     __typeof(r) _r2 = r * r; \
     __typeof(r) _r2H, _r2L; \
     POLY_EVAL_SPLIT(_r2, _r2H, _r2L); \
     \
     __typeof(r) _L1_0_H = L0_1_H; \
     __typeof(r) _L1_0_L = L0_1_L; \
     POLY_EVAL_A_X_B_PLUS_C(_L1_0_H, _L1_0_L, _r2, _r2H, _r2L, L0_0_H, L0_0_L); \
     \
     __typeof(r) _L1_1 = _L0_3 * _r2 + _L0_2; \
     __typeof(r) _L1_2 = _L0_4; \
     __typeof(r) _r4 = _r2 * _r2; \
     __typeof(r) _r4H, _r4L; \
     POLY_EVAL_SPLIT(_r4, _r4H, _r4L); \
     \
     __typeof(r) _L2_0_H = _L1_1; \
     __typeof(r) _L2_0_L = POLY_EVAL_ZERO(r); \
     POLY_EVAL_A_X_B_PLUS_C(_L2_0_H, _L2_0_L, _r4, _r4H, _r4L, _L1_0_H, _L1_0_L); \
     \
     __typeof(r) _L2_1 = _L1_2; \
     __typeof(r) _r8 = _r4 * _r4; \
     __typeof(r) _r8H, _r8L; \
     POLY_EVAL_SPLIT(_r8, _r8H, _r8L); \
     \
     __typeof(r) _qH = _L2_1; \
     __typeof(r) _qL = POLY_EVAL_ZERO(r); \
     POLY_EVAL_A_X_B_PLUS_C(_qH, _qL, _r8, _r8H, _r8L, _L2_0_H, _L2_0_L); \
     _qH + _qL; \
 })
 
 /* Estrin-10 combiner:
  * L0: 5 pairs -> L0_0, L0_1, L0_2, L0_3, L0_4
  * L1: L1_0 = L0_1*r^2 + L0_0, L1_1 = L0_3*r^2 + L0_2, L1_2 = L0_4
  * L2: L2_0 = L1_1*r^4 + L1_0, L2_1 = L1_2
  * Final: q = L2_1*r^8 + L2_0
  */
#define POLY_EVAL_ESTRIN_10_TAIL_COMMON(r, L0_0_H, L0_0_L, L0_1_H, L0_1_L, c4, c5, c6, c7, c8, c9) ({ \
     __typeof(r) _L0_2 = c5 * r + c4; \
     __typeof(r) _L0_3 = c7 * r + c6; \
     __typeof(r) _L0_4 = c9 * r + c8; \
     __typeof(r) _r2 = r * r; \
     __typeof(r) _r2H, _r2L; \
     POLY_EVAL_SPLIT(_r2, _r2H, _r2L); \
     \
     __typeof(r) _L1_0_H = L0_1_H; \
     __typeof(r) _L1_0_L = L0_1_L; \
     POLY_EVAL_A_X_B_PLUS_C(_L1_0_H, _L1_0_L, _r2, _r2H, _r2L, L0_0_H, L0_0_L); \
     \
     __typeof(r) _L1_1 = _L0_3 * _r2 + _L0_2; \
     __typeof(r) _L1_2 = _L0_4; \
     __typeof(r) _r4 = _r2 * _r2; \
     __typeof(r) _r4H, _r4L; \
     POLY_EVAL_SPLIT(_r4, _r4H, _r4L); \
     \
     __typeof(r) _L2_0_H = _L1_1; \
     __typeof(r) _L2_0_L = POLY_EVAL_ZERO(r); \
     POLY_EVAL_A_X_B_PLUS_C(_L2_0_H, _L2_0_L, _r4, _r4H, _r4L, _L1_0_H, _L1_0_L); \
     \
     __typeof(r) _L2_1 = _L1_2; \
     __typeof(r) _r8 = _r4 * _r4; \
     __typeof(r) _r8H, _r8L; \
     POLY_EVAL_SPLIT(_r8, _r8H, _r8L); \
     \
     __typeof(r) _qH = _L2_1; \
     __typeof(r) _qL = POLY_EVAL_ZERO(r); \
     POLY_EVAL_A_X_B_PLUS_C(_qH, _qL, _r8, _r8H, _r8L, _L2_0_H, _L2_0_L); \
     _qH + _qL; \
 })
 
 /* Estrin-11 combiner:
  * L0: 6 values -> L0_0, L0_1, L0_2, L0_3, L0_4, L0_5
  * L1: L1_0 = L0_1*r^2 + L0_0, L1_1 = L0_3*r^2 + L0_2, L1_2 = L0_5*r^2 + L0_4
  * L2: L2_0 = L1_1*r^4 + L1_0, L2_1 = L1_2
  * Final: q = L2_1*r^8 + L2_0
  */
#define POLY_EVAL_ESTRIN_11_TAIL_COMMON(r, L0_0_H, L0_0_L, L0_1_H, L0_1_L, c4, c5, c6, c7, c8, c9, c10) ({ \
     __typeof(r) _L0_2 = c5 * r + c4; \
     __typeof(r) _L0_3 = c7 * r + c6; \
     __typeof(r) _L0_4 = c9 * r + c8; \
     __typeof(r) _L0_5 = c10; \
     __typeof(r) _r2 = r * r; \
     __typeof(r) _r2H, _r2L; \
     POLY_EVAL_SPLIT(_r2, _r2H, _r2L); \
     \
     __typeof(r) _L1_0_H = L0_1_H; \
     __typeof(r) _L1_0_L = L0_1_L; \
     POLY_EVAL_A_X_B_PLUS_C(_L1_0_H, _L1_0_L, _r2, _r2H, _r2L, L0_0_H, L0_0_L); \
     \
     __typeof(r) _L1_1 = _L0_3 * _r2 + _L0_2; \
     __typeof(r) _L1_2 = _L0_5 * _r2 + _L0_4; \
     __typeof(r) _r4 = _r2 * _r2; \
     __typeof(r) _r4H, _r4L; \
     POLY_EVAL_SPLIT(_r4, _r4H, _r4L); \
     \
     __typeof(r) _L2_0_H = _L1_1; \
     __typeof(r) _L2_0_L = POLY_EVAL_ZERO(r); \
     POLY_EVAL_A_X_B_PLUS_C(_L2_0_H, _L2_0_L, _r4, _r4H, _r4L, _L1_0_H, _L1_0_L); \
     \
     __typeof(r) _L2_1 = _L1_2; \
     __typeof(r) _r8 = _r4 * _r4; \
     __typeof(r) _r8H, _r8L; \
     POLY_EVAL_SPLIT(_r8, _r8H, _r8L); \
     \
     __typeof(r) _qH = _L2_1; \
     __typeof(r) _qL = POLY_EVAL_ZERO(r); \
     POLY_EVAL_A_X_B_PLUS_C(_qH, _qL, _r8, _r8H, _r8L, _L2_0_H, _L2_0_L); \
     _qH + _qL; \
 })
 
/* Estrin degree 8 with [c0,c1] at higher precision */
#define POLY_EVAL_ESTRIN_8_TAIL_2(r, c0H, c0L, c1H, c1L, c2, c3, c4, c5, c6, c7) ({ \
    __typeof(r) _L0_0_H, _L0_0_L; \
    POLY_EVAL_ESTRIN_L0_TAIL_2(r, c0H, c0L, c1H, c1L, _L0_0_H, _L0_0_L); \
    \
    __typeof(r) _L0_1_H = c3 * r + c2; \
    __typeof(r) _L0_1_L = POLY_EVAL_ZERO(r); \
    \
    POLY_EVAL_ESTRIN_8_TAIL_COMMON(r, _L0_0_H, _L0_0_L, _L0_1_H, _L0_1_L, c4, c5, c6, c7); \
})
 
 /* Estrin degree 9 with c0 at higher precision */
#define POLY_EVAL_ESTRIN_9_TAIL_1(r, c0H, c0L, c1, c2, c3, c4, c5, c6, c7, c8) ({ \
     __typeof(r) _L0_0_H, _L0_0_L; \
     POLY_EVAL_ESTRIN_L0_TAIL_1(r, c0H, c0L, c1, _L0_0_H, _L0_0_L); \
     \
     __typeof(r) _L0_1_H = c3 * r + c2; \
     __typeof(r) _L0_1_L = POLY_EVAL_ZERO(r); \
     \
     POLY_EVAL_ESTRIN_9_TAIL_COMMON(r, _L0_0_H, _L0_0_L, _L0_1_H, _L0_1_L, c4, c5, c6, c7, c8); \
 })
 
 /* Estrin degree 9 with [c0,c1] at higher precision */
#define POLY_EVAL_ESTRIN_9_TAIL_2(r, c0H, c0L, c1H, c1L, c2, c3, c4, c5, c6, c7, c8) ({ \
     __typeof(r) _L0_0_H, _L0_0_L; \
     POLY_EVAL_ESTRIN_L0_TAIL_2(r, c0H, c0L, c1H, c1L, _L0_0_H, _L0_0_L); \
     \
     __typeof(r) _L0_1_H = c3 * r + c2; \
     __typeof(r) _L0_1_L = POLY_EVAL_ZERO(r); \
     \
     POLY_EVAL_ESTRIN_9_TAIL_COMMON(r, _L0_0_H, _L0_0_L, _L0_1_H, _L0_1_L, c4, c5, c6, c7, c8); \
 })
 
 /* Estrin degree 9 with [c0,c3] at higher precision */
#define POLY_EVAL_ESTRIN_9_TAIL_4(r, c0H, c0L, c1H, c1L, c2H, c2L, c3H, c3L, c4, c5, c6, c7, c8) ({ \
     __typeof(r) _L0_0_H, _L0_0_L; \
     POLY_EVAL_ESTRIN_L0_TAIL_2(r, c0H, c0L, c1H, c1L, _L0_0_H, _L0_0_L); \
     \
     __typeof(r) _L0_1_H, _L0_1_L; \
     POLY_EVAL_ESTRIN_L0_TAIL_2(r, c2H, c2L, c3H, c3L, _L0_1_H, _L0_1_L); \
     \
     POLY_EVAL_ESTRIN_9_TAIL_COMMON(r, _L0_0_H, _L0_0_L, _L0_1_H, _L0_1_L, c4, c5, c6, c7, c8); \
 })
 
 /* Estrin degree 10 with [c0,c3] at higher precision */
#define POLY_EVAL_ESTRIN_10_TAIL_4(r, c0H, c0L, c1H, c1L, c2H, c2L, c3H, c3L, c4, c5, c6, c7, c8, c9) ({ \
     __typeof(r) _L0_0_H, _L0_0_L; \
     POLY_EVAL_ESTRIN_L0_TAIL_2(r, c0H, c0L, c1H, c1L, _L0_0_H, _L0_0_L); \
     \
     __typeof(r) _L0_1_H, _L0_1_L; \
     POLY_EVAL_ESTRIN_L0_TAIL_2(r, c2H, c2L, c3H, c3L, _L0_1_H, _L0_1_L); \
     \
     POLY_EVAL_ESTRIN_10_TAIL_COMMON(r, _L0_0_H, _L0_0_L, _L0_1_H, _L0_1_L, c4, c5, c6, c7, c8, c9); \
 })
 
 /* Estrin degree 11 with c0 at higher precision */
#define POLY_EVAL_ESTRIN_11_TAIL_1(r, c0H, c0L, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10) ({ \
     __typeof(r) _L0_0_H, _L0_0_L; \
     POLY_EVAL_ESTRIN_L0_TAIL_1(r, c0H, c0L, c1, _L0_0_H, _L0_0_L); \
     \
     __typeof(r) _L0_1_H = c3 * r + c2; \
     __typeof(r) _L0_1_L = POLY_EVAL_ZERO(r); \
     \
     POLY_EVAL_ESTRIN_11_TAIL_COMMON(r, _L0_0_H, _L0_0_L, _L0_1_H, _L0_1_L, c4, c5, c6, c7, c8, c9, c10); \
 })
 
 /* Estrin degree 11 with [c0,c1] at higher precision */
#define POLY_EVAL_ESTRIN_11_TAIL_2(r, c0H, c0L, c1H, c1L, c2, c3, c4, c5, c6, c7, c8, c9, c10) ({ \
     __typeof(r) _L0_0_H, _L0_0_L; \
     POLY_EVAL_ESTRIN_L0_TAIL_2(r, c0H, c0L, c1H, c1L, _L0_0_H, _L0_0_L); \
     \
     __typeof(r) _L0_1_H = c3 * r + c2; \
     __typeof(r) _L0_1_L = POLY_EVAL_ZERO(r); \
     \
     POLY_EVAL_ESTRIN_11_TAIL_COMMON(r, _L0_0_H, _L0_0_L, _L0_1_H, _L0_1_L, c4, c5, c6, c7, c8, c9, c10); \
 })
 
#endif /* __LIBM_POLY_COMMON_H__ */
