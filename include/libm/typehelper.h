#ifndef __LIBM_TYPEHELPER_H__
#define __LIBM_TYPEHELPER_H__

#include <libm/types.h>

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
static inline   int32_t
cast_double_to_i32( double x )
{
	return (int32_t) x;
}

static inline   int64_t
cast_double_to_i64( double x )
{
	return (int64_t) x;
}

static inline double
cast_i32_to_double( int32_t x )
{
	return (double)x;
}

static inline double
cast_i64_to_double( int64_t x )
{
	return (double)x;
}

#endif	/* __LIBM_TYPEHELPER_H__ */
