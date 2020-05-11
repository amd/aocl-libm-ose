#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_SQRT alm_mp_sqrtf
#define FUNC_SQRT_ULP alm_mp_sqrtf_ULP
#elif defined(DOUBLE)
#define FUNC_SQRT alm_mp_sqrt
#define FUNC_SQRT_ULP alm_mp_sqrt_ULP
#else
#error
#endif

REAL FUNC_SQRT(REAL x)
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
    MPROOT(xmp, ymp, params, &ifail);


    MPTOD(ymp, params, &y, &ifail);

    free(xmp);
    free(ymp);

    return y;
}

REAL FUNC_SQRT_ULP(REAL x,REAL z, double   *sulps, double   *sreldiff)
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
    MPROOT(xmp, ymp, params, &ifail);

   reldiff = MPRELDIFF(z, base, mantis, emin, emax,
                      ymp, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;


    MPTOD(ymp, params, &y, &ifail);

    free(xmp);
    free(ymp);

    return y;
}

