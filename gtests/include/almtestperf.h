/*
 * Google Test framework for Libm math library
 *
 */
#ifndef __ALMTESTPERF_H__
#define __ALMTESTPERF_H__

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <limits>
#include "../../include/external/amdlibm.h"
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
        outbuff = (T*)aligned_alloc(_ALIGN_FACTOR, 32);
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
