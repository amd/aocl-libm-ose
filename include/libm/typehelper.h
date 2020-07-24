/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

#ifndef __LIBM_TYPEHELPER_H__
#define __LIBM_TYPEHELPER_H__

#include <float.h>
#include <libm/types.h>

#define _MM_SET1_PS4(x)					\
	_Generic((x),					\
		 float: (__m128){(x), (x), (x), (x)})

#define _MM_SET1_PS8(x)					\
	_Generic((x),					\
		 float: (__m256){(x), (x), (x), (x),	\
			 (x), (x), (x), (x)})

#define _MM_SET1_PD2(x)					\
	_Generic((x),					\
		 double: (__m128d){(x), (x)})

#define _MM_SET1_PD4(x)					\
	_Generic((x),					\
		 double: (__m256d){(x), (x), (x), (x)})

#define _MM_SET1_I32(x) {(x), (x), (x), (x)}

#define _MM_SET1_I64(x) {(x), (x), (x), (x)}

#define _MM256_SET1_I32(x) {(x), (x), (x), (x), (x), (x), (x), (x) }

#define _MM256_SET1_PS8(x) {(x), (x), (x), (x), (x), (x), (x), (x) }

static inline uint32_t
asuint32(float f)
{
	flt32u_t fl = {.f = f};
	return fl.i;
}

static inline float
asfloat(uint32_t i)
{
	flt32u_t fl = {.i = i};
	return fl.f;
}

static inline double
asdouble(uint64_t i)
{
	flt64_t dbl = {.i = i};
	return dbl.d;
}

static inline uint64_t
asuint64(double f)
{
	flt64u_t fl = {.d = f};
	return fl.i;
}

static inline double
eval_as_double(double d)
{
    return d;
}

static inline int32_t
cast_float_to_i32( float x )
{
	return (int32_t) x;
}

static inline   int64_t
cast_double_to_i64( double x )
{
	return (int64_t) x;
}

static inline float
cast_i32_to_float( int32_t x )
{
	return (float)x;
}

static inline double
cast_i64_to_double( int64_t x )
{
	return (double)x;
}

static inline v_u32x4_t
v_as_u32_f32 (v_f32x4_t x)
{
  union { v_f32x4_t f; v_u32x4_t u; } r = {x};
  return r.u;
}

static inline v_i32x4_t
as_v_i32x4(v_f32x4_t x)
{
	union {
		v_i32x4_t _xi;
		v_f32x4_t _xf;
	} val = {
		._xf = x,
	};

	return val._xi;
}

static inline v_u64x4_t
as_v_u64x4(v_f64x4_t x)
{
    union {
        v_u64x4_t _xi;
        v_f64x4_t _xf;
    } val = {
        ._xf = x,
    };

    return val._xi;
}

static inline v_u32x8_t
as_v_u32x8(v_f32x8_t x)
{
    union {
        v_u32x8_t _xi;
        v_f32x8_t _xf;
    } val = {
        ._xf = x,
    };

    return val._xi;
}

static inline v_f64x4_t
as_f64(v_u64x4_t x)
{
    union {
        v_u64x4_t _xi;
        v_f64x4_t _xf;
    } val = {
        ._xi = x,
    };

    return val._xf;
}

static inline v_f32x4_t
as_v_f32x4(v_i32x4_t x)
{
	union {
		v_i32x4_t _xi;
		v_f32x4_t _xf;
	} val = {
		._xi = x,
	};

	return val._xf;
}

static inline v_i64x4_t
as_v_i64x4(v_f64x4_t x)
{
    union {
        v_i64x4_t _xi;
        v_f64x4_t _xf;
   } val = {
       ._xf = x,
   };

  return val._xi;
}

static inline v_f64x4_t
as_v_f64x4(v_i64x4_t x)
{
    union {
        v_i64x4_t _xi;
        v_f64x4_t _xf;
    } val = {
        ._xi = x,
    };

    return val._xf;
}

static inline v_f64x4_t
as_v_f64(v_u64x4_t x)
{
     union {
         v_u64x4_t _xu;
         v_f64x4_t _xf;
     } val = {
         ._xu = x,
     };

     return val._xf;
}

static inline v_f32x8_t
as_v_f32x8(v_u32x8_t x)
{
    union {
        v_u32x8_t _xu;
        v_f32x8_t _xf;
    } val = {
        ._xu = x,
    };

    return val._xf;
}
/*
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
*/
static inline v_f64x2_t
as_v_f64x2(v_i64x2_t x)
{
    union {
        v_i64x2_t _xi;
        v_f64x2_t _xf;
        } val = {
            ._xi = x,
        };

    return val._xf;
}


static inline float
v_f32x4_to_float(v_f32x4_t f32x4, int idx)
{
    union  {
        v_f32x4_t __f32x4;
        float     f32[4];
    } val = {
        .__f32x4 = f32x4,
    };

    return val.f32[idx];
}

static inline uint32_t
v_f32x4_to_uint32(v_f32x4_t f32x4, int idx)
{
    union  {
        v_f32x4_t __f32x4;
        uint32_t   u32[4];
    } val = {
        .__f32x4 = f32x4,
    };

    return val.u32[idx];
}

static inline v_f32x4_t
v_lookup_f32(const float *tab, v_i32x4_t idx)
{
    return (v_f32x4_t) {tab[idx[0]], tab[idx[1]], tab[idx[2]], tab[idx[3]]};
}

static inline v_u32x4_t
v_lookup_u32(const uint32_t *tab, v_u32x4_t idx)
{
    return (v_u32x4_t) {tab[idx[0]], tab[idx[1]], tab[idx[2]], tab[idx[3]]};
}

static inline v_f32x8_t
v_call_f32_2(float (*fn)(float),
	   v_f32x8_t orig,
	   v_f32x8_t result,
	   v_i32x8_t cond)
{
    return (v_f32x8_t) {
        cond[0] ? fn(orig[0]) : result[0],
        cond[1] ? fn(orig[1]) : result[1],
        cond[2] ? fn(orig[2]) : result[2],
        cond[3] ? fn(orig[3]) : result[3],
        cond[4] ? fn(orig[4]) : result[4],
        cond[5] ? fn(orig[5]) : result[5],
        cond[6] ? fn(orig[6]) : result[6],
        cond[7] ? fn(orig[7]) : result[7]
    };
}

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

#endif	/* __LIBM_TYPEHELPER_H__ */
