#ifndef __LIBM_TYPEHELPER_VEC_H__
#define __LIBM_TYPEHELPER_VEC_H__

#include <libm/types.h>

#include <emmintrin.h>

/***********************
***** v4d functions ****
***********************/

static inline v_f64x4_t
as_f64x4(v_u64x4_t x)
{
    union {
        v_u64x4_t _xi;
        v_f64x4_t _xf;
    } val = {
        ._xi = x,
    };

    return val._xf;
}

static inline v_u64x4_t
as_u64x4(v_f64x4_t x)
{
    union {
        v_u64x4_t _xi;
        v_f64x4_t _xf;
    } val = {
        ._xf = x,
    };

    return val._xi;
}

// v_f64x4_t to v_i64x4_t
static inline v_i64x4_t
v4_to_f64_i64(v_f64x4_t _xf64)
{
    return (v_i64x4_t){_xf64[0], _xf64[1], _xf64[2], _xf64[3]};
}

static inline int
v4_any_u64(v_i64x4_t cond)
{
    const v_i64x4_t zero = _MM_SET1_I64(0);
    return _mm256_testz_si256(cond, zero);
}

// Condition check with for loop for better performance
static inline int
v4_any_u64_loop(v_i64x4_t cond)
{
    int ret = 0;
    for (int i = 0; i < 4; i++)
    {
        if(cond[i] !=0)
            ret= 1;
    }
    return ret;
}


/***********************
***** v2d functions ****
***********************/

#define _MM_SET1_I64x2(x) {(x), (x)}

static inline v_f64x2_t
as_f64x2(v_u64x2_t x)
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
as_v_u64x2(v_f64x2_t x)
{
    union {
        v_u64x2_t _xi;
        v_f64x2_t _xf;
    } val = {
        ._xf = x,
    };

    return val._xi;
}

// v_f64x2_t to v_i64x2_t
static inline v_i64x2_t
v2_to_f64_i64(v_f64x2_t _xf64)
{
    return (v_i64x2_t){_xf64[0], _xf64[1]};
}

static inline int
v2_any_u64(v_i64x2_t cond)
 {
     const v_i64x2_t zero = _MM_SET1_I64x2(0);
     return _mm_testz_si128(cond, zero);
 }

 // Condition check with for loop for better performance
static inline int
v2_any_u64_loop(v_i64x2_t cond)
 {
     int ret = 0;

     for (int i = 0; i < 2; i++)
     {
         if(cond[i] !=0)
             ret= 1;
     }
     return ret;
 }



/***********************
***** v4s functions ****
***********************/

// v_f32x4_t to v_f64x4_t
static inline v_f64x4_t
v4_to_f32_f64(v_f32x4_t _xf32)
{
    return _mm256_cvtps_pd(_xf32);
}

// v_f64x4_t to v_f32x4_t
static inline v_f32x4_t
v4_to_f64_f32(v_f64x4_t _xf64)
{
    return _mm256_cvtpd_ps(_xf64);
}

// v_i32x4_t to v_f32x4_t
static inline v_f32x4_t
v4_to_f32_s32(v_i32x4_t _xi32)
{
    return (v_f32x4_t){_xi32[0], _xi32[1], _xi32[2], _xi32[3]};
}

// v_f32x4_t to v_i32x4_t
static inline v_i32x4_t
v4_to_f32_i32(v_f32x4_t _xf32)
{
    return (v_i32x4_t){_xf32[0], _xf32[1], _xf32[2], _xf32[3]};
}

/*
 * On x86, 'cond' contains all 0's for false, and all 1's for true
 * IOW, 0=>false, -1=>true
 */
static inline int
v4_any_u32(v_i32x4_t cond)
{
    const v_i32x4_t zero = _MM_SET1_I32(0);
    return _mm_testz_si128(cond, zero);
}

// Condition check with for loop for better performance
static inline int
v4_any_u32_loop(v_i32x4_t cond)
{
    int ret = 0;

    for (int i = 0; i < 4; i++)
    {
        if(cond[i] !=0)
            ret= 1;
    }
    return ret;
}


/***********************
***** v8s functions ****
***********************/

// v_f32x8_t to v_u32x8_t
static inline v_u32x8_t
as_v_u32x8_t (v_f32x8_t x)
{
  union { v_f32x8_t f; v_u32x8_t u; } r = {x};
  return r.u;
}

// v_f32x8_t to v_i32x8_t
static inline v_i32x8_t
v8_to_f32_i32(v_f32x8_t _xf32)
{
    return (v_i32x8_t){_xf32[0], _xf32[1], _xf32[2], _xf32[3],
                        _xf32[4], _xf32[5], _xf32[6], _xf32[7]};
}

// v_i32x8_t to v_f32x8_t
static inline v_f32x8_t
v8_to_f32_s32(v_i32x8_t _xi32)
{
    return (v_f32x8_t){_xi32[0], _xi32[1], _xi32[2], _xi32[3],
            _xi32[4], _xi32[5], _xi32[6], _xi32[7] };
}


/*
 * On x86, 'cond' contains all 0's for false, and all 1's for true
 * IOW, 0=>false, -1=>true
 */
static inline int
v8_any_u32(v_i32x8_t cond)
{
    const v_i32x8_t zero = _MM256_SET1_I32(0);
    return _mm256_testz_si256(cond, zero);
}



// Condition check with for loop for better performance
static inline int
v8_any_u32_loop(v_i32x8_t cond)
{
     int ret = 0;

    for (int i = 0; i < 8; i++)
    {
        if(cond[i] !=0)
            ret= 1;
    }
    return ret;
}


/***********************
***** v8s functions ****
************************/




#endif
