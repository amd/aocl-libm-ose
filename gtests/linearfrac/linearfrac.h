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

#ifndef __LINEARFRAC_H__
#define __LINEARFRAC_H__

#include "defs.h"
#include <cstdint>
#include "almstruct.h"
#include "almtest.h"

/*
 * Setup function for linearfrac conformance tests
 * Handles 6 inputs and 1 output
 * linearfrac(x, y, scale_x, shift_x, scale_y, shift_y) = (x * scale_x + shift_x) / (y * scale_y + shift_y)
 */
template <typename T, typename U>
void ConfSetUpLinearFrac(T **inp1, T **inp2, T **inp3, T **inp4, T **inp5, T **inp6, uint32_t count, U *data) {
  size_t size = sizeof(T);
  uint32_t arr_size = count * size;

  if((arr_size % _ALIGN_FACTOR) != 0)
  {
    int factor = (arr_size / _ALIGN_FACTOR) + 1;
    arr_size = _ALIGN_FACTOR * factor;
  }

  T* in1 = NULL;
  T* in2 = NULL;
  T* in3 = NULL;
  T* in4 = NULL;
  T* in5 = NULL;
  T* in6 = NULL;

  aocl_libm_aligned_alloc(arr_size, in1);
  aocl_libm_aligned_alloc(arr_size, in2);
  aocl_libm_aligned_alloc(arr_size, in3);
  aocl_libm_aligned_alloc(arr_size, in4);
  aocl_libm_aligned_alloc(arr_size, in5);
  aocl_libm_aligned_alloc(arr_size, in6);

  LIBM_TEST_DPRINTF(DBG2, ,
                  "Testing conformance/special case for ",count, " items");

  for (uint32_t i = 0; i < count; i++) {
    in1[i] = data[i].in;
    in2[i] = data[i].in2;
    in3[i] = data[i].in3;
    in4[i] = data[i].in4;
    in5[i] = data[i].in5;
    in6[i] = data[i].in6;
  }

  *inp1  = (T *)in1;
  *inp2  = (T *)in2;
  *inp3  = (T *)in3;
  *inp4  = (T *)in4;
  *inp5  = (T *)in5;
  *inp6  = (T *)in6;
}

/*
 * The derived class for Conformance and special cases for
 * datatype "float" array variants (vrsa_*) where data members
 * and member functions are declared and defined
 */
class SpecTestFixtureLinearFracFloatArray : public ::testing::TestWithParam<SpecParams> {
 public:
  template <typename T>
  bool ConfVerifyFlt(T input1, T input2, T input3, T input4, T input5, T input6,
                     T actual_output, double expected_output, int *nfail) {
    int output_match = 0;

    val ip1 = {.f = input1};
    val ip2 = {.f = input2};
    val ip3 = {.f = input3};
    val ip4 = {.f = input4};
    val ip5 = {.f = input5};
    val ip6 = {.f = input6};
    val e = {.f = (float)expected_output};
    val a = {.f = actual_output};

    #if defined(_WIN64) || defined(_WIN32)
      bool both_nans = _isnanf(fabsf(e.f)) && _isnanf(fabsf(a.f));
    #else
      bool both_nans = isnanf(fabsf(e.f)) && isnanf(fabsf(a.f));
    #endif

    double ulp = getUlp(a.f, (double)e.f);

    if (((e.u ^ a.u) && (ulp > VECTOR_ULPTHD)) && (both_nans == false))
        output_match = 1;

    if (output_match == 1) {
        (*nfail)++;
        printf("Input1: 0x%x (%f) Input2: 0x%x (%f) ", ip1.u, ip1.f, ip2.u, ip2.f);
        printf("Input3: 0x%x (%f) Input4: 0x%x (%f) ", ip3.u, ip3.f, ip4.u, ip4.f);
        printf("Input5: 0x%x (%f) Input6: 0x%x (%f)\n", ip5.u, ip5.f, ip6.u, ip6.f);
        printf("Expected: 0x%x (%f) Actual: 0x%x (%f) ULP: %f\n", e.u, e.f, a.u, a.f, ulp);
        return false;
    }
    return true;
  }

  void SetUp() override {
    libm_test_special_data_f32 *dataf32 = GetParam().data32;
    vflag = GetParam().verboseflag;
    ptr  = GetParam().prttstres;
    count = GetParam().countf;
    nargs = GetParam().nargs;

    ConfSetUpLinearFrac<float>(&data1, &data2, &data3, &data4, &data5, &data6, count, dataf32);

    unsigned int arr_size = count * sizeof(float);
    if((arr_size % _ALIGN_FACTOR) != 0) {
      int factor = (arr_size / _ALIGN_FACTOR) + 1;
      arr_size = _ALIGN_FACTOR * factor;
    }
    aocl_libm_aligned_alloc(arr_size, aop_array);
  }

  void TearDown() override {
    aocl_libm_aligned_free(data1);
    aocl_libm_aligned_free(data2);
    aocl_libm_aligned_free(data3);
    aocl_libm_aligned_free(data4);
    aocl_libm_aligned_free(data5);
    aocl_libm_aligned_free(data6);
    aocl_libm_aligned_free(aop_array);
  }

 protected:
  int *expected_expection;
  uint32_t nargs;
  float *data1, *data2, *data3, *data4, *data5, *data6;
  float *expected_op, *aop_array;
  uint32_t count;
  PrintTstRes *ptr;
  int vflag;
};

/*
 * The derived class for Conformance and special cases for
 * datatype "double" array variants (vrda_*) where data members
 * and member functions are declared and defined
 */
class SpecTestFixtureLinearFracDoubleArray : public ::testing::TestWithParam<SpecParams> {
 public:
  template <typename T>
  bool ConfVerifyDbl(T input1, T input2, T input3, T input4, T input5, T input6,
                     T actual_output, long double expected_output, int *nfail) {
    int output_match = 0;

    val ip1 = {.d = input1};
    val ip2 = {.d = input2};
    val ip3 = {.d = input3};
    val ip4 = {.d = input4};
    val ip5 = {.d = input5};
    val ip6 = {.d = input6};
    val e = {.d = (double)expected_output};
    val a = {.d = actual_output};

    #if defined(_WIN64) || defined(_WIN32)
      bool both_nans = _isnan(fabs(e.d)) && _isnan(fabs(a.d));
    #else
      bool both_nans = isnan(fabs(e.d)) && isnan(fabs(a.d));
    #endif

    double ulp = getUlp(a.d, (long double)e.d);

    if (((e.lu ^ a.lu) && (ulp > VECTOR_ULPTHD)) && (both_nans == false))
        output_match = 1;

    if (output_match == 1) {
        (*nfail)++;
        printf("Input1: 0x%lx (%lf) Input2: 0x%lx (%lf) ", ip1.lu, ip1.d, ip2.lu, ip2.d);
        printf("Input3: 0x%lx (%lf) Input4: 0x%lx (%lf) ", ip3.lu, ip3.d, ip4.lu, ip4.d);
        printf("Input5: 0x%lx (%lf) Input6: 0x%lx (%lf)\n", ip5.lu, ip5.d, ip6.lu, ip6.d);
        printf("Expected: 0x%lx (%lf) Actual: 0x%lx (%lf) ULP: %lf\n", e.lu, e.d, a.lu, a.d, ulp);
        return false;
    }
    return true;
  }

  void SetUp() override {
    libm_test_special_data_f64 *dataf64 = GetParam().data64;
    vflag = GetParam().verboseflag;
    ptr  = GetParam().prttstres;
    count = GetParam().countd;
    nargs = GetParam().nargs;

    ConfSetUpLinearFrac<double>(&data1, &data2, &data3, &data4, &data5, &data6, count, dataf64);

    unsigned int arr_size = count * sizeof(double);
    if((arr_size % _ALIGN_FACTOR) != 0) {
      int factor = (arr_size / _ALIGN_FACTOR) + 1;
      arr_size = _ALIGN_FACTOR * factor;
    }
    aocl_libm_aligned_alloc(arr_size, aop_array);
  }

  void TearDown() override {
    aocl_libm_aligned_free(data1);
    aocl_libm_aligned_free(data2);
    aocl_libm_aligned_free(data3);
    aocl_libm_aligned_free(data4);
    aocl_libm_aligned_free(data5);
    aocl_libm_aligned_free(data6);
    aocl_libm_aligned_free(aop_array);
  }

 protected:
  uint32_t nargs;
  double *data1, *data2, *data3, *data4, *data5, *data6;
  double *expected_op, *aop_array;
  uint32_t count;
  PrintTstRes *ptr;
  int vflag;
};

#endif  /*__LINEARFRAC_H__*/
