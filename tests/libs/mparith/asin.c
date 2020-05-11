#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_ASIN alm_mp_asinf
#define FUNC_ASIN_ULP alm_mp_asinf_ULP
#elif defined(DOUBLE)
#define FUNC_ASIN alm_mp_asin
#define FUNC_ASIN_ULP alm_mp_asin_ULP
#else
#error
#endif

REAL FUNC_ASIN(REAL x)
{
    REAL y;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp;
    int ifail;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    MPASIN(xmp, params, ymp, &ifail);

    MPTOD(ymp, params, &y, &ifail);

    free(xmp);
    free(ymp);

    return y;
}


REAL FUNC_ASIN_ULP(REAL x,REAL z, double   *sulps, double   *sreldiff)
{
    REAL y;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp;
    int ifail;
    REAL reldiff,ulps;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    MPASIN(xmp, params, ymp, &ifail);
	reldiff = MPRELDIFF(z, base, mantis, emin, emax,
						 ymp, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;


    MPTOD(ymp, params, &y, &ifail);

    free(xmp);
    free(ymp);

    return y;
}
