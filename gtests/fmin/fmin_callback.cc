/*
 * Copyright (C) 2008-2025 Advanced Micro Devices, Inc. All rights reserved.
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
#include "test_fmin_data.h"
#include "../libs/mparith/alm_mp_funcs.h"

double LIBM_FUNC(fmin)(double);
float LIBM_FUNC(fminf)(float);

static uint32_t ipargs = 2;
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
  specp->data32 = test_fminf_conformance_data;
  specp->countf = ARRAY_SIZE(test_fminf_conformance_data);
}

void ConfSetupf64(SpecParams *specp) {
  specp->data64 = test_fmin_conformance_data;
  specp->countd = ARRAY_SIZE(test_fmin_conformance_data);
}


float getFuncOp(float *data) {
  return LIBM_FUNC(fminf)(data[0], data[1]);
}

double getFuncOp(double *data) {
  return LIBM_FUNC(fmin)(data[0], data[1]);
}


double getExpected(float *data) {
    auto val = alm_mp_fminf(data[0], data[1]);
    return val;
}

long double getExpected(double *data) {
  auto val = alm_mp_fmin(data[0], data[1]);
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
  return fminf(data[0], data[1]);
}

double getGlibcOp(double *data) {
  return fmin(data[0], data[1]);
}

/**********************
*FUNCTIONS*
**********************/
int test_s1s(test_data *data, int idx)  {
  float *ip1  = (float*)data->ip;
  float *ip2 = (float*)data->ip1;
  float *op  = (float*)data->op;
  op[0] = LIBM_FUNC(fminf)(ip1[idx], ip2[idx]);
  return 0;
}

int test_s1d(test_data *data, int idx)  {
  double *ip1  = (double*)data->ip;
  double *ip2 = (double*)data->ip1;
  double *op  = (double*)data->op;
  op[0] = LIBM_FUNC(fmin)(ip1[idx], ip2[idx]);
  return 0;
}


#ifdef __cplusplus
extern "C" {
#endif

/*vector routines*/
#if 0
__m128d LIBM_FUNC_VEC(d, 2, fmin)(__m128d, __m128d);
__m256d LIBM_FUNC_VEC(d, 4, rfmin)(__m256d, __m256d);
__m128 LIBM_FUNC_VEC(s, 4, fminf)(__m128, __m128);
__m256 LIBM_FUNC_VEC(s, 8, fminf)(__m256, __m256);
/*avx512*/
#if defined(__AVX512__)
__m512d LIBM_FUNC_VEC(d, 8, fmin) (__m512d, __m512d);
__m512 LIBM_FUNC_VEC(s, 16, fminf)(__m512, __m512);
#endif
#endif

int test_v2d(test_data *data, int idx)  {
#if 0
  double *ip1  = (double*)data->ip;
  double *ip2 = (double*)data->ip1;
  double *op  = (double*)data->op;
  __m128d ip21 = _mm_set_pd(ip1[idx+1], ip1[idx]);
  __m128d ip22 = _mm_set_pd(ip2[idx+1], ip2[idx]);
  __m128d op2 = LIBM_FUNC_VEC(d, 2, fmin)(ip21, ip22);
  _mm_store_pd(&op[0], op2);
#endif
  return 0;
}

int test_v4s(test_data *data, int idx)  {
#if 0
  float *ip1  = (float*)data->ip;
  float *ip2 = (float*)data->ip1;
  float *op  = (float*)data->op;
  __m128 ip41 = _mm_set_ps(ip1[idx+3], ip1[idx+2], ip1[idx+1], ip1[idx]);
  __m128 ip42 = _mm_set_ps(ip2[idx+3], ip2[idx+2], ip2[idx+1], ip2[idx]);
  __m128 op4 = LIBM_FUNC_VEC(s, 4, fminf)(ip41, ip42);
  _mm_store_ps(&op[0], op4);
#endif
  return 0;
}

int test_v4d(test_data *data, int idx)  {
#if 0
  double *ip1  = (double*)data->ip;
  double *ip2 = (double*)data->ip1;
  double *op  = (double*)data->op;
  __m256d ip41 = _mm256_set_pd(ip1[idx+3], ip1[idx+2], ip1[idx+1], ip1[idx]);
  __m256d ip42 = _mm256_set_pd(ip2[idx+3], ip2[idx+2], ip2[idx+1], ip2[idx]);
  __m256d op4 = LIBM_FUNC_VEC(d, 4, fmin)(ip41, ip42);
  _mm256_store_pd(&op[0], op4);
#endif
  return 0;
}

int test_v8s(test_data *data, int idx)  {
#if 0
  float *ip1  = (float*)data->ip;
  float *ip2 = (float*)data->ip1;
  float *op  = (float*)data->op;
  __m256 ip81 = _mm256_set_ps(ip1[idx+7], ip1[idx+6], ip1[idx+5], ip1[idx+4],
                              ip1[idx+3], ip1[idx+2], ip1[idx+1], ip1[idx]);
  __m256 ip82 = _mm256_set_ps(ip2[idx+7], ip2[idx+6], ip2[idx+5], ip2[idx+4],
                              ip2[idx+3], ip2[idx+2], ip2[idx+1], ip2[idx]);
  __m256 op8 = LIBM_FUNC_VEC(s, 8, fminf)(ip81, ip82);
  _mm256_store_ps(&op[0], op8);
#endif
  return 0;
}

int test_v8d(test_data *data, int idx)  {
#if 0
#if defined(__AVX512__)
  double *ip1 = (double*)data->ip;
  double *ip2 = (double*)data->ip1;
  double *op  = (double*)data->op;

  __m512d ip8_1 = _mm512_set_pd(ip1[idx+7], ip1[idx+6], ip1[idx+5], ip1[idx+4],
                             ip1[idx+3], ip1[idx+2], ip1[idx+1], ip1[idx]);
  __m512d ip8_2 = _mm512_set_pd(ip2[idx+7], ip2[idx+6], ip2[idx+5], ip2[idx+4],
                             ip2[idx+3], ip2[idx+2], ip2[idx+1], ip2[idx]);

  __m512d op8 = LIBM_FUNC_VEC(d, 8, fmin)(ip8_1, ip8_2);

  _mm512_store_pd(&op[0], op8);
#endif
#endif
  return 0;
}

int test_v16s(test_data *data, int idx)  {
#if 0
#if defined(__AVX512__)
  float *ip1 = (float*)data->ip;
  float *ip2 = (float*)data->ip1;
  float *op  = (float*)data->op;
  __m512 ip16_1 = _mm512_set_ps(ip1[idx+15], ip1[idx+14], ip1[idx+13], ip1[idx+12],
                              ip1[idx+11], ip1[idx+10], ip1[idx+9], ip1[idx+8],
                              ip1[idx+7], ip1[idx+6], ip1[idx+5], ip1[idx+4],
                             ip1[idx+3], ip1[idx+2], ip1[idx+1], ip1[idx]);
  __m512 ip16_2 = _mm512_set_ps(ip2[idx+15], ip2[idx+14], ip2[idx+13], ip2[idx+12],
                              ip2[idx+11], ip2[idx+10], ip2[idx+9], ip2[idx+8],
                              ip2[idx+7], ip2[idx+6], ip2[idx+5], ip2[idx+4],
                             ip2[idx+3], ip2[idx+2], ip2[idx+1], ip2[idx]);
  __m512 op16 = LIBM_FUNC_VEC(s, 16, fminf)(ip16_1, ip16_2);
  _mm512_store_ps(&op[0], op16);
#endif
#endif
  return 0;
}

int test_vad(test_data *data, int count)  {
#if (LIBM_PROTOTYPE != PROTOTYPE_GLIBC)
  double *ip1 = (double*)data->ip;
  double *ip2 = (double*)data->ip1;
  double *op  = (double*)data->op;
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
  amd_vrda_fmin(count, ip1, ip2, op);
#elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
  vdFmin(count, ip1, ip2, op);
#endif
#endif
  return 0;
}

int test_vas(test_data *data, int count)  {
#if (LIBM_PROTOTYPE != PROTOTYPE_GLIBC)
  float *ip1 = (float*)data->ip;
  float *ip2 = (float*)data->ip1;
  float *op  = (float*)data->op;
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
  amd_vrsa_fminf(count, ip1, ip2, op);
#elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
  vsFmin(count, ip1, ip2, op);
#endif
#endif
  return 0;
}

#ifdef __cplusplus
}
#endif
