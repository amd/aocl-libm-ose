#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_REMQUO am_mp_remquof
#define FUNC_REMQUO_ULP am_mp_remquof_ULP
#elif defined(DOUBLE)
#define FUNC_REMQUO am_mp_remquo
#define FUNC_REMQUO_ULP am_mp_remquo_ULP
#else
#error
#endif

REAL FUNC_REMQUO(REAL x, REAL y, int* quotient)
{
    REAL ret;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp, *qmp, *result;
    int ifail;
    REAL  temp_quotient;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);
    qmp = new_mp(params);
    result = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    DTOMP(y, ymp, params, &ifail);
    DTOMP(y, qmp, params, &ifail);
    /* Computes remquo(XMP, YMP, &QMP), result in *QMP and RESULT */
    MPREMQUO(xmp, ymp, qmp, params, result, &ifail);

    MPTOD(result, params, &ret, &ifail);
    /* Return integer part of number XMP */
    
    MPTOD(qmp, params, &temp_quotient, &ifail);
    *quotient = (int)temp_quotient;
    
    free(xmp);
    free(ymp);
    free(qmp);
    free(result);

    return ret;
}


REAL FUNC_REMQUO_ULP(REAL x, REAL y, int* quotient, REAL z, double * sulps, double *sreldiff)
{
    REAL ret;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp, *qmp, *result;
    int ifail;
    REAL temp_quotient;
    REAL ulps, reldiff;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);
    qmp = new_mp(params);
    result = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    DTOMP(y, ymp, params, &ifail);
    DTOMP(y, qmp, params, &ifail);
    /* Computes remquo(XMP, YMP, &QMP), result in *QMP and RESULT */
    MPREMQUO(xmp, ymp, qmp, params, result, &ifail);
/********/
    reldiff = MPRELDIFF(z, base, mantis, emin, emax,
                      ymp, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;

/********/

    MPTOD(result, params, &ret, &ifail);
    /* Return integer part of number XMP */
    MPTOD(qmp, params, &temp_quotient, &ifail);
    *quotient = (int)temp_quotient;

    free(xmp);
    free(ymp);
    free(qmp);
    free(result);

    return ret;
}

