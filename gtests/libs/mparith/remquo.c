#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_REMQUO alm_mp_remquof
#define FUNC_REMQUO_ULP alm_mp_remquof_ULP
#elif defined(DOUBLE)
#define FUNC_REMQUO alm_mp_remquo
#define FUNC_REMQUO_ULP alm_mp_remquo_ULP
#else
#error
#endif

REAL FUNC_REMQUO_ULP123(REAL x, REAL y, int* quotient, REAL z, double * sulps, double *sreldiff)
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



#include <mpfr.h>

REAL FUNC_REMQUO(REAL x, REAL y, int *quo)
{
    REAL y1;

    long l;
    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t mpx, mpy, mp_rop;

    mpfr_inits2(ALM_MP_PRECI_BITS, mpx, mpy, mp_rop, (mpfr_ptr) 0);

#if defined(FLOAT)
    mpfr_set_flt(mpx, x, rnd);
    mpfr_set_flt(mpy, y, rnd);
#elif defined(DOUBLE)
    mpfr_set_d(mpx, x, rnd);
    mpfr_set_d(mpy, y, rnd);
#endif

    mpfr_remquo(mp_rop, &l, mpx, mpy, rnd);

#if defined(FLOAT)
    y1 = mpfr_get_flt(mp_rop, rnd);
#elif defined(DOUBLE)
    y1 = mpfr_get_d(mp_rop, rnd);
#endif

    *quo = (int)l;

    mpfr_clears (mpx, mpy, mp_rop, (mpfr_ptr) 0);
    return y1;
}

