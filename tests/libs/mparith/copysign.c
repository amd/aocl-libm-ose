#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_COPYSIGN am_mp_copysignf
#define FUNC_COPYSIGN_ULP am_mp_copysignf_ULP
#elif defined(DOUBLE)
#define FUNC_COPYSIGN am_mp_copysign
#define FUNC_COPYSIGN_ULP am_mp_copysign_ULP
#else
#error
#endif

REAL FUNC_COPYSIGN(REAL x, REAL y)
{
    REAL ret;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp, *result;
    int ifail;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);
    result = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    DTOMP(y, ymp, params, &ifail);
    MPCOPYSIGN(xmp, ymp, result, params, &ifail);

    MPTOD(result, params, &ret, &ifail);

    free(xmp);
    free(ymp);
    free(result);

    return ret;
}

REAL FUNC_COPYSIGN_ULP(REAL x, REAL y,REAL z,double   *sulps, double   *sreldiff)
{
    REAL ret;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp, *result;
    int ifail;
    REAL reldiff,ulps;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);
    result = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    DTOMP(y, ymp, params, &ifail);
    MPCOPYSIGN(xmp, ymp, result, params, &ifail);

	reldiff = MPRELDIFF(z, base, mantis, emin, emax,
						 result, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;



    MPTOD(result, params, &ret, &ifail);

    free(xmp);
    free(ymp);
    free(result);

    return ret;
}