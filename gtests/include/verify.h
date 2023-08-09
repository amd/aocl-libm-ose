/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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


#ifndef __VERIFY_H__
#define __VERIFY_H__

#include <math.h>
#include <fenv.h>
#include <stdbool.h>
#include "defs.h"
#include "callback.h"

using namespace std;
using namespace ALM;

namespace Test {
typedef union {
  float f;
  double d;
  uint32_t u;
  uint64_t lu;
} val;

class RealNum {
 protected:
  FloatType m_ftype;
  FloatQuantity m_fqty;
  FloatWidth m_fwidth;
  val a;

 public:
  explicit RealNum(float f) {
    a = {.f = f};
    m_ftype = ALM::FloatType::E_Real;
    m_fqty = ALM::FloatQuantity::E_Scalar;
    m_fwidth = ALM::FloatWidth::E_F32;
  }

  explicit RealNum(double d) {
    a = {.d = d};
    m_ftype = ALM::FloatType::E_Real;
    m_fqty = ALM::FloatQuantity::E_Scalar;
    m_fwidth = ALM::FloatWidth::E_F64;
  }

  bool Verify(float expected);

  bool Verify(double expected);
};

class RealNumConf {
 private:
  val a;
  val e;
  int re;

 public:
  explicit RealNumConf(float *ip) {
    re = feclearexcept(FE_ALL_EXCEPT);
    float af = getFuncOp(ip);
    re = fetestexcept(FE_ALL_EXCEPT);
    float ef = getExpected(ip);
    a = {.f = af};
    e = {.f = ef};
  }

  explicit RealNumConf(double *ip) {
    re = feclearexcept(FE_ALL_EXCEPT);
    double ad = getFuncOp(ip);
    re = fetestexcept(FE_ALL_EXCEPT);
    double ed = getGlibcOp(ip);
    a = {.d = ad};
    e = {.d = ed};
  }

  int getRaisedException(void);

  uint32_t ToUnsignedInt(float temp);

  uint64_t ToUnsignedInt(double temp);

  int CompQnans(float inp);

  int CompQnans(double inp);

  void PrintConformance(float inp);

  void PrintConformance(double inp);
};

void PrintConfExpections(int raised_exception, int expected_exception);

void PrintUlpResultsFloat(int nargs, float input1, float input2, double expected, float actual, double ulp);
void PrintUlpResultsDouble(int nargs, double input1, double input2, long double expected, double actual, double ulp);

}  // namespace Test
#endif
