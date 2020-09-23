#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_REMAINDER alm_mp_remainderf
#define FUNC_REMAINDER_ULP alm_mp_remainderf_ULP
#elif defined(DOUBLE)
#define FUNC_REMAINDER alm_mp_remainder
#define FUNC_REMAINDER_ULP alm_mp_remainder_ULP
#else
#error
#endif


REAL FUNC_REMAINDER_ULP123(REAL x, REAL y,REAL z, double   *sulps, double   *sreldiff)
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
    /* Computes remainder(XMP, YMP), result in RESULT */
    MPREMAINDER(xmp, ymp, params, result, &ifail);

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

#include <mpfr.h>

REAL FUNC_REMAINDER(REAL x, REAL y)
{
    REAL y1;

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

    mpfr_remainder(mp_rop, mpx, mpy, rnd);

#if defined(FLOAT)
    y1 = mpfr_get_flt(mp_rop, rnd);
#elif defined(DOUBLE)
    y1 = mpfr_get_d(mp_rop, rnd);
#endif

    mpfr_clears (mpx, mpy, mp_rop, (mpfr_ptr) 0);
    return y1;
}

