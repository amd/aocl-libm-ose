/*
 * Copyright (C) 2019-2020 Advanced Micro Devices, Inc. All rights reserved
 */

#include <climits>
#include <limits>
#include <cmath>
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

    int myexp = ((u128 >> nmantissa) & nexp_mask) - nexp_bias;
    myexp = myexp - nmantissa; /* e-p-1 */

    /* 2^(e-p-1) */
    if (FloatWidth::E_F32 == width)
      return pow(2, myexp);
    else
      return powl(2,myexp);
  }
};

/*
 * checks for -INF
 */
template <typename T>
bool isNInf(T _x) {
  return isinf(_x) && signbit(_x);
}

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

  // if both are NAN
  if (isnan(output) && isnan(expected)) return 0.0;

  // if either one is NAN
  if (isnan(output) || isnan(expected)) return INFINITY;

  // if output and expectedare infinity
  if (isinf(output) && (isinf(expected) || (expected > fmax))) return 0.0;

  // if output and expectedare -infinity
  if (isNInf<FAT>(output) && (isNInf<FAT>(expected) || (expected < f_low)))
    return 0.0;

  // if output and input are infinity with opposite signs
  if (isinf(output) && isinf(expected)) return INFINITY;

  // If output and expectedare finite (The most common case)
  if (isfinite(output)) {
    if (expected < fmax)
      return fabsl(output - _expected) / Ulp<FAT>(expected).Get();

    // If the expectedis infinity and the output is finite
    if ((expected > fmax) || isinf(expected))
      return fabs(output - fmax) / Ulp<FAT>(fmax).Get() + 1;

    // If the expectedis -infinity and the output is finite
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

double getUlp(float aop, double exptd) {
  return ALM::ComputeUlp(aop, exptd);
}

double getUlp(double aop, long double exptd) {
  return ALM::ComputeUlp(aop, exptd);
}

bool update_ulp(double ulp, double &max_ulp_err, double ulp_threshold)
{
  if (isinf(ulp)) {
    return false;
  }

  if ((ulp - max_ulp_err) > 0.0) {
    LIBM_TEST_DPRINTF(VERBOSE2, ,"MaxULPError: ",max_ulp_err,
                       "Ulp: ", ulp);
    max_ulp_err = ulp;
    return true;
  }

  if ((ulp - ulp_threshold) > 0.0)
    return false;                   /* fail; as greater than threshold */

  return true;
}

