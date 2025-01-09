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


#include <cstdio>
#include <iostream>
#include <string>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <vector>
#include "almstruct.h"
#include "defs.h"
#include "almtest.h"
#include "cmdline.h"
#include "callback.h"
#include <inttypes.h>
#include "verify.h"
#include <external/amdlibm.h>
#include "func_var_existence.h"

extern vector<AccuParams> accuData;
extern vector<SpecParams> specData;

/*****************************************************************************/
/***                            INSTANTIATE_TEST_SUITE_P                   ***/
/*****************************************************************************/
INSTANTIATE_TEST_SUITE_P(SpecTests, SpecTestFixtureComplexFloat,
                         ::testing::ValuesIn(specData));

INSTANTIATE_TEST_SUITE_P(SpecTests, SpecTestFixtureComplexDouble,
                         ::testing::ValuesIn(specData));

INSTANTIATE_TEST_SUITE_P(AccuTests, AccuTestFixtureComplexFloat,
                         ::testing::ValuesIn(accuData));

INSTANTIATE_TEST_SUITE_P(AccuTests, AccuTestFixtureComplexDouble,
                         ::testing::ValuesIn(accuData));

INSTANTIATE_TEST_SUITE_P(AccuTests, AccuTestFixtureFloat,
                         ::testing::ValuesIn(accuData));

INSTANTIATE_TEST_SUITE_P(AccuTests, AccuTestFixtureDouble,
                         ::testing::ValuesIn(accuData));

INSTANTIATE_TEST_SUITE_P(SpecTests, SpecTestFixtureFloat,
                         ::testing::ValuesIn(specData));

INSTANTIATE_TEST_SUITE_P(SpecTests, SpecTestFixtureDouble,
                         ::testing::ValuesIn(specData));
/*****************************************************************************/

int gtest_main(int argc, char **argv, InputParams *inparams) {
  AlmTestFramework almTest;
  string filter_data("");
  InputData *inData = NULL;
  PrintTstRes *ptr = NULL;

  inData = almTest.getInputData();
  ptr    = almTest.getPrintTetRes();

  memset(inData, 0, sizeof(InputData));
  inData->max_ulp_err = 0.0;
  inData->ulp_threshold = 0.5;

  memset(ptr, 0, sizeof(PrintTstRes));
  almTest.AlmTestType(inparams, inData, ptr);
  almTest.CreateGtestFilters(inparams, filter_data);

  if (inparams->ttype == ALM::TestType::E_Accuracy)
  {
    filter_data = validateFilterData(inparams->testFunction, filter_data);
    if(filter_data.length() == 0)
    {
      cout << inparams->testFunction << "() does not support the requested variants!" << endl;
      return 1;
    }
    cout << "Supported variants: " << filter_data << endl;
  }

  ::testing::GTEST_FLAG(filter) = filter_data.c_str();
  testing::InitGoogleTest(&argc, argv);
  int ret_val = RUN_ALL_TESTS();

  cout << ptr->print[0] << endl;
  cout << ptr->print[1] << endl;
  cout << ptr->print[0] << endl;
  for (uint64_t i = 2; i < ptr->tstcnt ; i++ ) {
    if(strlen(ptr->print[i]) != 0) {
      cout << ptr->print[i] << endl;
    }
  }
  cout << ptr->print[0] << endl;
  return ret_val;
}
