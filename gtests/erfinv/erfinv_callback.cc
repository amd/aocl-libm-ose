/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
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

#include <libm_amd.h>
#include <libm/amd_funcs_internal.h>
#include <fmaintrin.h>
#include <immintrin.h>
#include "callback.h"
#include "test_erfinv_data.h"
#include "../libs/mparith/alm_mp_funcs.h"


#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
double LIBM_FUNC(erfinv)(double);
#endif

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
  specp->data32 = test_erfinvf_conformance_data;
  specp->countf = ARRAY_SIZE(test_erfinvf_conformance_data);
}

void ConfSetupf64(SpecParams *specp) {
  specp->data64 = test_erfinv_conformance_data;
  specp->countd = ARRAY_SIZE(test_erfinv_conformance_data);
}

float getFuncOp(float *data) {
  return 0.0;
}

double getFuncOp(double *data) {
#if (LIBM_PROTOTYPE == PROTOTYPE_SVML)
  double op;
  vdErfInv(1, data, &op);
  return op;
#elif (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
  return LIBM_FUNC(erfinv)(data[0]);
#else
  return 0.0;
#endif
}

double getExpected(float *data) {
  /* Not implemented now */
  /* auto val = alm_mp_erfinvf(data[0]); */
  double val=0;
  return val;
}

long double getExpected(double *data) {
  auto val = alm_mp_erfinv(data[0]);
  return val;
}

// Used by the Complex Number Functions only!
double _Complex getExpected(float _Complex *data) {
  return 0.0;
}

long double _Complex getExpected(double _Complex *data) {
  return 0.0;
}

float getGlibcOp(float *data) {
  return 0.0; /* erfinvf is not available in glibc */
}

double getGlibcOp(double *data) {
  return 0.0; /* erfinv is not available in glibc */
}

/**********************
*FUNCTIONS*
**********************/
int test_s1s(test_data *data, int idx)  {
  return 0;
}

int test_s1d(test_data *data, int idx)  {
  #if ((LIBM_PROTOTYPE == PROTOTYPE_SVML) || (LIBM_PROTOTYPE == PROTOTYPE_AOCL))
    double *ip  = (double*)data->ip;
    double *op  = (double*)data->op;
    #if (LIBM_PROTOTYPE == PROTOTYPE_SVML)
      vdErfInv(1, &ip[idx], op);
    #elif (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
      op[0] = LIBM_FUNC(erfinv)(ip[idx]);
    #endif
  #endif
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

/*vector routines, glibc doesnt have these */
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_SVML)
  __m128d LIBM_FUNC_VEC(d, 2, erfinv)(__m128d);
  __m256d LIBM_FUNC_VEC(d, 4, erfinv)(__m256d);

  #if defined(__AVX512__)
  __m512d LIBM_FUNC_VEC(d, 8, erfinv)(__m512d);
  #endif
#endif

int test_v2d(test_data *data, int idx)  {
  #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_SVML)
    double *ip  = (double*)data->ip;
    double *op  = (double*)data->op;

    #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
      __m128d ip2 = _mm_set_pd(ip[idx+1], ip[idx]);
      __m128d op2 = LIBM_FUNC_VEC(d, 2, erfinv)(ip2);
      _mm_store_pd(&op[0], op2);
    #elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
      vdErfInv(2, ip, op);
    #endif
  #endif
  return 0;
}

int test_v4s(test_data *data, int idx)  {
  return 0;
}

int test_v4d(test_data *data, int idx)  {
  #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_SVML)
    double *ip  = (double*)data->ip;
    double *op  = (double*)data->op;

    #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
      __m256d ip4 = _mm256_set_pd(ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
      __m256d op4 = LIBM_FUNC_VEC(d, 4, erfinv)(ip4);
      _mm256_store_pd(&op[0], op4);
    #elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
      vdErfInv(4, ip, op);
    #endif
  #endif
  return 0;
}

int test_v8s(test_data *data, int idx)  {
  return 0;
}

int test_v8d(test_data *data, int idx)  {
  #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_SVML)
    #if defined(__AVX512__)
    double *ip  = (double*)data->ip;
    double *op  = (double*)data->op;

    #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
      __m512d ip8 = _mm512_set_pd(ip[idx+7], ip[idx+6], ip[idx+5], ip[idx+4],
                                   ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
      __m512d op8 = LIBM_FUNC_VEC(d, 8, erfinv)(ip8);
      _mm512_store_pd(&op[0], op8);
    #elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
      vdErfInv(8, ip, op);
    #endif
    #endif /* __AVX512__ */
  #endif
  return 0;
}

int test_v16s(test_data *data, int idx)  {
  return 0;
}

int test_vas(test_data *data, int count)  {
  return 0;
}

int test_vad(test_data *data, int count)  {
  #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_SVML)
    double *ip  = (double*)data->ip;
    double *op  = (double*)data->op;

    #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
      amd_vrda_erfinv(count, ip, op);
    #elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
      vdErfInv(count, ip, op);
    #endif
  #endif
  return 0;
}

#ifdef __cplusplus
}
#endif

