#include "../nag/precision.h"
#include "../nag/mparith_c.h"

#if defined(FLOAT)
#define FUNC_CBRT amd_ref_cbrtf
#define FUNC_CBRT_ULP amd_ref_cbrtf_ULP
#elif defined(DOUBLE)
#define FUNC_CBRT amd_ref_cbrt
#define FUNC_CBRT_ULP amd_ref_cbrt_ULP
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

REAL FUNC_CBRT(REAL x)
{
    REAL y;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp, *result_cbrt;
    int ifail;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);
    result_cbrt= new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    constructoneby3(params,ymp);
    MPCBRT(xmp, ymp, params, result_cbrt, &ifail);

    MPTOD(result_cbrt, params, &y, &ifail);

    free(xmp);
    free(ymp);
    free(result_cbrt);

    return y;
}


REAL FUNC_CBRT_ULP(REAL x, REAL z, double   *sulps, double   *sreldiff)
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

