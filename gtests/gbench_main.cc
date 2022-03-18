/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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

