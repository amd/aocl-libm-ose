#include "../nag/precision.h"
#include "../nag/mparith_c.h"

#if defined(FLOAT)
#define FUNC_REMAINDER amd_ref_remainderf
#define FUNC_REMAINDER_ULP amd_ref_remainderf_ULP
#elif defined(DOUBLE)
#define FUNC_REMAINDER amd_ref_remainder
#define FUNC_REMAINDER_ULP amd_ref_remainder_ULP
#else
#error
#endif

REAL FUNC_REMAINDER(REAL x, REAL y)
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
    /* Computes remainder(XMP, YMP), result in RESULT */
    MPREMAINDER(xmp, ymp, params, result, &ifail);

    MPTOD(result, params, &ret, &ifail);

    free(xmp);
    free(ymp);
    free(result);

    return ret;
}

REAL FUNC_REMAINDER_ULP(REAL x, REAL y,REAL z, double   *sulps, double   *sreldiff)
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
    /* Computes remainder(XMP, YMP), result in RESULT */
    MPREMAINDER(xmp, ymp, params, result, &ifail);

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