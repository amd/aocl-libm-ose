#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#include "benchmark.h"
#include "almtestperf.h"

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

#ifdef __cplusplus
extern "C" {
#endif
int test_v4s(test_data *data, int idx);
int test_v8s(test_data *data, int idx);

int test_v2d(test_data *data, int idx);
int test_v4d(test_data *data, int idx);
#ifdef __cplusplus
}
#endif

#endif
