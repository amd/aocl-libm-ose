#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_FMA am_mp_fmaf
#define FUNC_FMA_ULP am_mp_fmaf_ULP
#elif defined(DOUBLE)
#define FUNC_FMA am_mp_fma
#define FUNC_FMA_ULP am_mp_fma_ULP
#else
#error
#endif

REAL FUNC_FMA(REAL x, REAL y, REAL z)
{
    REAL ret;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp, *zmp, *result;
    int ifail;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);
    zmp = new_mp(params);
    result = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    DTOMP(y, ymp, params, &ifail);
    DTOMP(z, zmp, params, &ifail);
    MPFMA(xmp, ymp, zmp, result, params, &ifail);

    MPTOD(result, params, &ret, &ifail);

    free(xmp);
    free(ymp);
    free(zmp);
    free(result);

    return ret;
}

REAL FUNC_FMA_ULP(REAL x, REAL y, REAL z,REAL re, double   *sulps, double   *sreldiff)
{
    REAL ret;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp, *zmp, *result;
    int ifail;
	REAL reldiff,ulps;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);
    zmp = new_mp(params);
    result = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    DTOMP(y, ymp, params, &ifail);
    DTOMP(z, zmp, params, &ifail);
    MPFMA(xmp, ymp, zmp, result, params, &ifail);

   reldiff = MPRELDIFF(re, base, mantis, emin, emax,
                      result, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;


    MPTOD(result, params, &ret, &ifail);

    free(xmp);
    free(ymp);
    free(zmp);
    free(result);

    return ret;
}
