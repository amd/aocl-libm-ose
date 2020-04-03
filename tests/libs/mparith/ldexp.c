#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_LDEXP am_mp_ldexpf
#define FUNC_LDEXP_ULP am_mp_ldexpf_ULP
#elif defined(DOUBLE)
#define FUNC_LDEXP am_mp_ldexp
#define FUNC_LDEXP_ULP am_mp_ldexp_ULP
#else
#error
#endif

REAL FUNC_LDEXP(REAL x, int expn)
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

