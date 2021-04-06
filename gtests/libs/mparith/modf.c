#include "precision.h"


#if defined(FLOAT)
#define FUNC_MODF alm_mp_modff

#elif defined(DOUBLE)

#define FUNC_MODF alm_mp_modf
#else
#error
#endif

#include <mpfr.h>

REAL_L FUNC_MODF(REAL x, REAL *p)
{
    REAL_L y;

    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t mpx, mpy, mp_rop;

    mpfr_inits2(ALM_MP_PRECI_BITS, mpx, mpy, mp_rop, (mpfr_ptr) 0);

#if defined(FLOAT)
    mpfr_set_d(mpx, x, rnd);
#elif defined(DOUBLE)
    mpfr_set_ld(mpx, x, rnd);
#endif

    mpfr_modf(mp_rop, mpy, mpx, rnd);

#if defined(FLOAT)
    y = mpfr_get_d(mp_rop, rnd);
    *p = mpfr_get_d(mpy, rnd);
#elif defined(DOUBLE)
    y = mpfr_get_ld(mp_rop, rnd);
    *p = mpfr_get_ld(mpy, rnd);
#endif

    mpfr_clears(mpx, mpy, mp_rop, (mpfr_ptr) 0);

    return y;
}

