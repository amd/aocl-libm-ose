/*
 * Copyright (C) 2025-2026 Advanced Micro Devices, Inc. All rights reserved.
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
#include "linearfrac.h"

extern vector<SpecParams> specData;

/* Real Number Function Variants */

TEST_P(AccuTestFixtureFloat, ACCURACY_VECTOR_4FLOATS) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  float ip[6];

  data.ip1 = (void *)inpbuff1;
  data.ip2 = (void *)inpbuff2;
  data.ip3 = (void *)inpbuff3;
  data.ip4 = (void *)inpbuff4;
  data.ip5 = (void *)inpbuff5;

  for (uint32_t i = 0; i < count; i += 4) {
    test_v4s(&data, i);

    for (uint32_t j = 0; j < 4; j++) {
      ip[0] = inpbuff [i + j];
      ip[1] = inpbuff1[i + j];
      ip[2] = inpbuff2[i];
      ip[3] = inpbuff3[i];
      ip[4] = inpbuff4[i];
      ip[5] = inpbuff5[i];

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
  float ip[6];

  data.ip1 = (void *)inpbuff1;
  data.ip2 = (void *)inpbuff2;
  data.ip3 = (void *)inpbuff3;
  data.ip4 = (void *)inpbuff4;
  data.ip5 = (void *)inpbuff5;

  for (uint32_t i = 0; i < count; i += 8) {
    test_v8s(&data, i);

    for (uint32_t j = 0; j < 8; j++) {
      ip[0] = inpbuff [i + j];
      ip[1] = inpbuff1[i + j];
      ip[2] = inpbuff2[i];
      ip[3] = inpbuff3[i];
      ip[4] = inpbuff4[i];
      ip[5] = inpbuff5[i];

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
  float ip[6];

  data.ip1 = (void *)inpbuff1;
  data.ip2 = (void *)inpbuff2;
  data.ip3 = (void *)inpbuff3;
  data.ip4 = (void *)inpbuff4;
  data.ip5 = (void *)inpbuff5;

  for (uint32_t i = 0; i < count; i += 16) {
    test_v16s(&data, i);

    for (uint32_t j = 0; j < 16; j++) {
      ip[0] = inpbuff [i + j];
      ip[1] = inpbuff1[i + j];
      ip[2] = inpbuff2[i];
      ip[3] = inpbuff3[i];
      ip[4] = inpbuff4[i];
      ip[5] = inpbuff5[i];

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

TEST_P(AccuTestFixtureDouble, ACCURACY_VECTOR_2DOUBLES) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  double ip[6];

  data.ip1 = (void *)inpbuff1;
  data.ip2 = (void *)inpbuff2;
  data.ip3 = (void *)inpbuff3;
  data.ip4 = (void *)inpbuff4;
  data.ip5 = (void *)inpbuff5;

  for (uint32_t i = 0; i < count; i += 2) {
    test_v2d(&data, i);

    for (uint32_t j = 0; j < 2; j++) {
      ip[0] = inpbuff [i + j];
      ip[1] = inpbuff1[i + j];
      ip[2] = inpbuff2[i];
      ip[3] = inpbuff3[i];
      ip[4] = inpbuff4[i];
      ip[5] = inpbuff5[i];

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
  double ip[6];

  data.ip1 = (void *)inpbuff1;
  data.ip2 = (void *)inpbuff2;
  data.ip3 = (void *)inpbuff3;
  data.ip4 = (void *)inpbuff4;
  data.ip5 = (void *)inpbuff5;

  for (uint32_t i = 0; i < count; i += 4) {
    test_v4d(&data, i);

    for (uint32_t j = 0; j < 4; j++) {
      ip[0] = inpbuff [i + j];
      ip[1] = inpbuff1[i + j];
      ip[2] = inpbuff2[i];
      ip[3] = inpbuff3[i];
      ip[4] = inpbuff4[i];
      ip[5] = inpbuff5[i];

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
  double ip[6];

  data.ip1 = (void *)inpbuff1;
  data.ip2 = (void *)inpbuff2;
  data.ip3 = (void *)inpbuff3;
  data.ip4 = (void *)inpbuff4;
  data.ip5 = (void *)inpbuff5;

  for (uint32_t i = 0; i < count; i += 8) {
    test_v8d(&data, i);

    for (uint32_t j = 0; j < 8; j++) {
      ip[0] = inpbuff [i + j];
      ip[1] = inpbuff1[i + j];
      ip[2] = inpbuff2[i];
      ip[3] = inpbuff3[i];
      ip[4] = inpbuff4[i];
      ip[5] = inpbuff5[i];

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

TEST_P(AccuTestFixtureFloat, ACCURACY_VECTOR_ARRAY_FLOATS) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  float ip[6];

  data.ip1 = (void *)inpbuff1;
  data.ip2 = (void *)inpbuff2;
  data.ip3 = (void *)inpbuff3;
  data.ip4 = (void *)inpbuff4;
  data.ip5 = (void *)inpbuff5;

  test_vas(&data, count);
  for (uint32_t i = 0; i < count; i++)
  {
    ip[0] = inpbuff[i];
    ip[1] = inpbuff1[i];
    ip[2] = inpbuff2[0];
    ip[3] = inpbuff3[0];
    ip[4] = inpbuff4[0];
    ip[5] = inpbuff5[0];

    double exptd = getExpected(ip);
    double ulp = getUlp(aop[i], exptd);
    if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold))
    {
      nfail++;
    }

    if ((vflag == 1) && (ulp > inData->ulp_threshold))
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd, aop[i], ulp);
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vec_Array","Accuracy","vas",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureDouble, ACCURACY_VECTOR_ARRAY_DOUBLES) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  double ip[6];

  data.ip1 = (void *)inpbuff1;
  data.ip2 = (void *)inpbuff2;
  data.ip3 = (void *)inpbuff3;
  data.ip4 = (void *)inpbuff4;
  data.ip5 = (void *)inpbuff5;

  test_vad(&data, count);
  for (uint32_t i = 0; i < count; i++)
  {
    ip[0] = inpbuff[i];
    ip[1] = inpbuff1[i];
    ip[2] = inpbuff2[0];
    ip[3] = inpbuff3[0];
    ip[4] = inpbuff4[0];
    ip[5] = inpbuff5[0];

    long double exptd = getExpected(ip);
    double ulp = getUlp(aop[i], exptd);
    if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold))
    {
      nfail++;
    }

    if ((vflag == 1) && (ulp > inData->ulp_threshold))
        PrintUlpResultsDouble(nargs, ip[0], ip[1], exptd, aop[i], ulp);
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vec_Array","Accuracy","vad",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(SpecTestFixtureLinearFracFloatArray, CONFORMANCE_VECTOR_ARRAY_FLOATS) {
  int nfail = 0;
  int cnt = 0;
  test_data tdata;
  tdata.ip  = (void *)data1;
  tdata.ip1 = (void *)data2;
  tdata.op  = (void *)aop_array;
  float ip[6];

  for (uint32_t j = 0; j < (count-8); j++) {/*vrsa takes zen3 path */
      tdata.ip2 = (void *)&data3[j];
      tdata.ip3 = (void *)&data4[j];
      tdata.ip4 = (void *)&data5[j];
      tdata.ip5 = (void *)&data6[j];

      test_vas(&tdata, count);

      ip[2] = data3[j];
      ip[3] = data4[j];
      ip[4] = data5[j];
      ip[5] = data6[j];
      for (uint32_t i = 0; i < count; i+=8) {
        ip[0] = data1[i];
        ip[1] = data2[i];

        double expected_op = getExpected(ip);

        ConfVerifyFlt(ip[0], ip[1], ip[2], ip[3], ip[4], ip[5], aop_array[i], expected_op, &nfail);
        cnt++;
      }
  }

  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d",
  "VecArr","Conformance","vrsa",cnt,(cnt-nfail), nfail);
  ptr->tstcnt++;
}

TEST_P(SpecTestFixtureLinearFracDoubleArray, CONFORMANCE_VECTOR_ARRAY_DOUBLES) {
  int nfail = 0;
  int cnt = 0;
  test_data tdata;
  tdata.ip  = (void *)data1;
  tdata.ip1 = (void *)data2;
  tdata.op  = (void *)aop_array;
  double ip[6];

  for (uint32_t j = 0; j < (count-4); j++) {/*vrda takes zen3 path */
      tdata.ip2 = (void *)&data3[j];
      tdata.ip3 = (void *)&data4[j];
      tdata.ip4 = (void *)&data5[j];
      tdata.ip5 = (void *)&data6[j];

      test_vad(&tdata, count);

      ip[2] = data3[j];
      ip[3] = data4[j];
      ip[4] = data5[j];
      ip[5] = data6[j];
      for (uint32_t i = 0; i < count; i+=4) {
        ip[0] = data1[i];
        ip[1] = data2[i];

        long double expected_op = getExpected(ip);

        ConfVerifyDbl(ip[0], ip[1], ip[2], ip[3], ip[4], ip[5], aop_array[i], expected_op, &nfail);
        cnt++;
      }
  }

  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d",
  "VecArr","Conformance","vrda",cnt,(cnt-nfail), nfail);
  ptr->tstcnt++;
}

/*****************************************************************************/
/***                            INSTANTIATE_TEST_SUITE_P                   ***/
/*****************************************************************************/
INSTANTIATE_TEST_SUITE_P(SpecTests, SpecTestFixtureLinearFracFloatArray,
                         ::testing::ValuesIn(specData));

INSTANTIATE_TEST_SUITE_P(SpecTests, SpecTestFixtureLinearFracDoubleArray,
                         ::testing::ValuesIn(specData));
/*****************************************************************************/
