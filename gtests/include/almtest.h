/*
 * Google Test framework for Libm math library
 *
 */
#ifndef __ALMTEST_H__
#define __ALMTEST_H__

#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include "args.h"
#include "gtest.h"
#include "almstruct.h"
#include "defs.h"
#include "random.h"
#include "verify.h"
#include "debug.h"

using namespace std;
using namespace ALM;
using namespace ALMTest;
using namespace Test;

#define is_inf(x) (!isfinite((x)))
#define is_inf_neg(g) (isinf((g)) && (g) < 0.0)
#define is_inf_negf(g) (isinf((g)) && (g) < 0.0f)
#define is_inf_negq(y) (isinfq((y)) && (y) < 0.0)

void SpecSetupf32(SpecParams *);
void SpecSetupf64(SpecParams *);
void ConfSetupf32(SpecParams *);
void ConfSetupf64(SpecParams *);

/*
 * The Function populates the input values based on min value, max value
 * and the type of generation(simple or random or linear)
 */
template <typename T>
int PopulateInputSamples(T **inpbuff, InputRange &range, uint32_t len) {
  Random<T> r = Random<T>(range.type);
  unsigned int arr_size = len * sizeof(T);
  int sz = (arr_size << 1) + _ALIGN_FACTOR;
  #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
    T* buff = (T*)_aligned_malloc(sz, _ALIGN_FACTOR);
  #else
    T *buff = (T *)aligned_alloc(_ALIGN_FACTOR, sz);
  #endif
  if (!range.min && !range.max) {
    #if defined(_WIN64) || defined(_WIN32)
      range.min = (std::numeric_limits<T>::min)();
      range.max = (std::numeric_limits<T>::max)();
    #else
      range.min = std::numeric_limits<T>::min();
      range.max = std::numeric_limits<T>::max();
    #endif
  }
  LIBM_TEST_DPRINTF(DBG2, ,"Input:", buff);

  LIBM_TEST_DPRINTF(DBG2, ,
                  "Testing accuracy for ", len ," items in range [",
                  range.min," ", range.max,"]");

  int res = r.Fill(buff, len, range.min, range.max, range.type);
  *inpbuff = buff;
  return res;
}

double getUlp(float aop, double exptd);
double getUlp(double aop, long double exptd);
bool update_ulp(double ulp, double &, double);

/*
 * The derived class for Accuracy test cases for datatype "float"
 * where all the data members and member functions are
 * declared and defined
 */
class AccuTestFixtureFloat : public ::testing::TestWithParam<AccuParams> {
 public:
  void SetUp() override {
    InputRange *range = (InputRange *)(GetParam().range);
    count = GetParam().count;
    inData = GetParam().inpData;
    vflag = GetParam().verboseflag;
    ptr = GetParam().prttstres;
    nargs = GetParam().nargs;

    PopulateInputSamples(&inpbuff, range[0], count);
    if (nargs == 2)
    PopulateInputSamples(&inpbuff1, range[1], count);
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      aop = (float*)_aligned_malloc(64, _ALIGN_FACTOR);
    #else
      aop = (float *)aligned_alloc(_ALIGN_FACTOR, 64);
    #endif
  }

  void TearDown() override {
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      _aligned_free(inpbuff);
    #else
      free(inpbuff);
    #endif

    inpbuff = nullptr;
    if (nargs == 2) {
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      _aligned_free(inpbuff1);
    #else
      free(inpbuff1);
    #endif

    inpbuff1 = nullptr;
    }
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      _aligned_free(aop);
    #else
      free(aop);
    #endif
    aop = nullptr;
  }

 protected:
  float *inpbuff;
  float *inpbuff1;
  float *aop;
  uint32_t count;
  uint32_t nargs;
  InputData *inData;
  PrintTstRes *ptr;
  int vflag;
};

/*
 * The derived class for Accuracy test cases for datatype "double"
 * where data members and member functions are declared and defined
 */
class AccuTestFixtureDouble : public ::testing::TestWithParam<AccuParams> {
 public:
  void SetUp() override {
    InputRange *range = (InputRange *)  (GetParam().range);
    count = GetParam().count;
    inData = GetParam().inpData;
    vflag = GetParam().verboseflag;
    ptr = GetParam().prttstres;
    nargs = GetParam().nargs;

    PopulateInputSamples(&inpbuff, range[0], count);
    if (nargs == 2)
    PopulateInputSamples(&inpbuff1, range[1], count);
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      aop = (double*)_aligned_malloc(64, _ALIGN_FACTOR);
    #else
      aop = (double *)aligned_alloc(_ALIGN_FACTOR,64);
    #endif
  }

  void TearDown() override {
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      _aligned_free(inpbuff);
    #else
      free(inpbuff);
    #endif

    inpbuff = nullptr;
    if (nargs == 2) {
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      _aligned_free(inpbuff1);
    #else
      free(inpbuff1);
    #endif
    inpbuff1 = nullptr;
    }
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      _aligned_free(aop);
    #else
      free(aop);
    #endif
    aop = nullptr;
  }

 protected:
  double *inpbuff;
  double *inpbuff1;
  double *aop;
  uint32_t count;
  uint32_t nargs;
  InputData *inData;
  PrintTstRes *ptr;
  int vflag;
};

/*
 * The Function reads the input values and expected expections
 * from the given table
 */
template <typename T, typename U>
void SpecialSetUp(T **inp, int **exptdexpt, uint32_t count, U *data,
                   uint32_t nargs, T **inp2, T **op) {
  size_t size = sizeof(T);
  uint32_t arr_size = count * size;
  uint32_t sz = (arr_size << 1) + _ALIGN_FACTOR;
  #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
    int *ee = (int *)_aligned_malloc(sz, _ALIGN_FACTOR);
    T *in = (T *)_aligned_malloc(sz, _ALIGN_FACTOR);
    T *opp = (T *)_aligned_malloc(sz, _ALIGN_FACTOR);
  #else
    int *ee = (int *)aligned_alloc(_ALIGN_FACTOR, sz);
    T *in = (T *)aligned_alloc(_ALIGN_FACTOR, sz);
    T *opp = (T *)aligned_alloc(_ALIGN_FACTOR, sz);
  #endif
  LIBM_TEST_DPRINTF(DBG2, ,"Input:", in);

  LIBM_TEST_DPRINTF(DBG2, ,
                  "Testing conformance/special case for ",count, " items");

  for (uint32_t i = 0; i < count; i++) {
    in[i] = data[i].in;
    ee[i] = data[i].exptdexpt;
    opp[i] = data[i].out;
  }
  *inp  = (T *)in;
  *exptdexpt = (int *)ee;
  *op  = (T *)opp;

  if(nargs == 2) {
    #if ((defined (_WIN64) || defined (_WIN32)) && defined(__clang__))
      T* in2 = (T*)_aligned_malloc(sz, _ALIGN_FACTOR);
    #else
      T *in2 = (T *)aligned_alloc(_ALIGN_FACTOR, sz);
    #endif
    LIBM_TEST_DPRINTF(DBG2, ,"Input1:", in2);
    for (uint32_t i = 0; i < count; i++) {
      in2[i] = data[i].in2;
    }
    *inp2 = (T *)in2;
  }
}

/*
 * The Function that verifies for the special cases
 */
template <typename T>
bool SpecialVerify(T actual, T expected) {
  RealNum myval(actual);
  return myval.Verify(expected);
}

/*
 * The Function that verifies for the conformance cases
 */
template <typename T>
bool ConformanceVerify(T inp, int expected_exception) {
  RealNumConf myval(inp);

  int match = myval.CompQnans(inp[0]);
  int raised_exception = myval.getRaisedException();
  if (!match) {
    if (raised_exception != expected_exception) {
      myval.PrintConformance(inp[0]);
      PrintConfExpections(raised_exception, expected_exception);
      return false;
    }
  }
  return true;
}

/*
 * The derived class for Conformance and special cases for
 * datatype "float" where data members and member functions are
 * declared and defined
 */
class SpecTestFixtureFloat : public ::testing::TestWithParam<SpecParams> {
 public:
  static bool ConfTestVerifyFloat(float *ip, int except, int *nfail) {
    bool flag = ConformanceVerify(ip, except);
    if(!flag)
      (*nfail)++;
    return flag;
  }

  template <typename T>
  bool ConfVerifyFlt(int nargs, T input, T input2, T actual_output, T expected_output, int raised_exception, int expected_exception, int *nfail) {
    int output_match = 0, exception_match = 0;
    /* check if exceptions match */
    if (raised_exception != expected_exception) {
        if (raised_exception < expected_exception)
            exception_match=1;
    }
    val e = {.f = expected_output};
    val a = {.f = actual_output};
    val ip = {.f = input};
    val ip2 = {.f = input2};

    #if defined(_WIN64) || defined(_WIN32)
      bool both_nans = _isnanf(fabsf(e.f)) && _isnanf(fabsf(a.f));
    #else
      bool both_nans = isnanf(fabsf(e.f)) && isnanf(fabsf(a.f));
    #endif

    /* if op and expected dont match, check if ulp error is > 1.0 */
    double ulp = getUlp(a.f, (double)e.f);

    /* if both are nans, output will always match, regardless of the sign bit */
    if (((e.u ^ a.u) && (ulp > 2.0)) && (both_nans == false))
        output_match=1;

    if (output_match==1 || exception_match==1) {
        (*nfail)++;
        printf ("Input: 0x%x (%f) ", ip.u, ip.f);
        if (nargs == 2)
            printf ("Input2: 0x%x (%f) ", ip2.u, ip2.f);
        printf ("Expected: 0x%x (%f) Actual: 0x%x (%f)\n", e.u, e.f, a.u, a.f);
        /* print exceptions */
        PrintConfExpections(raised_exception, expected_exception);
        return false;
    }
    return true;
  }

  static bool SpecialVerifyFloat(float actual, float expected, int *nfail) {
    bool flag = SpecialVerify(actual, expected);
    if(!flag)
      (*nfail)++;
    return flag;
  }

  void SetUp() override {
    libm_test_special_data_f32 *dataf32 = GetParam().data32;
    count = GetParam().countf;
    vflag = GetParam().verboseflag;
    ptr = GetParam().prttstres;
    nargs = GetParam().nargs;

    SpecialSetUp(&idata, &expected_expection, count, dataf32, nargs, &idata1, &iop);
    data = (float *)idata;
    op = (float *)iop;

    if (nargs == 2) {
      data1 = (float *)idata1;
    }
  }

  void TearDown() override {
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      _aligned_free(idata);
      _aligned_free(iop);
    #else
      free(idata);
      free(iop);
    #endif

    idata = nullptr;
    if (nargs == 2) {
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      _aligned_free(idata1);
    #else
      free(idata1);
    #endif

      idata1 = nullptr;
    }
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      _aligned_free(expected_expection);
    #else
      free(expected_expection);
    #endif
    expected_expection = nullptr;
  }

 protected:
  uint32_t *idata, *idata1, *iop;
  uint32_t nargs;
  float *data, *data1, *op;
  int *expected_expection;
  uint32_t count;
  PrintTstRes *ptr;
  int vflag;
};

/*
 * The derived class for Conformance and special cases for
 * datatype "double" where all the data members and member functions are
 * declared and defined
 */
class SpecTestFixtureDouble : public ::testing::TestWithParam<SpecParams> {
 public:
  static bool ConfTestVerifyDouble(double *ip, int except, int *nfail) {
    bool flag = ConformanceVerify(ip, except);
    if(!flag)
      (*nfail)++;
    return flag;
  }

  /* verify double */
  template <typename T>
  bool ConfVerifyDbl(int nargs, T input, T input2, T actual_output, T expected_output, int raised_exception, int expected_exception, int *nfail) {
    int output_match = 0, exception_match = 0;
    /* check if exceptions match */
    if (raised_exception != expected_exception) {
      if (raised_exception < expected_exception)
        exception_match=1;
    }
    val e = {.d = expected_output};
    val a = {.d = actual_output};
    val ip = {.d = input};
    val ip2 = {.d = input2};

    bool both_nans = isnan(fabs(e.d)) && isnan(fabs(a.d));

    /* if op and expected dont match, check if ulp error is > 1.0 */
    double ulp = getUlp(a.d, (long double)e.d);

    /* if both are nans, output will always match, regardless of the sign bit */
    if (((e.lu ^ a.lu) && (ulp > 2.0)) && (both_nans == false))
        output_match=1;

    if (output_match==1 || exception_match==1) {
        (*nfail)++;
        printf ("Input: 0x%lx (%f) ", ip.lu, ip.d);
        if (nargs == 2)
            printf ("Input2: 0x%lx (%f) ", ip2.lu, ip2.d);
        printf ("Expected: 0x%lx (%f) Actual: 0x%lx (%f)\n", e.lu, e.d, a.lu, a.d);
        /* print exceptions */
        PrintConfExpections(raised_exception, expected_exception);
        return false;
    }
    return true;
  }


  static bool SpecialVerifyDouble(double actual, double expected, int *nfail) {
    bool flag = SpecialVerify(actual, expected);
    if(!flag)
      (*nfail)++;
    return flag;
  }

  void SetUp() override {
    libm_test_special_data_f64 *dataf64 = GetParam().data64;
    count = GetParam().countd;
    vflag = GetParam().verboseflag;
    ptr = GetParam().prttstres;
    nargs = GetParam().nargs;

    SpecialSetUp(&idata, &expected_expection, count, dataf64, nargs, &idata1, &iop);
    data = (double *)idata;
    op = (double *) iop;

    if (nargs == 2) {
      data1 = (double *)idata1;
    }
  }

  void TearDown() override {
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      _aligned_free(idata);
      _aligned_free(iop);
    #else
      free(idata);
      free(iop);
    #endif

    idata = nullptr;
    if (nargs == 2) {
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      _aligned_free(idata1);
    #else
      free(idata1);
    #endif
    idata1 = nullptr;
    }
    #if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
      _aligned_free(expected_expection);
    #else
      free(expected_expection);
    #endif
    expected_expection = nullptr;
  }

 protected:
  uint64_t *idata, *idata1, *iop;
  double *data, *data1, *op;
  uint32_t nargs;
  int *expected_expection;
  uint32_t count;
  PrintTstRes *ptr;
  int vflag;
};

class AlmTestFramework {
 private:
  InputParams inpParams;
  InputData inpData;
  PrintTstRes prttstres;

 public:
  ~AlmTestFramework();
  InputData *getInputData() { return &(this->inpData); }
  PrintTstRes *getPrintTetRes() { return &(this->prttstres); }

  int AlmTestType(InputParams *, InputData *, PrintTstRes *);
  void CreateGtestFilters(InputParams *, string &filter_data);
};
#endif
