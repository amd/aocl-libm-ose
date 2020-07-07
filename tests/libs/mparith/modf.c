#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_MODF alm_mp_modff
#define FUNC_MODF_ULP alm_mp_modff_ULP
#elif defined(DOUBLE)
#define FUNC_MODF_ULP alm_mp_modf_ULP
#define FUNC_MODF alm_mp_modf
#else
#error
#endif

REAL FUNC_MODF_ULP123(REAL x, REAL *ptr, REAL z, double   *sulps, double   *sreldiff)
{
    REAL ret;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *ymp, *iptr;
    int ifail;
    REAL reldiff,ulps;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    ymp = new_mp(params);
    iptr = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    MPMODF(xmp, ymp, iptr, params, &ifail);
/********/
    reldiff = MPRELDIFF(z, base, mantis, emin, emax,
                      ymp, params,&ulps, &ifail);
	*sreldiff = reldiff;
	*sulps = ulps;

/********/

    MPTOD(iptr, params, ptr, &ifail);
    MPTOD(ymp, params, &ret, &ifail);

    free(xmp);
    free(ymp);
    free(iptr);

    return ret;
}


#include <mpfr.h>

REAL FUNC_MODF(REAL x, REAL *p)
{
    REAL y;

    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t mpx, mpy, mp_rop;

    mpfr_inits2(256, mpx, mpy, mp_rop, (mpfr_ptr) 0);

#if defined(FLOAT)
    mpfr_set_flt(mpx, x, rnd);
#elif defined(DOUBLE)
    mpfr_set_d(mpx, x, rnd);
#endif

    mpfr_modf(mp_rop, mpy, mpx, rnd);

#if defined(FLOAT)
    y = mpfr_get_flt(mp_rop, rnd);
    *p = mpfr_get_flt(mpy, rnd);
#elif defined(DOUBLE)
    y = mpfr_get_d(mp_rop, rnd);
    *p = mpfr_get_d(mpy, rnd);
#endif

    mpfr_clears(mpx, mpy, mp_rop, (mpfr_ptr) 0);

    return y;
}

