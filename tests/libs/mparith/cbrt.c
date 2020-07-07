#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_CBRT alm_mp_cbrtf
#define FUNC_CBRT_ULP alm_mp_cbrtf_ULP
#elif defined(DOUBLE)
#define FUNC_CBRT alm_mp_cbrt
#define FUNC_CBRT_ULP alm_mp_cbrt_ULP
#else
#error
#endif

int constructoneby3(fp_params params, int *result)
{
  int *xmp, *zmp;
  int ifail;
  xmp = new_mp(params);
  zmp = new_mp(params);
  DTOMP(1.0, xmp, params, &ifail);
  DTOMP(3.0, zmp, params, &ifail);
  /*By dividing we are able to set the more accurate 1/3
   * rather than blind initializing*/
  DIVIDE(xmp, zmp, result,params, &ifail);

  free(xmp);
  free(zmp);

  return 0;
}

REAL FUNC_CBRT_ULP123(REAL x, REAL z, double   *sulps, double   *sreldiff)
{
    REAL y;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp, *result_cbrt;
    int ifail;
    REAL reldiff,ulps;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);
    result_cbrt= new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    constructoneby3(params,ymp);
    MPCBRT(xmp, ymp, params, result_cbrt, &ifail);
/********/
    reldiff = MPRELDIFF(z, base, mantis, emin, emax,
                      result_cbrt, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;

/********/
    MPTOD(result_cbrt, params, &y, &ifail);

    free(xmp);
    free(ymp);
    free(result_cbrt);

    return y;
}



#include <mpfr.h>

REAL FUNC_CBRT(REAL x)
{
    REAL y;

    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t mpx, mp_rop;

    mpfr_inits2(256, mpx, mp_rop, (mpfr_ptr) 0);

#if defined(FLOAT)
    mpfr_set_flt(mpx, x, rnd);
#elif defined(DOUBLE)
    mpfr_set_d(mpx, x, rnd);
#endif

    mpfr_cbrt(mp_rop, mpx, rnd);

#if defined(FLOAT)
    y = mpfr_get_flt(mp_rop, rnd);
#elif defined(DOUBLE)
    y = mpfr_get_d(mp_rop, rnd);
#endif

    mpfr_clears (mpx, mp_rop, (mpfr_ptr) 0);
    return y;
}

