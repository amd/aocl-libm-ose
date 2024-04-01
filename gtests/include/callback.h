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


#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#include "benchmark.h"
#include "almtestperf.h"
#include <external/amdlibm.h>
#include <complex.h>

typedef struct {
  void *ip;
  void *ip1;
  void *op;
}test_data;

/* Used _mm256_storeu_pd for windows as the test cases for some vectors was failing with SEH
exception with a code 0xc0000005 inconsistently, since mem_addr was not aligned on a
32-byte boundary, hence a general-protection exception was thrown in test body */
#if ((defined (WIN64) || defined (_WIN32)) && defined(__clang__))
#define _MM256_STORE_PS _mm256_storeu_ps
#define _MM256_STORE_PD _mm256_storeu_pd
#else
#define _MM256_STORE_PS _mm256_store_ps
#define _MM256_STORE_PD _mm256_store_pd
#endif

uint32_t GetnIpArgs( void );

float getFuncOp(float *);
double getFuncOp(double *);


double _Complex getExpected(float _Complex *);
long double _Complex getExpected(double _Complex *);

double getExpected(float *);
long double getExpected(double *);

float getGlibcOp(float *);
double getGlibcOp(double *);

int test_s1s(test_data *data, int idx);
int test_s1d(test_data *data, int idx);

void LibmPerfTestf(benchmark::State& st, InputParams* param);
void LibmPerfTest4f(benchmark::State& st, InputParams* param);
void LibmPerfTest8f(benchmark::State& st, InputParams* param);

void LibmPerfTestd(benchmark::State& st, InputParams* param);
void LibmPerfTest2d(benchmark::State& st, InputParams* param);
void LibmPerfTest4d(benchmark::State& st, InputParams* param);

void LibmPerfTest8d(benchmark::State& st, InputParams* param);
void LibmPerfTest16f(benchmark::State& st, InputParams* param);

#ifdef __cplusplus
extern "C" {
#endif
int test_v4s(test_data *data, int idx);
int test_v8s(test_data *data, int idx);
int test_v16s(test_data *data, int idx);

int test_v2d(test_data *data, int idx);
int test_v4d(test_data *data, int idx);
int test_v8d(test_data *data, int idx);

int test_vad(test_data *data, int idx);
int test_vas(test_data *data, int idx);
#ifdef __cplusplus
}
#endif

#endif
