/*
 * Copyright (C) 2019-2020 Advanced Micro Devices, Inc. All rights reserved
 */

#include <cmath>
#include "libm_tests.h"
#include "../../include/libm_macros.h"

#define AMD_LIBM_VEC_EXPERIMENTAL

#include "../../include/libm_amd.h"
#include "../../include/libm/amd_funcs_internal.h"
#include <fmaintrin.h>
#include <immintrin.h>
#include "callback.h"
#include "test_pow_data.h"
#include "../libs/mparith/alm_mp_funcs.h"

double LIBM_FUNC(pow)(double);
float LIBM_FUNC(powf)(float);

static uint32_t ipargs = 2;

uint32_t GetnIpArgs( void )
{
	return ipargs;
}

void ConfSetupf32(SpecParams *specp) {
  specp->data32 = test_powf_conformance_data;
  specp->countf = ARRAY_SIZE(test_powf_conformance_data); 
}

void ConfSetupf64(SpecParams *specp) {
  specp->data64 = test_pow_conformance_data;
  specp->countd = ARRAY_SIZE(test_pow_conformance_data); 
}

float getFuncOp(float *data) {
  return LIBM_FUNC(powf)(data[0], data[1]);
}

double getFuncOp(double *data) {
  return LIBM_FUNC(pow)(data[0], data[1]);
}

float getExpected(float *data) {  
  auto val = alm_mp_powf(data[0], data[1]);
  return val;
}

double getExpected(double *data) {
  auto val = alm_mp_pow(data[0], data[1]);  
  return val;
}

float getGlibcOp(float *data) {
  return powf(data[0], data[1]);
}

double getGlibcOp(double *data) {
  return pow(data[0], data[1]);
}

/**********************
*FUNCTIONS*
**********************/
int test_s1s(test_data *data, int idx)  {
  float *ip1  = (float*)data->ip;
  float *ip2 = (float*)data->ip1;
  float *op  = (float*)data->op;
  op[0] = LIBM_FUNC(powf)(ip1[idx], ip2[idx]);
  return 0;
}

int test_s1d(test_data *data, int idx)  {
  double *ip1  = (double*)data->ip;
  double *ip2 = (double*)data->ip1;
  double *op  = (double*)data->op; 
  op[0] = LIBM_FUNC(pow)(ip1[idx], ip2[idx]);
  return 0;
}


#ifdef __cplusplus
extern "C" {
#endif

/* GLIBC prototype declarations */
#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
#define _ZGVdN2v_pow _ZGVbN2vv_pow
#define _ZGVdN4v_pow _ZGVdN4vv_pow
#define _ZGVsN4v_powf _ZGVbN4vv_powf
#define _ZGVsN8v_powf _ZGVdN8vv_powf
#endif

/*vector routines*/
__m128d LIBM_FUNC_VEC(d, 2, pow)(__m128d, __m128d);
__m256d LIBM_FUNC_VEC(d, 4, pow)(__m256d, __m256d);
__m128 LIBM_FUNC_VEC(s, 4, powf)(__m128, __m128);
__m256 LIBM_FUNC_VEC(s, 8, powf)(__m256, __m256);

int test_v2d(test_data *data, int idx)  {
  double *ip1  = (double*)data->ip;
  double *ip2 = (double*)data->ip1;
  double *op  = (double*)data->op;
  __m128d ip21 = _mm_set_pd(ip1[idx+1], ip1[idx]);
  __m128d ip22 = _mm_set_pd(ip2[idx+1], ip2[idx]);
  __m128d op2 = LIBM_FUNC_VEC(d, 2, pow)(ip21, ip22);
  _mm_store_pd(&op[0], op2);
  return 0;
}

int test_v4s(test_data *data, int idx)  {
  float *ip1  = (float*)data->ip;
  float *ip2 = (float*)data->ip1;
  float *op  = (float*)data->op; 
  __m128 ip41 = _mm_set_ps(ip1[idx+3], ip1[idx+2], ip1[idx+1], ip1[idx]);
  __m128 ip42 = _mm_set_ps(ip2[idx+3], ip2[idx+2], ip2[idx+1], ip2[idx]);
  __m128 op4 = LIBM_FUNC_VEC(s, 4, powf)(ip41, ip42);
  _mm_store_ps(&op[0], op4);
  return 0;
}

int test_v4d(test_data *data, int idx)  {
  double *ip1  = (double*)data->ip;
  double *ip2 = (double*)data->ip1;
  double *op  = (double*)data->op; 
  __m256d ip41 = _mm256_set_pd(ip1[idx+3], ip1[idx+2], ip1[idx+1], ip1[idx]);
  __m256d ip42 = _mm256_set_pd(ip2[idx+3], ip2[idx+2], ip2[idx+1], ip2[idx]);
  __m256d op4 = LIBM_FUNC_VEC(d, 4, pow)(ip41, ip42);
  _mm256_store_pd(&op[0], op4);
  return 0;
}

int test_v8s(test_data *data, int idx)  {

  float *ip1  = (float*)data->ip;
  float *ip2 = (float*)data->ip1;
  float *op  = (float*)data->op; 
  __m256 ip81 = _mm256_set_ps(ip1[idx+7], ip1[idx+6], ip1[idx+5], ip1[idx+4],
                              ip1[idx+3], ip1[idx+2], ip1[idx+1], ip1[idx]);
  __m256 ip82 = _mm256_set_ps(ip2[idx+7], ip2[idx+6], ip2[idx+5], ip2[idx+4],
                              ip2[idx+3], ip2[idx+2], ip2[idx+1], ip2[idx]);
  __m256 op8 = LIBM_FUNC_VEC(s, 8, powf)(ip81, ip82);
  _mm256_store_ps(&op[0], op8);
  return 0;
}
#ifdef __cplusplus
}
#endif

