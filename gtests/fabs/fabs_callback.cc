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
#include "test_fabs_data.h"
#include "../libs/mparith/alm_mp_funcs.h"

float LIBM_FUNC(fabsf)(float);
double LIBM_FUNC(fabs)(double);

static uint32_t ipargs = 1;

uint32_t GetnIpArgs( void )
{
	return ipargs;
}

void ConfSetupf32(SpecParams *specp) {
  specp->data32 = test_fabsf_conformance_data;
  specp->countf = ARRAY_SIZE(test_fabsf_conformance_data); 
}

void ConfSetupf64(SpecParams *specp) {
  specp->data64 = test_fabs_conformance_data;
  specp->countd = ARRAY_SIZE(test_fabs_conformance_data); 
}

float getFuncOp(float *data) {
  return LIBM_FUNC(fabsf)(data[0]);
}

double getFuncOp(double *data) {
  return LIBM_FUNC(fabs)(data[0]);
}

float getExpected(float *data) {
  auto val = alm_mp_fabsf(data[0]);
  return val;
}

double getExpected(double *data) {
  auto val = alm_mp_fabs(data[0]);
  return val;
}

float getGlibcOp(float *data) {
  return fabsf(data[0]);
}

double getGlibcOp(double *data) {
  return fabs(data[0]);
}

/**********************
*FUNCTIONS*
**********************/
int test_s1s(test_data *data, int idx)  {
  float *ip  = (float*)data->ip;
  float *op  = (float*)data->op;
  op[0] = LIBM_FUNC(fabsf)(ip[idx]);
  return 0;
}

int test_s1d(test_data *data, int idx)  {
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op; 
  op[0] = LIBM_FUNC(fabs)(ip[idx]);
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
#define _ZGVdN2v_fabs _ZGVbN2v_fabs
#define _ZGVdN4v_fabs _ZGVdN4v_fabs
#define _ZGVsN4v_fabsf _ZGVbN4v_fabsf
#define _ZGVsN8v_fabsf _ZGVdN8v_fabsf
#endif

/*vector routines*/
__m128d LIBM_FUNC_VEC(d, 2, fabs)(__m128d);
__m256d LIBM_FUNC_VEC(d, 4, fabs)(__m256d);

__m128 LIBM_FUNC_VEC(s, 4, fabsf)(__m128);
__m256 LIBM_FUNC_VEC(s, 8, fabsf)(__m256);

int test_v2d(test_data *data, int idx)  {
#if 0
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op; 
  __m128d ip2 = _mm_set_pd(ip[idx+1], ip[idx]);
  __m128d op2 = LIBM_FUNC_VEC(d, 2, fabs)(ip2);
  _mm_store_pd(&op[0], op2);
#endif
  return 0;
}

int test_v4s(test_data *data, int idx)  {
#if 0
  float *ip  = (float*)data->ip;
  float *op  = (float*)data->op; 
  __m128 ip4 = _mm_set_ps(ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m128 op4 = LIBM_FUNC_VEC(s, 4, fabsf)(ip4);
  _mm_store_ps(&op[0], op4);
#endif
  return 0;
}

int test_v4d(test_data *data, int idx)  {
#if 0
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op; 
  __m256d ip4 = _mm256_set_pd(ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m256d op4 = LIBM_FUNC_VEC(d, 4, fabs)(ip4);
  _mm256_store_pd(&op[0], op4);
#endif
  return 0;
}

int test_v8s(test_data *data, int idx)  {
#if 0
  float *ip  = (float*)data->ip;
  float *op  = (float*)data->op; 
  __m256 ip8 = _mm256_set_ps(ip[idx+7], ip[idx+6], ip[idx+5], ip[idx+4],
                             ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m256 op8 = LIBM_FUNC_VEC(s, 8, fabsf)(ip8);
  _mm256_store_ps(&op[0], op8);
#endif  
  return 0;
}

#ifdef __cplusplus
}
#endif
