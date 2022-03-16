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

 
#include <float.h>
#include <string>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "verify.h"
#include "libm_util_amd.h"

using namespace std;
using namespace ALM;
using namespace Test;

namespace Test {

int RealNumConf::getRaisedException(void) { return re; }

uint32_t RealNumConf::ToUnsignedInt(float inp) {
  val a = {.f = inp};
  return a.u;
}

uint64_t RealNumConf::ToUnsignedInt(double inp) {
  val a = {.d = inp};
  return a.lu;
}

int RealNumConf::CompQnans(float inp) {
  if ((a.u & QNANBITPATT_SP32) == (e.u & QNANBITPATT_SP32)) {
    return 1;
  }
  return 0;
}

int RealNumConf::CompQnans(double inp) {
  if ((a.lu & QNANBITPATT_DP64) == (e.lu & QNANBITPATT_DP64)) {
    return 1;
  }
  return 0;
}

void RealNumConf::PrintConformance(float inp) {
  val i = {.f = inp};
  cout << "Input:" << hex << i.u << "Expected:" << hex << e.u
       << "Output:" << hex << a.u << endl;
}

void RealNumConf::PrintConformance(double inp) {
  val i = {.d = inp};
  cout << "Input:" << hex << i.lu << "Expected:" << hex << e.lu
       << "Output:" << hex << a.lu << endl;
}

bool RealNum::Verify(float expected) {
  val e = {.f = expected};
  if ((a.u ^ e.u) != 0) {
    cout << "Expected:" << hex << e.u << "Output:" << hex << a.u << endl;
    return false;
  }
  return true;
}

bool RealNum::Verify(double expected) {
  val e = {.d = expected};
  if ((a.lu ^ e.lu) != 0) {
    cout << "Expected:" << hex << e.lu << "Output:" << hex << a.lu << endl;
    return false;
  }
  return true;
}

static void print_errors(const int flags) {
  if (flags & FE_INVALID) {
    printf("FE_INVALID ");
  }

  if (flags & FE_DIVBYZERO) {
    printf("FE_DIVBYZERO ");
  }

  if (flags & FE_OVERFLOW) {
    printf("FE_OVERFLOW ");
  }

  if (flags & FE_UNDERFLOW) {
    printf("FE_UNDERFLOW ");
  }

  if (flags & FE_INEXACT) {
    printf("FE_INEXACT ");
  }

  if (flags == 0) {
    printf("-NONE-");
  }
}

void PrintConfExpections(int raised_exception, int expected_exception) {
  printf("Raised exception: ");
  print_errors(raised_exception);
  printf(" Expected exception: ");
  print_errors(expected_exception);
  puts("");
}

void PrintUlpResultsFloat(int nargs, float input1, float input2, double expected, float actual, double ulp) {
  val val_ip1, val_ip2, val_exptd, val_aop;
  val_ip1.f = input1;
  val_exptd.d = expected;
  val_aop.f = actual;
  printf ("Input1: %f (0x%" PRIx32 ") ", input1, val_ip1.u);
  if (nargs == 2) {
    val_ip2.f = input2;
    printf ("Input2: %f (0x%" PRIx32 ") ", input2, val_ip2.u);
  }
  printf ("Expected: %f (0x%" PRIx64 ") ", expected, val_exptd.lu);
  printf ("Actual: %f (0x%" PRIx32 ") ", actual, val_aop.u);
  printf ("ULP: %f\n", ulp);
}

void PrintUlpResultsDouble(int nargs, double input1, double input2, long double expected, double actual, double ulp) {
  val val_ip1, val_ip2, val_exptd, val_aop;
  val_ip1.d = input1;
  val_exptd.d = expected;
  val_aop.d = actual;
  printf ("Input1: %f (0x%" PRIx64 ") ", input1, val_ip1.lu);
  if (nargs == 2) {
    val_ip2.d = input2;
    printf ("Input2: %f (0x%" PRIx64 ") ", input2, val_ip2.lu);
  }
  printf ("Expected: %Lf (0x%" PRIx64 ") ", expected, val_exptd.lu);
  printf ("Actual: %f (0x%" PRIx64 ") ", actual, val_aop.lu);
  printf ("ULP: %f\n", ulp);
}

}  // namespace Test
