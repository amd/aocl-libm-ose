#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_ATAN am_mp_atanf
#define FUNC_ATAN_ULP am_mp_atanf_ULP
#elif defined(DOUBLE)
#define FUNC_ATAN am_mp_atan
#define FUNC_ATAN_ULP am_mp_atan_ULP
#else
#error
#endif

REAL FUNC_ATAN(REAL x)
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
    MPATAN(xmp, params, ymp, &ifail);

    MPTOD(ymp, params, &y, &ifail);

    free(xmp);
    free(ymp);

    return y;
}

REAL FUNC_ATAN_ULP(REAL x,REAL z, double   *sulps, double   *sreldiff)
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
    MPATAN(xmp, params, ymp, &ifail);

	reldiff = MPRELDIFF(z, base, mantis, emin, emax,
						 ymp, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;


    MPTOD(ymp, params, &y, &ifail);

    free(xmp);
    free(ymp);

    return y;
}
