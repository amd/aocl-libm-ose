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
  test_data tdata;
  float ip[2];
  float aop[2] = {0};
  double scp[2] = {0};
  tdata.ip  = (void *)data;
  tdata.op  = (void *)&aop[0];
  tdata.sc  = (void *)&aop[1];

  for (uint32_t i = 0; i < count; i++) {
    feclearexcept (FE_ALL_EXCEPT);
    ip[0] = data[i];
    test_s1s(&tdata, i);
    int raised_exception = fetestexcept(FE_ALL_EXCEPT);
    feclearexcept (FE_ALL_EXCEPT);

    getExpected(ip,scp);
    int eef = expected_expection[i];

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
  test_data tdata;
  double ip[2];
  double aop[2] = {0};
  long double scp[2] = {0};
  tdata.ip  = (void *)data;
  tdata.op  = (void *)&aop[0];
  tdata.sc  = (void *)&aop[1];

  for (uint32_t i = 0; i < count; i++) {
    feclearexcept (FE_ALL_EXCEPT);
    ip[0] = data[i];
    test_s1d(&tdata, i);
    int raised_exception = fetestexcept(FE_ALL_EXCEPT);
    feclearexcept (FE_ALL_EXCEPT);

    getExpected(ip,scp);
    int eef = expected_expection[i];

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

TEST_P(SpecTestFixtureSinCosFloatArray, CONFORMANCE_VECTOR_ARRAY_FLOATS) {
  int nfail = 0;
  float ip[2];
  test_data tdata;
  float aop_temp[2];
  double expected_op[2];
  tdata.ip  = (void *)data;
  tdata.op  = (void *)aop_array;
  tdata.sc  = (void *)scp_array;

  test_vas(&tdata, count);
  for (uint32_t i = 0; i < count; i++) {
    ip[0] = data[i];

    getExpected(ip, expected_op);

    aop_temp[0] = aop_array[i];
    aop_temp[1] = scp_array[i];

    ConfVerifyFlt<float, double>(ip[0], aop_temp, expected_op, &nfail);

  }

  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d",
  "VecArr","Conformance","vrsa",count,(count-nfail), nfail);
  ptr->tstcnt++;
}

TEST_P(SpecTestFixtureSinCosDoubleArray, CONFORMANCE_VECTOR_ARRAY_DOUBLES) {
  int nfail = 0;
  double ip[2];
  test_data tdata;
  double aop_temp[2];
  long double expected_op[2];
  tdata.ip  = (void *)data;
  tdata.op  = (void *)aop_array;
  tdata.sc  = (void *)scp_array;

  test_vad(&tdata, count);
  for (uint32_t i = 0; i < count; i++) {
    ip[0] = data[i];

    getExpected(ip, expected_op);

    aop_temp[0] = aop_array[i];
    aop_temp[1] = scp_array[i];

    ConfVerifyDbl<double, long double>(ip[0], aop_temp, expected_op, &nfail);

  }

  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d",
  "VecArr","Conformance","vrda",count,(count-nfail), nfail);
  ptr->tstcnt++;
}

/* Vector Array (vrsa/vrda) In-Place Tests with Memory Boundary Checking for sincos */
/*
 * Generic test function for sincos in-place operations with dual output validation
 *
 * sincos is unique as it computes TWO outputs (sin and cos) from a single input.
 * This requires validating BOTH outputs in in-place mode with proper memory safety.
 *
 * Validation Strategy:
 * 1. MEMORY SAFETY: Guard zones detect out-of-bounds writes for BOTH sin and cos buffers
 * 2. IN-PLACE EXECUTION: Tests with input == output for primary result (sin)
 * 3. DUAL OUTPUT CHECKING: Validates both sin (aop) and cos (scp) outputs
 * 4. EQUIVALENCE VALIDATION: Compares in-place results against normal mode
 * 5. ACCURACY VERIFICATION: ULP error checking for both sin and cos results
 *
 * Memory Layout (for primary sin output):
 *   ┌─────────────────┬──────────────────────┬─────────────────┐
 *   │ GUARD_BEFORE    │   WORKING_DATA       │  GUARD_AFTER    │
 *   │ (ALM_GUARD_     │   (count elements)   │  (ALM_GUARD_    │
 *   │  ZONE_SIZE)     │   ip == op (sin)     │   ZONE_SIZE)    │
 *   └─────────────────┴──────────────────────┴─────────────────┘
 *
 * Note: cos output (scp) uses separate buffer (not in-place)
 */
template<typename T, typename GuardType, typename ExpectedType, typename TestInput, typename TestPrint>
void TestInPlaceVectorArraySinCos(
    T* inpbuff,               // Original input buffer
    T* sin,                   // Output buffer for sin results
    T* cos,                   // Output buffer for cos results (cosine output array)
    int (*test_func)(test_data*, int),  // Vector array function (vas/vad)
    void (*print_func)(int, T, T, ExpectedType, T, double),  // ULP error printer
    void (*get_expected_typed)(T*, ExpectedType*),  // Computes expected [sin, cos] results
    uint32_t count,           // Number of elements
    int nargs,                // Number of arguments (1 for sincos)
    int vflag,                // Verbosity flag
    GuardType guard_pattern,  // Sentinel pattern
    const char* variant_name, // Test variant name
    TestInput* inData,        // Test configuration
    TestPrint* ptr)           // Test result accumulator
{
  int nfail = 0;
  int boundary_violations = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  T ip[2];
  ExpectedType exptd[2];  // [sin_expected, cos_expected]

  /* Phase 1: Allocate guarded buffers for both sin and cos in-place testing */
  const uint32_t GUARD_SIZE = ALM_GUARD_ZONE_SIZE;
  uint32_t total_elements = GUARD_SIZE + count + GUARD_SIZE;
  size_t arr_size = total_elements * sizeof(T);

  // Guarded buffer for sin in-place test
  T *guarded_buff_sin = (T *)malloc(arr_size);
  // Guarded buffer for cos in-place test
  T *guarded_buff_cos = (T *)malloc(arr_size);

  /* Phase 2: Initialize guard zones and working buffers */
  // Sin in-place buffer setup
  InitGuardZone(guarded_buff_sin, GUARD_SIZE, guard_pattern);
  T *inplace_sin_buff = guarded_buff_sin + GUARD_SIZE;
  memcpy(inplace_sin_buff, inpbuff, count * sizeof(T));
  InitGuardZone(inplace_sin_buff + count, GUARD_SIZE, guard_pattern);

  // Cos in-place buffer setup
  InitGuardZone(guarded_buff_cos, GUARD_SIZE, guard_pattern);
  T *inplace_cos_buff = guarded_buff_cos + GUARD_SIZE;
  memcpy(inplace_cos_buff, inpbuff, count * sizeof(T));
  InitGuardZone(inplace_cos_buff + count, GUARD_SIZE, guard_pattern);

  // Allocate separate output buffers for non-in-place outputs
  std::vector<T> cos_buff(count);  // For cos when sin is in-place
  std::vector<T> sin_buff(count);  // For sin when cos is in-place

  /* Phase 3a: Execute in-place (ip == op for sin output) */
  data.ip  = (void *)inplace_sin_buff;     // Input
  data.op  = (void *)inplace_sin_buff;     // Sin output (IN-PLACE, overwrites input)
  data.sc  = (void *)cos_buff.data();      // Cos output (separate buffer)
  test_func(&data, count);                 // Sin in-place

  /* Phase 3b: Execute in-place (ip == sc for cos output) */
  data.ip  = (void *)inplace_cos_buff;     // Input
  data.op  = (void *)sin_buff.data();      // Sin output (separate buffer)
  data.sc  = (void *)inplace_cos_buff;     // Cos output (IN-PLACE, overwrites input)
  test_func(&data, count);                 // Cos in-place

  /* Phase 4: Check memory boundaries for both in-place buffers */
  // Check sin in-place buffer boundaries
  boundary_violations += CheckGuardZone(guarded_buff_sin, GUARD_SIZE, guard_pattern,
                                        "BEFORE sin data", vflag);
  boundary_violations += CheckGuardZone(inplace_sin_buff + count, GUARD_SIZE, guard_pattern,
                                        "AFTER sin data", vflag);

  // Check cos in-place buffer boundaries
  boundary_violations += CheckGuardZone(guarded_buff_cos, GUARD_SIZE, guard_pattern,
                                        "BEFORE cos data", vflag);
  boundary_violations += CheckGuardZone(inplace_cos_buff + count, GUARD_SIZE, guard_pattern,
                                        "AFTER cos data", vflag);

  /* Phase 5: Execute normal mode for comparison */
  data.ip  = (void *)inpbuff;
  data.op  = (void *)sin;
  data.sc  = (void *)cos;
  test_func(&data, count);                // Normal mode

  /* Phase 6: Validate both sin and cos outputs */
  for (uint32_t i = 0; i < count; i++) {
    ip[0] = inpbuff[i];

    get_expected_typed(ip, exptd);

    // Verify sin results match (from sin in-place test)
    // Handle NaN specially since NaN != NaN by IEEE 754 spec
    bool both_nan_sin = std::isnan(sin[i]) && std::isnan(inplace_sin_buff[i]);
    bool sin_match = both_nan_sin || (sin[i] == inplace_sin_buff[i]);
    EXPECT_TRUE(sin_match)
      << "In-place sin result differs from normal at index " << i
      << " (in-place: " << inplace_sin_buff[i] << ", normal: " << sin[i] << ")";

    // Verify cos results match (from cos in-place test)
    bool both_nan_cos = std::isnan(cos[i]) && std::isnan(inplace_cos_buff[i]);
    bool cos_match = both_nan_cos || (cos[i] == inplace_cos_buff[i]);
    EXPECT_TRUE(cos_match)
      << "In-place cos result differs from normal at index " << i
      << " (in-place: " << inplace_cos_buff[i] << ", normal: " << cos[i] << ")";

    // Calculate ULP errors for both sin and cos
    double ulps = getUlp(sin[i], exptd[0]);  // sin ULP
    double ulpc = getUlp(cos[i], exptd[1]);  // cos ULP

    if((!update_ulp(ulps, max_ulp_err, inData->ulp_threshold)) ||
       (!update_ulp(ulpc, max_ulp_err, inData->ulp_threshold))) {
      nfail++;
    }

    if ((vflag == 1) && ((ulps > inData->ulp_threshold) || (ulpc > inData->ulp_threshold))) {
      print_func(nargs, ip[0], ip[1], exptd[0], sin[i], ulps);
      print_func(nargs, ip[0], ip[1], exptd[1], cos[i], ulpc);
    }
  }

  free(guarded_buff_sin);
  free(guarded_buff_cos);

  EXPECT_EQ(boundary_violations, 0)
    << "Detected " << boundary_violations
    << " memory boundary violation(s) in " << variant_name
    << " in-place operation!";

  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vec_Array","Accuracy", variant_name, count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

// Wrapper for getExpected (sincos returns two values)
static void getExpectedSinCosDouble(float* ip, double* exptd) {
  getExpected(ip, exptd);
}

static void getExpectedSinCosLongDouble(double* ip, long double* exptd) {
  getExpected(ip, exptd);
}

/*
 * TEST: INPLACE_VECTOR_ARRAY_FLOATS (sincos variant)
 *
 * Purpose:
 *   Tests vrsa sincos in IN-PLACE mode where sin output buffer == input buffer.
 *   The cos output uses a separate buffer (sincos produces 2 outputs).
 *
 * Memory Safety:
 *   Guard zones protect sin buffer from overruns/underruns.
 *   Both sin and cos results are validated for correctness.
 *
 * What This Catches:
 *   - Buffer overruns in sin output
 *   - Incorrect sin/cos result separation
 *   - Memory corruption from vector operations
 */
TEST_P(AccuTestFixtureFloat, INPLACE_VECTOR_ARRAY_FLOATS) {
  std::vector<float> scp_temp(count);
  TestInPlaceVectorArraySinCos<float, uint32_t, double, InputData, PrintTstRes>(
    inpbuff,
    aop,
    scp_temp.data(),
    test_vas,
    PrintUlpResultsFloat,
    getExpectedSinCosDouble,
    count,
    1,  // nargs = 1 for sincos
    vflag,
    ALM_GUARD_PATTERN_F32,
    "vas_inplace",
    inData,
    ptr
  );
}

/*
 * TEST: INPLACE_VECTOR_ARRAY_DOUBLES (sincos variant)
 *
 * Purpose:
 *   Tests vrda sincos in IN-PLACE mode where sin output buffer == input buffer.
 *   The cos output uses a separate buffer.
 *
 * Memory Safety:
 *   Guard zones protect sin buffer from overruns/underruns.
 *   Both sin and cos results validated against high-precision reference.
 */
TEST_P(AccuTestFixtureDouble, INPLACE_VECTOR_ARRAY_DOUBLES) {
  std::vector<double> scp_temp(count);
  TestInPlaceVectorArraySinCos<double, uint64_t, long double, InputData, PrintTstRes>(
    inpbuff,
    aop,
    scp_temp.data(),
    test_vad,
    PrintUlpResultsDouble,
    getExpectedSinCosLongDouble,
    count,
    1,  // nargs = 1 for sincos
    vflag,
    ALM_GUARD_PATTERN_F64,
    "vad_inplace",
    inData,
    ptr
  );
}

/*****************************************************************************/
/***                            INSTANTIATE_TEST_SUITE_P                   ***/
/*****************************************************************************/
INSTANTIATE_TEST_SUITE_P(SpecTests, SpecTestFixtureSinCosF,
                         ::testing::ValuesIn(specData));

INSTANTIATE_TEST_SUITE_P(SpecTests, SpecTestFixtureSinCos,
                         ::testing::ValuesIn(specData));

INSTANTIATE_TEST_SUITE_P(SpecTests, SpecTestFixtureSinCosFloatArray,
                         ::testing::ValuesIn(specData));

INSTANTIATE_TEST_SUITE_P(SpecTests, SpecTestFixtureSinCosDoubleArray,
                         ::testing::ValuesIn(specData));
/*****************************************************************************/
