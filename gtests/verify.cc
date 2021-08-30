/*
 * Copyright (C) 2019-2020 Advanced Micro Devices, Inc. All rights reserved
 */
 
#include <float.h>
#include <string>
#include <stdlib.h>
#include <stdbool.h>

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

}  // namespace Test
