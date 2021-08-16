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
#include "../../include/external/amdlibm.h"
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
  T *buff = (T *)aligned_alloc(_ALIGN_FACTOR, sz);
  if (!range.min && !range.max) {
    range.min = std::numeric_limits<T>::min();
    range.max = std::numeric_limits<T>::max();
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
    
    aop = (float *)aligned_alloc(_ALIGN_FACTOR, 32);
  }

  void TearDown() override {
    free(inpbuff);
    inpbuff = nullptr;
    if (nargs == 2) {
    free(inpbuff1);
    inpbuff1 = nullptr;
    }
    free(aop);
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
    
    aop = (double *)aligned_alloc(_ALIGN_FACTOR,32);    
  }

  void TearDown() override {
    free(inpbuff);
    inpbuff = nullptr;
    if (nargs == 2) {
    free(inpbuff1);
    inpbuff1 = nullptr;
    }
    free(aop);
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
                   uint32_t nargs, T **inp2) {
  size_t size = sizeof(T);
  uint32_t arr_size = count * size;
  uint32_t sz = (arr_size << 1) + _ALIGN_FACTOR;
  int *ee = (int *)aligned_alloc(_ALIGN_FACTOR, sz);
  
  T *in = (T *)aligned_alloc(_ALIGN_FACTOR, sz);
  LIBM_TEST_DPRINTF(DBG2, ,"Input:", in);  
  
  LIBM_TEST_DPRINTF(DBG2, ,
                  "Testing conformance/special case for ",count, " items");
                  
  for (uint32_t i = 0; i < count; i++) {
    in[i] = data[i].in;
    ee[i] = data[i].exptdexpt;
  }
  *inp  = (T *)in;
  *exptdexpt = (int *)ee;
  
  if(nargs == 2) {
    T *in2 = (T *)aligned_alloc(_ALIGN_FACTOR, sz);
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
  bool ConfVerifyFlt(T input, T actual_output, T expected_output, int raised_exception, int expected_exception, int *nfail) {
    int output_match = 0, exception_match = 0;
    /* check if exceptions match */
    if (raised_exception != expected_exception) {
      exception_match=1;
    }
    val e = {.f = expected_output};
    val a = {.f = actual_output};
    val ip = {.f = input};

    bool both_nans = isnanf(fabsf(e.f)) && isnanf(fabsf(a.f));

    /* if both are nans, output will always match, regardless of the sign bit */
    if ((e.u ^ a.u) && (both_nans == false))
        output_match=1;

    if (output_match==1 || exception_match==1) {
        (*nfail)++;
        cout << "Input: " << ip.f << " Expected: " << e.f << " Actual: " << a.f << endl;
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
    
    SpecialSetUp(&idata, &expected_expection, count, dataf32, nargs, &idata1);
    data = (float *)idata;
    if (nargs == 2) {
      data1 = (float *)idata1;
    }
  }

  void TearDown() override {
    free(idata);
    idata = nullptr;
    if (nargs == 2) {
      free(idata1);
      idata1 = nullptr;
    }
    free(expected_expection);
    expected_expection = nullptr;
  }

 protected:
  uint32_t *idata, *idata1;
  uint32_t nargs;
  float *data, *data1;
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
  bool ConfVerifyDbl(T input, T actual_output, T expected_output, int raised_exception, int expected_exception, int *nfail) {
    int output_match = 0, exception_match = 0;
    /* check if exceptions match */
    if (raised_exception != expected_exception) {
      exception_match=1;
    }
    val e = {.d = expected_output};
    val a = {.d = actual_output};
    val ip = {.d = input};

    bool both_nans = isnan(fabs(e.d)) && isnan(fabs(a.d));

    /* if both are nans, output will always match, regardless of the sign bit */
    if ((e.u ^ a.u) && (both_nans == false))
        output_match=1;

    if (output_match==1 || exception_match==1) {
        (*nfail)++;
        cout << "Input: " << ip.d << " Expected: " << e.d << " Actual: " << a.d << endl;
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
    
    SpecialSetUp(&idata, &expected_expection, count, dataf64, nargs, &idata1);
    data = (double *)idata;
    if (nargs == 2) {
      data1 = (double *)idata1;
    }
  }

  void TearDown() override {
    free(idata);
    idata = nullptr;
    if (nargs == 2) {
    free(idata1);
    idata1 = nullptr;
    }
    free(expected_expection);
    expected_expection = nullptr;
  }

 protected:
  uint64_t *idata, *idata1;
  double *data, *data1;
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
