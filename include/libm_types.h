#ifndef __LIBM_TYPES_H__
#define __LIBM_TYPES_H__

#define PASTE2(a, b) a##b
#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

#define OPT_O1 __attribute__((optimize("O1")))
#define OPT_O2 __attribute__((optimize("O2")))
#define OPT_O3 __attribute__((optimize("O3")))
#define OPT_Og __attribute__((optimize("Og")))

#define OPTIMIZE(x) __attribute__((optimize("O" STRINGIFY(x))))

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

#endif	/* LIBM_TYPES_H */
