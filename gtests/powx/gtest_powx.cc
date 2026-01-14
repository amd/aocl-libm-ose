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

TEST_P(AccuTestFixtureFloat, ACCURACY_VECTOR_4FLOATS) {
  int nfail = 0;
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  data.ip  = (void *)inpbuff;
  data.op  = (void *)aop;
  float ip[6];
  data.ip1 = (void *)inpbuff1;

  for (uint32_t i = 0; i < count; i += 4) {
    test_v4s(&data, i);

    for (uint32_t j = 0; j < 4; j++) {
      ip[0] = inpbuff[i + j];
      ip[1] = inpbuff1[i];

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

  for (uint32_t i = 0; i < count; i += 8) {
    test_v8s(&data, i);

    for (uint32_t j = 0; j < 8; j++) {
      ip[0] = inpbuff[i + j];
      ip[1] = inpbuff1[i];

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

  for (uint32_t i = 0; i < count; i += 16) {
    test_v16s(&data, i);

    for (uint32_t j = 0; j < 16; j++) {
      ip[0] = inpbuff[i + j];
      ip[1] = inpbuff1[i];

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

  for (uint32_t i = 0; i < count; i += 2) {
    test_v2d(&data, i);

    for (uint32_t j = 0; j < 2; j++) {
      ip[0] = inpbuff[i + j];
      ip[1] = inpbuff1[i];

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

  for (uint32_t i = 0; i < count; i += 4) {
    test_v4d(&data, i);

    for (uint32_t j = 0; j < 4; j++) {
      ip[0] = inpbuff[i + j];
      ip[1] = inpbuff1[i];

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

  for (uint32_t i = 0; i < count; i += 8) {
    test_v8d(&data, i);

    for (uint32_t j = 0; j < 8; j++) {
      ip[0] = inpbuff[i + j];
      ip[1] = inpbuff1[i];

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
  float ip[2];

  data.ip1 = (void *)inpbuff1;

  test_vas(&data, count);
  for (uint32_t i = 0; i < count; i++)
  {
    ip[0] = inpbuff[i];
    ip[1] = inpbuff1[0];

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
  double ip[2];

  data.ip1 = (void *)inpbuff1;

  test_vad(&data, count);
  for (uint32_t i = 0; i < count; i++)
  {
    ip[0] = inpbuff[i];
    ip[1] = inpbuff1[0];

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


/* Vector Array (vrsa/vrda) In-Place Tests with Memory Boundary Checking */
/*
 * Generic test function that validates in-place vector array operations through
 * comprehensive triple-validation strategy:
 *
 * 1. MEMORY SAFETY: Allocates buffer with guard zones (sentinel-protected memory
 *    before/after data) to detect out-of-bounds writes
 * 2. IN-PLACE EXECUTION: Executes function with input == output (in-place operation)
 * 3. BOUNDARY CHECKING: Verifies guard zones remain intact (detects buffer overruns/underruns)
 * 4. EQUIVALENCE VALIDATION: Executes same function in normal mode (ip != op) and
 *    verifies in-place results match normal results
 * 5. ACCURACY VERIFICATION: Validates mathematical correctness via ULP error checking
 *    against high-precision reference implementation
 *
 * This comprehensive approach ensures:
 * - No memory corruption (safety)
 * - Correctness of in-place optimization (equivalence)
 * - Mathematical accuracy (precision)
 *
 * Template parameters allow reuse for both float (vrsa) and double (vrda) variants
 * with appropriate guard patterns and precision-specific validation functions.
 *
 * Memory Layout:
 *   ┌─────────────────┬──────────────────────┬─────────────────┐
 *   │ GUARD_BEFORE    │   WORKING_DATA       │  GUARD_AFTER    │
 *   │ (ALM_GUARD_     │   (count elements)   │  (ALM_GUARD_    │
 *   │  ZONE_SIZE)     │   ip == op           │   ZONE_SIZE)    │
 *   └─────────────────┴──────────────────────┴─────────────────┘
 *   ^                 ^                      ^
 *   guarded_buff      inplace_buff           inplace_buff+count
 *
 * Sentinel Patterns: ALM_GUARD_PATTERN_F32 (float), ALM_GUARD_PATTERN_F64 (double)
 * Detection: Any corruption in guard zones indicates out-of-bounds write
 */
template<typename T, typename GuardType, typename ExpectedType, typename TestInput, typename TestPrint>
void TestInPlaceVectorArray(
    T* inpbuff,               // Original input buffer (preserved for verification)
    T* inpbuff1,              // Second input buffer for 2-arg functions (optional)
    T* aop,                   // Output buffer to store results
    int (*test_func)(test_data*, int),  // Vector array function to test (vas/vad)
    void (*print_func)(int, T, T, ExpectedType, T, double),  // ULP error printer
    ExpectedType (*get_expected_typed)(T*),  // Computes expected result
    uint32_t count,           // Number of elements to process
    int nargs,                // Number of arguments (1 or 2)
    int vflag,                // Verbosity flag for detailed error reporting
    GuardType guard_pattern,  // Sentinel pattern (ALM_GUARD_PATTERN_F32/ALM_GUARD_PATTERN_F64)
    const char* variant_name, // Test variant name for reporting (e.g., "vas_inplace")
    TestInput* inData,        // Test configuration data
    TestPrint* ptr)           // Test result accumulator
{
  // Counters for test results
  int nfail = 0;              // Count of ULP accuracy failures
  int boundary_violations = 0; // Count of guard zone corruptions
  double max_ulp_err = inData->max_ulp_err;
  test_data data;
  T ip[2];  // Temporary array to hold input values for expected result computation (powx uses 2 args)

  /* ===================================================================
   * PHASE 1: MEMORY ALLOCATION WITH GUARD ZONES
   * =================================================================== */

  // Define guard zone size using standard macro
  // (16 elements = 64 bytes for float, 128 bytes for double)
  // Sufficient to detect AVX-512 overreach (processes up to 16 floats or 8 doubles)
  const uint32_t GUARD_SIZE = ALM_GUARD_ZONE_SIZE;

  // Calculate total buffer size: guard_before + working_data + guard_after
  // Memory Layout: [16 guards][count elements][16 guards]
  uint32_t total_elements = GUARD_SIZE + count + GUARD_SIZE;
  size_t arr_size = total_elements * sizeof(T);

  // Allocate contiguous buffer for guards and working data
  // Using regular malloc (not aligned) to avoid masking alignment-related bugs
  T *guarded_buff = (T *)malloc(arr_size);

  /* ===================================================================
   * PHASE 2: GUARD ZONE INITIALIZATION
   * =================================================================== */

  // Step 1: Fill GUARD_BEFORE region with sentinel pattern
  // This protects against buffer underruns (writes before array start)
  InitGuardZone(guarded_buff, GUARD_SIZE, guard_pattern);

  // Step 2: Calculate working buffer pointer (skips GUARD_BEFORE)
  // This is where actual input/output data resides
  T *inplace_buff = guarded_buff + GUARD_SIZE;

  // Step 3: Copy test input data into working area
  // NOTE: Original 'inpbuff' is preserved because in-place operation will overwrite data
  // We need the original values later for ULP verification
  memcpy(inplace_buff, inpbuff, count * sizeof(T));

  // Step 4: Fill GUARD_AFTER region with sentinel pattern
  // This protects against buffer overruns (writes past array end)
  InitGuardZone(inplace_buff + count, GUARD_SIZE, guard_pattern);

  /* ===================================================================
   * PHASE 3: IN-PLACE OPERATION EXECUTION
   * =================================================================== */

  // Step 5: Configure test_data structure for IN-PLACE execution
  // CRITICAL: Both ip and op point to the SAME buffer location
  data.ip  = (void *)inplace_buff;  // Input pointer
  data.op  = (void *)inplace_buff;  // Output pointer (SAME as input = in-place)
  data.ip1 = (void *)inpbuff1;      // Second input (constant exponent for powx)

  // Step 6: Execute the function under test
  // Function receives count elements and should process ONLY those elements
  // Any write outside [inplace_buff, inplace_buff+count) will corrupt guards
  test_func(&data, count);

  /* ===================================================================
   * PHASE 4: MEMORY INTEGRITY VERIFICATION
   * =================================================================== */

  // Step 7: Check GUARD_BEFORE for corruption
  // If corrupted → function wrote to negative offsets or before array start
  boundary_violations += CheckGuardZone(guarded_buff, GUARD_SIZE, guard_pattern,
                                        "BEFORE data", vflag);

  // Step 8: Check GUARD_AFTER for corruption
  // If corrupted → function wrote past array end (overrun/off-by-one/vector overreach)
  boundary_violations += CheckGuardZone(inplace_buff + count, GUARD_SIZE, guard_pattern,
                                        "AFTER data", vflag);

  /* ===================================================================
   * PHASE 5: FUNCTIONAL ACCURACY VERIFICATION (ULP ERRORS)
   * =================================================================== */

  // Step 9: Run the SAME test in NORMAL mode (ip != op) for comparison
  // This validates that in-place optimization produces identical results
  data.ip  = (void *)inpbuff;  // Input pointer (original data)
  data.op  = (void *)aop;      // Output pointer (separate buffer)
  data.ip1 = (void *)inpbuff1; // Second input (constant exponent)

  // Execute function in normal mode (non-in-place)
  test_func(&data, count);

  // Step 10: Verify mathematical correctness and equivalence
  for (uint32_t i = 0; i < count; i++) {
    // Use ORIGINAL input from 'inpbuff' (not 'inplace_buff' which was overwritten)
    // For powx: ip[0] = base (varies), ip[1] = exponent (constant inpbuff1[0])
    ip[0] = inpbuff[i];
    ip[1] = inpbuff1[0];

    // Compute expected result using high-precision reference implementation
    ExpectedType exptd = get_expected_typed(ip);

    // CRITICAL CHECK: Verify in-place result == normal result
    // Handle NaN specially since NaN != NaN by IEEE 754 spec
    bool both_nan = std::isnan(aop[i]) && std::isnan(inplace_buff[i]);
    bool values_match = both_nan || (aop[i] == inplace_buff[i]);
    EXPECT_TRUE(values_match)
      << "In-place result differs from normal operation at index " << i
      << " (in-place: " << inplace_buff[i] << ", normal: " << aop[i] << ")";

    // Calculate ULP (Units in Last Place) error
    // ULP quantifies floating-point precision: how many representable values apart
    double ulp = getUlp(aop[i], exptd);

    // Check if ULP exceeds threshold (mathematical accuracy failure)
    if(!update_ulp(ulp, max_ulp_err, inData->ulp_threshold)) {
      nfail++;
    }

    // Print detailed error info if verbose mode enabled and threshold exceeded
    if ((vflag == 1) && (ulp > inData->ulp_threshold))
        print_func(nargs, ip[0], ip[1], exptd, aop[i], ulp);
  }

  /* ===================================================================
   * PHASE 6: CLEANUP AND RESULT REPORTING
   * =================================================================== */

  // Free allocated buffer
  free(guarded_buff);

  // Assert no memory violations detected (CRITICAL FAILURE if violated)
  // Memory corruption indicates potential security vulnerability or crash risk
  EXPECT_EQ(boundary_violations, 0)
    << "Detected " << boundary_violations
    << " memory boundary violation(s) in " << variant_name
    << " in-place operation! Function wrote outside allocated buffer.";

  // Format and store test results for summary report
  sprintf(ptr->print[ptr->tstcnt], "%-12s %-12s %-12s %-12d %-12d %-12d %-12g",
  "Vec_Array","Accuracy", variant_name, count,(count-nfail), nfail, max_ulp_err);
  ptr->tstcnt++;
}

// Wrapper for getExpected with double return type
static double getExpectedPowxDouble (float* ip) {
  return getExpected(ip);
}

// Wrapper for getExpected with long double return type
static long double getExpectedPowxLongDouble(double* ip) {
  return getExpected(ip);
}

/*
 * TEST: INPLACE_VECTOR_ARRAY_FLOATS
 *
 * Purpose:
 *   Tests vrsa (vector array single precision) functions in IN-PLACE mode where
 *   input and output buffers are the SAME memory location (ip == op).
 *
 * Memory Safety Validation:
 *   Uses GUARD ZONES (ALM_GUARD_ZONE_SIZE elements before/after) with
 *   ALM_GUARD_PATTERN_F32 sentinel pattern to detect buffer overruns/underruns
 *   and other out-of-bounds writes.
 *
 * What This Catches:
 *   - Buffer overruns (writing past end of array)
 *   - Buffer underruns (writing before start of array)
 *   - Off-by-one errors in loop bounds
 *   - Vector instruction overreach
 */
TEST_P(AccuTestFixtureFloat, INPLACE_VECTOR_ARRAY_FLOATS) {
  TestInPlaceVectorArray<float, uint32_t, double, std::remove_reference_t<decltype(*inData)>, std::remove_reference_t<decltype(*ptr)>>(
    inpbuff,
    inpbuff1,
    aop,
    test_vas,
    PrintUlpResultsFloat,
    getExpectedPowxDouble ,
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
 * TEST: INPLACE_VECTOR_ARRAY_DOUBLES
 *
 * Purpose:
 *   Tests vrda (vector array double precision) functions in IN-PLACE mode where
 *   input and output buffers are the SAME memory location (ip == op).
 *
 * Memory Safety Validation:
 *   Uses GUARD ZONES (ALM_GUARD_ZONE_SIZE elements before/after) with
 *   ALM_GUARD_PATTERN_F64 sentinel pattern to detect buffer overruns/underruns
 *   and other out-of-bounds writes.
 *
 * Example Failure Scenarios:
 *   1. Function writes beyond count elements -> corrupts GUARD_AFTER
 *   2. Function writes negative offsets -> corrupts GUARD_BEFORE
 *   3. Unaligned SIMD access writes extra bytes -> detected in guards
 *   4. Incorrect loop termination -> processes extra elements
 */
TEST_P(AccuTestFixtureDouble, INPLACE_VECTOR_ARRAY_DOUBLES) {
  TestInPlaceVectorArray<double, uint64_t, long double, std::remove_reference_t<decltype(*inData)>, std::remove_reference_t<decltype(*ptr)>>(
    inpbuff,
    inpbuff1,
    aop,
    test_vad,
    PrintUlpResultsDouble,
    getExpectedPowxLongDouble,
    count,
    nargs,
    vflag,
    ALM_GUARD_PATTERN_F64,
    "vad_inplace",
    inData,
    ptr
  );
}