#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_POW am_mp_powf
#define FUNC_POW_ULP am_mp_powf_ULP
#elif defined(DOUBLE)
#define FUNC_POW am_mp_pow
#define FUNC_POW_ULP am_mp_pow_ULP
#else
#error
#endif

REAL FUNC_POW(REAL x, REAL y)
{
    REAL ret;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp, *result;
    int ifail;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);
    result = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    DTOMP(y, ymp, params, &ifail);
    /* Computes pow(rXMP, YMP), result in RESULT */
    MPPOW(xmp, ymp, params, result, &ifail);

    MPTOD(result, params, &ret, &ifail);

    free(xmp);
    free(ymp);
    free(result);

    return ret;
}

REAL FUNC_POW_ULP(REAL x, REAL y,REAL z, double   *sulps, double   *sreldiff)
{
    REAL ret;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp, *result;
    int ifail;
	REAL reldiff,ulps;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);
    result = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    DTOMP(y, ymp, params, &ifail);
    /* Computes pow(rXMP, YMP), result in RESULT */
    MPPOW(xmp, ymp, params, result, &ifail);

   reldiff = MPRELDIFF(z, base, mantis, emin, emax,
                      result, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;


    MPTOD(result, params, &ret, &ifail);

    free(xmp);
    free(ymp);
    free(result);

    return ret;
}
