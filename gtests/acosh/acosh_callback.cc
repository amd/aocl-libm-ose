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
#include "test_acosh_data.h"
#include "../libs/mparith/alm_mp_funcs.h"

float LIBM_FUNC(acoshf)(float);
double LIBM_FUNC(acosh)(double);

static uint32_t ipargs = 1;

uint32_t GetnIpArgs( void )
{
	return ipargs;
}

void ConfSetupf32(SpecParams *specp) {
  specp->data32 = test_acoshf_conformance_data;
  specp->countf = ARRAY_SIZE(test_acoshf_conformance_data); 
}

void ConfSetupf64(SpecParams *specp) {
  specp->data64 = test_acosh_conformance_data;
  specp->countd = ARRAY_SIZE(test_acosh_conformance_data); 
}

float getFuncOp(float *data) {
  return LIBM_FUNC(acoshf)(data[0]);
}

double getFuncOp(double *data) {
  return LIBM_FUNC(acosh)(data[0]);
}

double getExpected(float *data) {
  auto val = alm_mp_acoshf(data[0]);
  return val;
}

long double getExpected(double *data) {
  auto val = alm_mp_acosh(data[0]);
  return val;
}

float getGlibcOp(float *data) {
  return acoshf(data[0]);
}

double getGlibcOp(double *data) {
  return acosh(data[0]);
}

/**********************
*FUNCTIONS*
**********************/
int test_s1s(test_data *data, int idx)  {
  float *ip  = (float*)data->ip;
  float *op  = (float*)data->op;
  op[0] = LIBM_FUNC(acoshf)(ip[idx]);
  return 0;
}

int test_s1d(test_data *data, int idx)  {
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op; 
  op[0] = LIBM_FUNC(acosh)(ip[idx]);
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

/*
#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
#define _ZGVdN2v_acosh _ZGVbN2v_acosh
#define _ZGVdN4v_acosh _ZGVdN4v_acosh
#define _ZGVsN4v_acoshf _ZGVbN4v_acoshf
#define _ZGVsN8v_acoshf _ZGVdN8v_acoshf
#endif
*/

/*vector routines*/
/*
__m128d LIBM_FUNC_VEC(d, 2, acosh)(__m128d);
__m256d LIBM_FUNC_VEC(d, 4, acosh)(__m256d);

__m128 LIBM_FUNC_VEC(s, 4, acoshf)(__m128);
__m256 LIBM_FUNC_VEC(s, 8, acoshf)(__m256);
*/

int test_v2d(test_data *data, int idx)  {
#if 0
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op; 
  __m128d ip2 = _mm_set_pd(ip[idx+1], ip[idx]);
  __m128d op2 = LIBM_FUNC_VEC(d, 2, acosh)(ip2);
  _mm_store_pd(&op[0], op2);
#endif
  return 0;
}

int test_v4s(test_data *data, int idx)  {
#if 0
  float *ip  = (float*)data->ip;
  float *op  = (float*)data->op; 
  __m128 ip4 = _mm_set_ps(ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m128 op4 = LIBM_FUNC_VEC(s, 4, acoshf)(ip4);
  _mm_store_ps(&op[0], op4);
#endif
  return 0;
}

int test_v4d(test_data *data, int idx)  {
#if 0
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op; 
  __m256d ip4 = _mm256_set_pd(ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m256d op4 = LIBM_FUNC_VEC(d, 4, acosh)(ip4);
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
  __m256 op8 = LIBM_FUNC_VEC(s, 8, acoshf)(ip8);
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

int test_v16s(test_data *data, int idx)  {
#if defined(__AVX512__)
  float *ip = (float*)data->ip;
  float *op  = (float*)data->op;
  __m512 ip16 = _mm512_set_ps(ip[idx+15], ip[idx+14], ip[idx+13], ip[idx+12],
                              ip[idx+11], ip[idx+10], ip[idx+9], ip[idx+8],
                              ip[idx+7], ip[idx+6], ip[idx+5], ip[idx+4],
                             ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m512 op16 = LIBM_FUNC_VEC(s, 16, acoshf)(ip16);
  _mm512_store_ps(&op[0], op16);
#endif
  return 0;
}

#ifdef __cplusplus
}
#endif
