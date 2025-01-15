/*
 * Copyright (C) 2025 Advanced Micro Devices, Inc. All rights reserved.
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
#include "test_linearfrac_data.h"
#include "../libs/mparith/alm_mp_funcs.h"

static uint32_t ipargs = 6;
bool specialcase = true;

uint32_t GetnIpArgs( void )
{
  return ipargs;
}

bool getSpecialCase(void)
{
  return specialcase;
}

void ConfSetupf64(SpecParams *specp) {
  specp->data64 = test_linearfrac_conformance_data;
  specp->countd = ARRAY_SIZE(test_linearfrac_conformance_data);
}

void ConfSetupf32(SpecParams *specp) {
  specp->data32 = test_linearfracf_conformance_data;
  specp->countf = ARRAY_SIZE(test_linearfracf_conformance_data);
}


long double getExpected(double *data) {
  auto val = alm_mp_linearfrac(data[0], data[1], data[2], data[3], data[4], data[5]);
  return val;
}

double getExpected(float *data) {
  auto val = alm_mp_linearfracf(data[0], data[1], data[2], data[3], data[4], data[5]);
  return val;
}

double _Complex getExpected(float _Complex *data) {
  return {0};
}

long double _Complex getExpected(double _Complex *data) {
  return {0};
}

int test_s1s(test_data *data, int idx)  {
  return 0;
}

int test_s1d(test_data *data, int idx)  {
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

#if (LIBM_PROTOTYPE != PROTOTYPE_MSVC)
  __m128d LIBM_FUNC_VEC(d, 2, linearfrac)(__m128d, __m128d, double, double, double, double);
  __m256d LIBM_FUNC_VEC(d, 4, linearfrac)(__m256d, __m256d, double, double, double, double);
  __m128 LIBM_FUNC_VEC(s, 4, linearfracf)(__m128, __m128, float, float, float, float);
  __m256 LIBM_FUNC_VEC(s, 8, linearfracf)(__m256, __m256, float, float, float, float);

  /*avx512*/
  #if defined(__AVX512__)
    __m512d LIBM_FUNC_VEC(d, 8, linearfrac) (__m512d, __m512d, double, double, double, double);
    __m512 LIBM_FUNC_VEC(s, 16, linearfracf) (__m512, __m512, float, float, float, float);
  #endif
#endif

int test_v2d(test_data *data, int idx)  {
  #if (LIBM_PROTOTYPE != PROTOTYPE_MSVC)
    double *ip1 = (double*)data->ip;
    double *ip2 = (double*)data->ip1;
    double *ip3 = (double*)data->ip2;
    double *ip4 = (double*)data->ip3;
    double *ip5 = (double*)data->ip4;
    double *ip6 = (double*)data->ip5;

    double *op  = (double*)data->op;
    #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
      __m128d ip21 = _mm_set_pd(ip1[idx+1], ip1[idx]);
      __m128d ip22 = _mm_set_pd(ip2[idx+1], ip2[idx]);
      __m128d op2 = LIBM_FUNC_VEC(d, 2, linearfrac)(ip21, ip22, ip3[idx], ip4[idx], ip5[idx], ip6[idx]);
      _mm_store_pd(&op[0], op2);
    #elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
      vdLinearFrac(2, ip1, ip2, ip3[idx], ip4[idx], ip5[idx], ip6[idx], op);
    #endif
  #endif
  return 0;
}

int test_v4d(test_data *data, int idx)  {
  #if (LIBM_PROTOTYPE != PROTOTYPE_MSVC)
    double *ip1 = (double*)data->ip;
    double *ip2 = (double*)data->ip1;
    double *ip3 = (double*)data->ip2;
    double *ip4 = (double*)data->ip3;
    double *ip5 = (double*)data->ip4;
    double *ip6 = (double*)data->ip5;

    double *op  = (double*)data->op;
    #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
    __m256d ip41 = _mm256_set_pd(ip1[idx+3], ip1[idx+2], ip1[idx+1], ip1[idx]);
    __m256d ip42 = _mm256_set_pd(ip2[idx+3], ip2[idx+2], ip2[idx+1], ip2[idx]);
      __m256d op4 = LIBM_FUNC_VEC(d, 4, linearfrac)(ip41, ip42, ip3[idx], ip4[idx], ip5[idx], ip6[idx]);
      _mm256_store_pd(&op[0], op4);
    #elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
      vdLinearFrac(4, ip1, ip2, ip3[idx], ip4[idx], ip5[idx], ip6[idx], op);
    #endif
  #endif
  return 0;
}

int test_v8d(test_data *data, int idx)  {
  #if (LIBM_PROTOTYPE != PROTOTYPE_MSVC)
    #if defined(__AVX512__)
        double *ip1 = (double*)data->ip;
        double *ip2 = (double*)data->ip1;
        double *ip3 = (double*)data->ip2;
        double *ip4 = (double*)data->ip3;
        double *ip5 = (double*)data->ip4;
        double *ip6 = (double*)data->ip5;

        double *op  = (double*)data->op;
        __m512d ip8_1 = _mm512_set_pd(ip1[idx+7], ip1[idx+6], ip1[idx+5], ip1[idx+4],
                                ip1[idx+3], ip1[idx+2], ip1[idx+1], ip1[idx]);
        __m512d ip8_2 = _mm512_set_pd(ip2[idx+7], ip2[idx+6], ip2[idx+5], ip2[idx+4],
                                ip2[idx+3], ip2[idx+2], ip2[idx+1], ip2[idx]);
        #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
          __m512d op8 = LIBM_FUNC_VEC(d, 8, linearfrac)(ip8_1, ip8_2, ip3[idx], ip4[idx], ip5[idx], ip6[idx]);
          _mm512_store_pd(&op[0], op8);
        #elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
          vdLinearFrac(8, ip1, ip2, ip3[idx], ip4[idx], ip5[idx], ip6[idx], op);
        #endif
    #endif
  #endif
  return 0;
}

int test_v16s(test_data *data, int idx)  {
   #if (LIBM_PROTOTYPE != PROTOTYPE_MSVC)
    #if defined(__AVX512__)
        float *ip1 = (float*)data->ip;
        float *ip2 = (float*)data->ip1;
        float *ip3 = (float*)data->ip2;
        float *ip4 = (float*)data->ip3;
        float *ip5 = (float*)data->ip4;
        float *ip6 = (float*)data->ip5;

        float *op  = (float*)data->op;
        __m512 ip16_1 = _mm512_set_ps(ip1[idx+15], ip1[idx+14], ip1[idx+13], ip1[idx+12],
                                      ip1[idx+11], ip1[idx+10], ip1[idx+9], ip1[idx+8],
                                      ip1[idx+7], ip1[idx+6], ip1[idx+5], ip1[idx+4],
                                      ip1[idx+3], ip1[idx+2], ip1[idx+1], ip1[idx]);
        __m512 ip16_2 = _mm512_set_ps(ip2[idx+15], ip2[idx+14], ip2[idx+13], ip2[idx+12],
                                      ip2[idx+11], ip2[idx+10], ip2[idx+9], ip2[idx+8],
                                      ip2[idx+7], ip2[idx+6], ip2[idx+5], ip2[idx+4],
                                      ip2[idx+3], ip2[idx+2], ip2[idx+1], ip2[idx]);
        #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
          __m512 op16 = LIBM_FUNC_VEC(s, 16, linearfracf)(ip16_1, ip16_2, ip3[idx], ip4[idx], ip5[idx], ip6[idx]);
          _mm512_store_ps(&op[0], op16);
        #elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
          vsLinearFrac(16, ip1, ip2, ip3[idx], ip4[idx], ip5[idx], ip6[idx], op);
        #endif
    #endif
  #endif
  return 0;
}

int test_v4s(test_data *data, int idx)  {
  #if (LIBM_PROTOTYPE != PROTOTYPE_MSVC)
    float *ip1 = (float*)data->ip;
    float *ip2 = (float*)data->ip1;
    float *ip3 = (float*)data->ip2;
    float *ip4 = (float*)data->ip3;
    float *ip5 = (float*)data->ip4;
    float *ip6 = (float*)data->ip5;

    float *op  = (float*)data->op;
    #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
    __m128 ip41 = _mm_set_ps(ip1[idx+3], ip1[idx+2], ip1[idx+1], ip1[idx]);
    __m128 ip42 = _mm_set_ps(ip2[idx+3], ip2[idx+2], ip2[idx+1], ip2[idx]);
      __m128 op4 = LIBM_FUNC_VEC(s, 4, linearfracf)(ip41, ip42, ip3[idx], ip4[idx], ip5[idx], ip6[idx]);
      _mm_store_ps(&op[0], op4);
    #elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
      vsLinearFrac(4, ip1, ip2, ip3[idx], ip4[idx], ip5[idx], ip6[idx], op);
    #endif
  #endif
  return 0;
}

int test_v8s(test_data *data, int idx)  {
   #if (LIBM_PROTOTYPE != PROTOTYPE_MSVC)
    float *ip1 = (float*)data->ip;
    float *ip2 = (float*)data->ip1;
    float *ip3 = (float*)data->ip2;
    float *ip4 = (float*)data->ip3;
    float *ip5 = (float*)data->ip4;
    float *ip6 = (float*)data->ip5;

    float *op  = (float*)data->op;
    #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
      __m256 ip81 = _mm256_set_ps(ip1[idx+7], ip1[idx+6], ip1[idx+5], ip1[idx+4],
                                ip1[idx+3], ip1[idx+2], ip1[idx+1], ip1[idx]);
    __m256 ip82 = _mm256_set_ps(ip2[idx+7], ip2[idx+6], ip2[idx+5], ip2[idx+4],
                                ip2[idx+3], ip2[idx+2], ip2[idx+1], ip2[idx]);
      __m256 op8 = LIBM_FUNC_VEC(s, 8, linearfracf)(ip81, ip82, ip3[idx], ip4[idx], ip5[idx], ip6[idx]);
      _mm256_store_ps(&op[0], op8);
    #elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
      vsLinearFrac(8, ip1, ip2, ip3[idx], ip4[idx], ip5[idx], ip6[idx], op);
    #endif
  #endif
  return 0;
}

int test_vad(test_data *data, int count)  {
  double *ip1  = (double*)data->ip;
  double *ip2  = (double*)data->ip1;
  double *ip3  = (double*)data->ip2;
  double *ip4  = (double*)data->ip3;
  double *ip5  = (double*)data->ip4;
  double *ip6  = (double*)data->ip5;
  double *op  = (double*)data->op;

  #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
      amd_vrda_linearfrac(count, ip1, ip2, ip3[0], ip4[0], ip5[0], ip6[0], op);
  #elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
    vdLinearFrac(count, ip1, ip2, ip3[0], ip4[0], ip5[0], ip6[0], op);
  #endif
  return 0;
}

int test_vas(test_data *data, int count)  {
  float *ip1  = (float*)data->ip;
  float *ip2  = (float*)data->ip1;
  float *ip3  = (float*)data->ip2;
  float *ip4  = (float*)data->ip3;
  float *ip5  = (float*)data->ip4;
  float *ip6  = (float*)data->ip5;
  float *op  = (float*)data->op;

  #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
      amd_vrsa_linearfracf(count, ip1, ip2, ip3[0], ip4[0], ip5[0], ip6[0], op);
  #elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
    vsLinearFrac(count, ip1, ip2, ip3[0], ip4[0], ip5[0], ip6[0], op);
  #endif
  return 0;
}


#ifdef __cplusplus
}
#endif

