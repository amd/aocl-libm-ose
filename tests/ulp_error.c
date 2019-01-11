
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
#define is_inf_neg(g) (isinf((g)) && (g) < 0.0)
#define is_inf_negq(y) (isinfq((y)) && (y) < 0.0)
#define AMD_LIBM_MAX_FLOAT      FLT_MAX
#define AMD_LIBM_MAX_DOUBLE     DBL_MAX
#define AMD_LIBM_INF            PINFBITPATT_SP32
//#define AMD_LIBM_INF 0x7FF0000000000000
#include <math.h>
#include <libm_util_amd.h>
#include <libm_types.h>

/*
 * We need the uint32_t version, default one gives us with an int64
 */
static inline double __ulpf(float val)
{
    flt32u_t c = {.f = val};

    int exp = ((c.i >> EXPSHIFTBITS_SP32) & 0x7f) - 127; /* 7 bits ignoring sign bit */

    exp = exp - (MANTLENGTH_SP32 - 1);  /* e-p-1 */

    return pow(2, exp);                 /* 2^(e-p-1) */
}

double libm_test_ulp_errorf(float output, double expected)
{
    //if both are NAN
    if (isnan(output) && isnan(expected)) return 0.0;

    // if either one is NAN
    if (isnan(output) || isnan(expected)) return INFINITY;

    //if output and expected are infinity
    if (isinf(output) && (isinf(expected) || (expected > AMD_LIBM_MAX_FLOAT)))
        return 0.0;

    //if output and expected are -infinity
    if (is_inf_neg(output) &&
        (is_inf_neg(expected) || (expected < -AMD_LIBM_MAX_FLOAT)))
        return 0.0;

    //if output and input are infinity with opposite signs
    if (isinf(output) && isinf(expected))
        return INFINITY;

    /*If output and expected are finite (The most common case) */
    if (isfinite(output)) {
        if (expected < AMD_LIBM_MAX_FLOAT)
            return fabs(output - expected) / __ulpf(expected);

        /*If the expected is infinity and the output is finite */
        if ((expected > AMD_LIBM_MAX_FLOAT) || isinf(expected))
            return fabs(output - AMD_LIBM_MAX_FLOAT) / __ulpf(AMD_LIBM_MAX_FLOAT) + 1;

        /*If the expected is -infinity and the output is finite */
        if (is_inf_neg(expected) || (expected < -AMD_LIBM_MAX_FLOAT))
            return fabs(output - (-AMD_LIBM_MAX_FLOAT)) /
                __ulpf(-AMD_LIBM_MAX_FLOAT) + 1;
    }

    /*If output alone is infinity */
    if (isinf(output))
        return fabs(AMD_LIBM_MAX_FLOAT - expected / __ulpf(expected)) + 1;

    /*If output alone is -infinity */
    if (is_inf_neg(output))
        return fabs((-AMD_LIBM_MAX_FLOAT - expected) / __ulpf(expected)) + 1;

    return 0.0;
}

/*
 *  1ULP = B^(e - p - 1)
 *        B = radix (2 for IEEE floating point)
 *        e = exponent
 *        p = precision (53 for IEEE double precision floaint point)
 */
#include <quadmath.h>

static inline __float128 __ulp(double val)
{
    flt64u_t c = {.d = val};

    int exp = ((c.i >> EXPSHIFTBITS_DP64) & 0x7ff) - 1023; /* 11 bits ignoring sign bit */

    exp = exp - (MANTLENGTH_DP64 - 1);  /* e-p-1 */

    return powq(2, exp);                /* 2^(e-p-1) */
}

double libm_test_ulp_error(double output, __float128 expected)
{
    //if both are NAN
    if (isnan(output) && isnanq(expected)) return 0.0;

    // if either one is NAN
    if (isnan(output) || isnanq(expected)) return INFINITY;

    //if output and expected are infinity
    if (isinf(output) && (isinfq(expected) || (expected > AMD_LIBM_MAX_DOUBLE)))
        return 0.0;

    //if output and expected are -infinity
    if (is_inf_neg(output) &&
        (is_inf_negq(expected) || (expected < -AMD_LIBM_MAX_DOUBLE)))
        return 0.0;

    //if output and input are infinity with opposite signs
    if (isinf(output) && isinfq(expected))
        return INFINITY;

    /*If output and expected are finite (The most common case) */
    if (isfinite(output)) {
        if (expected < AMD_LIBM_MAX_DOUBLE)
            return fabsq(output - expected) / __ulp(expected);

        /*If the expected is infinity and the output is finite */
        if ((expected > AMD_LIBM_MAX_DOUBLE) || isinfq(expected))
            return fabs(output - AMD_LIBM_MAX_DOUBLE) / __ulp(AMD_LIBM_MAX_DOUBLE) + 1;

        /*If the expected is -infinity and the output is finite */
        if (is_inf_negq(expected) || (expected < -AMD_LIBM_MAX_DOUBLE))
            return fabsq(output - (-AMD_LIBM_MAX_DOUBLE)) /
                __ulp(-AMD_LIBM_MAX_DOUBLE) + 1;
    }

    /*If output alone is infinity */
    if (isinf(output))
        return fabsq(AMD_LIBM_MAX_DOUBLE - expected / __ulp(expected)) + 1;

    /*If output alone is -infinity */
    if (is_inf_neg(output))
        return fabsq((-AMD_LIBM_MAX_DOUBLE - expected) / __ulp(expected)) + 1;

    return 0.0;
}
