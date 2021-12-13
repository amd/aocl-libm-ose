#include "precision.h"

#if defined(FLOAT)
#define FUNC_ATANH alm_mp_atanhf

#elif defined(DOUBLE)
#define FUNC_ATANH alm_mp_atanh

#else
#error
#endif

#include <mpfr.h>

REAL_L FUNC_ATANH(REAL x)
{
    REAL_L y;

    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t mpx, mp_rop;

    mpfr_inits2(ALM_MP_PRECI_BITS, mpx, mp_rop, (mpfr_ptr) 0);

#if defined(FLOAT)
    mpfr_set_d(mpx, x, rnd);
#elif defined(DOUBLE)
    mpfr_set_ld(mpx, x, rnd);
#endif

    mpfr_atanh(mp_rop, mpx, rnd);

#if defined(FLOAT)
    y = mpfr_get_d(mp_rop, rnd);
#elif defined(DOUBLE)
    y = mpfr_get_ld(mp_rop, rnd);
#endif

    mpfr_clears (mpx, mp_rop, (mpfr_ptr) 0);
    return y;
}

