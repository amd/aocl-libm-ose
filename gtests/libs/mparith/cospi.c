#include "precision.h"


#if defined(FLOAT)
#define FUNC_COSPI alm_mp_cospif
#define FUNC_COSPI_ULP alm_mp_cospif_ULP
#elif defined(DOUBLE)
#define FUNC_COSPI alm_mp_cospi
#define FUNC_COSPI_ULP alm_mp_cospi_ULP
#else
#error
#endif

REAL FUNC_COSPI_ULP(REAL x,REAL z, double   *sulps, double   *sreldiff)
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
                      result_cos, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;


    MPTOD(result_cos, params, &y, &ifail);

    free(xmp);
    free(result_sin);
    free(result_cos);
    free(result_tan);

    return y;
}
