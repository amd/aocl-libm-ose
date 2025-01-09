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


#ifndef __ALMTESTPERF_H__
#define __ALMTESTPERF_H__

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <limits>
#include "args.h"
#include "almstruct.h"
#include "defs.h"
#include "random.h"
#include "debug.h"
#include "benchmark.h"

using namespace std;
using namespace ALM;
using namespace ALMTest;

void LibmPerfTestf(benchmark::State& st, InputParams* param);
void LibmPerfTest4f(benchmark::State& st, InputParams* param);
void LibmPerfTest8f(benchmark::State& st, InputParams* param);
void LibmPerfTest16f(benchmark::State& st, InputParams* param);

void LibmPerfTestd(benchmark::State& st, InputParams* param);
void LibmPerfTest2d(benchmark::State& st, InputParams* param);
void LibmPerfTest4d(benchmark::State& st, InputParams* param);
void LibmPerfTest8d(benchmark::State& st, InputParams* param);

void LibmPerfTestaf(benchmark::State& st, InputParams* param);
void LibmPerfTestad(benchmark::State& st, InputParams* param);

 /*
 * The Function populates the input values based on min value, max value
 * and the type of generation(simple or random or linear)
 */
template <typename T>
int PopulateInputSamples(T *inpbuff, InputRange &range, unsigned int len) {
  Random<T> r = Random<T>(range.type);
  if (!range.min && !range.max) {
    #if defined(_WIN64) || defined(_WIN32)
      range.min = (std::numeric_limits<T>::min)();
      range.max = (std::numeric_limits<T>::max)();
    #else
      range.min = std::numeric_limits<T>::min();
      range.max = std::numeric_limits<T>::max();
    #endif
  }

  int res = r.Fill(inpbuff, len, range.min, range.max, range.type);
  return res;
}

template<typename T>
class AoclLibmTest {
  protected:

  public:
    T *inpbuff;
    T *inpbuff1;
    T *inpbuff2;
    T *inpbuff3;
    T *inpbuff4;
    T *inpbuff5;
    T *outbuff;
    uint32_t ipargs;
    explicit AoclLibmTest(InputParams *params, uint32_t nargs) {
      ipargs = nargs;
      unsigned int sz = params->count;
      int vec_input_count = (int)params->fqty;
      if (params->fqty == ALM::FloatQuantity::E_All)
        vec_input_count = (int)ALM::FloatQuantity::E_Vector_16;

      if ((sz % vec_input_count != 0)) {
        sz = sz + (vec_input_count - sz % vec_input_count);
        params->count = sz;
      }

      unsigned int arr_size =  sz * sizeof(T);
      if((arr_size % _ALIGN_FACTOR) != 0)
      {
        int factor = (arr_size / _ALIGN_FACTOR) + 1;
        arr_size = _ALIGN_FACTOR * factor;
      }
      aocl_libm_aligned_alloc(arr_size, inpbuff);
      aocl_libm_aligned_alloc(arr_size, outbuff);
      PopulateInputSamples(inpbuff, params->range[0], params->count);
      if (nargs == 2) {
        aocl_libm_aligned_alloc(arr_size, inpbuff1);
        PopulateInputSamples(inpbuff1, params->range[1], params->count);
      }

      if(nargs == 6) {
        aocl_libm_aligned_alloc(arr_size, inpbuff1);
        aocl_libm_aligned_alloc(arr_size, inpbuff2);
        aocl_libm_aligned_alloc(arr_size, inpbuff3);
        aocl_libm_aligned_alloc(arr_size, inpbuff4);
        aocl_libm_aligned_alloc(arr_size, inpbuff5);

        PopulateInputSamples(inpbuff1, params->range[1], params->count);
        PopulateInputSamples(inpbuff2, params->range[2], params->count);
        PopulateInputSamples(inpbuff3, params->range[3], params->count);
        PopulateInputSamples(inpbuff4, params->range[4], params->count);
        PopulateInputSamples(inpbuff5, params->range[5], params->count);
      }
    }

    ~AoclLibmTest()
    {
      if (inpbuff != NULL)
      {
        aocl_libm_aligned_free(inpbuff);
        if (ipargs == 2)
        {
          aocl_libm_aligned_free(inpbuff1);
        }

        if(ipargs == 6)
        {
          aocl_libm_aligned_free(inpbuff1);
          aocl_libm_aligned_free(inpbuff2);
          aocl_libm_aligned_free(inpbuff3);
          aocl_libm_aligned_free(inpbuff4);
          aocl_libm_aligned_free(inpbuff5);
        }
        aocl_libm_aligned_free(outbuff);
      }
    }
};

class AlmTestPerfFramework {

 public:
  ~AlmTestPerfFramework();

  int AlmTestPerformance(InputParams *);
};
#endif
