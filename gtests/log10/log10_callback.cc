/*
 * Copyright (C) 2008-2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <cmath>
#include "libm_tests.h"
#include <libm_macros.h>

#define AMD_LIBM_VEC_EXPERIMENTAL

#include <libm_amd.h>
#include <libm/amd_funcs_internal.h>
#include <fmaintrin.h>
#include <immintrin.h>
#include "callback.h"
#include "test_log10_data.h"
#include "../libs/mparith/alm_mp_funcs.h"

float LIBM_FUNC(log10f)(float);
double LIBM_FUNC(log10)(double);

static uint32_t ipargs = 1;
bool special_case = false;

uint32_t GetnIpArgs( void )
{
	return ipargs;
}

bool getSpecialCase(void)
{
  return special_case;
}

void ConfSetupf32(SpecParams *specp) {
  specp->data32 = test_log10f_conformance_data;
  specp->countf = ARRAY_SIZE(test_log10f_conformance_data);
}

void ConfSetupf64(SpecParams *specp) {
  specp->data64 = test_log10_conformance_data;
  specp->countd = ARRAY_SIZE(test_log10_conformance_data);
}

float getFuncOp(float *data) {
  return LIBM_FUNC(log10f)(data[0]);
}

double getFuncOp(double *data) {
  return LIBM_FUNC(log10)(data[0]);
}

double getExpected(float *data) {
  auto val = alm_mp_log10f(data[0]);
  return val;
}

long double getExpected(double *data) {
  auto val = alm_mp_log10(data[0]);
  return val;
}

// Used by the Complex Number Functions only!
double _Complex getExpected(float _Complex *data) {
  return {0};
}

long double _Complex getExpected(double _Complex *data) {
  return {0};
}

float getGlibcOp(float *data) {
  return log10f(data[0]);
}

double getGlibcOp(double *data) {
  return log10(data[0]);
}

/**********************
*FUNCTIONS*
**********************/
int test_s1s(test_data *data, int idx)  {
  float *ip  = (float*)data->ip;
  float *op  = (float*)data->op;
  op[0] = LIBM_FUNC(log10f)(ip[idx]);
  return 0;
}

int test_s1d(test_data *data, int idx)  {
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op;
  op[0] = LIBM_FUNC(log10)(ip[idx]);
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

/* GLIBC vector function symbols needs to be re-defined accordingly */
#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
  #define _ZGVdN2v_log10 _ZGVbN2v_log10
  #define _ZGVdN4v_log10 _ZGVdN4v_log10
  #define _ZGVsN4v_log10f _ZGVbN4v_log10f
  #define _ZGVsN8v_log10f _ZGVdN8v_log10f
  #if defined(__AVX512__)
    #define _ZGVsN16v_log10f _ZGVeN16v_log10f
    #define _ZGVdN8v_log10 _ZGVeN8v_log10
  #endif
#endif

/* Declaration of vector routines */
#if (LIBM_PROTOTYPE != PROTOTYPE_MSVC)
  __m128d LIBM_FUNC_VEC(d, 2, log10) (__m128d);
  __m256d LIBM_FUNC_VEC(d, 4, log10) (__m256d);
  __m128  LIBM_FUNC_VEC(s, 4, log10f)(__m128);
  __m256  LIBM_FUNC_VEC(s, 8, log10f)(__m256);
  #if defined(__AVX512__)
    __m512d LIBM_FUNC_VEC(d, 8, log10)(__m512d);
    __m512  LIBM_FUNC_VEC(s, 16, log10f)(__m512);
  #endif
#endif

int test_v2d(test_data *data, int idx)  {
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op;
  __m128d ip2 = _mm_set_pd(ip[idx+1], ip[idx]);
  __m128d op2 = LIBM_FUNC_VEC(d, 2, log10)(ip2);
  _mm_store_pd(&op[0], op2);
  return 0;
}

int test_v4s(test_data *data, int idx)  {
/* glibc doesnt have log10f vectors */
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_SVML || LIBM_PROTOTYPE == PROTOTYPE_MSVC)
  float *ip  = (float*)data->ip;
  float *op  = (float*)data->op;
  __m128 ip4 = _mm_set_ps(ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  #if (LIBM_PROTOTYPE == PROTOTYPE_MSVC)
    __m128 op4 = LIBM_FUNC_VEC(s, 4, Log10)(ip4);
  #else
    __m128 op4 = LIBM_FUNC_VEC(s, 4, log10f)(ip4);
  #endif
  _mm_store_ps(&op[0], op4);
#endif
  return 0;
}

int test_v4d(test_data *data, int idx)  {
#if 0
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op;
  __m256d ip4 = _mm256_set_pd(ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m256d op4 = LIBM_FUNC_VEC(d, 4, log10)(ip4);
  _mm256_store_pd(&op[0], op4);
#endif
  return 0;
}

int test_v8s(test_data *data, int idx)  {
/* glibc doesnt have log10f vectors */
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_SVML)
  float *ip  = (float*)data->ip;
  float *op  = (float*)data->op;
  __m256 ip8 = _mm256_set_ps(ip[idx+7], ip[idx+6], ip[idx+5], ip[idx+4],
                             ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m256 op8 = LIBM_FUNC_VEC(s, 8, log10f)(ip8);
  _mm256_store_ps(&op[0], op8);
#endif
  return 0;
}

int test_v8d(test_data *data, int idx)  {
/* change it to #if defined(__AVX512__) later on*/
#if 0
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
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_SVML)
#if defined(__AVX512__)
  float *ip = (float*)data->ip;
  float *op  = (float*)data->op;
  __m512 ip16 = _mm512_set_ps(ip[idx+15], ip[idx+14], ip[idx+13], ip[idx+12],
                              ip[idx+11], ip[idx+10], ip[idx+9], ip[idx+8],
                              ip[idx+7], ip[idx+6], ip[idx+5], ip[idx+4],
                             ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m512 op16 = LIBM_FUNC_VEC(s, 16, log10f)(ip16);
  _mm512_store_ps(&op[0], op16);
#endif
#endif
  return 0;
}

int test_vad(test_data *data, int count)  {
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op;
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
  amd_vrda_log10(count, ip, op);
#elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
  vdLog10(count, ip, op);
#endif
  return 0;
}

int test_vas(test_data *data, int count)  {
  float *ip  = (float*)data->ip;
  float *op  = (float*)data->op;
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
  amd_vrsa_log10f(count, ip, op);
#elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
  vsLog10(count, ip, op);
#endif
  return 0;
}

#ifdef __cplusplus
}
#endif
