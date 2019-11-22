#include "../nag/precision.h"
#include "../nag/mparith_c.h"

#if defined(FLOAT)
#define FUNC_COS amd_ref_cosf
#define FUNC_COS_ULP amd_ref_cosf_ULP
#elif defined(DOUBLE)
#define FUNC_COS amd_ref_cos
#define FUNC_COS_ULP amd_ref_cos_ULP
#else
#error
#endif

REAL FUNC_COS(REAL x)
{
    REAL y;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *result_sin,*result_cos,*result_tan;
    int ifail;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    result_sin = new_mp(params);
    result_cos = new_mp(params);
    result_tan = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    MPSINCOSTAN(xmp, params, result_sin, result_cos, result_tan, &ifail);

    MPTOD(result_cos, params, &y, &ifail);

    free(xmp);
    free(result_sin);
    free(result_cos);
    free(result_tan);

    return y;
}

REAL FUNC_COS_ULP(REAL x, REAL z, double   *sulps, double   *sreldiff)
{
    REAL y;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *result_sin,*result_cos,*result_tan;
    int ifail;
    REAL reldiff,ulps;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    result_sin = new_mp(params);
    result_cos = new_mp(params);
    result_tan = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    MPSINCOSTAN(xmp, params, result_sin, result_cos, result_tan, &ifail);
/********/
    reldiff = MPRELDIFF(z, base, mantis, emin, emax,
                      result_cos, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;

/********/

    MPTOD(result_cos, params, &y, &ifail);

    free(xmp);
    free(result_sin);
    free(result_cos);
    free(result_tan);

    return y;
}