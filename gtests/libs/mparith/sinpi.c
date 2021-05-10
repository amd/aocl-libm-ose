#include "precision.h"


#if defined(FLOAT)
#define FUNC_SINPI alm_mp_sinpif

#elif defined(DOUBLE)
#define FUNC_SINPI alm_mp_sinpi

#else
#error
#endif

REAL FUNC_SINPI(REAL x)
{
    REAL y;
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


    MPTOD(result_sin, params, &y, &ifail);

    free(xmp);
    free(result_sin);
    free(result_cos);
    free(result_tan);

    return y;
}

REAL FUNC_SINPI_ULP(REAL x,REAL z, double   *sulps, double   *sreldiff)
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
                      result_sin, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;


    MPTOD(result_sin, params, &y, &ifail);

    free(xmp);
    free(result_sin);
    free(result_cos);
    free(result_tan);

    return y;
}
