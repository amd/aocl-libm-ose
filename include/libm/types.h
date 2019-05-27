#ifndef __LIBM_TYPES_H__
#define __LIBM_TYPES_H__

#include <stdint.h>
#include <float.h>
#include <immintrin.h>

#define PASTE2(a, b) a##b
#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

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



/*
 * Packed data structure to access different parts of a floating point number
 */

struct __flt64_packed {
    unsigned long long sign     :1;
    unsigned long long exponent :11;
    unsigned long long mantissa :52;
} flt64_packed_t;

struct __flt32_packed {
    unsigned sign      :1;
    unsigned exponent  :8;
    unsigned mantissa  :23;
} flt32_packed_t;


/*****************************
 * Internal vector types
 *****************************/


/*
 * (u)int32 - 4 elements - 128 bit
 */
typedef union {
    int32_t i[4];
    __m128i m128i;
} int128_t;

typedef union {
    uint32_t i[4];
    __m128i m128i;
} int128u_t;


/*
 * float32 - 4 elements - 128 bit
 */
typedef union {
    int32_t i[4];
    float   f[4];
    __m128  m128;
} flt128f_t;

typedef union {
    uint32_t i[4];
    float    f[4];
    __m128   m128;
} flt128fu_t;

/*
 * float32 - 8 element - 256 bits
 */
typedef union {
    int32_t i[8];
    float   f[8];
    __m256  m256;
} flt256f_t;

typedef union {
    uint32_t i[8];
    float    f[8];
    __m256   m256;
} flt256fu_t;

/*
 * float64 - 2 element - 128 bits
 */
typedef union {
    int64_t i[2];
    double  d[2];
    __m128d  m128;
} flt128d_t;

typedef union {
    uint64_t  i[2];
    double   d[2];
    __m256d  m128;
} flt128du_t;

/*
 * float64 - 4 element - 256 bits
 */
typedef union {
    int64_t  i[4];
    double   d[4];
    __m256d  m256d;
} flt256d_t;

typedef union {
    uint64_t  i[4];
    double    d[4];
    __m256d   m256d;
} flt256du_t;


#endif	/* LIBM_TYPES_H */
