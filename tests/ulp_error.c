
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
#define is_inf_negf(g) (isinf((g)) && (g) < 0.0f)
#define is_inf_negq(y) (isinfq((y)) && (y) < 0.0)
#define AMD_LIBM_MAX_FLOAT      FLT_MAX
#define AMD_LIBM_MAX_DOUBLE     DBL_MAX
#define AMD_LIBM_INF            PINFBITPATT_SP32
//#define AMD_LIBM_INF 0x7FF0000000000000
#include <math.h>
#include <libm_util_amd.h>
#include <libm/compiler.h>
#include <libm/types.h>

/*
 * We need the uint32_t version, default one gives us with an int64
 */
static inline double __ulp(float val)
{
    flt32u_t c = {.f = val};

    int32_t expo = ((c.i >> 23) & 0xff) - 127; /* 7 bits ignoring sign bit */

    expo = expo - (24 - 1);  /* e-p-1 */

    return pow(2, expo);                 /* 2^(e-p-1) */
}

double
NO_OPTIMIZE
libm_test_ulp_errorf(float output, double expected)
{
    float expectedf = (float)expected;
    //if both are NAN
    if (isnan(output) && isnan(expectedf)) return 0.0;

    // if either one is NAN
    if (isnan(output) || isnan(expectedf)) return INFINITY;

    //if output and expected are infinity
    if (isinf(output) && (isinf(expectedf) || (expectedf > AMD_LIBM_MAX_FLOAT)))
        return 0.0;

    //if output and expected are -infinity
    if (is_inf_negf(output) &&
        (is_inf_negf(expectedf) || (expectedf < -AMD_LIBM_MAX_FLOAT)))
        return 0.0;

    //if output and input are infinity with opposite signs
    if (isinf(output) && isinf(expectedf))
        return INFINITY;

    /*If output and expected are finite (The most common case) */
    if (isfinite(output)) {
        if (expectedf < AMD_LIBM_MAX_FLOAT)
            return fabs(output - expected) / __ulp(expectedf);

        /*If the expected is infinity and the output is finite */
        if ((expectedf > AMD_LIBM_MAX_FLOAT) || isinf(expectedf))
            return fabs(output - AMD_LIBM_MAX_FLOAT) / __ulp(AMD_LIBM_MAX_FLOAT) + 1;

        /*If the expected is -infinity and the output is finite */
        if (is_inf_negf(expected) || (expectedf < -AMD_LIBM_MAX_FLOAT))
            return fabs(output - (-AMD_LIBM_MAX_FLOAT)) /
                __ulp(-AMD_LIBM_MAX_FLOAT) + 1;
    }

    /*If output alone is infinity */
    if (isinf(output))
        return fabs(AMD_LIBM_MAX_FLOAT - expected / __ulp(expectedf)) + 1;

    /*If output alone is -infinity */
    if (is_inf_neg(output))
        return fabs((-AMD_LIBM_MAX_FLOAT - expected) / __ulp(expectedf)) + 1;

    return 0.0;
}

/*
 *  1ULP = B^(e - p - 1)
 *        B = radix (2 for IEEE floating point)
 *        e = exponent
 *        p = precision (53 for IEEE double precision floaint point)
 */
static inline __float128 __ulpq(double val)
{
    flt64u_t c = {.d = fabs(val)};

    int expo = ((c.i >> 52) & 0x7ff) - 1023;     /* 11 bits ignoring sign bit */

    expo = expo - (53 - 1);             /* e-p-1 */

    return powl(2, expo);                /* 2^(e-p-1) */
}

/*
 * Warning,
 *     exp2(1025.0) is +inf, however exp2q(1025.0) is a valid
 *     finite number.
 *     Here we convert the higer precision value to double-precision,
 *     to check agains isinf() or isnan().
 *     But use the higher precision when calculating ulp
 */
double
NO_OPTIMIZE
libm_test_ulp_error(double output, long double expected)
{

    /* convert __float128 to double */
    double dexpected = (double)expected;

    //if both are NAN
    if (isnan(output) && isnan(dexpected)) return 0.0;

    // if either one is NAN
    if (isnan(output) || isnan(dexpected)) return INFINITY;

    //if output and expected are infinity
    if (isinf(output) && (isinf(dexpected) || (dexpected > AMD_LIBM_MAX_DOUBLE)))
        return 0.0;

    //if output and expected are -infinity
    if (is_inf_neg(output) &&
        (is_inf_neg(dexpected) || (dexpected < -AMD_LIBM_MAX_DOUBLE)))
        return 0.0;

    //if output and input are infinity with opposite signs
    if (isinf(output) && isinf(dexpected))
        return INFINITY;

//    if (output == dexpected) return 0.0;

    /*If output and expected are finite (The most common case) */
    if (isfinite(output)) {
        if (dexpected < AMD_LIBM_MAX_DOUBLE)
            return fabsl(output - expected) / __ulpq(expected);

        /*If the expected is infinity and the output is finite */
        if ((dexpected > AMD_LIBM_MAX_DOUBLE) || isinf(dexpected))
            return fabs(output - AMD_LIBM_MAX_DOUBLE) / __ulpq(AMD_LIBM_MAX_DOUBLE) + 1;

        /*If the expected is -infinity and the output is finite */
        if (is_inf_neg(dexpected) || (dexpected < -AMD_LIBM_MAX_DOUBLE))
            return fabs(output - (-AMD_LIBM_MAX_DOUBLE)) /
                __ulpq(-AMD_LIBM_MAX_DOUBLE) + 1;
    }

    /*If output alone is infinity */
    if (isinf(output))
        return fabsl(AMD_LIBM_MAX_DOUBLE - expected / (long double)__ulpq(expected)) + 1;

    /*If output alone is -infinity */
    if (is_inf_neg(output))
        return fabsl((-AMD_LIBM_MAX_DOUBLE - expected) / (long double)__ulpq(expected)) + 1;

    return 0.0;
}

