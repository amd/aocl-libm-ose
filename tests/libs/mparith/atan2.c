#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_ATAN2 alm_mp_atan2f
#define FUNC_ATAN2_ULP alm_mp_atan2f_ULP
#elif defined(DOUBLE)
#define FUNC_ATAN2 alm_mp_atan2
#define FUNC_ATAN2_ULP alm_mp_atan2_ULP
#else
#error
#endif

REAL FUNC_ATAN2(REAL x, REAL y)
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
    /* Computes ATAN2(XMP, YMP), result in RESULT */
    MPATAN2(xmp, ymp, params, result, &ifail);


    MPTOD(result, params, &ret, &ifail);

    free(xmp);
    free(ymp);
    free(result);

    return ret;
}

REAL FUNC_ATAN2_ULP(REAL x, REAL y,REAL z, double   *sulps, double   *sreldiff)
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
    /* Computes ATAN2(XMP, YMP), result in RESULT */
    MPATAN2(xmp, ymp, params, result, &ifail);

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