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

#ifndef __LIBM_TYPEHELPER_VEC_H__
#define __LIBM_TYPEHELPER_VEC_H__

#include <libm/types.h>

#include <emmintrin.h>

#define _MM_SET1_PS4(x)                           \
    _Generic((x),                                 \
             float: (__m128){(x), (x), (x), (x)})

#define _MM_SET1_PS8(x)                           \
    _Generic((x),                                 \
             float: (__m256){(x), (x), (x), (x),  \
                     (x), (x), (x), (x)})

#define _MM_SET1_PD2(x)                         \
    _Generic((x),                               \
             double: (__m128d){(x), (x)})

#define _MM_SET1_PD4(x)                             \
    _Generic((x),                                   \
             double: (__m256d){(x), (x), (x), (x)})

/* TODO: check if _MM_SET1_I64x2 is used */
#define _MM_SET1_I64x2(x) {(x), (x)}

#define _MM_SET1_I64x4(x)                             \
    _Generic((x),                                     \
             long int: (__m256i){(x), (x), (x), (x)}, \
             uint64_t: (__m256i){(x), (x), (x), (x)}, \
             int:      (__m256i){(x), (x), (x), (x)},	\
             uint32_t: (__m256i){(x), (x), (x), (x)})


#define _MM_SET1_I32(x) {(x), (x), (x), (x)}

#define _MM_SET1_I64(x) {(x), (x), (x), (x)}

#define _MM256_SET1_I32(x) {(x), (x), (x), (x), (x), (x), (x), (x) }

#define _MM256_SET1_PS8(x) {(x), (x), (x), (x), (x), (x), (x), (x) }


/*
 * Naming convention
 *  1. Access as different data
 *      ``as_v<v>_<to>_<from>``
 *        eg: as_v2_u32_f32  - access an given vector f32x2 element as u32x2
 *                             (without changing memory contents)
 *  2. Cast - Portable C style
 *      - cast_v<v>_<to>_<from>
 *  3. Check
 *      - any_v<v>_<type>
 *
 *      <v> - values: 2, 4, 8 - shows number of elements in vector
 *      <to>/<from>/<type>
 *              - u32, f32, u64, f64 shows underlying type
 *
 *  4. Convert - Non-portable x86 style
 *      - cvt_v<v>_<to>_<from>
 */


/*
 * 1. Low-level-access          as_vN_A_B()
 * 2. Cast functions            cast_vN_A_to_B()
 * 3. Converters                cvt_vN_A_to_B()
 * 4. Condition Checkers        any_vN_A()
 * 5. Function callers          call_vN_A()
 *
 * where: N is a number of vector elements 2,4,8 etc,
 *        A is a type like f32, f64, u32, u64, i32, i64 etc,
 *        B is the other type just like 'A'
 */

/* v4 - single precision */

/* Access a f32x4 as u32x4 */
inline v_u32x4_t
as_v4_u32_f32(v_f32x4_t x)
{
    union {
        v_f32x4_t f; v_u32x4_t u;
    } r = {.f = x};

    return r.u;
}

/* Access a u32x4 as f32x4 */
inline v_f32x4_t
as_v4_f32_u32(v_u32x4_t x)
{
    union {
        v_f32x4_t f; v_u32x4_t u;
    } r = {.u = x};

    return r.f;
}

/* v2 double precision */

static inline v_f64x2_t
as_v2_f64_u64(v_u64x2_t x)
{
    union {
        v_u64x2_t _xi; v_f64x2_t _xf;
    } val = { ._xi = x };

    return val._xf;
}

static inline v_u64x2_t
as_v2_u64_f64 (v_f64x2_t x)
{
    union {
        v_f64x2_t f; v_u64x2_t u;
    } r = {.f = x};

    return r.u;
}

static inline v_i64x2_t
as_v2_i64_f64 (v_f64x2_t x)
{
    union {
        v_f64x2_t f; v_i64x2_t i;
    } r = {.f = x};

    return r.i;
}

static inline v_f64x2_t
as_v2_f64_i64 (v_i64x2_t x)
{
    union {
        v_i64x2_t _xi; v_f64x2_t _xf;
    } val = { ._xi = x };

    return val._xf;
}

/* v4 double precision */

/* Access a u64x4 as f64x4 */
static inline v_f64x4_t
as_v4_f64_u64(v_u64x4_t x)
{
    union {
        v_f64x4_t f; v_u64x4_t u;
    } r = {.u = x};

    return r.f;
}

/* Access a u64x4 as f64x4 */
static inline v_u64x4_t
as_v4_u64_f64(v_f64x4_t x)
{
    union {
        v_f64x4_t f; v_u64x4_t u;
    } r = {.f = x};
    return r.u;
}

/* Access a i64x4 as f64x4 */
static inline v_f64x4_t
as_v4_f64_i64(v_i64x4_t x)
{
    union {
        v_f64x4_t f; v_i64x4_t i;
    } r = {.i = x};

    return r.f;
}

/* Access a i64x4 as f64x4 */
static inline v_i64x4_t
as_v4_i64_f64(v_f64x4_t x)
{
    union {
        v_f64x4_t f; v_i64x4_t i;
    } r = {.f = x};
    return r.i;
}

/*
 * v8 single precision
 */
static inline v_f32x8_t
as_v8_f32_u32(v_u32x8_t x)
{
    union {
        v_u32x8_t _xi; v_f32x8_t _xf;
    } val = { ._xi = x};

    return val._xf;
}

static inline v_u32x8_t
as_v8_u32_f32(v_f32x8_t x)
{
    union {
        v_u32x8_t _xi; v_f32x8_t _xf;
    } val = { ._xf = x};

    return val._xi;
}


/*
 * Casting
 */

/* v4 unsigned int 64 -> 32 */
static inline v_u32x4_t
cast_v4_u64_to_u32(v_u64x4_t _xu64)
{
    return (v_u32x4_t){_xu64[0], _xu64[1], _xu64[2], _xu64[3]};
}

/* v4 signed int -> float */
static inline v_f32x4_t
cast_v4_s32_to_f32(v_i32x4_t _xi32)
{
    return (v_f32x4_t){_xi32[0], _xi32[1], _xi32[2], _xi32[3]};
}

/* v4 float -> double */
inline v_f64x4_t
cast_v4_f32_to_f64(v_f32x4_t _x)
{
    return (v_f64x4_t){_x[0], _x[1], _x[2], _x[3]};
}

/* v4 double -> float */
inline v_f32x4_t
cast_v4_f64_to_f32(v_f64x4_t _x)
{
    return (v_f32x4_t){_x[0], _x[1], _x[2], _x[3]};
}

// v4 double -> int64
static inline v_i64x4_t
cast_v4_f64_to_i64(v_f64x4_t _xf64)
{
    return (v_i64x4_t){_xf64[0], _xf64[1], _xf64[2], _xf64[3]};
}

// v2 double -> int64
static inline v_i64x2_t
cast_v2_f64_to_i64(v_f64x2_t _xf64)
{
    return (v_i64x2_t){_xf64[0], _xf64[1]};
}

/*
static inline v_u32x8_t
cast_v8_u64_to_u32(v_u32x8_t _xf64)
{
    return (v_u32x8_t){
        _xf64[0], _xf64[1], _xf64[2], _xf64[3],
            _xf64[4], _xf64[5], _xf64[6], _xf64[7]
            };
}
*/

static inline v_f64x4_t
cvt_v4_f32_to_f64(v_f32x4_t _xf32 /* cond */)
{
    return _mm256_cvtps_pd(_xf32);
}

static inline v_f32x4_t
cvt_v4_f64_to_f32(v_f64x4_t _xf64 /* cond */)
{
    return _mm256_cvtpd_ps(_xf64);
}

/*
 * Condition Check
 *    check if any of the vector elements are set
 */

/*
 * On x86, 'cond' contains all 0's for false, and all 1's for true
 * IOW, 0=>false, -1=>true
 */

static inline int
any_v4_u32(v_i32x4_t cond)
{
    const v_i32x4_t zero = _MM_SET1_I32(0);
    return ! _mm_testz_si128((__m128i)cond, (__m128i)zero);
}

static inline int
any_v8_u32(v_i32x8_t cond)
{
    const v_i32x8_t zero = {0,};
    return ! _mm256_testz_si256((__m256i)cond, (__m256i)zero);
}

static inline int
any_v4_u64(v_i64x4_t cond)
{
    const v_i64x4_t zero = _MM_SET1_I64(0);
    return ! _mm256_testz_si256((__m256i)cond, (__m256i)zero);
}

static inline int
any_v2_u64(v_i64x2_t cond)
{
    const v_i64x2_t zero = _MM_SET1_I64x2(0);
    return ! _mm_testz_si128((__m128i)cond, (__m128i)zero);
}

// Condition check with for loop for better performance
static inline int
any_v4_u32_loop(v_u32x4_t cond)
{
    int ret = 0;

    for (int i = 0; i < 4; i++) {
        if (cond[i] !=0) {
            ret= 1;
            break;
        }
    }

    return ret;
}

// Condition check with for loop for better performance
static inline int
any_v2_u64_loop(v_u64x2_t cond)
{
    int ret = 0;

    for (int i = 0; i < 2; i++) {
        if (cond[i] !=0) {
            ret= 1;
            break;
        }
    }

    return ret;
}

// Condition check with for loop for better performance
static inline int
any_v4_u64_loop(v_u64x4_t cond)
{
    int ret = 0;
    for (int i = 0; i < 4; i++) {
        if (cond[i] != 0) {
            ret = 1;
            break;
        }
    }

    return ret;
}


#ifndef ALM_HAS_V8_CALL_F32
#define ALM_HAS_V8_CALL_F32

static inline v_f32x8_t
call_v8_f32(float (*fn)(float),
            v_f32x8_t x,
            v_f32x8_t result,
            v_u32x8_t cond)
{
    return (v_f32x8_t) {
        cond[0] ? fn(x[0]) : result[0],
            cond[1] ? fn(x[1]) : result[1],
            cond[2] ? fn(x[2]) : result[2],
            cond[3] ? fn(x[3]) : result[3],
            cond[4] ? fn(x[4]) : result[4],
            cond[5] ? fn(x[5]) : result[5],
            cond[6] ? fn(x[6]) : result[6],
            cond[7] ? fn(x[7]) : result[7]
            };
}
#endif

/*
 * TODO: Convert all following to format
 *    call_vN_A
 *    call2_vN_A  - call a function with 2 args
 * Where
 *     N - number of vector elements 2, 4, 8
 *     A - vector element type f32/f64
 */
#ifndef ALM_HAS_V8_CALL2_F32
#define ALM_HAS_V8_CALL2_F32

static inline v_f32x8_t
call2_v8_f32(float (*fn)(float, float),
        v_f32x8_t x,
        v_f32x8_t y,
        v_f32x8_t result,
        v_i32x8_t cond)
{
    return (v_f32x8_t) {
        cond[0] ? fn(x[0], y[0]) : result[0],
            cond[1] ? fn(x[1], y[1]) : result[1],
            cond[2] ? fn(x[2], y[2]) : result[2],
            cond[3] ? fn(x[3], y[3]) : result[3],
            cond[4] ? fn(x[4], y[4]) : result[4],
            cond[5] ? fn(x[5], y[5]) : result[5],
            cond[6] ? fn(x[6], y[6]) : result[6],
            cond[7] ? fn(x[7], y[7]) : result[7]
            };
}

#endif

#ifndef ALM_HAS_V4_CALL_F32
#define ALM_HAS_V4_CALL_F32

static inline v_f32x4_t
call_v4_f32(float (*fn)(float),
           v_f32x4_t orig,
           v_f32x4_t result,
           v_u32x4_t cond)
{
    return (v_f32x4_t){cond[0] ? fn(orig[0]) : result[0],
            cond[1] ? fn(orig[1]) : result[1],
            cond[2] ? fn(orig[2]) : result[2],
            cond[3] ? fn(orig[3]) : result[3]};
}
#endif

#ifndef ALM_HAS_V4_CALL_2_F32
#define ALM_HAS_V4_CALL_2_F32

#define v_call2_f32(...)	call2_v4_f32(__VA_ARGS__)

static inline v_f32x4_t
call2_v4_f32(float (*fn)(float, float),
            v_f32x4_t x,
            v_f32x4_t y,
            v_f32x4_t result,
            v_i32x4_t cond)
{
    return (v_f32x4_t){cond[0] ? fn(x[0], y[0]) : result[0],
            cond[1] ? fn(x[1], y[1]) : result[1],
            cond[2] ? fn(x[2], y[2]) : result[2],
            cond[3] ? fn(x[3], y[3]) : result[3]};
}

#endif

#ifndef ALM_HAS_V4_CALL_F64
#define ALM_HAS_V4_CALL_F64

#define v_call_f64(...)  call_v4_f64(__VA_ARGS__)

static inline v_f64x4_t
call_v4_f64(double (*fn)(double),
           v_f64x4_t orig,
           v_f64x4_t result,
           v_u64x4_t cond)
{
    return (v_f64x4_t){cond[0] ? fn(orig[0]) : result[0],
            cond[1] ? fn(orig[1]) : result[1],
            cond[2] ? fn(orig[2]) : result[2],
            cond[3] ? fn(orig[3]) : result[3]};
}
#endif

#ifndef ALM_HAS_V2_CALL_F64
#define ALM_HAS_V2_CALL_F64

#define v2_call_f64(...) call_v2_f64(__VA_ARGS__)

static inline v_f64x2_t
call_v2_f64(double (*fn)(double),
            v_f64x2_t x,
            v_f64x2_t result,
            v_u64x2_t cond)
{
    return (v_f64x2_t) {
        cond[0] ? fn(x[0]) : result[0],
        cond[1] ? fn(x[1]) : result[1],
        };
}
#endif


#ifndef ALM_HAS_V4_CALL_2_F64
#define ALM_HAS_V4_CALL_2_F64
static inline v_f64x4_t
call2_v4_f64(double (*fn)(double, double),
       v_f64x4_t x,
       v_f64x4_t y,
       v_f64x4_t result,
       v_i64x4_t cond)
{
    return (v_f64x4_t){cond[0] ? fn(x[0], y[0]) : result[0],
            cond[1] ? fn(x[1], y[1]) : result[1],
            cond[2] ? fn(x[2], y[2]) : result[2],
            cond[3] ? fn(x[3], y[3]) : result[3]};
}
#endif

#ifndef ALM_HAS_V2_CALL_2_F64
#define ALM_HAS_V2_CALL_2_F64
static inline v_f64x2_t
call2_v2_f64(double (*fn)(double, double),
       v_f64x2_t x,
       v_f64x2_t y,
       v_f64x2_t result,
       v_i64x2_t cond)
{
    return (v_f64x2_t){cond[0] ? fn(x[0], y[0]) : result[0],
            cond[1] ? fn(x[1], y[1]) : result[1]};
}
#endif



// v_f32x8_t to v_i32x8_t
static inline v_i32x8_t
cast_v8_f32_to_i32(v_f32x8_t _xf32)
{
    return (v_i32x8_t){_xf32[0], _xf32[1], _xf32[2], _xf32[3],
            _xf32[4], _xf32[5], _xf32[6], _xf32[7]};
}

// v_i32x8_t to v_f32x8_t
static inline v_f32x8_t
cast_v8_f32_to_s32(v_i32x8_t _xi32)
{
    return (v_f32x8_t){_xi32[0], _xi32[1], _xi32[2], _xi32[3],
            _xi32[4], _xi32[5], _xi32[6], _xi32[7] };
}

// Condition check with for loop for better performance
static inline int
any_v8_u32_loop(v_u32x8_t cond)
{
    int ret = 0;

    for (int i = 0; i < 8; i++) {
        if (cond[i] !=0) {
            ret= 1;
            break;
        }
    }

    return ret;
}

#endif
