#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>

int test_exp(void*);
int test_log(void*);
int test_pow(void*);
int test_sin(void*);
int test_cos(void*);

__m128d (*exp_v2d)  (__m128d); //v2d exp
__m256d (*exp_v4d)  (__m256d); // v4d
__m128  (*expf_v4s) (__m128);  //v4s expf
__m256  (*expf_v8s) (__m256);  //v8s expf

__m128d (*log_v2d)  (__m128d);
__m256d (*log_v4d)  (__m256d);
__m128  (*logf_v4s) (__m128);
__m256  (*logf_v8s) (__m256);

__m128d (*pow_v2d)  (__m128d, __m128d);
__m256d (*pow_v4d)  (__m256d, __m256d);
__m128  (*powf_v4s) (__m128,  __m128);
__m256  (*powf_v8s) (__m256,  __m256);

__m128d (*sin_v2d)  (__m128d);
__m256d (*sin_v4d)  (__m256d);
__m128  (*sinf_v4s) (__m128);
__m256  (*sinf_v8s) (__m256);

__m128d (*cos_v2d)  (__m128d);
__m256d (*cos_v4d)  (__m256d);
__m128  (*cosf_v4s) (__m128);
__m256  (*cosf_v8s) (__m256);
