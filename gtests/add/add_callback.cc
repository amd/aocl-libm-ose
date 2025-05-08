/*
 * Copyright (C) 2024-2025 Advanced Micro Devices, Inc. All rights reserved.
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
#include "test_add_data.h"
#include "../libs/mparith/alm_mp_funcs.h"

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

double getExpected(float *data) {
  auto val = alm_mp_addf(data[0], data[1]);
  return val;
}

long double getExpected(double *data) {
  auto val = alm_mp_add(data[0], data[1]);
  return val;
}

// FIXME: To be used by Complex Number Functions only!
double _Complex getExpected(float _Complex *data) {
  return {0};
}

// FIXME: To be used by Complex Number Functions only!
long double _Complex getExpected(double _Complex *data) {
  return {0};
}

void ConfSetupf32(SpecParams *specp) {
  specp->data32 = test_addf_conformance_data;
  specp->countf = ARRAY_SIZE(test_addf_conformance_data);
}

void ConfSetupf64(SpecParams *specp) {
  specp->data64 = test_add_conformance_data;
  specp->countd = ARRAY_SIZE(test_add_conformance_data);
}

// FIXME: No scalar SP variant available for add
int test_s1s(test_data *data, int idx)  {
  return 0;
}

// FIXME: No scalar DP variant available for add
int test_s1d(test_data *data, int idx)  {
  return 0;
}

// FIXME: No V2D variant available for add
int test_v2d(test_data *data, int idx)  {
  return 0;
}

// FIXME: No V4S variant available for add
int test_v4s(test_data *data, int idx)  {
  return 0;
}

// FIXME: No V4D variant available for add
int test_v4d(test_data *data, int idx)  {
  return 0;
}

// FIXME: No V8S variant available for add
int test_v8s(test_data *data, int idx)  {
  return 0;
}

// FIXME: No V8D variant available for add
int test_v8d(test_data *data, int idx)  {
  return 0;
}

// FIXME: No V16S variant available for add
int test_v16s(test_data *data, int idx)  {
  return 0;
}

int test_vad(test_data *data, int count)  {
#if (LIBM_PROTOTYPE != PROTOTYPE_GLIBC)
  double *ip1 = (double*)data->ip;
  double *ip2 = (double*)data->ip1;
  double *op  = (double*)data->op;
  #if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
    amd_vrda_add(count, ip1, ip2, op);
  #elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
    vdAdd(count, ip1, ip2, op);
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
    amd_vrsa_addf(count, ip1, ip2, op);
  #elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
    vsAdd(count, ip1, ip2, op);
  #endif
#endif
  return 0;
}
