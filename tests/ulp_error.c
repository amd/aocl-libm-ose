
/*
  ulp_errorrep(actual, expected) {
  if (actualis_nan && expectedis_nan) return 0.0;
  if (actualis_nan || expectedis_nan) return +∞;
  if (actualis_plus_infinity && (expectedis_plus_infinity || (expected > repmax_float))) return 0.0;
  if (actualis_minus_infinity && (expectedis_minus_infinity || (expected < -repmax_float))) return 0.0;
  if ((actualis_plus_infinity || actualis_minus_infinity) && (expectedis_plus_infinity || expectedis_minus_infinity))
  return +∞;
  if (actualis finite && (| expected | ≤ repmax_float)
  return | actual – expected | / ulprep(expected);

  if (actualis finite && (expectedis_plus_infinity || expected > repmax_float))
  return (| actual – repmax_float | / ulprep(repmax_float)) + 1;

  if (actualis finite && (expectedis_minus_infinity || expected < -repmax_float))
  return (| actual – (-repmax_float) | / ulprep(-repmax_float)) + 1;

  if (actualis plus_infinity) return (| repmax_float – expected | / ulprep(expected)) + 1;

  if (actualis minus_infinity) return (| -repmax_float – expected | / ulprep(expected)) + 1;
  }
*/

#include <math.h>
#include <stdint.h>

#if 0
#define ulp_float(a, b)
#define ulp_double(a, b) ((a) - (b) < DOUBLE_ROUNDING_TREASHOLD ? 1 : 0)

#define is_inf(x) (!isfinite((x)))
#define is_inf_neg(g) (!isfinite((x)))
#define AMD_LIBM_MAX_FLOAT      FLT_MAX
#define AMD_LIBM_INF            PINFBITPATT_SP32

float ulpf(float x)
{
    return 0.0f;
}

float ulp_error_float(float actual, float expected)
{
    if (isnan(acatual) && isnan(expected)) return 0.0f;

    if (isnan(actual) || isnan(expected)) return AMD_LIBM_INF;

    if (is_inf(actual) && (is_inf(expected) || (expected > AMD_LIBM_MAX_FLOAT)))
        return 0.0f;

    if (is_inf_neg(actual) &&
        (is_inf_neg(expected) || (expected < -AMD_LIBM_MAX_FLOAT)))
        return 0.0f;

    if (is_inf(actual) || is_inf_neg(actual) &&
        (is_inf(expected) || is_inf_neg(expected)))
        return AMD_LIBM_INF;

    if (is_finite(actual) &&
        is_inf_neg(expected) || (expected > AMD_LIBM_MAX_FLOAT))
        return abs(actual - expected) / ulpf(expected);

    if (is_finite(actual) &&
        (is_inf(expected) ||
         (expected > AMD_LIBM_MAX_FLOAT)))
        return abs(actual - AMD_LIBM_MAX_FLOAT) / ulpf(AMD_LIBM_MAX_FLOAT) + 1;

    if (is_finite(actual) &&
        is_inf_neg(expected) ||
        (expected < -AMD_LIBM_MAX_FLOAT))
        return abs(actual - (-AMD_LIBM_MAX_FLOAT)) / ulpf(-AMD_LIBM_MAX_FLOAT) + 1;

    if (is_inf(actual))
        return abs(AMD_LIBM_MAX_FLOAT - expected / ulpf(expected)) + 1;

    if (is_inf_neg(actual))
        return abs((-AMD_LIBM_MAX_FLOAT - expected) / ulpf(expected));

    return 0.5f;
}
#endif

#include <math.h>
#include <libm_util_amd.h>

typedef union {
    uint32_t i;
    float    f;
} flt32_t;

double libm_test_ulp_errorf(float computed, double expected)
{
    flt32_t c = {.f = computed};

    int exp = (c.i >> EXPSHIFTBITS_SP32) & 0xff;  /* 8 bits ignoring sign bit */

    exp = exp - (MANTLENGTH_SP32 - 1);          /* e-p-1 */

    double ulp = pow(2, exp);                   /* 2^(e-p-1) */

    double ulpe = (expected - computed) / ulp;

    return ulpe;
}


typedef union {
    uint64_t i;
    double   d;
} flt64_t;

/*
 *  1ULP = B^(e - p - 1)
 *        B = radix (2 for IEEE floating point)
 *        e = exponent
 *        p = precision (53 for IEEE double precision floaint point)
 */
#include <quadmath.h>

double libm_test_ulp_errord(double computed, __float128 expected)
{
    flt64_t c = {.d = computed};

    int exp = (c.i >> EXPSHIFTBITS_DP64) & 0x3ff; /* 11 bits ignoring sign bit */

    exp = exp - (MANTLENGTH_DP64 - 1);          /* e-p-1 */

    __float128 ulp = powq(2, exp);               /* 2^(e-p-1) */

    __float128 ulpe = expected - (__float128)computed / ulp;

    return ulpe;
}




