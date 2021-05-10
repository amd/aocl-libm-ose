#include "precision.h"


#if defined(FLOAT)
#define FUNC_TRUNC alm_mp_truncf

#elif defined(DOUBLE)
#define FUNC_TRUNC alm_mp_trunc

#else
#error
#endif

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

