#ifndef INCLUDE_H_INCLUDED
#define INCLUDE_H_INCLUDED

#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>

//function pointers
float (*amd_expf) (float);
double (*amd_exp) (double);

float (*amd_logf) (float);
double (*amd_log) (double);

float (*amd_powf) (float,  float);
double (*amd_pow) (double, double);

float (*amd_sinf) (float);
double (*amd_sin) (double);

float (*amd_cosf) (float);
double (*amd_cos) (double);

float (*amd_tanf) (float);
double (*amd_tan) (double);

float (*amd_fabsf) (float);
double (*amd_fabs) (double);

float (*amd_atanf) (float);
double (*amd_atan) (double);

/*vector*/
__m128d (*amd_vrd2_exp)  (__m128d);
__m128  (*amd_vrs4_expf) (__m128);
__m256d (*amd_vrd4_exp)  (__m256d);
__m256  (*amd_vrs8_expf) (__m256);

__m128d (*amd_vrd2_log)  (__m128d);
__m128  (*amd_vrs4_logf) (__m128);
__m256d (*amd_vrd4_log)  (__m256d);
__m256  (*amd_vrs8_logf) (__m256);

__m128d (*amd_vrd2_sin)  (__m128d);
__m128  (*amd_vrs4_sinf) (__m128);
__m256d (*amd_vrd4_sin)  (__m256d);
__m256  (*amd_vrs8_sinf) (__m256);

__m128d (*amd_vrd2_cos)  (__m128d);
__m128  (*amd_vrs4_cosf) (__m128);
//__m256d (*amd_vrd4_cos)  (__m256d);
//__m256  (*amd_vrs8_cosf) (__m256);

__m128d (*amd_vrd2_tan)  (__m128d);
__m128  (*amd_vrs4_tanf) (__m128);
//__m256d (*amd_vrd4_tan)  (__m256d);
//__m256  (*amd_vrs8_tanf) (__m256);

__m128d (*amd_vrd2_pow)  (__m128d, __m128d);
__m128  (*amd_vrs4_powf) (__m128,  __m128);
__m256d (*amd_vrd4_pow)  (__m256d, __m256d);
__m256  (*amd_vrs8_powf) (__m256,  __m256);

//tets functions
int test_exp(void*);
int test_log(void*);
int test_pow(void*);
int test_sin(void*);
int test_cos(void*);
int test_tan(void*);
int test_fabs(void*);
int test_atan(void*);

#endif
