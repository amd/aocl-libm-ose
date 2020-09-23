/*
 * Copyright (C) 2019-2020 Advanced Micro Devices, Inc. All rights reserved
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

  double val = rand_simple<T>(min, max);

  for (uint32_t i = 0; i < nelem; i++) {
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
