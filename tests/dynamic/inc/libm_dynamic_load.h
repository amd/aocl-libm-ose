#ifndef INCLUDE_H_INCLUDED
#define INCLUDE_H_INCLUDED

#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>

//function pointers
extern float (*amd_expf) (float);
extern double (*amd_exp) (double);

extern float (*amd_logf) (float);
extern double (*amd_log) (double);

extern float (*amd_powf) (float,  float);
extern double (*amd_pow) (double, double);

extern float (*amd_sinf) (float);
extern double (*amd_sin) (double);

extern float (*amd_cosf) (float);
extern double (*amd_cos) (double);

extern float (*amd_tanf) (float);
extern double (*amd_tan) (double);

extern float (*amd_tanhf) (float);
extern double (*amd_tanh) (double);

extern float (*amd_coshf) (float);
extern float (*amd_cosh)  (double);

extern float (*amd_fabsf) (float);
extern double (*amd_fabs) (double);

extern float (*amd_atanf) (float);
extern double (*amd_atan) (double);

/*vector*/
extern __m128d (*amd_vrd2_exp)  (__m128d);
extern __m128 (*amd_vrs4_expf) (__m128);
extern __m256d (*amd_vrd4_exp)  (__m256d);
extern __m256 (*amd_vrs8_expf) (__m256);

extern __m128d (*amd_vrd2_log)  (__m128d);
extern __m128 (*amd_vrs4_logf) (__m128);
extern __m256d (*amd_vrd4_log)  (__m256d);
extern __m256 (*amd_vrs8_logf) (__m256);

extern __m128d (*amd_vrd2_sin)  (__m128d);
extern __m128 (*amd_vrs4_sinf) (__m128);
extern __m256d (*amd_vrd4_sin)  (__m256d);
extern __m256 (*amd_vrs8_sinf) (__m256);

extern __m128d (*amd_vrd2_cos)  (__m128d);
extern __m128 (*amd_vrs4_cosf) (__m128);
extern __m256d (*amd_vrd4_cos)  (__m256d);
extern __m256 (*amd_vrs8_cosf) (__m256);

extern __m128d (*amd_vrd2_tan)  (__m128d);
extern __m128 (*amd_vrs4_tanf) (__m128);
extern __m256d (*amd_vrd4_tan)  (__m256d);
extern __m256 (*amd_vrs8_tanf) (__m256);

extern __m128d (*amd_vrd2_pow)  (__m128d, __m128d);
extern __m128 (*amd_vrs4_powf) (__m128,  __m128);
extern __m256d (*amd_vrd4_pow)  (__m256d, __m256d);
extern __m256 (*amd_vrs8_powf) (__m256,  __m256);

/*hyperbolic*/
extern __m128 (*amd_vrs4_coshf) (__m128);
extern __m256 (*amd_vrs8_coshf) (__m256);

//tets functions
extern int test_exp(void*);
extern int test_log(void*);
extern int test_pow(void*);
extern int test_sin(void*);
extern int test_cos(void*);
extern int test_tan(void*);
extern int test_fabs(void*);
extern int test_atan(void*);
extern int test_cosh(void*);
extern int test_tanh(void*);
extern int test_sinh(void*);
#endif
