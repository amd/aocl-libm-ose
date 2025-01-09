/*
 * Copyright (C) 2008-2025 Advanced Micro Devices, Inc. All rights reserved.
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
  // NOTE: Todo: Accuracy for complex function variants to be designed!

  if((params->fwidth == ALM::FloatWidth::E_ALL) ||
    (params->fwidth == ALM::FloatWidth::E_F32)) {
    if((params->fqty == ALM::FloatQuantity::E_All) ||
     (params->fqty == ALM::FloatQuantity::E_Scalar)) {
      string varnam = "_s1s(cexpf)";
      libm = funcnam + varnam;
      benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTestf, params)
                 ->Args({(int)params->count})->Iterations(params->niter);
    }
  }

  if((params->fwidth == ALM::FloatWidth::E_ALL) ||
    (params->fwidth == ALM::FloatWidth::E_F64)) {
    if((params->fqty == ALM::FloatQuantity::E_All) ||
     (params->fqty == ALM::FloatQuantity::E_Scalar)) {
      string varnam = "_s1d(cexp)";
      libm = funcnam + varnam;
      benchmark::RegisterBenchmark(libm.c_str(), &LibmPerfTestd, params)
                 ->Args({(int)params->count})->Iterations(params->niter);
    }
  }
  size_t retval = benchmark::RunSpecifiedBenchmarks();; // benchmark::RunSpecifiedBenchmarks();

  return (int)retval;
}

AlmTestPerfFramework::~AlmTestPerfFramework() {
#if defined(DEBUG_PRINTS)
  cout << "AlmTestPerfFramework destructor completed" << endl;
#endif
}
