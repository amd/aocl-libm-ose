#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_HYPOT alm_mp_hypotf
#define FUNC_HYPOT_ULP alm_mp_hypotf_ULP
#elif defined(DOUBLE)
#define FUNC_HYPOT alm_mp_hypot
#define FUNC_HYPOT_ULP alm_mp_hypot_ULP
#else
#error
#endif

REAL FUNC_HYPOT(REAL x, REAL y)
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
    /* Computes hypot(XMP, YMP), result in RESULT */
    MPHYPOT(xmp, ymp, result, params, &ifail);

	MPTOD(result, params, &ret, &ifail);

    free(xmp);
    free(ymp);
    free(result);

    return ret;
}

REAL FUNC_HYPOT_ULP(REAL x, REAL y,REAL z, double   *sulps, double   *sreldiff)
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
    /* Computes hypot(XMP, YMP), result in RESULT */
    MPHYPOT(xmp, ymp, result, params, &ifail);

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
