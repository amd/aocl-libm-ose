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

using namespace std;
using namespace ALM;
using namespace ALMTest;

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
      sz = (arr_size << 1) + _ALIGN_FACTOR;

      #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
        inpbuff = (T*)_aligned_malloc(sz, _ALIGN_FACTOR);
        outbuff = (T*)_aligned_malloc(32, _ALIGN_FACTOR);
      #else
        inpbuff = (T*)aligned_alloc(_ALIGN_FACTOR, sz);
        outbuff = (T*)aligned_alloc(_ALIGN_FACTOR, sz);
      #endif

      PopulateInputSamples(inpbuff, params->range[0], params->count);
      if (nargs == 2) {
        #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
          inpbuff1 = (T*)_aligned_malloc(sz, _ALIGN_FACTOR);
        #else
          inpbuff1 = (T*)aligned_alloc(_ALIGN_FACTOR, sz);
        #endif
        PopulateInputSamples(inpbuff, params->range[1], params->count);
      }
    }

    ~AoclLibmTest() {
  if (inpbuff != NULL) {
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      _aligned_free(inpbuff);
    #else
      free(inpbuff);
    #endif
    inpbuff = nullptr;
  if (ipargs == 2) {
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      _aligned_free(inpbuff1);
    #else
      free(inpbuff1);
    #endif
    inpbuff1 = nullptr;
      }
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      _aligned_free(outbuff);
    #else
      free(outbuff);
    #endif
        outbuff = nullptr;
      }
    }
};

class AlmTestPerfFramework {

 public:
  ~AlmTestPerfFramework();

  int AlmTestPerformance(InputParams *);
};
#endif
