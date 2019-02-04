#ifndef __LIBM_TYPES_H__
#define __LIBM_TYPES_H__

#include <stdint.h>
#include <float.h>
#include <immintrin.h>

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


/*****************************
 * Internal vector types
 *****************************/
typedef union {
    int64_t i[2];
    float   f[4];
    __m128  m128;
} flt128_t;

typedef union {
    uint64_t i[2];
    float    f[4];
    __m128   m128;
} flt128u_t;

typedef union {
    int64_t i[2];
    double  d[2];
    __m128d  m128;
} flt128d_t;

typedef union {
    uint64_t i[4];
    double    f[4];
    __m128d   m128;
} flt128du_t;

typedef union {
    int64_t i[2];
    float   f[4];
    __m256  m256;
} flt256_t;

typedef union {
    uint64_t i[2];
    float    f[4];
    __m256   m256;
} flt256u_t;

typedef union {
    int64_t i[2];
    double  d[2];
    __m256d  m256;
} flt256d_t;

typedef union {
    uint64_t i[4];
    double    f[4];
    __m256d   m256;
} flt256du_t;


static inline uint32_t
asuint (float f)
{
	flt32u_t fl = {.f = f};
	return fl.i;
}

static inline float
asfloat (uint32_t i)
{
	flt32u_t fl = {.i = i};
	return fl.f;
}

static inline float
asdouble (uint64_t i)
{
	flt64u_t dbl = {.i = i};
	return dbl.d;
}

static inline uint64_t
asuint64 (double f)
{
	flt64u_t fl = {.d = f};
	return fl.i;
}
#endif	/* LIBM_TYPES_H */
