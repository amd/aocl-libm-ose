#include "precision.h"


#if defined(FLOAT)
#define FUNC_FMOD alm_mp_fmodf

#elif defined(DOUBLE)
#define FUNC_FMOD alm_mp_fmod

#else
#error
#endif

#include <mpfr.h>

REAL_L FUNC_FMOD(REAL x, REAL y)
{
    REAL_L y1;

    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t mpx, mpy, mp_rop;

    mpfr_inits2(ALM_MP_PRECI_BITS, mpx, mpy, mp_rop, (mpfr_ptr) 0);

#if defined(FLOAT)
    mpfr_set_d(mpx, x, rnd);
    mpfr_set_d(mpy, y, rnd);
#elif defined(DOUBLE)
    mpfr_set_ld(mpx, x, rnd);
    mpfr_set_ld(mpy, y, rnd);
#endif

    mpfr_fmod(mp_rop, mpx, mpy, rnd);

#if defined(FLOAT)
    y1 = mpfr_get_d(mp_rop, rnd);
#elif defined(DOUBLE)
    y1 = mpfr_get_ld(mp_rop, rnd);
#endif

    mpfr_clears (mpx, mpy, mp_rop, (mpfr_ptr) 0);
    return y1;
}


