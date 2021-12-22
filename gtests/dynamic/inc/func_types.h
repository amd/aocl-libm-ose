#ifndef FUNC_TYPES_H_INCLUDED
#define FUNC_TYPES_H_INCLUDED

#include <immintrin.h>
#include <complex.h>

/*func ptr types*/
/*scalar*/
typedef float  (*funcf)     (float);
typedef float  (*funcf_2)   (float, float);
typedef double (*func)      (double);
typedef double (*func_2)    (double, double);
typedef float  (*funcf_int) (float, int);
typedef double (*func_int)  (double, int);

/* complex scalar */
typedef float _Complex (*funcf_cmplx) (float _Complex);
typedef double _Complex (*func_cmplx) (double _Complex);
typedef float _Complex (*funcf_cmplx_2) (float _Complex, float _Complex);
typedef double _Complex (*func_cmplx_2) (double _Complex, double _Complex);

/*other*/
typedef float  (*funcf_nan)    (const char*);
typedef double (*func_nan)     (const char*);
typedef float  (*funcf_remquo) (float, float, int*);
typedef double (*func_remquo)  (double, double, int*);
typedef void   (*funcf_sincos) (float, float*, float*);
typedef void   (*func_sincos)  (double, double*, double*);

/*vector*/
typedef __m128d (*func_v2d)    (__m128d);
typedef __m128d (*func_v2d_2)  (__m128d, __m128d);
typedef __m256d (*func_v4d)    (__m256d);
typedef __m256d (*func_v4d_2)  (__m256d, __m256d);
typedef __m128  (*funcf_v4s)   (__m128);
typedef __m128  (*funcf_v4s_2) (__m128, __m128);
typedef __m256  (*funcf_v8s)   (__m256);
typedef __m256  (*funcf_v8s_2) (__m256, __m256);

/*array vector*/
typedef void (*funcf_va)   (int, float*, float*);
typedef void (*funcf_va_2) (int, float*, float*, float*);
typedef void (*func_va)    (int, double*, double*);
typedef void (*func_va_2)  (int, double*, double*, double*);

/* avx512 functions */
#if defined(__AVX512__)
typedef __m512d (*func_v8d)     (__m512d);
typedef __m512d (*func_v8d_2)   (__m512d, __m512d);

typedef __m512  (*funcf_v16s)   (__m512);
typedef __m512  (*funcf_v16s_2) (__m512, __m512);
#endif

#endif
