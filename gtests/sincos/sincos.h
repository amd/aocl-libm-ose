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

#ifndef __SINCOS_H__
#define __SINCOS_H__

#include "defs.h"
#include <cstdint>
#include "almstruct.h"
#include "almtest.h"

/*
 * Setup function for sincos conformance tests
 * Handles one input and two outputs (sin and cos)
 */
template <typename T, typename U>
void ConfSetUpSinCos(T **inp, int **exptdexpt, uint32_t count, U *data,
                     T **sin_op, T **cos_op) {
  size_t size = sizeof(T);
  uint32_t arr_size = count * size;

  if((arr_size % _ALIGN_FACTOR) != 0)
  {
    int factor = (arr_size / _ALIGN_FACTOR) + 1;
    arr_size = _ALIGN_FACTOR * factor;
  }

  int* ee = NULL;
  T* in = NULL;
  T* sin_opp = NULL;
  T* cos_opp = NULL;

  aocl_libm_aligned_alloc(arr_size, ee);
  aocl_libm_aligned_alloc(arr_size, in);
  aocl_libm_aligned_alloc(arr_size, sin_opp);
  aocl_libm_aligned_alloc(arr_size, cos_opp);

  LIBM_TEST_DPRINTF(DBG2, ,"Input:", in);

  LIBM_TEST_DPRINTF(DBG2, ,
                  "Testing conformance/special case for ",count, " items");

  for (uint32_t i = 0; i < count; i++) {
    in[i] = data[i].in;
    sin_opp[i] = data[i].sin;
    cos_opp[i] = data[i].cos;
    ee[i] = data[i].exptdexpt;
  }

  *inp  = (T *)in;
  *exptdexpt = (int *)ee;
  *sin_op  = (T *)sin_opp;
  *cos_op  = (T *)cos_opp;
}

/*
 * The derived class for Conformance and special cases for
 * datatype "float" where data members and member functions are
 * declared and defined
 */
class SpecTestFixtureSinCosF : public ::testing::TestWithParam<SpecParams> {
 public:

  template <typename T, typename TL>
  bool ConfVerifyFlt(T input, T *aop, TL *eop, int raised_exception, int expected_exception, int *nfail) {
    int output_match = 0, exception_match = 0;
    /* check if exceptions match */
    if (raised_exception != expected_exception) {
        if (raised_exception < expected_exception)
            exception_match=1;
    }
    float sop = (float )eop[0];
    float cop = (float )eop[1];
    val ip = {.f = input};
    val es = {.f = sop};
    val ec = {.f = cop};
    val as = {.f = aop[0]};
    val ac = {.f = aop[1]};

    #if defined(_WIN64) || defined(_WIN32)
      bool both_nans = ((_isnanf(fabsf(es.f)) && _isnanf(fabsf(as.f))) || (_isnanf(fabsf(ec.f)) && _isnanf(fabsf(ac.f))));
    #else
      bool both_nans = ((isnanf(fabsf(es.f)) && isnanf(fabsf(as.f))) || (isnanf(fabsf(ec.f)) && isnanf(fabsf(ac.f))));
    #endif

    /* if op and expected dont match, check if ulp error is > 1.0 */
    double ulps = getUlp(as.f, (double)es.f);
    double ulpc = getUlp(ac.f, (double)ec.f);

    /* if both are nans, output will always match, regardless of the sign bit */
    if ((((es.u ^ as.u) && (ulps > SCALAR_ULPTHD)) || ((ec.u ^ ac.u) && (ulpc > SCALAR_ULPTHD))) && (both_nans == false))
        output_match=1;

    if (output_match==1 || exception_match==1) {
        (*nfail)++;
        printf ("Input: 0x%x (%f) ", ip.u, ip.f);
        printf ("Expected: 0x%x (%f) Actual: 0x%x (%f) ULP: %f\n", es.u, es.f, as.u, as.f, ulps);
        printf ("                                      ");
        printf ("Expected: 0x%x (%f) Actual: 0x%x (%f) ULP: %f\n", ec.u, ec.f, ac.u, ac.f, ulpc);
        /* print exceptions */
        PrintConfExpections(raised_exception, expected_exception);
        return false;
    }
    return true;
  }

  void SetUp() override {
    libm_spec_sincosf_data *datasc32 = GetParam().datasc32;
    vflag = GetParam().verboseflag;
    ptr  = GetParam().prttstres;
    count = GetParam().countf;
    nargs = GetParam().nargs;

    ConfSetUpSinCos<uint32_t>(&idata, &expected_expection, count, datasc32, &isop, &icop);
    data = (float *)idata;
    sop = (float *)isop;
    cop = (float *)icop;

    unsigned int arr_size = count * sizeof(float);
    if((arr_size % _ALIGN_FACTOR) != 0) {
      int factor = (arr_size / _ALIGN_FACTOR) + 1;
      arr_size = _ALIGN_FACTOR * factor;
    }
  }

  void TearDown() override {
    aocl_libm_aligned_free(idata);
    aocl_libm_aligned_free(expected_expection);
    aocl_libm_aligned_free(isop);
    aocl_libm_aligned_free(icop);
  }

 protected:
  int *expected_expection;
  float  *data, *sop, *cop;
  uint32_t *idata, *isop, *icop;
  uint32_t count;
  PrintTstRes *ptr;
  int vflag;
  uint32_t nargs;
};

class SpecTestFixtureSinCos : public ::testing::TestWithParam<SpecParams> {
 public:

  template <typename T, typename TL>
  bool ConfVerifyDbl(T input, T *aop, TL *eop, int raised_exception, int expected_exception, int *nfail) {
    int output_match = 0, exception_match = 0;
    /* check if exceptions match */
    if (raised_exception != expected_exception) {
        if (raised_exception < expected_exception)
            exception_match=1;
    }
    double sop = (double )eop[0];
    double cop = (double )eop[1];
    val ip = {.d = input};
    val es = {.d = sop};
    val ec = {.d = cop};
    val as = {.d = aop[0]};
    val ac = {.d = aop[1]};

    #if defined(_WIN64) || defined(_WIN32)
      bool both_nans = ((_isnan(fabs(es.d)) && _isnan(fabs(as.d))) || (_isnan(fabs(ec.d)) && _isnan(fabs(ac.d))));
    #else
      bool both_nans = ((isnan(fabs(es.d)) && isnan(fabs(as.d))) || (isnan(fabs(ec.d)) && isnan(fabs(ac.d))));
    #endif

    /* if op and expected dont match, check if ulp error is > 1.0 */
    double ulps = getUlp(as.d, (long double)es.d);
    double ulpc = getUlp(ac.d, (long double)ec.d);

    /* if both are nans, output will always match, regardless of the sign bit */
    if ((((es.lu ^ as.lu) && (ulps > SCALAR_ULPTHD)) || ((ec.lu ^ ac.lu) && (ulpc > SCALAR_ULPTHD))) && (both_nans == false))
        output_match=1;

    if (output_match==1 || exception_match==1) {
        (*nfail)++;
        printf ("Input: 0x%lx (%f) ", ip.lu, ip.d);
        printf ("Expected: 0x%lx (%f) Actual: 0x%lx (%f) ULP: %f\n", es.lu, es.d, as.lu, as.d, ulps);
        printf ("                               ");
        printf ("Expected: 0x%lx (%f) Actual: 0x%lx (%f) ULP: %f\n", ec.lu, ec.d, ac.lu, ac.d, ulpc);
        /* print exceptions */
        PrintConfExpections(raised_exception, expected_exception);
        return false;
    }
    return true;
  }

  void SetUp() override {
    libm_spec_sincos_data *datasc64 = GetParam().datasc64;
    vflag = GetParam().verboseflag;
    ptr  = GetParam().prttstres;
    count = GetParam().countd;
    nargs = GetParam().nargs;

    ConfSetUpSinCos<uint64_t>(&idata, &expected_expection, count, datasc64, &isop, &icop);
    data = (double *)idata;
    sop = (double *)isop;
    cop = (double *)icop;

    unsigned int arr_size = count * sizeof(double);
    if((arr_size % _ALIGN_FACTOR) != 0) {
      int factor = (arr_size / _ALIGN_FACTOR) + 1;
      arr_size = _ALIGN_FACTOR * factor;
    }
  }

  void TearDown() override {
    aocl_libm_aligned_free(idata);
    aocl_libm_aligned_free(expected_expection);
    aocl_libm_aligned_free(isop);
    aocl_libm_aligned_free(icop);
  }

 protected:
  int *expected_expection;
  uint64_t *idata, *isop, *icop;
  double  *data, *sop, *cop;
  uint32_t count;
  PrintTstRes *ptr;
  int vflag;
  uint32_t nargs;
};

class SpecTestFixtureSinCosFloatArray : public ::testing::TestWithParam<SpecParams> {
 public:
  template <typename T, typename TL>
  bool ConfVerifyFlt(T input, T *aop, TL *eop, int *nfail) {
    int output_match = 0;
    float sop = (float )eop[0];
    float cop = (float )eop[1];
    val ip = {.f = input};
    val es = {.f = sop};
    val ec = {.f = cop};
    val as = {.f = aop[0]};
    val ac = {.f = aop[1]};

    #if defined(_WIN64) || defined(_WIN32)
      bool both_nans = ((_isnanf(fabsf(es.f)) && _isnanf(fabsf(as.f))) || (_isnanf(fabsf(ec.f)) && _isnanf(fabsf(ac.f))));
    #else
      bool both_nans = ((isnanf(fabsf(es.f)) && isnanf(fabsf(as.f))) || (isnanf(fabsf(ec.f)) && isnanf(fabsf(ac.f))));
    #endif

    double ulps = getUlp(as.f, (double)es.f);
    double ulpc = getUlp(ac.f, (double)ec.f);

    if ((((es.u ^ as.u) && (ulps > VECTOR_ULPTHD)) || ((ec.u ^ ac.u) && (ulpc > VECTOR_ULPTHD))) && (both_nans == false))
        output_match = 1;

    if (output_match == 1) {
        (*nfail)++;
        printf("Input: 0x%x (%f) ", ip.u, ip.f);
        printf("Expected: 0x%x (%f) Actual: 0x%x (%f) ULP: %f\n", es.u, es.f, as.u, as.f, ulps);
        printf("                                      ");
        printf("Expected: 0x%x (%f) Actual: 0x%x (%f) ULP: %f\n", ec.u, ec.f, ac.u, ac.f, ulpc);
        return false;
    }
    return true;
  }

  void SetUp() override {
    libm_spec_sincosf_data *datasc32 = GetParam().datasc32;
    vflag = GetParam().verboseflag;
    ptr  = GetParam().prttstres;
    count = GetParam().countf;
    nargs = GetParam().nargs;

    ConfSetUpSinCos<uint32_t>(&idata, &expected_expection, count, datasc32, &isop, &icop);
    data = (float *)idata;
    sop = (float *)isop;
    cop = (float *)icop;

    unsigned int arr_size = count * sizeof(float);
    if((arr_size % _ALIGN_FACTOR) != 0) {
      int factor = (arr_size / _ALIGN_FACTOR) + 1;
      arr_size = _ALIGN_FACTOR * factor;
    }
    aocl_libm_aligned_alloc(arr_size, aop_array);
    aocl_libm_aligned_alloc(arr_size, scp_array);
  }

  void TearDown() override {
    aocl_libm_aligned_free(idata);
    aocl_libm_aligned_free(expected_expection);
    aocl_libm_aligned_free(isop);
    aocl_libm_aligned_free(icop);
    aocl_libm_aligned_free(aop_array);
    aocl_libm_aligned_free(scp_array);
  }

 protected:
  int *expected_expection;
  uint32_t nargs;
  uint32_t *idata, *isop, *icop;
  float  *data, *sop, *cop, *aop_array, *scp_array;
  uint32_t count;
  PrintTstRes *ptr;
  int vflag;
};

class SpecTestFixtureSinCosDoubleArray : public ::testing::TestWithParam<SpecParams> {
 public:
  template <typename T, typename TL>
  bool ConfVerifyDbl(T input, T *aop, TL *eop, int *nfail) {
    int output_match = 0;
    double sop = (double )eop[0];
    double cop = (double )eop[1];
    val ip = {.d = input};
    val es = {.d = sop};
    val ec = {.d = cop};
    val as = {.d = aop[0]};
    val ac = {.d = aop[1]};

    #if defined(_WIN64) || defined(_WIN32)
      bool both_nans = ((_isnan(fabs(es.d)) && _isnan(fabs(as.d))) || (_isnan(fabs(ec.d)) && _isnan(fabs(ac.d))));
    #else
      bool both_nans = ((isnan(fabs(es.d)) && isnan(fabs(as.d))) || (isnan(fabs(ec.d)) && isnan(fabs(ac.d))));
    #endif

    double ulps = getUlp(as.d, (long double)es.d);
    double ulpc = getUlp(ac.d, (long double)ec.d);

    if ((((es.lu ^ as.lu) && (ulps > VECTOR_ULPTHD)) || ((ec.lu ^ ac.lu) && (ulpc > VECTOR_ULPTHD))) && (both_nans == false))
        output_match = 1;

    if (output_match == 1) {
        (*nfail)++;
        printf("Input: 0x%lx (%f) ", ip.lu, ip.d);
        printf("Expected: 0x%lx (%f) Actual: 0x%lx (%f) ULP: %f\n", es.lu, es.d, as.lu, as.d, ulps);
        printf("                               ");
        printf("Expected: 0x%lx (%f) Actual: 0x%lx (%f) ULP: %f\n", ec.lu, ec.d, ac.lu, ac.d, ulpc);
        return false;
    }
    return true;
  }

  void SetUp() override {
    libm_spec_sincos_data *datasc64 = GetParam().datasc64;
    vflag = GetParam().verboseflag;
    ptr  = GetParam().prttstres;
    count = GetParam().countd;
    nargs = GetParam().nargs;

    ConfSetUpSinCos<uint64_t>(&idata, &expected_expection, count, datasc64, &isop, &icop);
    data = (double *)idata;
    sop = (double *)isop;
    cop = (double *)icop;

    unsigned int arr_size = count * sizeof(double);
    if((arr_size % _ALIGN_FACTOR) != 0) {
      int factor = (arr_size / _ALIGN_FACTOR) + 1;
      arr_size = _ALIGN_FACTOR * factor;
    }
    aocl_libm_aligned_alloc(arr_size, aop_array);
    aocl_libm_aligned_alloc(arr_size, scp_array);
  }

  void TearDown() override {
    aocl_libm_aligned_free(idata);
    aocl_libm_aligned_free(expected_expection);
    aocl_libm_aligned_free(isop);
    aocl_libm_aligned_free(icop);
    aocl_libm_aligned_free(aop_array);
    aocl_libm_aligned_free(scp_array);
  }

 protected:
  int *expected_expection;
  uint32_t nargs;
  uint64_t *idata, *isop, *icop;
  double  *data, *sop, *cop, *aop_array, *scp_array;
  uint32_t count;
  PrintTstRes *ptr;
  int vflag;
};

#endif  /*__TEST_SINcos_DATA_H__*/