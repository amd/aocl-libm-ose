#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_FABS alm_mp_fabsf
#define FUNC_FABS_ULP alm_mp_fabsf_ULP
#elif defined(DOUBLE)
#define FUNC_FABS alm_mp_fabs
#define FUNC_FABS_ULP alm_mp_fabs_ULP
#else
#error
#endif

REAL FUNC_FABS(REAL x)
{
    REAL y;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp;
    int ifail;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    MPABS(xmp, params, &ifail);
    MPTOD(xmp, params, &y, &ifail);

    free(xmp);

    return y;
}

REAL FUNC_FABS_ULP(REAL x, REAL z, double   *sulps, double   *sreldiff)
{
    REAL y;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp;
    int ifail;
    REAL reldiff,ulps;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    MPABS(xmp, params, &ifail);
/********/
    reldiff = MPRELDIFF(z, base, mantis, emin, emax,
                      xmp, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;

/********/
    MPTOD(xmp, params, &y, &ifail);

    free(xmp);

    return y;
}

