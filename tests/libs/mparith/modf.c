#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_MODF am_mp_modff
#define FUNC_MODF_ULP am_mp_modff_ULP
#elif defined(DOUBLE)
#define FUNC_MODF_ULP am_mp_modf_ULP
#define FUNC_MODF am_mp_modf
#else
#error
#endif

REAL FUNC_MODF(REAL x, REAL *ptr)
{
    REAL ret;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp, *iptr;
    int ifail;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);
    iptr = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    MPMODF(xmp, ymp, iptr, params, &ifail);

    MPTOD(iptr, params, ptr, &ifail);
    MPTOD(ymp, params, &ret, &ifail);

    free(xmp);
    free(ymp);
    free(iptr);

    return ret;
}

REAL FUNC_MODF_ULP(REAL x, REAL *ptr, REAL z, double   *sulps, double   *sreldiff)
{
    REAL ret;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp, *iptr;
    int ifail;
    REAL reldiff,ulps;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);
    iptr = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    MPMODF(xmp, ymp, iptr, params, &ifail);
/********/
    reldiff = MPRELDIFF(z, base, mantis, emin, emax,
                      ymp, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;

/********/

    MPTOD(iptr, params, ptr, &ifail);
    MPTOD(ymp, params, &ret, &ifail);

    free(xmp);
    free(ymp);
    free(iptr);

    return ret;
}
