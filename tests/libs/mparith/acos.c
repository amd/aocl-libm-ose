#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_ACOS am_mp_acosf
#define FUNC_ACOS_ULP am_mp_acosf_ULP
#elif defined(DOUBLE)
#define FUNC_ACOS am_mp_acos
#define FUNC_ACOS_ULP am_mp_acos_ULP
#else
#error
#endif


REAL FUNC_ACOS(REAL x)
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
    MPACOS(xmp, params, ymp, &ifail);
    MPTOD(ymp, params, &y, &ifail);

    free(xmp);
    free(ymp);

    return y;
}


REAL FUNC_ACOS_ULP(REAL x,REAL z, double   *sulps, double   *sreldiff)
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
    MPACOS(xmp, params, ymp, &ifail);
	reldiff = MPRELDIFF(z, base, mantis, emin, emax,
						 ymp, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;


    MPTOD(ymp, params, &y, &ifail);

    free(xmp);
    free(ymp);

    return y;
}

