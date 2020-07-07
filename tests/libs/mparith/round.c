#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_ROUND alm_mp_roundf
#define FUNC_ROUND_ULP alm_mp_roundf_ULP
#elif defined(DOUBLE)
#define FUNC_ROUND alm_mp_round
#define FUNC_ROUND_ULP alm_mp_round_ULP
#else
#error
#endif

REAL FUNC_ROUND_ULP123(REAL x,REAL z, double   *sulps, double   *sreldiff)
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
    /* Sets YMP to be the round function of XMP. */
    MPROUND(xmp, ymp, params, &ifail);

   reldiff = MPRELDIFF(z, base, mantis, emin, emax,
                      ymp, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;


    MPTOD(ymp, params, &y, &ifail);

    free(xmp);
    free(ymp);

    return y;
}



#include <mpfr.h>

REAL FUNC_ROUND(REAL x)
{
    REAL y;

    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t mpx, mp_rop;

    mpfr_inits2(256, mpx, mp_rop, (mpfr_ptr) 0);

#if defined(FLOAT)
    mpfr_set_flt(mpx, x, rnd);
#elif defined(DOUBLE)
    mpfr_set_d(mpx, x, rnd);
#endif

    mpfr_round(mp_rop, mpx);

#if defined(FLOAT)
    y = mpfr_get_flt(mpx, rnd);
#elif defined(DOUBLE)
    y = mpfr_get_d(mpx, rnd);
#endif

    mpfr_clears(mpx, mp_rop, (mpfr_ptr) 0);

    return y;
}

