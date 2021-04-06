#include "precision.h"


#if defined(FLOAT)
#define FUNC_TANPI alm_mp_tanpif

#elif defined(DOUBLE)
#define FUNC_TANPI alm_mp_tanpi

#else
#error
#endif

REAL_L FUNC_TANPI(REAL x)
{
    REAL_L y;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *result_sin,*result_cos,*result_tan;
    int ifail;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    result_sin = new_mp(params);
    result_cos = new_mp(params);
    result_tan = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    MPSINCOSTANPI(xmp, params, result_sin, result_cos, result_tan, &ifail);


    MPTOD(result_tan, params, &y, &ifail);

    free(xmp);
    free(result_sin);
    free(result_cos);
    free(result_tan);

    return y;
}

REAL FUNC_TANPI_ULP(REAL x,REAL z, double   *sulps, double   *sreldiff)
{
    REAL y;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *result_sin,*result_cos,*result_tan;
    int ifail;
	REAL reldiff,ulps;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    result_sin = new_mp(params);
    result_cos = new_mp(params);
    result_tan = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
  MPSINCOSTANPI(xmp, params, result_sin, result_cos, result_tan, &ifail);
	reldiff = MPRELDIFF(z, base, mantis, emin, emax,
                      result_tan, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;


    MPTOD(result_tan, params, &y, &ifail);

    free(xmp);
    free(result_sin);
    free(result_cos);
    free(result_tan);

    return y;
}
