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
#include "test_log1p_data.h"
#include "../libs/mparith/alm_mp_funcs.h"

float LIBM_FUNC(log1pf)(float);
double LIBM_FUNC(log1p)(double);

static uint32_t ipargs = 1;

uint32_t GetnIpArgs( void )
{
	return ipargs;
}

void ConfSetupf32(SpecParams *specp) {
  specp->data32 = test_log1pf_conformance_data;
  specp->countf = ARRAY_SIZE(test_log1pf_conformance_data); 
}

void ConfSetupf64(SpecParams *specp) {
  specp->data64 = test_log1p_conformance_data;
  specp->countd = ARRAY_SIZE(test_log1p_conformance_data); 
}

float getFuncOp(float *data) {
  return LIBM_FUNC(log1pf)(data[0]);
}

double getFuncOp(double *data) {
  return LIBM_FUNC(log1p)(data[0]);
}

double getExpected(float *data) {
  auto val = alm_mp_log1pf(data[0]);
  return val;
}

long double getExpected(double *data) {
  auto val = alm_mp_log1p(data[0]);
  return val;
}

float getGlibcOp(float *data) {
  return log1pf(data[0]);
}

double getGlibcOp(double *data) {
  return log1p(data[0]);
}

/**********************
*FUNCTIONS*
**********************/
int test_s1s(test_data *data, int idx)  {
  float *ip  = (float*)data->ip;
  float *op  = (float*)data->op;
  op[0] = LIBM_FUNC(log1pf)(ip[idx]);
  return 0;
}

int test_s1d(test_data *data, int idx)  {
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op; 
  op[0] = LIBM_FUNC(log1p)(ip[idx]);
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

/*
#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
#define _ZGVdN2v_log1p _ZGVbN2v_log1p
#define _ZGVdN4v_log1p _ZGVdN4v_log1p
#define _ZGVsN4v_log1pf _ZGVbN4v_log1pf
#define _ZGVsN8v_log1pf _ZGVdN8v_log1pf
#endif
*/

/*vector routines*/
/*
__m128d LIBM_FUNC_VEC(d, 2, log1p)(__m128d);
__m256d LIBM_FUNC_VEC(d, 4, log1p)(__m256d);

__m128 LIBM_FUNC_VEC(s, 4, log1pf)(__m128);
__m256 LIBM_FUNC_VEC(s, 8, log1pf)(__m256);
*/

int test_v2d(test_data *data, int idx)  {
#if 0
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op; 
  __m128d ip2 = _mm_set_pd(ip[idx+1], ip[idx]);
  __m128d op2 = LIBM_FUNC_VEC(d, 2, log1p)(ip2);
  _mm_store_pd(&op[0], op2);
#endif
  return 0;
}

int test_v4s(test_data *data, int idx)  {
#if 0
  float *ip  = (float*)data->ip;
  float *op  = (float*)data->op; 
  __m128 ip4 = _mm_set_ps(ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m128 op4 = LIBM_FUNC_VEC(s, 4, log1pf)(ip4);
  _mm_store_ps(&op[0], op4);
#endif
  return 0;
}

int test_v4d(test_data *data, int idx)  {
#if 0
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op; 
  __m256d ip4 = _mm256_set_pd(ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m256d op4 = LIBM_FUNC_VEC(d, 4, log1p)(ip4);
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
  __m256 op8 = LIBM_FUNC_VEC(s, 8, log1pf)(ip8);
  _mm256_store_ps(&op[0], op8);
#endif  
  return 0;
}

int test_v8d(test_data *data, int idx)  {
#if defined(__AVX512__)
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op;
  __m512d ip8 = _mm512_set_pd(ip[idx+7], ip[idx+6], ip[idx+5], ip[idx+4],
                             ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m512d op8 = LIBM_FUNC_VEC(d, 8, exp)(ip8);
  _mm512_store_pd(&op[0], op8);
#endif
  return 0;
}

#ifdef __cplusplus
}
#endif
