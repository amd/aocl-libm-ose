#ifndef INCLUDE_H_INCLUDED
#define INCLUDE_H_INCLUDED

#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>

/*func ptr types*/
/*scalar*/
typedef float  (*funcf)   (float);
typedef float  (*funcf_2) (float, float);
typedef double (*func)    (double);
typedef double (*func_2)  (double, double);
typedef float  (*funcf_int) (float, int);
typedef double (*func_int)  (double, int);

/*other*/
typedef float (*funcf_nan) (const char*);
typedef double (*func_nan) (const char*);
typedef float (*funcf_remquo) (float, float, int*);
typedef double (*func_remquo) (double, double, int*);
typedef void (*funcf_sincos) (float, float*, float*);
typedef void (*func_sincos)  (double, double*, double*);

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

/*utlity functions*/
float  GenerateRangeFloat   (float  min,  float  max);
double GenerateRangeDouble  (double min,  double max);

//test functions
int test_exp        (void*);
int test_log        (void*);
int test_pow        (void*);
int test_sin        (void*);
int test_cos        (void*);
int test_tan        (void*);
int test_fabs       (void*);
int test_asin       (void*);
int test_acos       (void*);
int test_atan       (void*);
int test_sinh       (void*);
int test_cosh       (void*);
int test_tanh       (void*);
int test_asinh      (void*);
int test_acosh      (void*);
int test_atanh      (void*);
int test_atan2      (void*);
int test_sqrt       (void*);
int test_nextafter  (void*);
int test_exp10      (void*);
int test_remainder  (void*);
int test_nan        (void*);
int test_floor      (void*);
int test_round      (void*);
int test_rint       (void*);
int test_lround     (void*);
int test_llround    (void*);
int test_nearbyint  (void*);
int test_exp2       (void*);
int test_fmod       (void*);
int test_finite     (void*);
int test_nexttoward (void*);
int test_lrint      (void*);
int test_llrint     (void*);
int test_hypot      (void*);
int test_ceil       (void*);
int test_log2       (void*);
int test_logb       (void*);
int test_log10      (void*);
int test_scalbln    (void*);
int test_ilogb      (void*);
int test_log1p      (void*);
int test_modf       (void*);
int test_remquo     (void*);
int test_ldexp      (void*);
int test_frexp      (void*);
int test_scalbn     (void*);
int test_cbrt       (void*);
int test_expm1      (void*);
int test_trunc      (void*);
int test_copysign   (void*);
int test_fdim       (void*);
int test_fmax       (void*);
int test_fmin       (void*);
int test_sincos     (void*);

#endif
