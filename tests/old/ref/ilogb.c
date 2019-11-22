#include "../nag/precision.h"
#include "../nag/mparith_c.h"

#if defined(FLOAT)
#define FUNC_ILOGB amd_ref_ilogbf
#define FUNC_ILOGB_ULP amd_ref_ilogbf_ULP
#elif defined(DOUBLE)
#define FUNC_ILOGB amd_ref_ilogb
#define FUNC_ILOGB_ULP amd_ref_ilogb_ULP
#else
#error
#endif

int FUNC_ILOGB(REAL x)
{
    int y;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp;
    int ifail;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    y = MPILOGB(xmp, emin, params, &ifail);

	free(xmp);
    return y;
}

int FUNC_ILOGB_ULP(REAL x, int z, double   *sulps, double   *sreldiff)
{
    int y;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp;
    int ifail;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    y = MPILOGB(xmp, emin, params, &ifail);
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

