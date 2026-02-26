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
#include <climits>
#include <limits>
#include <cmath>
#include <cstring>
#include <type_traits>
#include "almstruct.h"
#include "almtest.h"
#include "defs.h"

#if (defined _WIN32 || defined _WIN64 ) && (defined(__clang__))
typedef uint64_t u_int64_t;
typedef uint32_t u_int32_t;
#endif

namespace ALM {
template <typename T>
constexpr int get_exponent_bits() {
  int exponent_range = ::std::numeric_limits<T>::max_exponent -
                       ::std::numeric_limits<T>::min_exponent;
  int bits = 0;

  while ((exponent_range >> bits) > 0) ++bits;

  return bits;
}

template <typename T>
constexpr int get_mantissa_bits() {
  return ::std::numeric_limits<T>::digits - 1;
}

template <typename T>
class Ulp {
 private:
  FloatWidth width;

  float f;
  double d;
  long double ld;

 public:
  Ulp() : f(0.0f), d(0.0), ld(0.0) {}

  explicit Ulp(float _f) {
    f = _f;
    width = FloatWidth::E_F32;
  }
  explicit Ulp(double _d) {
    d = _d;
    width = FloatWidth::E_F64;
  }
  explicit Ulp(long double _ld) {
    ld = _ld;
    width = FloatWidth::E_F80;
  }

long double Get() {
    struct flt {
      char c[sizeof(__int128_t)];
      u_int32_t *u32;
      u_int64_t *u64;
      __uint128_t *u128;
    };
    flt fltnum;

    auto nexp = get_exponent_bits<T>();
    auto nexp_mask = (1UL << nexp) - 1;
    auto nexp_bias = (1UL << (nexp - 1)) - 1;  // looks same as nexp_mask;
    auto nmantissa = get_mantissa_bits<T>();

    __uint128_t u128 = 0;

    switch (width) {
      case FloatWidth::E_F32:
        fltnum.u32 = (reinterpret_cast<uint32_t *>(&f));
        u128 = (__uint128_t)*fltnum.u32;
        break;
      case FloatWidth::E_F64:
        fltnum.u64 = (reinterpret_cast<uint64_t *>(&d));
        u128 = (__uint128_t)*fltnum.u64;
        break;
      case FloatWidth::E_F80:
      case FloatWidth::E_F128:
        fltnum.u128 = (reinterpret_cast<__uint128_t *>(&ld));
        u128 = (__uint128_t)*fltnum.u128;
        break;
      // TODO(user): We'll revisit when F16 is available.
      case FloatWidth::E_F16:
      default:
        break;
    }

    int biased_exp = (u128 >> nmantissa) & nexp_mask;

    /* Handle subnormals: use minimum exponent instead of 0
     * For subnormals, the effective exponent is (1 - bias), not (0 - bias)
     * This matters on Windows where long double == double and powl(2, -1075) underflows
     */
    int actual_exp;
    if (biased_exp == 0) {
      // Subnormal: effective exponent is 1 - bias (e.g., -1022 for double)
      actual_exp = 1 - nexp_bias;
    } else {
      // Normal: exponent is biased_exp - bias
      actual_exp = biased_exp - nexp_bias;
    }
    int myexp = actual_exp - nmantissa; /* e-p-1 */

    /* 2^(e-p-1) */
    if (FloatWidth::E_F32 == width)
      return pow(2, myexp);
    else
      return powl(2, myexp);
  }
};

/*
 * checks for -INF
 */
template <typename T>
bool isNInf(T _x) {
  return isinf(_x) && signbit(_x);
}

/*
 * Windows-only: Helper functions to compute ULP using integer bit comparison.
 * This avoids floating-point underflow issues on Windows where long double == double
 * and powl(2, -1074) underflows to zero, causing incorrect ULP calculations for subnormals.
 * On Linux, long double has 80-bit extended precision, so this is not needed.
 */
#if (defined _WIN32 || defined _WIN64)
static double ComputeUlpBitwise(double output, double expected) {
  // Reinterpret as integers for bit-level comparison
  uint64_t out_bits, exp_bits;
  std::memcpy(&out_bits, &output, sizeof(double));
  std::memcpy(&exp_bits, &expected, sizeof(double));

  // Handle sign: if same sign, simple difference; if different signs, this is a large error
  bool out_neg = (out_bits >> 63) != 0;
  bool exp_neg = (exp_bits >> 63) != 0;

  if (out_neg != exp_neg) {
    // Different signs - return sum of distances from zero
    uint64_t out_mag = out_bits & 0x7FFFFFFFFFFFFFFFULL;
    uint64_t exp_mag = exp_bits & 0x7FFFFFFFFFFFFFFFULL;
    return (double)(out_mag + exp_mag);
  }

  // Same sign - compute absolute difference in bit representation
  uint64_t out_mag = out_bits & 0x7FFFFFFFFFFFFFFFULL;
  uint64_t exp_mag = exp_bits & 0x7FFFFFFFFFFFFFFFULL;

  if (out_mag >= exp_mag)
    return (double)(out_mag - exp_mag);
  else
    return (double)(exp_mag - out_mag);
}

static double ComputeUlpBitwise(float output, float expected) {
  uint32_t out_bits, exp_bits;
  std::memcpy(&out_bits, &output, sizeof(float));
  std::memcpy(&exp_bits, &expected, sizeof(float));

  bool out_neg = (out_bits >> 31) != 0;
  bool exp_neg = (exp_bits >> 31) != 0;

  if (out_neg != exp_neg) {
    uint32_t out_mag = out_bits & 0x7FFFFFFFU;
    uint32_t exp_mag = exp_bits & 0x7FFFFFFFU;
    return (double)(out_mag + exp_mag);
  }

  uint32_t out_mag = out_bits & 0x7FFFFFFFU;
  uint32_t exp_mag = exp_bits & 0x7FFFFFFFU;

  if (out_mag >= exp_mag)
    return (double)(out_mag - exp_mag);
  else
    return (double)(exp_mag - out_mag);
}

/**
 * Helper to check if a value is subnormal (denormalized)
 */
template <typename T>
static bool is_subnormal(T x) {
  return std::fpclassify(x) == FP_SUBNORMAL;
}
#endif  // _WIN32 || _WIN64

/**
 * ComputeUlp() - given two outputs, computes ULP
 *
 * If either of 'a' or 'b' is a NaN,
 *       then returns the largest representable value for T.
 *       for ex for  'double', return std::numeric_limits<double>::max()
 *       which is the same as DBL_MAX or 1.7976931348623157e+308.
 * If 'a' and 'b' differ in sign
 *       then returns the largest representable value for T.
 * If both 'a' and 'b' are both infinities (of the same sign),
 *       then returns zero.
 * If just one of 'a' and 'b' is an infinity,
 *       then returns the largest representable value for T.
 * If both 'a' and 'b' are zero
 *       then returns zero.
 * If just one of 'a' or 'b' is zero or denormals,
 *       then it is treated as if it were the smallest (non-denormalized)
 *       value representable in T for the purposes of the above calculation.
 */
template <typename FAT, typename FAT_L>
double ComputeUlp(FAT output, FAT_L _expected) {
  FAT expected = (FAT)_expected;
  static const FAT
#if defined(_WIN64) || defined(_WIN32)
      fmax = (std::numeric_limits<FAT>::max)(),      // FLT_MAX, DBL_MAX etc
      f_low = (std::numeric_limits<FAT>::lowest)();  // -FLT_MAX, -DBL_MAX etc
#else
      fmax = std::numeric_limits<FAT>::max(),      // FLT_MAX, DBL_MAX etc
      f_low = std::numeric_limits<FAT>::lowest();  // -FLT_MAX, -DBL_MAX etc
#endif

  // Handle NaN cases
  if (isnan(output) && isnan(expected)) {
    // Both are NaN: check if sign bits match
    if (signbit(output) == signbit(expected)) {
      return 0.0;  // Same sign NaN (payload ignored)
    } else {
      return INFINITY;  // Different sign NaN (e.g., -nan vs nan)
    }
  }

  // if either one (but not both) is NAN
  if (isnan(output) || isnan(expected))
      return INFINITY;

  // if both are zero (handles +0 == -0)
  if (output == 0 && expected == 0) return 0.0;

  // Handle infinity cases
  if (isinf(output) && isinf(expected)) {
    // Both are infinity: check if sign bits match
    if (signbit(output) == signbit(expected)) {
      return 0.0;  // Same sign infinity (both +inf or both -inf)
    } else {
      return INFINITY;  // Different sign infinity (e.g., +inf vs -inf)
    }
  }

#if (defined _WIN32 || defined _WIN64)
  // Windows-only: Handle subnormals and zeros using bitwise comparison to avoid
  // floating-point underflow in ULP calculation (long double == double on Windows,
  // so powl(2,-1074) underflows to zero). On Linux, this is not needed because
  // long double has 80-bit extended precision.
  if (is_subnormal(output) || is_subnormal(expected) ||
      output == 0 || expected == 0) {
    return ComputeUlpBitwise(output, expected);
  }
#endif

  // If output and expected are finite (The most common case)
  if (isfinite(output)) {
    if (expected < fmax)
      return fabsl(output - _expected) / Ulp<FAT>(expected).Get();

    // If the expected is infinity and the output is finite
    if ((expected > fmax) || isinf(expected))
      return fabs(output - fmax) / Ulp<FAT>(fmax).Get() + 1;

    // If the expected is -infinity and the output is finite
    if (isNInf<FAT>(_expected) || (expected < f_low))
      return fabs(output - f_low) / Ulp<FAT>(f_low).Get() + 1;
  }

  // If output alone is infinity
  if (isinf(output))
    return fabs((fmax - _expected) / Ulp<FAT>(expected).Get()) + 1;

  // If output alone is -infinity
  if (is_inf_neg(output))
    return fabs((f_low - _expected) / Ulp<FAT>(expected).Get()) + 1;

  return 0.0;
}

}  // namespace ALM

double getUlpr(float aop, double exptd) {
  return ALM::ComputeUlp(aop, exptd);
}

double getUlpr(double aop, long double exptd) {
  return ALM::ComputeUlp(aop, exptd);
}

/* The calculation of ULP in Complex Numbers is as follows:
 * cabs() is used to compute the complex absolute value (magnitude/modulus) of a complex number.
 * The magnitudes of actual and expected outputs will determine the resultant ULP.
 */

double getUlpc(float _Complex aop, double _Complex exptd) {
  #if (defined _WIN32 || defined _WIN64 )
    float f_aop = abs(complex<float> (__real__ aop, __imag__ aop));
    double d_exptd = abs(complex<double> (__real__ exptd, __imag__ exptd));
  #else
    float f_aop = cabs(aop);
    double d_exptd = cabs(exptd);
  #endif
  return ALM::ComputeUlp(f_aop, d_exptd);
}

#if (defined _WIN32 || defined _WIN64)
double getUlpc(double _Complex aop, double _Complex exptd) {
  double d_aop = abs(complex<double>(__real__ aop, __imag__ aop));
  double d_exptd = abs(complex<double>(__real__ exptd, __imag__ exptd));
  return ALM::ComputeUlp(d_aop, d_exptd);
}
// Overload for long double _Complex - delegates to double _Complex version
// (long double == double on Windows)
double getUlpc(double _Complex aop, long double _Complex exptd) {
  return getUlpc(aop, (double _Complex)exptd);
}
#else
double getUlpc(double _Complex aop, long double _Complex exptd) {
  double d_aop = cabs(aop);
  long double ld_exptd = cabsl(exptd);
  return ALM::ComputeUlp(d_aop, ld_exptd);
}
#endif
// Helper template to check and report infinity/NaN in ULP computation with input values
template<typename T, typename T_L>
inline double CheckAndReportUlp(T aop, T_L exptd, double ulp_result)
{
  if (std::isinf(ulp_result)) {
    std::cout << "ULP computation resulted in Infinity - aop: " << aop << ", exptd: " << exptd << ", ulp: " << ulp_result << std::endl;
  } else if (std::isnan(ulp_result)) {
    std::cout << "ULP computation resulted in NaN - aop: " << aop << ", exptd: " << exptd << ", ulp: " << ulp_result << std::endl;
  }
  return ulp_result;
}

double getUlp(float aop, double exptd) {
  return CheckAndReportUlp(aop, exptd, getUlpr(aop, exptd));
}

double getUlp(double aop, long double exptd) {
  return CheckAndReportUlp(aop, exptd, getUlpr(aop, exptd));
}

double getUlp(float _Complex aop, double _Complex exptd) {
  return CheckAndReportUlp(aop, exptd, getUlpc(aop, exptd));
}

#if (defined _WIN32 || defined _WIN64)
double getUlp(double _Complex aop, double _Complex exptd) {
  return CheckAndReportUlp(aop, exptd, getUlpc(aop, exptd));
}
// Overload for long double _Complex - delegates to double _Complex version
// (long double == double on Windows)
double getUlp(double _Complex aop, long double _Complex exptd) {
  return getUlp(aop, (double _Complex)exptd);
}
#else
double getUlp(double _Complex aop, long double _Complex exptd) {
  return CheckAndReportUlp(aop, exptd, getUlpc(aop, exptd));
}
#endif

bool update_ulp(double ulp, double &max_ulp_err, double ulp_threshold)
{
/* Removed early return for infinite ULP to allow tracking and logging via CheckAndReportUlp.
 * Infinite ULP values now update max_ulp_err and are logged for debugging.
 * The threshold comparison below will still correctly fail for infinite values.
  if (isinf(ulp)) {
    return false;
  }
*/
  if ((ulp - max_ulp_err) > 0.0) {
    LIBM_TEST_DPRINTF(VERBOSE2, ,"MaxULPError: ",max_ulp_err,
                       "Ulp: ", ulp);
    max_ulp_err = ulp;
  }

  if ((ulp - ulp_threshold) > 0.0)
    return false;                   /* fail; as greater than threshold */

  return true;
}

