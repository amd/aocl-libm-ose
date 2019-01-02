
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

#define is_inf(x)               (!isfinite((x)))
#define is_inf_neg(g)           (!isfinite((g)))
#define AMD_LIBM_MAX_FLOAT      FLT_MAX
#define AMD_LIBM_MAX_DOUBLE     DBL_MAX
#define AMD_LIBM_INF            PINFBITPATT_SP32

#include <math.h>
#include <libm_util_amd.h>
#include <libm_types.h>


double libm_test_ulp_errorf(float computed, double expected)
{
    flt32u_t c = {.f = computed};

    int exp = (c.i >> EXPSHIFTBITS_SP32) & 0xff;  /* 8 bits ignoring sign bit */

    exp = exp - (MANTLENGTH_SP32 - 1);          /* e-p-1 */

    double ulp = pow(2, exp);                   /* 2^(e-p-1) */

    double ulpe = (expected - computed) / ulp;

    return ulpe;
}


/*
 *  1ULP = B^(e - p - 1)
 *        B = radix (2 for IEEE floating point)
 *        e = exponent
 *        p = precision (53 for IEEE double precision floaint point)
 */
#include <quadmath.h>

#if 1
double libm_test_ulp_errord(double output, __float128 computed)
{
    flt64u_t c = {.d = output};

    /* 11 bits ignoring sign bit */
    int exp = ((c.i >> EXPSHIFTBITS_DP64) & 0x7ff) - 1023;

    exp = exp - (MANTLENGTH_DP64 - 1);          /* e-p-1 */

    __float128 ulp = powq(2, exp);               /* 2^(e-p-1) */

    __float128 ulpe = fabsq(computed - output)/ulp;

    return ulpe;
}

#else
static inline __float128 __ulp(double val)
{
    flt64_t c = {.d = val};
    /* 11 bits ignoring sign bit */
    int exp = ((c.i >> EXPSHIFTBITS_DP64) & 0x7ff) - 1023;

    exp = exp - (MANTLENGTH_DP64 - 1);  /* e-p-1 */

    return powq(2, exp);                /* 2^(e-p-1) */
}

double libm_test_ulp_errord(double output, __float128 expected)
{
    if (isnan(output) && isnanq(expected)) return 0.0;

    if (isnan(output) || isnanq(expected)) return AMD_LIBM_INF;

    if (is_inf(output) && (is_inf(expected) || (expected > AMD_LIBM_MAX_DOUBLE)))
        return 0.0;

    if (is_inf_neg(output) &&
        (is_inf_neg(expected) || (expected < -AMD_LIBM_MAX_DOUBLE)))
        return 0.0;

    if ((is_inf(output) || is_inf_neg(output)) &&
        (is_inf(expected) || is_inf_neg(expected)))
        return AMD_LIBM_INF;

    if (isfinite(output)) {
        if (is_inf_neg(expected) || (expected > AMD_LIBM_MAX_DOUBLE))
            return fabsq(output - expected) / __ulp(expected);

        if (is_inf(expected) || (expected > AMD_LIBM_MAX_DOUBLE))
        return fabs(output - AMD_LIBM_MAX_DOUBLE) / __ulp(AMD_LIBM_MAX_DOUBLE) + 1;

        if (is_inf_neg(expected) || (expected < -AMD_LIBM_MAX_DOUBLE))
            return (fabs(output - (-AMD_LIBM_MAX_DOUBLE)) /
                    __ulp(-AMD_LIBM_MAX_DOUBLE)) + 1;
    }

    if (is_inf(output))
        return (fabsq((__float128)AMD_LIBM_MAX_DOUBLE - expected) /
                __ulp(expected)) + 1;

    if (is_inf_neg(output))
        return (fabsq(-((__float128)AMD_LIBM_MAX_DOUBLE) - expected) /
                __ulp(expected)) + 1;

    return 0.5;
}
#endif
