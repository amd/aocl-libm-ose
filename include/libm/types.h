/*
 * Copyright (C) 2018-2020, Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __LIBM_TYPES_H__
#define __LIBM_TYPES_H__

#include <stdint.h>
#include <float.h>
#include <complex.h>
#include <immintrin.h>

#define PASTE2(a, b) a##b
#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)


typedef    short               f16_t;
typedef    float               f32_t;
typedef    double              f64_t;
typedef    long double         f80_t;

typedef    float  _Complex     fc32_t;
typedef    double _Complex     fc64_t;

#ifdef  HAVE_NATIVE_LONG_LONG_DOUBLE
typedef    long long double    f128_t;
#else
typedef    __m128              f128_t;
#endif


/*****************************
 * Internal types
 *****************************/
typedef union {
    uint32_t i;
    float    f;
} flt32u_t;

typedef union {
    int32_t i;
    float   f;
} flt32_t;

typedef union {
    double   d;
    uint64_t i;
} flt64u_t;

typedef union {
    int64_t i;
    double  d;
} flt64_t;



/*****************************
 * Internal vector types
 *****************************/


/*
 * (u)int32 - 4 elements - 128 bit
 */
typedef union {
    int32_t i[4];
    __m128i m128i;
} int128_t;

typedef union {
    uint32_t i[4];
    __m128i m128i;
} int128u_t;


/*
 * float32 - 4 elements - 128 bit
 */
typedef union {
    int32_t i[4];
    float   f[4];
    __m128  m128;
} flt128f_t;

typedef union {
    uint32_t i[4];
    float    f[4];
    __m128   m128;
} flt128fu_t;

/*
 * float32 - 8 element - 256 bits
 */
typedef union {
    int32_t i[8];
    float   f[8];
    __m256  m256;
} flt256f_t;

typedef union {
    uint32_t i[8];
    float    f[8];
    __m256   m256;
} flt256fu_t;

/*
 * float64 - 2 element - 128 bits
 */
typedef union {
    int64_t i[2];
    double  d[2];
    __m128d  m128;
} flt128d_t;

typedef union {
    uint64_t  i[2];
    double   d[2];
    __m256d  m128;
} flt128du_t;

/*
 * float64 - 4 element - 256 bits
 */
typedef union {
    int64_t  i[4];
    double   d[4];
    __m256d  m256d;
} flt256d_t;

typedef union {
    uint64_t  i[4];
    double    d[4];
    __m256d   m256d;
} flt256du_t;



/*
 * Vector types
 */

#define VEC(x) __attribute__ ((__vector_size__ (x)))
#define MAY_ALIAS __attribute ((__may_alias__))

/* Naming convention
 * v_/vu_   - prefix, prefix-unaligned
 * f/d/u/i  - float/double unsigned/signed
 * 32/64/80 - width of data
 * x2/x4/x8 - number of elements
 */

typedef float    v_f32x4_t VEC(16) MAY_ALIAS;
typedef uint32_t v_u32x4_t VEC(16) MAY_ALIAS;
typedef int32_t  v_i32x4_t VEC(16) MAY_ALIAS;
typedef float    v_f32x8_t VEC(32) MAY_ALIAS;
typedef uint32_t v_u32x8_t VEC(32) MAY_ALIAS;
typedef int32_t  v_i32x8_t VEC(32) MAY_ALIAS;

typedef double   v_f64x2_t VEC(16) MAY_ALIAS;
typedef uint64_t v_u64x2_t VEC(16) MAY_ALIAS;
typedef int64_t  v_i64x2_t VEC(16) MAY_ALIAS;
typedef double   v_f64x4_t VEC(32) MAY_ALIAS;
typedef uint64_t v_u64x4_t VEC(32) MAY_ALIAS;
typedef int64_t  v_i64x4_t VEC(32) MAY_ALIAS;


/*
 * Generic 32-bit, 4-element types
 */
typedef union {
    v_f32x4_t f32x4;
    v_i32x4_t i32x4;
} v_32x4;

typedef union {
    v_f32x4_t f32x4;
    v_u32x4_t u32x4;
} v_32x4_u;

/*
 * Generic 32-bit, 8-element types
 */
typedef union {
    v_f32x8_t f32x8;
    v_i32x8_t i32x8;
} v_32x8;

typedef union {
    v_f32x8_t f32x8;
    v_u32x8_t u32x8;
} v_32x8_u;

/*
 * Generic 64-bit, 2-element types
 */
typedef union {
    v_f64x2_t f64x2;
    v_i64x2_t i64x2;
} v_64x2;

typedef union {
    v_f64x2_t f64x2;
    v_u64x2_t u64x2;
} v_64x2_u;

/*
 * Generic 64-bit, 4-element types
 */
typedef union {
    v_f64x4_t f64x4;
    v_i64x4_t i64x4;
} v_64x4;

typedef union {
    v_f64x4_t f64x4;
    v_u64x4_t u64x4;
} v_64x4_u;

#endif	/* LIBM_TYPES_H */
