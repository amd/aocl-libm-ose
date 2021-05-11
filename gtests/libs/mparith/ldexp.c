#include "precision.h"


#if defined(FLOAT)
#define FUNC_LDEXP alm_mp_ldexpf

#elif defined(DOUBLE)
#define FUNC_LDEXP alm_mp_ldexp

#else
#error
#endif

REAL_L FUNC_LDEXP(REAL x, int expn)
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
    MPLDEXP(xmp, expn, ymp, params, &ifail);
    MPTOD(ymp, params, &ret, &ifail);

    free(xmp);
    free(ymp);

    return ret;
}

REAL FUNC_LDEXP_ULP(REAL x, int expn, REAL z, double *sulps, double *sreldiff)
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
    MPLDEXP(xmp, expn, ymp, params, &ifail);
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

