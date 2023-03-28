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


#ifndef __ALMSTRUCT_H__
#define __ALMSTRUCT_H__

#include "defs.h"
#include <cstdint>

using namespace ALM;
#define MAX_INPUT_RANGES 2

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
}libm_test_special_data_f32;

typedef struct {
	uint64_t in;
	uint64_t out;
	uint64_t exptdexpt;
	uint64_t in2;
	uint64_t in3;
}libm_test_special_data_f64;

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
} AccuParams;

/*
 * The structure is passed to the SpecTestFixtureFloat
 * or SpecTestFixtureDouble to SetUp function, to read the values
 * from the table and feed the values to the test fixture
 */
typedef struct {
  libm_test_special_data_f32 *data32;
  libm_test_special_data_f64 *data64;
  uint32_t countf;
  uint32_t countd;
  int verboseflag;
  PrintTstRes *prttstres;  
  uint32_t nargs;
} SpecParams;


int gtest_main(int argc, char **argv, InputParams *params);
int gbench_main(int argc, char **argv, InputParams *params);
#endif
