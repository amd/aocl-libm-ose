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

/* Vector Array (vrsa/vrda) In-Place Tests with Memory Boundary Checking for linearfrac */
/*
 * Generic test function for linearfrac in-place operations with 6-argument validation
 *
 * linearfrac is a complex function computing: (a*x + b) / (c*x + d)
 * It requires SIX input parameters arranged as:
 *   - ip[0], ip[1]: First two arguments (x and y - vectorized, varying per element)
 *   - ip[2], ip[3], ip[4], ip[5]: Four coefficients (a, b, c, d - constant across elements)
 *
 * This multi-argument structure requires special handling:
 * - Only ip[0] and ip[1] are stored in arrays that vary per element
 * - ip[2] through ip[5] are scalar constants applied to all elements
 *
 * Validation Strategy:
 * 1. MEMORY SAFETY: Guard zones detect out-of-bounds writes for primary input buffer
 * 2. IN-PLACE EXECUTION: Tests with input == output (first input buffer reused)
 * 3. MULTI-INPUT HANDLING: Properly manages 2 vectorized + 4 scalar inputs
 * 4. EQUIVALENCE VALIDATION: Compares in-place results against normal mode
 * 5. ACCURACY VERIFICATION: ULP error checking against high-precision reference
 *
 * Memory Layout (for first vectorized input):
 *   ┌─────────────────┬──────────────────────┬─────────────────┐
 *   │ GUARD_BEFORE    │   WORKING_DATA       │  GUARD_AFTER    │
 *   │ (ALM_GUARD_     │   (count elements)   │  (ALM_GUARD_    │
 *   │  ZONE_SIZE)     │   ip == op           │   ZONE_SIZE)    │
 *   └─────────────────┴──────────────────────┴─────────────────┘
 *
 * Note: Second vectorized input (inpbuff1) and scalar inputs remain unchanged
 */
template<typename T, typename GuardType, typename ExpectedType, typename TestInput, typename TestPrint>
void TestInPlaceVectorArrayLinearFrac(
    T* inpbuff,               // First input buffer (vectorized, varies per element)
    T* inpbuff1,              // Second input buffer (vectorized, varies per element)
    T* inpbuff2,              // Third input (scalar constant - coefficient a)
    T* inpbuff3,              // Fourth input (scalar constant - coefficient b)
    T* inpbuff4,              // Fifth input (scalar constant - coefficient c)
    T* inpbuff5,              // Sixth input (scalar constant - coefficient d)
    T* aop,                   // Output buffer to store results
    int (*test_func)(test_data*, int),  // Vector array function (vas/vad)
    void (*print_func)(int, T, T, ExpectedType, T, double),  // ULP error printer
    ExpectedType (*get_expected_typed)(T*),  // Computes expected result
    uint32_t count,           // Number of elements
    int nargs,                // Number of arguments (6 for linearfrac)
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
  T ip[6];  // Holds all 6 input parameters for expected result computation

  /* Phase 1: Allocate guarded buffer for first vectorized input (in-place) */
  const uint32_t GUARD_SIZE = ALM_GUARD_ZONE_SIZE;
  uint32_t total_elements = GUARD_SIZE + count + GUARD_SIZE;
  size_t arr_size = total_elements * sizeof(T);
  T *guarded_buff = (T *)malloc(arr_size);

  /* Phase 2: Initialize guard zones and working buffer */
  InitGuardZone(guarded_buff, GUARD_SIZE, guard_pattern);
  T *inplace_buff = guarded_buff + GUARD_SIZE;
  memcpy(inplace_buff, inpbuff, count * sizeof(T));
  InitGuardZone(inplace_buff + count, GUARD_SIZE, guard_pattern);

  /* Phase 3: Execute in-place (ip == op for first vectorized input) */
  data.ip  = (void *)inplace_buff;  // First input (IN-PLACE, overwrites)
  data.op  = (void *)inplace_buff;  // Output (SAME as first input)
  data.ip1 = (void *)inpbuff1;      // Second vectorized input (separate buffer)
  data.ip2 = (void *)inpbuff2;      // Scalar coefficient a
  data.ip3 = (void *)inpbuff3;      // Scalar coefficient b
  data.ip4 = (void *)inpbuff4;      // Scalar coefficient c
  data.ip5 = (void *)inpbuff5;      // Scalar coefficient d
  test_func(&data, count);

  /* Phase 4: Check memory boundaries for first input buffer */
  boundary_violations += CheckGuardZone(guarded_buff, GUARD_SIZE, guard_pattern,
                                        "BEFORE data", vflag);
  boundary_violations += CheckGuardZone(inplace_buff + count, GUARD_SIZE, guard_pattern,
                                        "AFTER data", vflag);

  /* Phase 5: Execute normal mode for comparison */
  data.ip  = (void *)inpbuff;   // Original first input
  data.op  = (void *)aop;       // Separate output buffer
  data.ip1 = (void *)inpbuff1;  // Second vectorized input
  data.ip2 = (void *)inpbuff2;  // Scalar coefficient a
  data.ip3 = (void *)inpbuff3;  // Scalar coefficient b
  data.ip4 = (void *)inpbuff4;  // Scalar coefficient c
  data.ip5 = (void *)inpbuff5;  // Scalar coefficient d
  test_func(&data, count);

  /* Phase 6: Validate results with all 6 input parameters */
  for (uint32_t i = 0; i < count; i++) {
    // Gather all 6 inputs for this element
    ip[0] = inpbuff[i];   // First vectorized input (varies)
    ip[1] = inpbuff1[i];  // Second vectorized input (varies)
    ip[2] = inpbuff2[0];  // Coefficient a
    ip[3] = inpbuff3[0];  // Coefficient b
    ip[4] = inpbuff4[0];  // Coefficient c
    ip[5] = inpbuff5[0];  // Coefficient d

    ExpectedType exptd = get_expected_typed(ip);

    // Verify in-place result matches normal result
    // Handle NaN specially since NaN != NaN by IEEE 754 spec
    bool both_nan = std::isnan(aop[i]) && std::isnan(inplace_buff[i]);
    bool values_match = both_nan || (aop[i] == inplace_buff[i]);
    EXPECT_TRUE(values_match)
      << "In-place result differs from normal at index " << i
      << " (in-place: " << inplace_buff[i] << ", normal: " << aop[i] << ")";

    // Calculate ULP error
    double ulp = getUlp(aop[i], exptd);

    if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
      nfail++;
    }

    if ((vflag == 1) && (ulp > inData->ulp_threshold))
        print_func(nargs, ip[0], ip[1], exptd, aop[i], ulp);
  }

  free(guarded_buff);

  EXPECT_EQ(boundary_violations, 0)
    << "Detected " << boundary_violations
    << " memory boundary violation(s) in " << variant_name
    << " in-place operation!";

  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vec_Array","Accuracy", variant_name, count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

// Wrapper for getExpected with double return type
static double getExpectedLinearFracDouble(float* ip) {
  return getExpected(ip);
}

// Wrapper for getExpected with long double return type
static long double getExpectedLinearFracLongDouble(double* ip) {
  return getExpected(ip);
}

/*
 * TEST: INPLACE_VECTOR_ARRAY_FLOATS (linearfrac variant)
 *
 * Purpose:
 *   Tests vrsa linearfrac in IN-PLACE mode where first input buffer == output buffer.
 *   Validates computation of (a*x + b) / (c*x + d) with 6 input parameters.
 *
 * Memory Safety:
 *   Guard zones protect first input buffer from overruns/underruns.
 *   All 6 inputs (2 vectorized + 4 scalar coefficients) are properly validated.
 *
 * What This Catches:
 *   - Buffer overruns in first input/output buffer
 *   - Incorrect multi-argument handling
 *   - Memory corruption from complex arithmetic operations
 */
TEST_P(AccuTestFixtureFloat, INPLACE_VECTOR_ARRAY_FLOATS) {
  TestInPlaceVectorArrayLinearFrac<float, uint32_t, double, InputData, PrintTstRes>(
    inpbuff,
    inpbuff1,
    inpbuff2,
    inpbuff3,
    inpbuff4,
    inpbuff5,
    aop,
    test_vas,
    PrintUlpResultsFloat,
    getExpectedLinearFracDouble,
    count,
    nargs,
    vflag,
    ALM_GUARD_PATTERN_F32,
    "vas_inplace",
    inData,
    ptr
  );
}

/*
 * TEST: INPLACE_VECTOR_ARRAY_DOUBLES (linearfrac variant)
 *
 * Purpose:
 *   Tests vrda linearfrac in IN-PLACE mode where first input buffer == output buffer.
 *   Validates high-precision computation with 6 double-precision parameters.
 *
 * Memory Safety:
 *   Guard zones protect first input buffer from overruns/underruns.
 *   Comprehensive validation of all inputs and mathematical accuracy.
 */
TEST_P(AccuTestFixtureDouble, INPLACE_VECTOR_ARRAY_DOUBLES) {
  TestInPlaceVectorArrayLinearFrac<double, uint64_t, long double, InputData, PrintTstRes>(
    inpbuff,
    inpbuff1,
    inpbuff2,
    inpbuff3,
    inpbuff4,
    inpbuff5,
    aop,
    test_vad,
    PrintUlpResultsDouble,
    getExpectedLinearFracLongDouble,
    count,
    nargs,
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
INSTANTIATE_TEST_SUITE_P(SpecTests, SpecTestFixtureLinearFracFloatArray,
                         ::testing::ValuesIn(specData));

INSTANTIATE_TEST_SUITE_P(SpecTests, SpecTestFixtureLinearFracDoubleArray,
                         ::testing::ValuesIn(specData));
/*****************************************************************************/
