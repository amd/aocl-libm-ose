#include "../nag/precision.h"
#include "../nag/mparith_c.h"

#if defined(FLOAT)
#define FUNC_FINITE amd_ref_finitef
#define FUNC_FINITE_ULP amd_ref_finitef_ULP
#elif defined(DOUBLE)
#define FUNC_FINITE amd_ref_finite
#define FUNC_FINITE_ULP amd_ref_finite_ULP
#else
#error
#endif

int FUNC_FINITE(REAL x)
{
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp,y;
    int ifail;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    y = MPFINITE(xmp, params, &ifail);

    free(xmp);
    return y;
}

int FUNC_FINITE_ULP(REAL x, int z, double   *sulps, double   *sreldiff)
{
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp,y;
    int ifail ;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    y = MPFINITE(xmp, params, &ifail);
/********/
    *sreldiff = (double)(y-z);
	if (y == z)
       *sulps = 0.0;
    else
       *sulps = 2.0;
         
/********/

    free(xmp);
    return y;
}
