#include "../nag/precision.h"
#include "../nag/mparith_c.h"

#if defined(FLOAT)
#define FUNC_EXPM1 amd_ref_expm1f
#define FUNC_EXPM1_ULP amd_ref_expm1f_ULP
#elif defined(DOUBLE)
#define FUNC_EXPM1 amd_ref_expm1
#define FUNC_EXPM1_ULP amd_ref_expm1_ULP
#else
#error
#endif

REAL FUNC_EXPM1(REAL x)
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
    MPEXPM1(xmp, params, ymp, &ifail);


    MPTOD(ymp, params, &y, &ifail);

    free(xmp);
    free(ymp);

    return y;
}

REAL FUNC_EXPM1_ULP(REAL x,REAL z, double   *sulps, double   *sreldiff)
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
    MPEXPM1(xmp, params, ymp, &ifail);

   reldiff = MPRELDIFF(z, base, mantis, emin, emax,
                      ymp, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;


    MPTOD(ymp, params, &y, &ifail);

    free(xmp);
    free(ymp);

    return y;
}
