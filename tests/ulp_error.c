
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
#include <float.h>

#define ulp_float(a, b)
#define ulp_double(a, b) ((a) - (b) < DOUBLE_ROUNDING_TREASHOLD ? 1 : 0)

#define is_inf(x) (!isfinite((x)))
#define is_inf_neg(g) (!isfinite((g)))
#define AMD_LIBM_MAX_FLOAT      FLT_MAX
#define AMD_LIBM_MAX_DOUBLE     DBL_MAX
#define AMD_LIBM_INF            PINFBITPATT_SP32

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


double libm_test_ulp_errord1(double output, __float128 computed)
{
    flt64_t c = {.d = output};

    int exp = (c.i >> EXPSHIFTBITS_DP64) & 0x3ff; /* 11 bits ignoring sign bit */

    exp = exp - (MANTLENGTH_DP64 - 1);          /* e-p-1 */

    __float128 ulp = powq(2, exp);               /* 2^(e-p-1) */

    __float128 ulpe = computed - output / ulp;

    return ulpe;
}

static inline __float128 __ulp(double val)
{
    flt64_t c = {.d = val};

    int exp = (c.i >> EXPSHIFTBITS_DP64) & 0x3ff; /* 11 bits ignoring sign bit */

    exp = exp - (MANTLENGTH_DP64 - 1);          /* e-p-1 */

    return powq(2, exp);               /* 2^(e-p-1) */
}

static inline __float128 __ulp1(double x)
{
    flt64_t input = {.d = fabs(x)};

    int exponent = (input.i >> (EXPSHIFTBITS_DP64 - 1)) - 1023; /* 1023 - bias */

    return pow(2, exponent-(EXPSHIFTBITS_DP64 - 1));  //2^(exponent-(precision-1))
}

double libm_test_ulp_errord(double output, __float128 expected)
{
    if (isnan(output) && isnan(expected)) return 0.0;

    if (isnan(output) || isnan(expected)) return AMD_LIBM_INF;

    if (is_inf(output) && (is_inf(expected) || (expected > AMD_LIBM_MAX_DOUBLE)))
        return 0.0;

    if (is_inf_neg(output) &&
        (is_inf_neg(expected) || (expected < -AMD_LIBM_MAX_DOUBLE)))
        return 0.0;

    if ((is_inf(output) || is_inf_neg(output)) &&
        (is_inf(expected) || is_inf_neg(expected)))
        return AMD_LIBM_INF;

    if (isfinite(output) &&
        (is_inf_neg(expected) || (expected > AMD_LIBM_MAX_DOUBLE)))
        return fabsq(output - expected) / __ulp(expected);

    if (isfinite(output) &&
        (is_inf(expected) ||
         (expected > AMD_LIBM_MAX_DOUBLE)))
        return fabs(output - AMD_LIBM_MAX_DOUBLE) / __ulp(AMD_LIBM_MAX_DOUBLE) + 1;

    if (isfinite(output) && (is_inf_neg(expected) ||
                             (expected < -AMD_LIBM_MAX_DOUBLE)))
        return fabsq(output - (-AMD_LIBM_MAX_DOUBLE)) /
            __ulp(-AMD_LIBM_MAX_DOUBLE) + 1;

    if (is_inf(output))
        return fabsq(AMD_LIBM_MAX_DOUBLE - expected / __ulp(expected)) + 1;

    if (is_inf_neg(output))
        return fabsq((-AMD_LIBM_MAX_DOUBLE - expected) / __ulp(expected)) + 1;

    return 0.5;
}
