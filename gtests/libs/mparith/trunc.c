#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_TRUNC alm_mp_truncf
#define FUNC_TRUNC_ULP alm_mp_truncf_ULP
#elif defined(DOUBLE)
#define FUNC_TRUNC alm_mp_trunc
#define FUNC_TRUNC_ULP alm_mp_trunc_ULP
#else
#error
#endif

REAL FUNC_TRUNC_ULP123(REAL x,REAL z,double   *sulps, double   *sreldiff)
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
    MPTRUNC(xmp, ymp, params, &ifail);

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

REAL FUNC_TRUNC(REAL x)
{
    REAL y;

    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t mpx, mp_rop;

    mpfr_inits2(ALM_MP_PRECI_BITS, mpx, mp_rop, (mpfr_ptr) 0);

#if defined(FLOAT)
    mpfr_set_flt(mpx, x, rnd);
#elif defined(DOUBLE)
    mpfr_set_d(mpx, x, rnd);
#endif

    mpfr_trunc(mp_rop, mpx);

#if defined(FLOAT)
    y = mpfr_get_flt(mpx, rnd);
#elif defined(DOUBLE)
    y = mpfr_get_d(mpx, rnd);
#endif

    mpfr_clears (mpx, mp_rop, (mpfr_ptr) 0);
    return y;
}

