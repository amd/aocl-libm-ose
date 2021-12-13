#ifndef LIBM_GLIBC_COMPAT_H_INCLUDED
#define LIBM_GLIBC_COMPAT_H_INCLUDED

#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>

extern int test_exp(void*);
extern int test_log(void*);
extern int test_pow(void*);
extern int test_sin(void*);
extern int test_cos(void*);

/*vector*/
typedef __m128d (*func_v2d)    (__m128d);
typedef __m128d (*func_v2d_2)  (__m128d, __m128d);
typedef __m256d (*func_v4d)    (__m256d);
typedef __m256d (*func_v4d_2)  (__m256d, __m256d);
typedef __m128  (*funcf_v4s)   (__m128);
typedef __m128  (*funcf_v4s_2) (__m128, __m128);
typedef __m256  (*funcf_v8s)   (__m256);
typedef __m256  (*funcf_v8s_2) (__m256, __m256);

#endif  /* LIBM_GLIBC_COMPAT_H_INCLUDED */
