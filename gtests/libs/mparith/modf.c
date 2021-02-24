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

#include <mpfr.h>

REAL FUNC_MODF(REAL x, REAL *p)
{
    REAL y;

    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t mpx, mpy, mp_rop;

    mpfr_inits2(ALM_MP_PRECI_BITS, mpx, mpy, mp_rop, (mpfr_ptr) 0);

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

