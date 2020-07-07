#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_LOG1P alm_mp_log1pf
#define FUNC_LOG1P_ULP alm_mp_log1pf_ULP
#elif defined(DOUBLE)
#define FUNC_LOG1P alm_mp_log1p
#define FUNC_LOG1P_ULP alm_mp_log1p_ULP
#else
#error
#endif

REAL FUNC_LOG1P_ULP123(REAL x,REAL z, double   *sulps, double   *sreldiff)
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
    MPLOG1P(xmp, params, ymp, &ifail);
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

REAL FUNC_LOG1P(REAL x)
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

    mpfr_log1p(mp_rop, mpx, rnd);

#if defined(FLOAT)
    y = mpfr_get_flt(mp_rop, rnd);
#elif defined(DOUBLE)
    y = mpfr_get_d(mp_rop, rnd);
#endif

    mpfr_clears (mpx, mp_rop, (mpfr_ptr) 0);
    return y;
}

