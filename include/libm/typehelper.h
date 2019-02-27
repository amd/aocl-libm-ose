#ifndef __LIBM_TYPEHELPER_H__
#define __LIBM_TYPEHELPER_H__

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
