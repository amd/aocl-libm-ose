#include "precision.h"


#if defined(FLOAT)
#define FUNC_FDIM alm_mp_fdimf

#elif defined(DOUBLE)
#define FUNC_FDIM alm_mp_fdim

#else
#error
#endif

REAL_L FUNC_FDIM(REAL x, REAL y)
{
    REAL_L ret;
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
    /* Computes FDIM(XMP, YMP), result in RESULT */
    MPFDIM(xmp, ymp, params, result, &ifail);

    MPTOD(result, params, &ret, &ifail);

    free(xmp);
    free(ymp);
    free(result);

    return ret;
}

REAL FUNC_FDIM_ULP(REAL x, REAL y,REAL z, double   *sulps, double   *sreldiff)
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
    /* Computes FDIM(XMP, YMP), result in RESULT */
    MPFDIM(xmp, ymp, params, result, &ifail);

   reldiff = MPRELDIFF(z, base, mantis, emin, emax,
                      ymp, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;


    MPTOD(result, params, &ret, &ifail);

    free(xmp);
    free(ymp);
    free(result);

    return ret;
}
