/*
 * Copyright (C) 2024 Advanced Micro Devices, Inc. All rights reserved.
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

//float getFuncOp(float *data) {
//  return LIBM_FUNC(linearfrac)(data[0], data[1], data[2], data[3], data[4], data[5]);/
//}

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

int test_v2d(test_data *data, int idx)  {
  #if (LIBM_PROTOTYPE != PROTOTYPE_MSVC)
    double *ip1 = (double*)data->ip;
    double *ip2 = (double*)data->ip1;
    double *ip3 = (double*)data->ip2;
    double *ip4 = (double*)data->ip3;
    double *ip5 = (double*)data->ip4;
    double *ip6 = (double*)data->ip5;

    double *op  = (double*)data->op;
    __m128d ip21 = _mm_set_pd(ip1[idx+1], ip1[idx]);
    __m128d ip22 = _mm_set_pd(ip2[idx+1], ip2[idx]);
    #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
      __m128d op2 = LIBM_FUNC_VEC(d, 2, linearfrac)(ip21, ip22, ip3[idx], ip4[idx], ip5[idx], ip6[idx]);
      _mm_store_pd(&op[0], op2);
    #elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
      vdlinearfrac(2, ip1, ip2, ip3[idx], ip4[idx], ip5[idx], ip6[idx], op);
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
    __m256d ip41 = _mm256_set_pd(ip1[idx+3], ip1[idx+2], ip1[idx+1], ip1[idx]);
    __m256d ip42 = _mm256_set_pd(ip2[idx+3], ip2[idx+2], ip2[idx+1], ip2[idx]);
    #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
      __m256d op4 = LIBM_FUNC_VEC(d, 4, linearfrac)(ip41, ip42, ip3[idx], ip4[idx], ip5[idx], ip6[idx]);
      _mm256_store_pd(&op[0], op4);
    #elif
      vdlinearfrac(4, ip1, ip2, ip3[idx], ip4[idx], ip5[idx], ip6[idx], op);
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
        #elif
          vdlinearfrac(8, ip1, ip2, ip3[idx], ip4[idx], ip5[idx], ip6[idx], op);
        #endif
    #endif
  #endif
  return 0;
}

int test_v16s(test_data *data, int idx)  {
  return 0;
}

int test_v4s(test_data *data, int idx)  {
  return 0;
}

int test_v8s(test_data *data, int idx)  {
  return 0;
}

#ifdef __cplusplus
}
#endif

