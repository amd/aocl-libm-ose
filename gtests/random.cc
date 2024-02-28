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

 
#include <cstdint>
#include <stdlib.h>
#include <cmath>

#include "cmdline.h"
#include "defs.h"
#include "random.h"

using namespace std;

namespace ALMTest {

/*
 * Generate a random floating point number from min to max
 * But then, floating point numbers itself is not uniformly distributed.
 * (towards 0 it is dense, not otherwise)
 */

template <typename T>
static T rand_simple(T min, T max) {
  T range = (max - min);
  T div = RAND_MAX / range;

  return (min + (rand() / div));
}

template <typename T>
static T rand_simple(void) {
  static uint64_t seed = 123456789;
  const uint64_t a = 9301;
  const uint64_t c = 49297;
  const uint64_t m = 233280;

  seed = (seed * a + c) % m;
  return (T)seed / (T)m;
}

/*
 * Generate uniformly distributed range in interval [a, b]
 */
template <typename T>
T rand_logdist(int i, int n, T a, T b, T logba) {
  T x, tx;

  if (i <= 0)
    tx = 0.0;
  else if (i >= n - 1)
    tx = 1.0;
  else
    tx = (T)((i + rand_simple<T>() - 0.5) / (n - 1));

  /* tx is uniformly distributed in [0,1] */
  if (a != 0.0 && b != 0.0 && ((a < 0.0) == (b < 0.0))) {
    /* x is logarithmically distributed in [a,b] */
    if (a < 0.0) {
      /*
      * Negative interval: negate b, tx and x so that  the numbers
      * are distributed in a similar fashion to  the equivalent
      * positive interval. Doesn't really make much difference
      */
      if (tx == 0.0)
        x = b;
      else if (tx == 1.0)
        x = a;
      else
        x = b * exp(-tx * logba);
    } else {
      if (tx == 0.0)
        x = a;
      else if (tx == 1.0)
        x = b;
      else
        x = a * exp(tx * logba);
    }
  } else {
    /* x is uniformly distributed in [a,b] */
    if (tx == 0.0)
      x = a;
    else if (tx == 1.0)
      x = b;
    else
      x = a + tx * (b - a);
  }

  if (x < a)
    x = a;
  else if (x > b)
    x = b;

  return x;
}

template <typename T>
int Random<T>::fillRandom(T *data, uint32_t nelem, T min, T max) {
  if (randfd <= 0) init_randfd();

  double val;

  for (uint32_t i = 0; i < nelem; i++) {
    val = rand_simple<T>(min, max);
    data[i] = (T)val;
  }

  return 0;
}

template <typename T>
int Random<T>::fillSimple(T *data, uint32_t nelem, T min, T max) {
  T *ptr = data;
  for (uint32_t i = 0; i < nelem; i++) {
    double val = 0.0;
    val = (min * (nelem - i) + max * i) / nelem;

    *ptr++ = (T)val;
  }

  return 0;
}

template <typename T>
int Random<T>::fillLinear(T *data, uint32_t nelem, T min, T max) {
  double logba = 0.0;
  T *ptr = data;

  if (min != 0.0 && max != 0.0 && ((min < 0.0) == (min < 0.0)))
    logba = log(max / min);

  for (uint32_t i = 0; i < nelem; i++) {
    double val = 0.0;
    val = rand_logdist<T>(i, nelem, min, max, logba);

    *ptr++ = (T)val;
  }

  return 0;
}

template <typename T>
int Random<T>::Fill(T *data, uint32_t nelem, T min, T max, ALM::RangeType r) {
  int ret = 0;

  switch (r) {
    case ALM::RangeType::E_Linear:
      ret = fillLinear(data, nelem, min, max);
      break;
    case ALM::RangeType::E_Random:
      ret = fillRandom(data, nelem, min, max);
      break;
    case ALM::RangeType::E_Simple:
    default:
      ret = fillSimple(data, nelem, min, max);
      break;
  }

  return ret;
}

template <typename T>
T Random<T>::Next() {
  T *a = new (T);
  *a = rand_simple<T>();
  return *a;
}

template class Random<float>;
template class Random<double>;
template class Random<long double>;
}  // namespace ALMTest
