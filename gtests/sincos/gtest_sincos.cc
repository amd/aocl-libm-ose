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
    if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) && (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
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
      if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) && (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
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
      if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) && (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
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
      if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) && (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
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
      if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) && (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
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

#if 0
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
      if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) && (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
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
#endif

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
      if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) && (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
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
      if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) && (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
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

#if 0
TEST_P(SpecTestFixtureFloat, CONFORMANCE_FLOAT) {
  int nargs = 1;
  int nfail = 0;
  float aop;
  float ip[2];
  float op[2];
  test_data t;
  t.ip  = (void *)data;
  t.op  = (void *)&aop;

  for (uint32_t i = 0; i < count; i++) {
    feclearexcept (FE_ALL_EXCEPT);
    test_s1s(&t, i);
    int raised_exception = fetestexcept(FE_ALL_EXCEPT);
    feclearexcept (FE_ALL_EXCEPT);

    ip[0] = data[i];

    getExpected(ip,(double)&op);
    int eef = expected_expection[i];

    SpecTestFixtureFloat::ConfVerifyFlt(1, ip[0], ip[0], ip[0], ip[0], ip[0], ip[0], aop, op[0], raised_exception, eef, &nfail);

    if (vflag == 1) {
      cout << "Input1: " << ip[0] << " Output: " << aop << " Expected: " << op << endl;
      PrintConfExpections(raised_exception, eef);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d",
  "Scalar","Conformance","s1s",count,(count-nfail), nfail);
  ptr->tstcnt++;
}

TEST_P(SpecTestFixtureDouble, CONFORMANCE_DOUBLE) {
  int nargs = 1;
  int nfail = 0;
  double aop;
  double ip[2];
  double op[2];
  test_data t;
  t.ip  = (void *)data;
  t.op  = (void *)&aop;

  for (uint32_t i = 0; i < count; i++) {
    feclearexcept (FE_ALL_EXCEPT);
    test_s1d(&t, i);
    int raised_exception = fetestexcept(FE_ALL_EXCEPT);
    feclearexcept (FE_ALL_EXCEPT);

    ip[0] = data[i];

    getExpected(ip,(long double)&op);
    int eef = expected_expection[i];

    SpecTestFixtureDouble::ConfVerifyDbl(1, ip[0], ip[0], ip[0], ip[0], ip[0], ip[0], aop, op[0], raised_exception, eef, &nfail);

    if (vflag == 1) {
      cout << "Input1: " << ip[0]  << " Output: " << aop << " Expected: " << op << endl;
      PrintConfExpections(raised_exception, eef);
    }
  }
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d",
  "Scalar","Conformance","s1d",count,(count-nfail), nfail);
  ptr->tstcnt++;
}
#endif