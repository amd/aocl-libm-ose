#ifndef __LIBM_TYPES_H__
#define __LIBM_TYPES_H__

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
	uint64_t i;
	double   d;
} flt64u_t;

typedef union {
	int64_t i;
	double  d;
} flt64_t;

#endif	/* LIBM_TYPES_H */
