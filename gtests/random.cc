/*
 * Copyright (C) 2008-2026 Advanced Micro Devices, Inc. All rights reserved.
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


/* Prevent Windows headers from defining min/max macros that break std::min/std::max and
 * std::numeric_limits<T>::min()/max() usages.
 */
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <cstdint>
#include <stdlib.h>
#include <cstdio>
#include <cmath>
#include <limits>
#include <algorithm>
#include <type_traits>

#include "cmdline.h"
#include "defs.h"
#include "random.h"
#include <libm/typehelper.h>

using namespace std;

namespace ALMTest {

/*
 * Extract unbiased exponent from IEEE 754 floating-point value.
 * Returns true only for normal numbers with a valid exponent.
 * Returns false for zero, subnormal, infinity, or NaN.
 *
 * @param x        Input floating-point value
 * @param exponent Output parameter for the unbiased exponent (valid only if return is true)
 * @return         true if x is a normal number, false otherwise
 */
template <typename T>
bool get_exponent(T x, int& exponent) {
    static_assert(std::is_floating_point<T>::value,
                  "get_exponent: only floating-point types are supported");
    exponent = 0;
    return false;
}

template <>
bool get_exponent<double>(double x, int& exponent) {
    uint64_t bits = asuint64(x);
    int biased_exp = static_cast<int>((bits >> 52) & 0x7FF);

    if (biased_exp == 0) {
        /* Zero or subnormal - no valid normal exponent */
        exponent = 0;
        return false;
    }
    if (biased_exp == 0x7FF) {
        /* Infinity or NaN */
        exponent = 0;
        return false;
    }

    exponent = biased_exp - 1023;
    return true;
}

template <>
bool get_exponent<float>(float x, int& exponent) {
    uint32_t bits = asuint32(x);
    int biased_exp = static_cast<int>((bits >> 23) & 0xFF);

    if (biased_exp == 0) {
        /* Zero or subnormal - no valid normal exponent */
        exponent = 0;
        return false;
    }
    if (biased_exp == 0xFF) {
        /* Infinity or NaN */
        exponent = 0;
        return false;
    }

    exponent = biased_exp - 127;
    return true;
}

/*
 * Compose a subnormal floating-point value from mantissa bits.
 * Subnormal: exponent field = 0, value = mantissa × 2^(min_exponent - mantissa_bits)
 *   - float:  value = mantissa × 2^(-149)   where mantissa ∈ [1, 2^23 - 1]
 *   - double: value = mantissa × 2^(-1074)  where mantissa ∈ [1, 2^52 - 1]
 *
 * @param mantissa  Mantissa value [1, 2^mantissa_bits - 1]
 * @param negative  If true, return negative value
 */
template <typename T>
T compose_subnormal(uint64_t mantissa, bool negative) {
    static_assert(std::is_floating_point<T>::value,
                  "compose_subnormal: only floating-point types are supported");
    (void)mantissa; (void)negative;  /* Suppress unused parameter warnings */
    return T{};
}

template <>
double compose_subnormal<double>(uint64_t mantissa, bool negative) {
    uint64_t bits = mantissa & 0x000FFFFFFFFFFFFFULL;
    if (negative) bits |= (1ULL << 63); /* Sign bit */
    return asdouble(bits);
}

template <>
float compose_subnormal<float>(uint64_t mantissa, bool negative) {
    uint32_t bits = static_cast<uint32_t>(mantissa) & 0x007FFFFF;
    if (negative) bits |= (1U << 31); /* Sign bit */
    return asfloat(bits);
}

/*
 * Compose a normal floating-point value from exponent and mantissa.
 * Normal: value = (1 + mantissa/2^mantissa_bits) × 2^exponent
 *
 * @param exponent  Unbiased exponent
 * @param mantissa  Mantissa value [0, 2^mantissa_bits - 1]
 * @param negative  If true, return negative value
 */
template <typename T>
T compose_normal(int exponent, uint64_t mantissa, bool negative) {
    static_assert(std::is_floating_point<T>::value,
                  "compose_normal: only floating-point types are supported");
    (void)exponent; (void)mantissa; (void)negative;  /* Suppress unused parameter warnings */
    return T{};
}

template <>
double compose_normal<double>(int exponent, uint64_t mantissa, bool negative) {
    uint64_t biased_exp = static_cast<uint64_t>(exponent + 1023);
    uint64_t bits = (biased_exp << 52) | (mantissa & 0x000FFFFFFFFFFFFFULL);
    if (negative) bits |= (1ULL << 63);
    return asdouble(bits);
}

template <>
float compose_normal<float>(int exponent, uint64_t mantissa, bool negative) {
    uint32_t biased_exp = static_cast<uint32_t>(exponent + 127);
    uint32_t bits = (biased_exp << 23) | (static_cast<uint32_t>(mantissa) & 0x007FFFFF);
    if (negative) bits |= (1U << 31);
    return asfloat(bits);
}

template <typename T>
struct BinadeSideInfo {
    bool has_subnormals;
    int exp_lo;
    int exp_hi;

    size_t normal_binade_count() const {
        return (exp_lo <= exp_hi) ? static_cast<size_t>(exp_hi - exp_lo + 1) : 0;
    }
};

/*
 * Analyze binades for one side of the number line.
 * Works with absolute values to handle both positive and negative sides uniformly.
 *
 * @param abs_near  Absolute value of bound closer to zero
 * @param abs_far   Absolute value of bound farther from zero
 */
template <typename T>
BinadeSideInfo<T> analyze_binades(T abs_near, T abs_far) {
    constexpr int min_exp = std::numeric_limits<T>::min_exponent - 1;
    constexpr int max_exp = std::numeric_limits<T>::max_exponent - 1;

    BinadeSideInfo<T> info;
    info.has_subnormals = abs_near < std::numeric_limits<T>::min();

    if (!get_exponent(abs_near, info.exp_lo)) {
        info.exp_lo = min_exp;  /* abs_near is subnormal, start from min_exp */
    }
    info.exp_lo = std::max(info.exp_lo, min_exp);

    if (!get_exponent(abs_far, info.exp_hi)) {
        info.exp_hi = min_exp - 1;  /* abs_far is subnormal or zero, no normal binades */
    }
    info.exp_hi = std::min(info.exp_hi, max_exp);

    return info;
}

/*
 * Count the number of binades (including subnormal sub-binades) in range [min, max].
 * Normal binades: one per exponent value.
 * Subnormal sub-binades: one per mantissa leading-bit position (23 for float, 52 for double).
 */
template <typename T>
size_t count_binades(T min, T max) {
    constexpr int mantissa_bits = std::numeric_limits<T>::digits - 1;
    size_t count = 0;

    /* Positive binades */
    if (max > T{0}) {
        T abs_near = (min > T{0}) ? min : std::numeric_limits<T>::denorm_min();
        auto info = analyze_binades(abs_near, max);
        if (info.has_subnormals) count += static_cast<size_t>(mantissa_bits);
        count += info.normal_binade_count();
    }

    /* Negative binades */
    if (min < T{0}) {
        T abs_near = (max < T{0}) ? -max : std::numeric_limits<T>::denorm_min();
        auto info = analyze_binades(abs_near, -min);
        if (info.has_subnormals) count += static_cast<size_t>(mantissa_bits);
        count += info.normal_binade_count();
    }

    return count;
}

/*
 * Generate a random floating point number from min to max
 * But then, floating point numbers itself is not uniformly distributed.
 * (towards 0 it is dense, not otherwise)
 */

template <typename T>
static T rand_simple(T min, T max) {
  T range = (max - min);
  T div = static_cast<T>(static_cast<T>(RAND_MAX) / range);

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

/*
 * Fill buffer with values that uniformly cover all IEEE 754 binades in [min, max].
 * Each binade (including subnormal sub-binades) gets equal representation.
 * Subnormals are split into sub-binades by mantissa leading-bit position.
 *
 * @return 0 on success, -1 if no binades in range, 1 if nelem was insufficient
 */
template <typename T>
int Random<T>::fillBinadex(T *data, uint32_t nelem, T min, T max) {
    /* Binadex only implemented for float and double - fall back to fillSimple for others */
    if constexpr (!std::is_same<T, float>::value && !std::is_same<T, double>::value) {
        printf("Warning: fillBinadex not implemented for this type, "
               "falling back to fillSimple\n");
        return fillSimple(data, nelem, min, max);
    }

    constexpr int mantissa_bits = std::numeric_limits<T>::digits - 1;
    constexpr uint64_t max_mantissa = (1ULL << mantissa_bits) - 1;

    size_t num_binades = count_binades(min, max);
    if (num_binades == 0) {
        return -1;
    }

    const size_t max_idx = static_cast<size_t>(nelem);

    /* Warn if nelem is insufficient to have at least 1 point per binade */
    if (max_idx < num_binades) {
        printf("Warning: fillBinadex: nelem (%u) is insufficient for 1 point per binade "
               "(need at least %zu for range [%g, %g])\n",
               nelem, num_binades, static_cast<double>(min), static_cast<double>(max));
    }

    /* Round up, minimum 1 point per binade */
    size_t points_per_binade = std::max(static_cast<size_t>(1),
                                        (max_idx + num_binades - 1) / num_binades);

    size_t idx = 0;

    /* Helper lambda to generate samples for a normal binade */
    auto generate_normal_binade = [&](int exp, bool negative) {
        for (size_t i = 0; i < points_per_binade && idx < max_idx; i++) {
            double frac = (points_per_binade == 1) ? 0.5
                        : static_cast<double>(i) / static_cast<double>(points_per_binade - 1);
            uint64_t mantissa = static_cast<uint64_t>(frac * static_cast<double>(max_mantissa));
            data[idx++] = compose_normal<T>(exp, mantissa, negative);
        }
    };

    /* Helper lambda to generate samples for subnormal sub-binades */
    auto generate_subnormal_sub_binades = [&](bool negative) {
        /* Each sub-binade corresponds to mantissa with leading 1 at bit position 'sub' */
        for (int sub = 0; sub < mantissa_bits && idx < max_idx; sub++) {
            uint64_t mantissa_min = (sub == 0) ? 1 : (1ULL << sub);
            uint64_t mantissa_max_sub = (1ULL << (sub + 1)) - 1;

            /* Sub-binade 0 has only 1 value */
            size_t points_this_sub = (sub == 0) ? 1 : points_per_binade;

            for (size_t i = 0; i < points_this_sub && idx < max_idx; i++) {
                double frac = (points_this_sub == 1) ? 0.0
                            : static_cast<double>(i) / static_cast<double>(points_this_sub - 1);
                uint64_t mantissa = mantissa_min +
                    static_cast<uint64_t>(frac * static_cast<double>(mantissa_max_sub - mantissa_min));
                data[idx++] = compose_subnormal<T>(mantissa, negative);
            }
        }
    };

    /* Helper lambda to generate samples for one side (positive or negative) */
    auto generate_side = [&](const BinadeSideInfo<T>& info, bool negative) {
        if (info.has_subnormals) {
            generate_subnormal_sub_binades(negative);
        }
        for (int e = info.exp_lo; e <= info.exp_hi; e++) {
            generate_normal_binade(e, negative);
        }
    };

    /* Positive binades */
    if (max > T{0}) {
        T abs_near = (min > T{0}) ? min : std::numeric_limits<T>::denorm_min();
        auto info = analyze_binades(abs_near, max);
        generate_side(info, false);
    }

    /* Negative binades */
    if (min < T{0}) {
        T abs_near = (max < T{0}) ? -max : std::numeric_limits<T>::denorm_min();
        auto info = analyze_binades(abs_near, -min);
        generate_side(info, true);
    }

    return (max_idx < num_binades) ? 1 : 0;
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
    case ALM::RangeType::E_Binadex:
      ret = fillBinadex(data, nelem, min, max);
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
