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


#include <math.h>
#include "libm_tests.h"
#include <libm_macros.h>

#define AMD_LIBM_VEC_EXPERIMENTAL

#include <libm_amd.h>
#include <libm/amd_funcs_internal.h>
#include <fmaintrin.h>
#include <immintrin.h>
#include "callback.h"
#include "test_exp10_data.h"
#include "../libs/mparith/alm_mp_funcs.h"

/* The functions exp10() and exp10f() are unsupported in SVML & MSVC ABIs of Windows platform.
 * So the below conditional compilation shall ensure that these 2 APIs will be enabled on
 * either Linux platform (for all ABIs) or Windows Platform (for only AOCL ABI).
 */

#if ( (!defined(_WIN64) || !defined(_WIN32)) || ((defined (_WIN64) || defined (_WIN32)) && LIBM_PROTOTYPE == PROTOTYPE_AOCL) )
  double LIBM_FUNC(exp10)(double);
  float LIBM_FUNC(exp10f)(float);
#endif

static uint32_t ipargs = 1;

uint32_t GetnIpArgs( void )
{
	return ipargs;
}

void ConfSetupf32(SpecParams *specp) {
  specp->data32 = test_exp10f_conformance_data;
  specp->countf = ARRAY_SIZE(test_exp10f_conformance_data);
}

void ConfSetupf64(SpecParams *specp) {
  specp->data64 = test_exp10_conformance_data;
  specp->countd = ARRAY_SIZE(test_exp10_conformance_data);
}

#if ( (!defined(_WIN64) || !defined(_WIN32)) || ((defined (_WIN64) || defined (_WIN32)) && LIBM_PROTOTYPE == PROTOTYPE_AOCL) )
float getFuncOp(float *data) {
  return LIBM_FUNC(exp10f)(data[0]);
}

double getFuncOp(double *data) {
  return LIBM_FUNC(exp10)(data[0]);
}
#endif

double getExpected(float *data) {
  auto val = alm_mp_exp10f(data[0]);
  return val;
}

long double getExpected(double *data) {
  auto val = alm_mp_exp10(data[0]);
  return val;
}

// Used by the Complex Number Functions only!
double _Complex getExpected(float _Complex *data) {
  return {0};
}

long double _Complex getExpected(double _Complex *data) {
  return {0};
}

/*
float getGlibcOp(float *data) {
  return exp10f(data[0]);
}

double getGlibcOp(double *data) {
  return exp10(data[0]);
}
*/

/**********************
*FUNCTIONS*
**********************/

int test_s1s(test_data *data, int idx)  {
#if ( (!defined(_WIN64) || !defined(_WIN32)) || ((defined (_WIN64) || defined (_WIN32)) && LIBM_PROTOTYPE == PROTOTYPE_AOCL) )
    float *ip  = (float*)data->ip;
    float *op  = (float*)data->op;
    op[0] = LIBM_FUNC(exp10f)(ip[idx]);
#endif
  return 0;
}

int test_s1d(test_data *data, int idx)  {
#if ( (!defined(_WIN64) || !defined(_WIN32)) || ((defined (_WIN64) || defined (_WIN32)) && LIBM_PROTOTYPE == PROTOTYPE_AOCL) )
    double *ip  = (double*)data->ip;
    double *op  = (double*)data->op;
    op[0] = LIBM_FUNC(exp10)(ip[idx]);
#endif
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
#define _ZGVdN2v_exp10 _ZGVbN2v_exp10
#define _ZGVdN4v_exp10 _ZGVdN4v_exp10
#define _ZGVsN4v_exp10f _ZGVbN4v_exp10f
#define _ZGVsN8v_exp10f _ZGVdN8v_exp10f

#define _ZGVsN16v_exp10f _ZGVeN16v_exp10f
#define _ZGVdN8v_exp10 _ZGVeN8v_exp10
#endif

/*vector routines*/
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_SVML)
  __m128d LIBM_FUNC_VEC(d, 2, exp10)(__m128d);
  __m256d LIBM_FUNC_VEC(d, 4, exp10)(__m256d);
  __m128 LIBM_FUNC_VEC(s, 4, exp10f)(__m128);
  __m256 LIBM_FUNC_VEC(s, 8, exp10f)(__m256);

  /*avx512*/
  #if defined(__AVX512__)
    __m512d LIBM_FUNC_VEC(d, 8, exp10) (__m512d);
    __m512 LIBM_FUNC_VEC(s, 16, exp10f) (__m512);
  #endif
#endif

int test_v2d(test_data *data, int idx)  {
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_SVML)
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op;
  __m128d ip2 = _mm_set_pd(ip[idx+1], ip[idx]);
  __m128d op2 = LIBM_FUNC_VEC(d, 2, exp10)(ip2);
  _mm_store_pd(&op[0], op2);
#endif
  return 0;
}

int test_v4s(test_data *data, int idx)  {
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_SVML || LIBM_PROTOTYPE == PROTOTYPE_MSVC)
  float *ip  = (float*)data->ip;
  float *op  = (float*)data->op;
  __m128 ip4 = _mm_set_ps(ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  #if (LIBM_PROTOTYPE == PROTOTYPE_MSVC)
    __m128 op4 = LIBM_FUNC_VEC(s, 4, Exp10)(ip4);
  #else
    __m128 op4 = LIBM_FUNC_VEC(s, 4, exp10f)(ip4);
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
  __m256d op4 = LIBM_FUNC_VEC(d, 4, exp10)(ip4);
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
  __m256 op8 = LIBM_FUNC_VEC(s, 8, exp10f)(ip8);
  _mm256_store_ps(&op[0], op8);
#endif
  return 0;
}

int test_v8d(test_data *data, int idx)  {
#if 0
#if defined(__AVX512__)
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op;
  __m512d ip8 = _mm512_set_pd(ip[idx+7], ip[idx+6], ip[idx+5], ip[idx+4],
                             ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m512d op8 = LIBM_FUNC_VEC(d, 8, exp10)(ip8);
  _mm512_store_pd(&op[0], op8);
#endif
#endif
  return 0;
}

int test_v16s(test_data *data, int idx)  {
#if 0
#if defined(__AVX512__)
  float *ip = (float*)data->ip;
  float *op  = (float*)data->op;
  __m512 ip16 = _mm512_set_ps(ip[idx+15], ip[idx+14], ip[idx+13], ip[idx+12],
                              ip[idx+11], ip[idx+10], ip[idx+9], ip[idx+8],
                              ip[idx+7], ip[idx+6], ip[idx+5], ip[idx+4],
                             ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m512 op16 = LIBM_FUNC_VEC(s, 16, exp10f)(ip16);
  _mm512_store_ps(&op[0], op16);
#endif
#endif
  return 0;
}

int test_vad(test_data *data, int count)  {
  double *ip  = (double*)data->ip;
  double *op  = (double*)data->op;
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
  amd_vrda_exp10(count, ip, op);
#elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
  vdExp10(count, ip, op);
#endif
  return 0;
}

int test_vas(test_data *data, int count)  {
  float *ip  = (float*)data->ip;
  float *op  = (float*)data->op;
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
  amd_vrsa_exp10f(count, ip, op);
#elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
  vsExp10(count, ip, op);
#endif
  return 0;
}

#ifdef __cplusplus
}
#endif

