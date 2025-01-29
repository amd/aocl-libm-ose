/*
 * Copyright (C) 2025 Advanced Micro Devices, Inc. All rights reserved.
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
#include "sincos.h"
#include "test_sincos_data.h"

extern vector<SpecParams> specData;

/* Real Number Function Variants */
TEST_P(AccuTestFixtureFloat, ACCURACY_SCALAR_FLOAT) {
  int nargs = 1;
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  float scp[16] = {0};
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  data.sc  = (void *)scp;
  float ip[2];
  double exptd[2];

  for (uint32_t i = 0; i < count; i++) {
    test_s1s(&data, i);

    ip[0] = inpbuff[i];

    getExpected(ip,exptd);
    double ulps = getUlp(aop[0], exptd[0]);
    double ulpc = getUlp(scp[0], exptd[1]);
    if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) ||
                   (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
      nfail++;
    }

    if ((vflag == 1) && ((ulps > inData->ulp_threshold) || (ulpc > inData->ulp_threshold))) {
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[0], aop[0], ulps);
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[1], scp[0], ulpc);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Scalar","Accuracy","s1s",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureFloat, ACCURACY_VECTOR_4FLOATS) {
  int nargs = 1;
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  float scp[16] = {0};
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  data.sc  = (void *)scp;
  float ip[2];
  double exptd[2];

  for (uint32_t i = 0; i < count; i += 4) {
    test_v4s(&data, i);

    for (uint32_t j = 0; j < 4; j++) {
      ip[0] = inpbuff[i + j];

      getExpected(ip,exptd);
      double ulps = getUlp(aop[j], exptd[0]);
      double ulpc = getUlp(scp[j], exptd[1]);
      if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) ||
                     (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
        nfail++;
      }

      if ((vflag == 1) && ((ulps > inData->ulp_threshold) || (ulpc > inData->ulp_threshold))) {
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[0], aop[j], ulps);
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[1], scp[j], ulpc);
      }
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","v4s",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureFloat, ACCURACY_VECTOR_8FLOATS) {
  int nargs = 1;
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  float scp[16] = {0};
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  data.sc  = (void *)scp;
  float ip[2];
  double exptd[2];

  for (uint32_t i = 0; i < count; i += 8) {
    test_v8s(&data, i);

    for (uint32_t j = 0; j < 8; j++) {
      ip[0] = inpbuff[i + j];

      getExpected(ip,exptd);
      double ulps = getUlp(aop[j], exptd[0]);
      double ulpc = getUlp(scp[j], exptd[1]);
      if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) ||
                     (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
        nfail++;
      }

      if ((vflag == 1) && ((ulps > inData->ulp_threshold) || (ulpc > inData->ulp_threshold))) {
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[0], aop[j], ulps);
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[1], scp[j], ulpc);
      }
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","v8s",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureFloat, ACCURACY_VECTOR_16FLOATS) {
  int nargs = 1;
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  float scp[16] = {0};
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  data.sc  = (void *)scp;
  float ip[2];
  double exptd[2];

  for (uint32_t i = 0; i < count; i += 16) {
    test_v16s(&data, i);

    for (uint32_t j = 0; j < 16; j++) {
      ip[0] = inpbuff[i + j];

      getExpected(ip,exptd);
      double ulps = getUlp(aop[j], exptd[0]);
      double ulpc = getUlp(scp[j], exptd[1]);
      if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) ||
                     (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
        nfail++;
      }

      if ((vflag == 1) && ((ulps > inData->ulp_threshold) || (ulpc > inData->ulp_threshold))) {
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[0], aop[j], ulps);
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[1], scp[j], ulpc);
      }
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","v16s",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureDouble, ACCURACY_SCALAR_DOUBLE) {
  int nargs = 1;
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  double scp[8] = {0};
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  data.sc  = (void *)scp;
  double ip[2];
  long double exptd[2];

  for (uint32_t i = 0; i < count; i++) {
    test_s1d(&data, i);

    ip[0] = inpbuff[i];

      getExpected(ip,exptd);
      double ulps = getUlp(aop[0], exptd[0]);
      double ulpc = getUlp(scp[0], exptd[1]);
      if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) ||
                     (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
        nfail++;
      }

      if ((vflag == 1) && ((ulps > inData->ulp_threshold) || (ulpc > inData->ulp_threshold))) {
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[0], aop[0], ulps);
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[1], scp[0], ulpc);
      }
  }

  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Scalar","Accuracy","s1d",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureDouble, ACCURACY_VECTOR_2DOUBLES) {
  int nargs = 1;
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  double scp[8] = {0};
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  data.sc  = (void *)scp;
  double ip[2];
  long double exptd[2];

  for (uint32_t i = 0; i < count; i += 2) {
    test_v2d(&data, i);

    for (uint32_t j = 0; j < 2; j++) {
      ip[0] = inpbuff[i + j];

      getExpected(ip,exptd);
      double ulps = getUlp(aop[j], exptd[0]);
      double ulpc = getUlp(scp[j], exptd[1]);
      if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) ||
                     (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
        nfail++;
      }

      if ((vflag == 1) && ((ulps > inData->ulp_threshold) || (ulpc > inData->ulp_threshold))) {
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[0], aop[j], ulps);
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[1], scp[j], ulpc);
      }
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","v2d",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureDouble, ACCURACY_VECTOR_4DOUBLES) {
  int nargs = 1;
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  double scp[8] = {0};
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  data.sc  = (void *)scp;
  double ip[2];
  long double exptd[2];

  for (uint32_t i = 0; i < count; i += 4) {
    test_v4d(&data, i);

    for (uint32_t j = 0; j < 4; j++) {
      ip[0] = inpbuff[i + j];

      getExpected(ip,exptd);
      double ulps = getUlp(aop[j], exptd[0]);
      double ulpc = getUlp(scp[j], exptd[1]);
      if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) ||
                     (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
        nfail++;
      }

      if ((vflag == 1) && ((ulps > inData->ulp_threshold) || (ulpc > inData->ulp_threshold))) {
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[0], aop[j], ulps);
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[1], scp[j], ulpc);
      }
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","v4d",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureDouble, ACCURACY_VECTOR_8DOUBLES) {
  int nargs = 1;
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  double scp[8] = {0};
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  data.sc  = (void *)scp;
  double ip[2];
  long double exptd[2];

  for (uint32_t i = 0; i < count; i += 8) {
    test_v8d(&data, i);

    for (uint32_t j = 0; j < 8; j++) {
      ip[0] = inpbuff[i + j];

      getExpected(ip,exptd);
      double ulps = getUlp(aop[j], exptd[0]);
      double ulpc = getUlp(scp[j], exptd[1]);
      if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) ||
                     (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
        nfail++;
      }

      if ((vflag == 1) && ((ulps > inData->ulp_threshold) || (ulpc > inData->ulp_threshold))) {
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[0], aop[j], ulps);
        PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[1], scp[j], ulpc);
      }
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","v8d",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureFloat, ACCURACY_VECTOR_ARRAY_FLOATS) {
  int nargs = 1;
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  std::vector<float> scp(count);
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  data.sc  = (void *)scp.data();
  float ip[2];
  double exptd[2];

  test_vas(&data, count);
  for (uint32_t i = 0; i < count; i++) {

    ip[0] = inpbuff[i];

    getExpected(ip,exptd);
    double ulps = getUlp(aop[i], exptd[0]);
    double ulpc = getUlp(scp[i], exptd[1]);
    if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) ||
                   (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
      nfail++;
    }

    if ((vflag == 1) && ((ulps > inData->ulp_threshold) || (ulpc > inData->ulp_threshold))) {
      PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[0], aop[i], ulps);
      PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[1], scp[i], ulpc);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","vas",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(AccuTestFixtureDouble, ACCURACY_VECTOR_ARRAY_DOUBLES) {
  int nargs = 1;
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  std::vector<double> scp(count);
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  data.sc  = (void *)scp.data();
  double ip[2];
  long double exptd[2];

  test_vad(&data, count);
  for (uint32_t i = 0; i < count; i++) {

    ip[0] = inpbuff[i];

    getExpected(ip,exptd);
    double ulps = getUlp(aop[i], exptd[0]);
    double ulpc = getUlp(scp[i], exptd[1]);
    if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) ||
                   (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
      nfail++;
    }

    if ((vflag == 1) && ((ulps > inData->ulp_threshold) || (ulpc > inData->ulp_threshold))) {
      PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[0], aop[i], ulps);
      PrintUlpResultsFloat(nargs, ip[0], ip[1], exptd[1], scp[i], ulpc);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vector","Accuracy","vad",count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

TEST_P(SpecTestFixtureSinCosF, CONFORMANCE_FLOAT) {
  int nfail = 0;
  test_data data;
  float ip[2];
  float aop[2] = {0};
  double scp[2] = {0};
  data.ip  = (void *)ip;
  data.op  = (void *)&aop[0];
  data.sc  = (void *)&aop[1];
  uint32_t count = sizeof(sincosf_conformance_data) / sizeof(libm_spec_sincosf_data);
  libm_spec_sincosf_data *sincos = sincosf_conformance_data;
  val i32;

  for (uint32_t i = 0; i < count; i++) {
    i32.u = sincos[i].in,
    ip[0] = i32.f;
    feclearexcept (FE_ALL_EXCEPT);
    test_s1s(&data, 0);
    int raised_exception = fetestexcept(FE_ALL_EXCEPT);
    feclearexcept (FE_ALL_EXCEPT);

    i32.u = sincos[i].sin;
    scp[0] = i32.f;
    i32.u = sincos[i].cos;
    scp[1] = i32.f;

    getExpected(ip,scp);
    int eef = (int)sincos[i].exptdexpt;

    SpecTestFixtureSinCosF::ConfVerifyFlt<float,double>(ip[0], aop, scp, raised_exception, eef, &nfail);

    if (vflag == 1) {
        cout << "Input: " << ip[0] << " Output: " << aop[0] << " Expected: " << scp[0] << endl;
        cout << "Input: " << ip[0] << " Output: " << aop[1] << " Expected: " << scp[1] << endl;
        PrintConfExpections(raised_exception, eef);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d",
  "Scalar","Conformance","s1s",count,(count-nfail), nfail);
  ptr->tstcnt++;
}

TEST_P(SpecTestFixtureSinCos, CONFORMANCE_DOUBLE) {
  int nfail = 0;
  test_data data;
  double ip[2];
  double aop[2] = {0};
  long double scp[2] = {0};
  data.ip  = (void *)ip;
  data.op  = (void *)&aop[0];
  data.sc  = (void *)&aop[1];
  uint32_t count = sizeof(sincos_conformance_data) / sizeof(libm_spec_sincos_data);
  libm_spec_sincos_data *sincos = sincos_conformance_data;
  val i64;

  for (uint32_t i = 0; i < count; i++) {
    i64.lu = sincos[i].in,
    ip[0] = i64.d;
    feclearexcept (FE_ALL_EXCEPT);
    test_s1d(&data, 0);
    int raised_exception = fetestexcept(FE_ALL_EXCEPT);
    feclearexcept (FE_ALL_EXCEPT);

    i64.lu = sincos[i].sin;
    scp[0] = i64.d;
    i64.lu = sincos[i].cos;
    scp[1] = i64.d;

    getExpected(ip,scp);
    int eef = (int)sincos[i].exptdexpt;

    SpecTestFixtureSinCos::ConfVerifyDbl<double, long double>(ip[0], aop, scp, raised_exception, eef, &nfail);

    if (vflag == 1) {
      cout << "Input: " << ip[0] << " Output: " << aop[0] << " Expected: " << scp[0] << endl;
      cout << "Input: " << ip[0] << " Output: " << aop[1] << " Expected: " << scp[1] << endl;
      PrintConfExpections(raised_exception, eef);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d",
  "Scalar","Conformance","s1d",count,(count-nfail), nfail);
  ptr->tstcnt++;
}


/*****************************************************************************/
/***                            INSTANTIATE_TEST_SUITE_P                   ***/
/*****************************************************************************/
INSTANTIATE_TEST_SUITE_P(SpecTests, SpecTestFixtureSinCosF,
                         ::testing::ValuesIn(specData));

INSTANTIATE_TEST_SUITE_P(SpecTests, SpecTestFixtureSinCos,
                         ::testing::ValuesIn(specData));
/*****************************************************************************/
