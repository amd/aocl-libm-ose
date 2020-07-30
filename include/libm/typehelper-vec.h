/*
 *
 * Copyright (C) 2019-2020, Advanced Micro Devices. All rights reserved.
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
 * v4s functions
 */

/* Access a f32x4 as u32x4 */
inline v_u32x4_t
as_v4_u32_f32(v_f32x4_t x)
{
    union { v_f32x4_t f; v_u32x4_t u; } r = {.f = x};

    return r.u;
}

/* Access a u32x4 as f32x4 */
inline v_f32x4_t
as_v4_f32_u32(v_u32x4_t x)
{
    union { v_f32x4_t f; v_u32x4_t u; } r = {.u = x};

    return r.f;
}

static inline v_u32x4_t
cast_v4_u64_to_u32(v_u64x4_t _xu64)
{
    return (v_u32x4_t){_xu64[0], _xu64[1], _xu64[2], _xu64[3]};
}

// v_i32x4_t to v_f32x4_t
static inline v_f32x4_t
cast_v4_s32_to_f32(v_i32x4_t _xi32)
{
    return (v_f32x4_t){_xi32[0], _xi32[1], _xi32[2], _xi32[3]};
}

/* Type cast f32x4 => f64x4 (4.float => 4.double) */
inline v_f64x4_t
cast_v4_f32_to_f64(v_f32x4_t _x)
{
    return (v_f64x4_t){_x[0], _x[1], _x[2], _x[3]};
}

/* Type cast f64x4 => f32x4 (4.double => 4.float) */
inline v_f32x4_t
cast_v4_f64_to_f32(v_f64x4_t _x)
{
    return (v_f32x4_t){_x[0], _x[1], _x[2], _x[3]};
}

/* Check if any of the vector elements are set */
static inline int
any_v4_u32(v_i32x4_t cond)
{
    const v_i32x4_t zero = _MM_SET1_I32(0);
    return _mm_testz_si128(cond, zero);
}

/*
 * On x86, 'cond' contains all 0's for false, and all 1's for true
 * IOW, 0=>false, -1=>true
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


// Condition check with for loop for better performance
static inline int
any_v4_u32_loop(v_i32x4_t cond)
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


/*
 * v4d functions
 */

/* Access a u64x4 as f64x4 */
static inline v_f64x4_t
as_v4_f64_u64(v_u64x4_t x)
{
    union { v_f64x4_t f; v_u64x4_t u; } r = {.u = x};
    return r.f;
}

/* Access a u64x4 as f64x4 */
static inline v_u64x4_t
as_v4_u64_f64(v_f64x4_t x)
{
    union { v_f64x4_t f; v_u64x4_t u; } r = {x};
    return r.u;
}

// v_f64x4_t to v_i64x4_t
static inline v_i64x4_t
cast_v4_f64_to_i64(v_f64x4_t _xf64)
{
    return (v_i64x4_t){_xf64[0], _xf64[1], _xf64[2], _xf64[3]};
}


/* Check if any of the vector elements are set */
static inline int
any_v4_u64(v_i64x4_t cond)
{
    const v_i64x4_t zero = _MM_SET1_I64(0);
    return _mm256_testz_si256(cond, zero);
}

// Condition check with for loop for better performance
static inline int
any_v4_u64_loop(v_i64x4_t cond)
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


/*
 * v2d functions
 */

#define _MM_SET1_I64x2(x) {(x), (x)}

static inline v_f64x2_t
as_v2_f64_u64(v_u64x2_t x)
{
    union {
        v_u64x2_t _xi;
        v_f64x2_t _xf;
    } val = {
        ._xi = x,
    };

    return val._xf;
}

static inline v_u64x2_t
as_v2_u64_f64 (v_f64x2_t x)
{
    union { v_f64x2_t f; v_u64x2_t u; } r = {.f = x};
    return r.u;
}

// v_f64x2_t to v_i64x2_t
static inline v_i64x2_t
cast_v2_f64_to_i64(v_f64x2_t _xf64)
{
    return (v_i64x2_t){_xf64[0], _xf64[1]};
}

static inline int
any_v2_u64(v_i64x2_t cond)
{
    const v_i64x2_t zero = _MM_SET1_I64x2(0);
    return _mm_testz_si128(cond, zero);
}

// Condition check with for loop for better performance
static inline int
any_v2_u64_loop(v_i64x2_t cond)
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


#ifndef ALM_HAS_V8_CALL_F32
#define ALM_HAS_V8_CALL_F32

static inline v_f32x8_t
v_call2_f32_2(float (*fn)(float, float),
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
v_call_f32(float (*fn)(float),
           v_f32x4_t orig,
           v_f32x4_t result,
           v_i32x4_t cond)
{
  return (v_f32x4_t){cond[0] ? fn(orig[0]) : result[0],
    cond[1] ? fn(orig[1]) : result[1],
    cond[2] ? fn(orig[2]) : result[2],
    cond[3] ? fn(orig[3]) : result[3]};
}
#endif

#ifndef ALM_HAS_V4_CALL_2_F32
#define ALM_HAS_V4_CALL_2_F32
static inline v_f32x4_t
v_call2_f32(float (*fn)(float, float),
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
static inline v_f64x4_t
v_call_f64(double (*fn)(double),
     v_f64x4_t orig,
     v_f64x4_t result,
     v_i64x4_t cond)
{
  return (v_f64x4_t){cond[0] ? fn(orig[0]) : result[0],
    cond[1] ? fn(orig[1]) : result[1],
    cond[2] ? fn(orig[2]) : result[2],
    cond[3] ? fn(orig[3]) : result[3]};
}
#endif

#ifndef ALM_HAS_V4_CALL_2_F64
#define ALM_HAS_V4_CALL_2_F64
static inline v_f64x4_t
v_call2_f64(double (*fn)(double, double),
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
v_call2_f64x2(double (*fn)(double, double),
       v_f64x2_t x,
       v_f64x2_t y,
       v_f64x2_t result,
       v_i64x2_t cond)
{
    return (v_f64x2_t){cond[0] ? fn(x[0], y[0]) : result[0],
        cond[1] ? fn(x[1], y[1]) : result[1]};
}
#endif


/*
 * v8s functions
 */
static inline v_f32x8_t
as_v8_f32_u32(v_u32x8_t x)
 {
     union {
         v_u32x8_t _xi;
         v_f32x8_t _xf;
     } val = {
         ._xi = x,
     };

     return val._xf;
 }

static inline v_u32x8_t
as_v8_u32_f32(v_f32x8_t x)
{
    union {
        v_u32x8_t _xi;
        v_f32x8_t _xf;
    } val = {
        ._xf = x,
    };

    return val._xi;
}

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

/*
 * On x86, 'cond' contains all 0's for false, and all 1's for true
 * IOW, 0=>false, -1=>true
 */
static inline int
any_v8_u32(v_i32x8_t cond)
{
    const v_i32x8_t zero = _MM256_SET1_I32(0);
    return _mm256_testz_si256(cond, zero);
}

// Condition check with for loop for better performance
static inline int
any_v8_u32_loop(v_i32x8_t cond)
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
