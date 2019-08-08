#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"


#undef _FUNCNAME
#define _FUNCNAME "atanh"
double FN_PROTOTYPE(atanh)(double x)
{

  unsigned long long ux, ax;
  double r, absx, t, poly;


  GET_BITS_DP64(x, ux);
  ax = ux & ~SIGNBIT_DP64;
  PUT_BITS_DP64(ax, absx);

  if ((ux & EXPBITS_DP64) == EXPBITS_DP64)
    {
      /* x is either NaN or infinity */
      if (ux & MANTBITS_DP64)
        {
          /* x is NaN */
#ifdef WINDOWS
          return _amd_handle_error(_FUNCNAME,_FpCodeAtanh, ux|0x0008000000000000, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0,1);
#else
	  if (ux & QNAN_MASK_64)
          return _amd_handle_error(_FUNCNAME,_FpCodeAtanh, ux|0x0008000000000000, _DOMAIN, AMD_F_NONE, EDOM, x, 0.0,1);
	  else
          return _amd_handle_error(_FUNCNAME,_FpCodeAtanh, ux|0x0008000000000000, _DOMAIN, AMD_F_INVALID, EDOM, x, 0.0,1);
#endif
        }
      else
        {
          /* x is infinity; return a NaN */

          return _amd_handle_error(_FUNCNAME,_FpCodeAtanh, INDEFBITPATT_DP64, _DOMAIN,
                              AMD_F_INVALID, EDOM, x, 0.0,1);
       }
    }
  else if (ax >= 0x3ff0000000000000)
    {
      if (ax > 0x3ff0000000000000)
        {
          /* abs(x) > 1.0; return NaN */
          return _amd_handle_error(_FUNCNAME,_FpCodeAtanh, INDEFBITPATT_DP64, _DOMAIN,
                              AMD_F_INVALID, EDOM, x, 0.0,1);
        }
      else if (ux == 0x3ff0000000000000)
        {
          /* x = +1.0; return infinity with the same sign as x
             and set the divbyzero status flag */
          return _amd_handle_error(_FUNCNAME,_FpCodeAtanh, PINFBITPATT_DP64, _DOMAIN,
                              AMD_F_DIVBYZERO, EDOM, x, 0.0,1);
        }
      else
        {
          /* x = -1.0; return infinity with the same sign as x */
          return _amd_handle_error(_FUNCNAME,_FpCodeAtanh, NINFBITPATT_DP64, _DOMAIN,
                              AMD_F_DIVBYZERO, EDOM, x, 0.0,1);
        }
    }


  if (ax < 0x3e30000000000000)
    {
      if (ax == 0x0000000000000000)
        {
          /* x is +/-zero. Return the same zero. */
          return x;
        }
      else
        {
          /* Arguments smaller than 2^(-28) in magnitude are
             approximated by atanh(x) = x, raising inexact flag. */
#ifdef WINDOWS
		return x;//return val_with_flags(x, AMD_F_INEXACT);
#else
          return _amd_handle_error(_FUNCNAME,_FpCodeAtanh, ux, _UNDERFLOW, AMD_F_UNDERFLOW|AMD_F_INEXACT, ERANGE, x, 0.0,1);
#endif
        }
    }
  else
    {
      if (ax < 0x3fe0000000000000)
        {
          /* Arguments up to 0.5 in magnitude are
             approximated by a [5,5] minimax polynomial */
          t = x*x;
          poly =
            (0.47482573589747356373e0 +
             (-0.11028356797846341457e1 +
              (0.88468142536501647470e0 +
               (-0.28180210961780814148e0 +
                (0.28728638600548514553e-1 -
                 0.10468158892753136958e-3 * t) * t) * t) * t) * t) /
            (0.14244772076924206909e1 +
             (-0.41631933639693546274e1 +
              (0.45414700626084508355e1 +
               (-0.22608883748988489342e1 +
                (0.49561196555503101989e0 -
                 0.35861554370169537512e-1 * t) * t) * t) * t) * t);
          return x + x*t*poly;
        }
      else
        {
          /* abs(x) >= 0.5 */
          /* Note that
               atanh(x) = 0.5 * ln((1+x)/(1-x))
             (see Abramowitz and Stegun 4.6.22).
             For greater accuracy we use the variant formula
             atanh(x) = log(1 + 2x/(1-x)) = log1p(2x/(1-x)).
          */
          r = (2.0 * absx) / (1.0 - absx);
          r = 0.5 * FN_PROTOTYPE(log1p)(r);
          if (ux & SIGNBIT_DP64)
            /* Argument x is negative */
            return -r;
          else
            return r;
        }
    }
}
weak_alias (atanh, FN_PROTOTYPE(atanh))

