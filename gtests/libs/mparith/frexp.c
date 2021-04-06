#include "precision.h"


#if defined(FLOAT)
#define FUNC_FREXP alm_mp_frexpf

#elif defined(DOUBLE)
#define FUNC_FREXP alm_mp_frexp

#else
#error
#endif

REAL_L FUNC_FREXP(REAL x, int *ptr)
{
    REAL_L ret;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp;
    int ifail;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    MPFREXP(xmp, ymp, ptr, params, &ifail);

    MPTOD(ymp, params, &ret, &ifail);

    free(xmp);
    free(ymp);

    return ret;
}

REAL FUNC_FREXP_ULP123(REAL x, int *ptr, REAL z, double *sulps, double *sreldiff);

REAL FUNC_FREXP_ULP123(REAL x, int *ptr, REAL z, double *sulps, double *sreldiff)
{
    REAL ret;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp;
    int ifail;
    REAL reldiff,ulps;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    MPFREXP(xmp, ymp, ptr, params, &ifail);
/********/
    reldiff = MPRELDIFF(z, base, mantis, emin, emax,
                      ymp, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;

/********/

    MPTOD(ymp, params, &ret, &ifail);

    free(xmp);
    free(ymp);

    return ret;
}


