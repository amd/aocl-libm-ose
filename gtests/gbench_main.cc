/*
 * Copyright (C) 2019-2020 Advanced Micro Devices, Inc. All rights reserved
 */

#include <cstdio>
#include <iostream>
#include <string>
#include <cstring>
#include "almstruct.h"
#include "benchmark.h"
#include "defs.h"
#include "almtestperf.h"
#include "cmdline.h"
#include "callback.h"

using namespace std;

void LibmPerfTestf(benchmark::State& st, InputParams* param) {
  uint32_t nargs = GetnIpArgs();
  AoclLibmTest<float> objtest(param, nargs);
  test_data data;
  data.ip  = (void *) objtest.inpbuff;
  data.op  = (void *) objtest.outbuff;
  double szn = param->niter * param->count;

  if(nargs == 2)
    data.ip1 = (void *) objtest.inpbuff1;

  for (auto _ : st) {
    for (int64_t i =  0 ; i < param->count; i++) {
      test_s1s(&data, i);
    }
  }

  namespace bm = benchmark;
  st.counters["MOPS"] = bm::Counter{szn, bm::Counter::kIsRate};
}

void LibmPerfTest4f(benchmark::State& st, InputParams* param) {
  uint32_t nargs = GetnIpArgs();
  AoclLibmTest<float> objtest(param, nargs);
  test_data data;
  data.ip  = (void *) objtest.inpbuff;
  data.op  = (void *) objtest.outbuff;
  double szn = param->niter * param->count;

  if(nargs == 2)
    data.ip1 = (void *) objtest.inpbuff1;

  for (auto _ : st) {
    for (int64_t i =  0 ; i < param->count; i += 4) {
      test_v4s(&data, i);
    }
  }

  namespace bm = benchmark;
  st.counters["MOPS"] = bm::Counter{szn, bm::Counter::kIsRate};
}

void LibmPerfTest8f(benchmark::State& st, InputParams* param) {
  uint32_t nargs = GetnIpArgs();
  AoclLibmTest<float> objtest(param, nargs);
  test_data data;
  data.ip  = (void *) objtest.inpbuff;
  data.op  = (void *) objtest.outbuff;
  double szn = param->niter * param->count;

  if(nargs == 2)
    data.ip1 = (void *) objtest.inpbuff1;

  for (auto _ : st) {
    for (int64_t i =  0 ; i < param->count; i += 8) {
      test_v8s(&data, i);
    }
  }

  namespace bm = benchmark;
  st.counters["MOPS"] = bm::Counter{szn, bm::Counter::kIsRate};
}

void LibmPerfTest16f(benchmark::State& st, InputParams* param) {
  uint32_t nargs = GetnIpArgs();
  AoclLibmTest<float> objtest(param, nargs);
  test_data data;
  data.ip  = (void *) objtest.inpbuff;
  data.op  = (void *) objtest.outbuff;
  double szn = param->niter * param->count;

  if(nargs == 2)
    data.ip1 = (void *) objtest.inpbuff1;

  for (auto _ : st) {
    for (int64_t i =  0 ; i < param->count; i += 16) {
      test_v16s(&data, i);
    }
  }

  namespace bm = benchmark;
  st.counters["MOPS"] = bm::Counter{szn, bm::Counter::kIsRate};
}

void LibmPerfTestd(benchmark::State& st, InputParams* param) {
  uint32_t nargs = GetnIpArgs();
  AoclLibmTest<double> objtest(param, nargs);
  test_data data;
  data.ip  = (void *) objtest.inpbuff;
  data.op  = (void *) objtest.outbuff;
  double szn = param->niter * param->count;

  if(nargs == 2)
    data.ip1 = (void *) objtest.inpbuff1;

  for (auto _ : st) {
    for (int64_t i =  0 ; i < param->count; i++) {
      test_s1d(&data, i);
    }
  }

  namespace bm = benchmark;
  st.counters["MOPS"] = bm::Counter{szn, bm::Counter::kIsRate};
}

void LibmPerfTest2d(benchmark::State& st, InputParams* param) {
  uint32_t nargs = GetnIpArgs();
  AoclLibmTest<double> objtest(param, nargs);
  test_data data;
  data.ip  = (void *) objtest.inpbuff;
  data.op  = (void *) objtest.outbuff;
  double szn = param->niter * param->count;

  if(nargs == 2)
    data.ip1 = (void *) objtest.inpbuff1;

  for (auto _ : st) {
    for (int64_t i =  0 ; i < param->count; i += 2) {
      test_v2d(&data, i);
    }
  }

  namespace bm = benchmark;
  st.counters["MOPS"] = bm::Counter{szn, bm::Counter::kIsRate};
}

void LibmPerfTest4d(benchmark::State& st, InputParams* param) {
  uint32_t nargs = GetnIpArgs();
  AoclLibmTest<double> objtest(param, nargs);
  test_data data;
  data.ip  = (void *) objtest.inpbuff;
  data.op  = (void *) objtest.outbuff;
  double szn = param->niter * param->count;

  if(nargs == 2)
    data.ip1 = (void *) objtest.inpbuff1;

  for (auto _ : st) {
    for (int64_t i =  0 ; i < param->count; i += 4) {
      test_v4d(&data, i);
    }
  }

  namespace bm = benchmark;
  st.counters["MOPS"] = bm::Counter{szn, bm::Counter::kIsRate};
}

void LibmPerfTest8d(benchmark::State& st, InputParams* param) {
  uint32_t nargs = GetnIpArgs();
  AoclLibmTest<double> objtest(param, nargs);
  test_data data;
  data.ip  = (void *) objtest.inpbuff;
  data.op  = (void *) objtest.outbuff;
  double szn = param->niter * param->count;

  if(nargs == 2)
    data.ip1 = (void *) objtest.inpbuff1;

  for (auto _ : st) {
    for (int64_t i =  0 ; i < param->count; i += 8) {
      test_v8d(&data, i);
    }
  }

  namespace bm = benchmark;
  st.counters["MOPS"] = bm::Counter{szn, bm::Counter::kIsRate};
}

int gbench_main(int argc, char **argv, InputParams *params) {
  AlmTestPerfFramework AlmTestPerf;

  if (1) {
    int argcs = 2;
    std::string arg0 = "--benchmark_counters_tabular=true";
    char *args[2] = {
      const_cast<char *>(argv[0]), const_cast<char *>(arg0.c_str())};
    benchmark::Initialize(&argcs, args);
  } else  {
    benchmark::Initialize(&argc, argv);
  }

  int retval = AlmTestPerf.AlmTestPerformance(params);

  return retval;
}

