/*
 * Copyright (C) 2008-2026 Advanced Micro Devices, Inc. All rights reserved.
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

long int LIBM_FUNC(lroundf)(float);
long int LIBM_FUNC(lround)(double);

static uint32_t ipargs = 1;
bool special_case = false;

uint32_t GetnIpArgs(void)
{
    return ipargs;
}

bool getSpecialCase(void)
{
    return special_case;
}

/* lround/lroundf return long int, not float/double, so the floating-point
 * conformance test infrastructure does not apply. Supply empty tables. */
void ConfSetupf32(SpecParams *specp) {
    specp->data32 = nullptr;
    specp->countf = 0;
}

void ConfSetupf64(SpecParams *specp) {
    specp->data64 = nullptr;
    specp->countd = 0;
}

float getFuncOp(float *data) {
    return (float)LIBM_FUNC(lroundf)(data[0]);
}

double getFuncOp(double *data) {
    return (double)LIBM_FUNC(lround)(data[0]);
}

void getExpected(float *data, mpfr_t result) {
    mpfr_set_d(result, (double)lroundf(data[0]), MPFR_RNDN);
}

void getExpected(double *data, mpfr_t result) {
    mpfr_set_d(result, (double)lround(data[0]), MPFR_RNDN);
}

double _Complex getExpected(float _Complex *data) {
    return 0.0;
}

long double _Complex getExpected(double _Complex *data) {
    return 0.0;
}

float getGlibcOp(float *data) {
    return (float)lroundf(data[0]);
}

double getGlibcOp(double *data) {
    return (double)lround(data[0]);
}

int test_s1s(test_data *data, int idx) {
    float *ip = (float *)data->ip;
    float *op = (float *)data->op;
    op[0] = (float)LIBM_FUNC(lroundf)(ip[idx]);
    return 0;
}

int test_s1d(test_data *data, int idx) {
    double *ip = (double *)data->ip;
    double *op = (double *)data->op;
    op[0] = (double)LIBM_FUNC(lround)(ip[idx]);
    return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

int test_v2d(test_data *data, int idx)  { return 0; }
int test_v4s(test_data *data, int idx)  { return 0; }
int test_v4d(test_data *data, int idx)  { return 0; }
int test_v8s(test_data *data, int idx)  { return 0; }
int test_v8d(test_data *data, int idx)  { return 0; }
int test_v16s(test_data *data, int idx) { return 0; }

#ifdef __cplusplus
}
#endif
