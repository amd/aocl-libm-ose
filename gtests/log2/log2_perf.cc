/*
 * Copyright (C) 2019-2020 Advanced Micro Devices, Inc. All rights reserved
 */

#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string>
#include <cstring>
#include <vector>
#include <chrono>
#include "benchmark.h"
#include "almtestperf.h"
#include "callback.h"

using namespace std;
using namespace ALM;

int AlmTestPerfFramework::AlmTestPerformance(InputParams *params) {
  string funcnam = "AoclLibm";
  string libm;

  if((params->fwidth == ALM::FloatWidth::E_ALL) ||
    (params->fwidth == ALM::FloatWidth::E_F32)) {
    if((params->fqty == ALM::FloatQuantity::E_All) ||
     (params->fqty == ALM::FloatQuantity::E_Scalar)) {
      string varnam = "_s1s(log2f)";
      libm = funcnam + varnam;
      benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTestf, params)
                 ->Args({(int)params->count})->Iterations(params->niter);
    }
    if((params->fqty == ALM::FloatQuantity::E_All) ||
     (params->fqty == ALM::FloatQuantity::E_Vector_4)) {
      string varnam = "_v4s(log2f)";
      libm = funcnam + varnam;
      benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTest4f, params)
                 ->Args({(int)params->count})->Iterations(params->niter);
    }
    if((params->fqty == ALM::FloatQuantity::E_All) ||
     (params->fqty == ALM::FloatQuantity::E_Vector_8)) {
      string varnam = "_v8s(log2f)";
      libm = funcnam + varnam;
      benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTest8f, params)
                 ->Args({(int)params->count})->Iterations(params->niter);
    }
  }

  if((params->fwidth == ALM::FloatWidth::E_ALL) ||
    (params->fwidth == ALM::FloatWidth::E_F64)) {
    if((params->fqty == ALM::FloatQuantity::E_All) ||
     (params->fqty == ALM::FloatQuantity::E_Scalar)) {
      string varnam = "_s1d(log2)";
      libm = funcnam + varnam;
      benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTestd, params)
                 ->Args({(int)params->count})->Iterations(params->niter);
    }
    if((params->fqty == ALM::FloatQuantity::E_All) ||
     (params->fqty == ALM::FloatQuantity::E_Vector_2)) {
      string varnam = "_v2d(log2)";
      libm = funcnam + varnam;
      benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTest2d, params)
                 ->Args({(int)params->count})->Iterations(params->niter);
    }
    if((params->fqty == ALM::FloatQuantity::E_All) ||
     (params->fqty == ALM::FloatQuantity::E_Vector_4)) {
      string varnam = "_v4d(log2)";
      libm = funcnam + varnam;
      benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTest4d, params)
                 ->Args({(int)params->count})->Iterations(params->niter);
    }
    #if defined(__AVX512__)
    if((params->fqty == ALM::FloatQuantity::E_All) ||
     (params->fqty == ALM::FloatQuantity::E_Vector_8)) {
      string varnam = "_v8d(log2)";
      libm = funcnam + varnam;
      benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTest8d, params)
                 ->Args({(int)params->count})->Iterations(params->niter);
    }
    #endif
  }

  size_t retval = benchmark::RunSpecifiedBenchmarks();

  return (int)retval;
}

AlmTestPerfFramework::~AlmTestPerfFramework() {
#if defined(DEBUG_PRINTS)
  cout << "AlmTestPerfFramework destructor completed" << endl;
#endif
}
