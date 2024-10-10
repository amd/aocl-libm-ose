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


#ifndef __ALMSTRUCT_H__
#define __ALMSTRUCT_H__

#include "defs.h"
#include <cstdint>
#include <external/amdlibm.h>
#include <string>

using namespace ALM;
#define MAX_INPUT_RANGES 6

#define _ALIGN_FACTOR 256
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define THOUSAND (1000)
#define MILLION  (THOUSAND * THOUSAND)

#define NITER    (100 * THOUSAND)
#define NELEM     5000

typedef struct {
  uint32_t in;
  uint32_t out;
  uint32_t exptdexpt;
  uint32_t in2;
  uint32_t in3;
  uint32_t in4;
  uint32_t in5;
  uint32_t in6;
}libm_test_special_data_f32;

typedef struct {
  uint64_t in;
  uint64_t out;
  uint64_t exptdexpt;
  uint64_t in2;
  uint64_t in3;
  uint64_t in4;
  uint64_t in5;
  uint64_t in6;
}libm_test_special_data_f64;

typedef struct {
  float _Complex in;
  float _Complex out;
  uint32_t exptdexpt;
  float _Complex in2;
  float _Complex in3;
}libm_test_complex_data_f32;

typedef struct {
  double _Complex in;
  double _Complex out;
  uint64_t exptdexpt;
  double _Complex in2;
  double _Complex in3;
}libm_test_complex_data_f64;

typedef struct {
  double min;
  double max;
  enum RangeType type;
} InputRange;

typedef struct {
  double max_ulp_err; /* ULP error */
  double ulp_threshold;
} InputData;

typedef struct {
   uint32_t tstcnt;
  char print[12][100];
} PrintTstRes;

/*
 * The structure is filled with the command line arguments
 * Based on the Testtype,Floattype and FloatWidth,
 *                        testcases are decided
 */
typedef struct {
  FloatType ftype;
  FloatWidth fwidth;
  FloatQuantity fqty;

  TestType ttype;
  int verboseflag;
  uint32_t niter;
  uint32_t count;
  std::string testFunction;
  InputRange range[MAX_INPUT_RANGES];
} InputParams;


/*
 * The structure is passed to the AccuTestFixtureFloat
 * or AccuTestFixtureDouble to SetUp function, to populate
 * the input values for the test fixture
 */
typedef struct {
  InputRange range[MAX_INPUT_RANGES];
  uint32_t count;
  int verboseflag;
  InputData *inpData;
  PrintTstRes *prttstres;
  uint32_t nargs;
  int vec_input_count;
} AccuParams;

/*
 * The structure is passed to the SpecTestFixtureFloat
 * or SpecTestFixtureDouble to SetUp function, to read the values
 * from the table and feed the values to the test fixture
 */
typedef struct {
  libm_test_special_data_f32 *data32;
  libm_test_special_data_f64 *data64;
  libm_test_complex_data_f32 *cdata32;
  libm_test_complex_data_f64 *cdata64;
  uint32_t countf;
  uint32_t countd;
  int verboseflag;
  PrintTstRes *prttstres;
  uint32_t nargs;
} SpecParams;


int gtest_main(int argc, char **argv, InputParams *params);
int gbench_main(int argc, char **argv, InputParams *params);

/*
 * This function is a wrapper around:
 * aligned_alloc() for Linux platform
 * _aligned_malloc() for Windows platform
 *
 * NOTE:
 * aligned_alloc() returns NULL pointer when:
 *     1. alignment param is not a valid data.
 *     2. size param is not an integral multiple of alignment.
 */
template <typename T>
int aocl_libm_aligned_alloc(unsigned int arr_size, T* &buff)
{
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      buff = (T*)_aligned_malloc(arr_size, _ALIGN_FACTOR);
    #else
      buff = (T*)aligned_alloc(_ALIGN_FACTOR, arr_size);
    #endif
    return 0;
}

/*
 * This function is a wrapper around:
 * free() for Linux platform
 * _aligned_free() for Windows platform
 */
template <typename T>
int aocl_libm_aligned_free(T* &buff)
{
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      _aligned_free(buff);
    #else
      free(buff);
    #endif
    buff = nullptr;
    return 0;
}
#endif
