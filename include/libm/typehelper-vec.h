#ifndef __LIBM_TYPEHELPER_VEC_H__
#define __LIBM_TYPEHELPER_VEC_H__

#include <libm/types.h>

#include <emmintrin.h>

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


#endif
