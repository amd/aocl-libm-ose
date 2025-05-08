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
    (params->fwidth == ALM::FloatWidth::E_F64)) {

    if((params->fqty == ALM::FloatQuantity::E_All) ||
     (params->fqty == ALM::FloatQuantity::E_Vector_2)) {
      string varnam = "_v2d(powx)";
      libm = funcnam + varnam;
      benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTest2d, params)
                 ->Args({(int)params->count})->Iterations(params->niter);
    }
    if((params->fqty == ALM::FloatQuantity::E_All) ||
     (params->fqty == ALM::FloatQuantity::E_Vector_4)) {
      string varnam = "_v4d(powx)";
      libm = funcnam + varnam;
      benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTest4d, params)
                 ->Args({(int)params->count})->Iterations(params->niter);
    }
    #if defined(__AVX512__)
    if((params->fqty == ALM::FloatQuantity::E_All) ||
     (params->fqty == ALM::FloatQuantity::E_Vector_8)) {
      string varnam = "_v8d(powx)";
      libm = funcnam + varnam;
      benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTest8d, params)
                 ->Args({(int)params->count})->Iterations(params->niter);
    }
    #endif

    if((params->fqty == ALM::FloatQuantity::E_All) ||
    (params->fqty == ALM::FloatQuantity::E_Vector_Array)) {
        string varnam = "_vad(powx)";
        libm = funcnam + varnam;
        benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTestad, params)
                    ->Args({(int)params->count})->Iterations(params->niter);
    }
  }

  if((params->fwidth == ALM::FloatWidth::E_ALL) ||
    (params->fwidth == ALM::FloatWidth::E_F32)) {

    if((params->fqty == ALM::FloatQuantity::E_All) ||
     (params->fqty == ALM::FloatQuantity::E_Vector_4)) {
      string varnam = "_v4s(powxf)";
      libm = funcnam + varnam;
      benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTest4f, params)
                 ->Args({(int)params->count})->Iterations(params->niter);
    }
    if((params->fqty == ALM::FloatQuantity::E_All) ||
     (params->fqty == ALM::FloatQuantity::E_Vector_8)) {
      string varnam = "_v8s(powxf)";
      libm = funcnam + varnam;
      benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTest8f, params)
                 ->Args({(int)params->count})->Iterations(params->niter);
    }
    #if defined(__AVX512__)
    if((params->fqty == ALM::FloatQuantity::E_All) ||
     (params->fqty == ALM::FloatQuantity::E_Vector_16)) {
      string varnam = "_v16s(powf)";
      libm = funcnam + varnam;
      benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTest16f, params)
                 ->Args({(int)params->count})->Iterations(params->niter);
    }
    #endif
    if((params->fqty == ALM::FloatQuantity::E_All) ||
      (params->fqty == ALM::FloatQuantity::E_Vector_Array)) {
          string varnam = "_vas(powxf)";
          libm = funcnam + varnam;
          benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTestaf, params)
                      ->Args({(int)params->count})->Iterations(params->niter);
   }
  }


  size_t retval = benchmark::RunSpecifiedBenchmarks();
  return (int)retval;
}

AlmTestPerfFramework::~AlmTestPerfFramework() {
#if defined(DEBUG_PRINTS)
  cout << "AlmTestPerfFramework destructor completed" << endl;
#endif
}
