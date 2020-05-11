#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_FREXP alm_mp_frexpf
#define FUNC_FREXP_ULP alm_mp_frexpf_ULP
#elif defined(DOUBLE)
#define FUNC_FREXP alm_mp_frexp
#define FUNC_FREXP_ULP alm_mp_frexp_ULP
#else
#error
#endif

REAL FUNC_FREXP(REAL x, int *ptr)
{
    REAL ret;
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

REAL FUNC_FREXP_ULP(REAL x, int *ptr, REAL z, double *sulps, double *sreldiff)
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
