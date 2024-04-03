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


#include <cstdio>
#include <iostream>
#include <string>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <vector>
#include "almstruct.h"
#include "gtest.h"
#include "defs.h"
#include "almtest.h"
#include "cmdline.h"
#include "callback.h"
#include <inttypes.h>
#include "verify.h"
#include <external/amdlibm.h>
#include <complex.h>
#include "func_var_existence.h"

extern vector<AccuParams> accuData;
extern vector<SpecParams> specData;

/* Real Number Function Variants */

TEST_P(AccuTestFixtureFloat, ACCURACY_SCALAR_FLOAT) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  float ip[2];

  if(nargs == 2)
    data.ip1 = (void *)inpbuff1;

  for (uint32_t i = 0; i < count; i++) {
    test_s1s(&data, i);

    ip[0] = inpbuff[i];
    if(nargs == 2)
      ip[1] = inpbuff1[i];

    double exptd = getExpected(ip);
    double ulp = getUlp(aop[0], exptd);
    if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
      nfail++;
    }

    if ((vflag == 1) && (ulp > inData->ulp_threshold))
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd, aop[0], ulp);
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Scalar","Accuracy","s1s",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureFloat, ACCURACY_VECTOR_4FLOATS) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  float ip[2];

  if(nargs == 2)
    data.ip1 = (void *)inpbuff1;

  for (uint32_t i = 0; i < count; i += 4) {
    test_v4s(&data, i);

    for (uint32_t j = 0; j < 4; j++) {
      ip[0] = inpbuff[i + j];
      if(nargs == 2)
        ip[1] = inpbuff1[i + j];

      double exptd = getExpected(ip);
      double ulp = getUlp(aop[j], exptd);
      if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
        nfail++;
      }

      if ((vflag == 1) && (ulp > inData->ulp_threshold))
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd, aop[j], ulp);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","v4s",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureFloat, ACCURACY_VECTOR_8FLOATS) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  float ip[2];

  if(nargs == 2)
    data.ip1 = (void *)inpbuff1;

  for (uint32_t i = 0; i < count; i += 8) {
    test_v8s(&data, i);

    for (uint32_t j = 0; j < 8; j++) {
      ip[0] = inpbuff[i + j];
      if(nargs == 2)
        ip[1] = inpbuff1[i + j];

      double exptd = getExpected(ip);
      double ulp = getUlp(aop[j], exptd);
      if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
        nfail++;
      }

      if ((vflag == 1) && (ulp > inData->ulp_threshold))
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd, aop[j], ulp);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","v8s",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureFloat, ACCURACY_VECTOR_16FLOATS) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  float ip[2];

  if(nargs == 2)
    data.ip1 = (void *)inpbuff1;

  for (uint32_t i = 0; i < count; i += 16) {
    test_v16s(&data, i);

    for (uint32_t j = 0; j < 16; j++) {
      ip[0] = inpbuff[i + j];
      if(nargs == 2)
        ip[1] = inpbuff1[i + j];

      double exptd = getExpected(ip);
      double ulp = getUlp(aop[j], exptd);
      if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
        nfail++;
      }

      if ((vflag == 1) && (ulp > inData->ulp_threshold))
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd, aop[j], ulp);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","v16s",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureDouble, ACCURACY_SCALAR_DOUBLE) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  double ip[2];

  if(nargs == 2)
    data.ip1 = (void *)inpbuff1;

  for (uint32_t i = 0; i < count; i++) {
    test_s1d(&data, i);

    ip[0] = inpbuff[i];
    if(nargs == 2)
      ip[1] = inpbuff1[i];

    long double exptd = getExpected(ip);
    double ulp = getUlp(aop[0], exptd);
    if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
      nfail++;
    }

    if ((vflag == 1) && (ulp > inData->ulp_threshold))
        PrintUlpResultsDouble(nargs, ip[0], ip[1], exptd, aop[0], ulp);
  }

  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Scalar","Accuracy","s1d",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureDouble, ACCURACY_VECTOR_2DOUBLES) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  double ip[2];

  if(nargs == 2)
    data.ip1 = (void *)inpbuff1;

  for (uint32_t i = 0; i < count; i += 2) {
    test_v2d(&data, i);

    for (uint32_t j = 0; j < 2; j++) {
      ip[0] = inpbuff[i + j];
      if(nargs == 2)
        ip[1] = inpbuff1[i + j];

      long double exptd = getExpected(ip);
      double ulp = getUlp(aop[j], exptd);
      if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
        nfail++;
      }

      if ((vflag == 1) && (ulp > inData->ulp_threshold))
        PrintUlpResultsDouble(nargs, ip[0], ip[1], exptd, aop[j], ulp);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","v2d",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureDouble, ACCURACY_VECTOR_4DOUBLES) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  double ip[2];

  if(nargs == 2)
    data.ip1 = (void *)inpbuff1;

  for (uint32_t i = 0; i < count; i += 4) {
    test_v4d(&data, i);

    for (uint32_t j = 0; j < 4; j++) {
      ip[0] = inpbuff[i + j];
      if(nargs == 2)
        ip[1] = inpbuff1[i + j];

      long double exptd = getExpected(ip);
      double ulp = getUlp(aop[j], exptd);
      if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
        nfail++;
      }

      if ((vflag == 1) && (ulp > inData->ulp_threshold))
        PrintUlpResultsDouble(nargs, ip[0], ip[1], exptd, aop[j], ulp);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","v4d",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureDouble, ACCURACY_VECTOR_8DOUBLES) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  double ip[2];

  if(nargs == 2)
    data.ip1 = (void *)inpbuff1;

  for (uint32_t i = 0; i < count; i += 8) {
    test_v8d(&data, i);

    for (uint32_t j = 0; j < 8; j++) {
      ip[0] = inpbuff[i + j];
      if(nargs == 2)
        ip[1] = inpbuff1[i + j];

      long double exptd = getExpected(ip);
      double ulp = getUlp(aop[j], exptd);
      if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
        nfail++;
      }

      if ((vflag == 1) && (ulp > inData->ulp_threshold))
        PrintUlpResultsDouble(nargs, ip[0], ip[1], exptd, aop[j], ulp);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","v8d",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(SpecTestFixtureFloat, CONFORMANCE_FLOAT) {
  int nfail = 0;
  float aop, op;
  float ip[2];
  test_data t;
  t.ip  = (void *)data;
  t.op  = (void *)&aop;

  if(nargs == 2)
    t.ip1 = (void *)data1;

  for (uint32_t i = 0; i < count; i++) {
    feclearexcept (FE_ALL_EXCEPT);
    test_s1s(&t, i);
    int raised_exception = fetestexcept(FE_ALL_EXCEPT);
    feclearexcept (FE_ALL_EXCEPT);

    ip[0] = data[i];
    if(nargs == 2)  {
      ip[1] = data1[i];
    }

    op = getExpected(ip);
    int eef = expected_expection[i];

    SpecTestFixtureFloat::ConfVerifyFlt(nargs, ip[0], ip[1], aop, op, raised_exception, eef, &nfail);

    if (vflag == 1) {
      if (nargs == 1) {
	cout << "Input: " << ip[0] << " Output: " << aop << " Expected: " << op << endl;
      }
      else if (nargs == 2) {
	cout << "Input1: " << ip[0] << " Input2: " << ip[1] << " Output: " << aop << " Expected: " << op << endl;
      }
      PrintConfExpections(raised_exception, eef);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d",
  "Scalar","Conformance","s1s",count,(count-nfail), nfail);
  ptr->tstcnt++;
}

TEST_P(SpecTestFixtureDouble, CONFORMANCE_DOUBLE) {
  int nfail = 0;
  double aop, op;
  double ip[2];
  test_data t;
  t.ip  = (void *)data;
  t.op  = (void *)&aop;

  if(nargs == 2)
    t.ip1 = (void *)data1;

  for (uint32_t i = 0; i < count; i++) {
    feclearexcept (FE_ALL_EXCEPT);
    test_s1d(&t, i);
    int raised_exception = fetestexcept(FE_ALL_EXCEPT);
    feclearexcept (FE_ALL_EXCEPT);

    ip[0] = data[i];

    if(nargs == 2)  {
      ip[1] = data1[i];
    }

    op = getExpected(ip);
    int eef = expected_expection[i];

    SpecTestFixtureDouble::ConfVerifyDbl(nargs, ip[0], ip[1], aop, op, raised_exception, eef, &nfail);

    if (vflag == 1) {
      if (nargs == 1) {
	cout << "Input: " << ip[0] << " Output: " << aop << " Expected: " << op << endl;
      }
      else if (nargs == 2) {
	cout << "Input1: " << ip[0] << " Input2: " << ip[1] << " Output: " << aop << " Expected: " << op << endl;
      }
      PrintConfExpections(raised_exception, eef);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d",
  "Scalar","Conformance","s1d",count,(count-nfail), nfail);
  ptr->tstcnt++;
}

/* Complex Number Function Variants */

TEST_P(AccuTestFixtureComplexFloat, ACCURACY_SCALAR_COMPLEX_FLOAT) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)complex_inpbuff;
  data.op  = (void *)aop;
  float _Complex ip[2];

  if(nargs == 2)
    data.ip1 = (void *)complex_inpbuff1;

  for (uint32_t i = 0; i < count*count; i++) {
    test_s1s(&data, i);

    ip[0] = complex_inpbuff[i];
    if(nargs == 2)
      ip[1] = complex_inpbuff1[i];

    double _Complex exptd = getExpected(ip);
    double ulp = getUlp(aop[0], exptd);

    if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
      nfail++;
    }

    if ((vflag == 1) && (ulp > inData->ulp_threshold))
        PrintUlpResultsComplexFloat(nargs, ip[0], ip[1], exptd, aop[0], ulp);
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Scalar","Accuracy","s1s complex",count*count,(count*count - nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureComplexDouble, ACCURACY_SCALAR_COMPLEX_DOUBLE) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)complex_inpbuff;
  data.op  = (void *)aop;
  double _Complex ip[2];

  if(nargs == 2)
    data.ip1 = (void *)complex_inpbuff1;

  for (uint32_t i = 0; i < count*count; i++) {
    test_s1d(&data, i);

    ip[0] = complex_inpbuff[i];
    if(nargs == 2)
      ip[1] = complex_inpbuff1[i];

    long double _Complex exptd = getExpected(ip);
    double ulp = getUlp(aop[0], exptd);

    if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
      nfail++;
    }

    if ((vflag == 1) && (ulp > inData->ulp_threshold))
        PrintUlpResultsComplexDouble(nargs, ip[0], ip[1], exptd, aop[0], ulp);
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Scalar","Accuracy","s1d complex",count*count,(count*count - nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(SpecTestFixtureComplexFloat, CONFORMANCE_COMPLEX_FLOAT) {
  int nfail = 0;
  float _Complex aop, op;
  float _Complex ip[2];
  test_data t;
  t.ip  = (void *)data;
  t.op  = (void *)&aop;

  if(nargs == 2)
    t.ip1 = (void *)data1;

  for (uint32_t i = 0; i < count; i++) {
    feclearexcept (FE_ALL_EXCEPT);
    test_s1s(&t, i);
    int raised_exception = fetestexcept(FE_ALL_EXCEPT);
    feclearexcept (FE_ALL_EXCEPT);

    ip[0] = data[i];
    if(nargs == 2)  {
      ip[1] = data1[i];
    }

    op = getExpected(ip);
    int eef = expected_expection[i];

    SpecTestFixtureComplexFloat::ConfVerifyComplexFlt(nargs, ip[0], ip[1], aop, op, raised_exception, eef, &nfail);

    if (vflag == 1) {
      cout << "Input          : " << (double) (__real__ ip[0]) << "+i" << (double) (__imag__ ip[0]) << endl;
      if(nargs == 2) {
        cout << "Input 2        : " << (double) (__real__ ip[1]) << "+i" << (double) (__imag__ ip[1]) << endl;
      }
      cout << "Actual Output  : " << (double) (__real__ aop) << "+i" << (double) (__imag__ aop) << endl;
      cout << "Expected Output: " << (double) (__real__ op) << "+i" << (double) (__imag__ op) << endl;
      PrintConfExpections(raised_exception, eef);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d",
  "Scalar","Conformance","s1s complex",count,(count-nfail), nfail);
  ptr->tstcnt++;
}

TEST_P(SpecTestFixtureComplexDouble, CONFORMANCE_COMPLEX_DOUBLE) {
  int nfail = 0;
  double _Complex aop, op;
  double _Complex ip[2];
  test_data t;
  t.ip  = (void *)data;
  t.op  = (void *)&aop;

  if(nargs == 2)
    t.ip1 = (void *)data1;

  for (uint32_t i = 0; i < count; i++) {
    feclearexcept (FE_ALL_EXCEPT);
    test_s1d(&t, i);
    int raised_exception = fetestexcept(FE_ALL_EXCEPT);
    feclearexcept (FE_ALL_EXCEPT);

    ip[0] = data[i];
    if(nargs == 2)  {
      ip[1] = data1[i];
    }

    op = getExpected(ip);
    int eef = expected_expection[i];

    SpecTestFixtureComplexDouble::ConfVerifyComplexDbl(nargs, ip[0], ip[1], aop, op, raised_exception, eef, &nfail);

    if (vflag == 1) {
      cout << "Input          : " << (double) (__real__ ip[0]) << "+i" << (double) (__imag__ ip[0]) << endl;
      if(nargs == 2) {
        cout << "Input 2        : " << (double) (__real__ ip[1]) << "+i" << (double) (__imag__ ip[1]) << endl;
      }
      cout << "Actual Output  : " << (double) (__real__ aop) << "+i" << (double) (__imag__ aop) << endl;
      cout << "Expected Output: " << (double) (__real__ op) << "+i" << (double) (__imag__ op) << endl;
      PrintConfExpections(raised_exception, eef);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d",
  "Scalar","Conformance","s1d complex",count,(count-nfail), nfail);
  ptr->tstcnt++;
}


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

