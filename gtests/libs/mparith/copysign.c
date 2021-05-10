#include "precision.h"

#if defined(FLOAT)
#define FUNC_COPYSIGN alm_mp_copysignf
#define FUNC_COPYSIGN_ULP alm_mp_copysignf_ULP
#elif defined(DOUBLE)
#define FUNC_COPYSIGN alm_mp_copysign
#define FUNC_COPYSIGN_ULP alm_mp_copysign_ULP
#else
#error
#endif

#include <mpfr.h>

REAL FUNC_COPYSIGN(REAL x, REAL y)
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

    mpfr_copysign(mp_rop, mpx, mpy, rnd);

#if defined(FLOAT)
    y1 = mpfr_get_flt(mp_rop, rnd);
#elif defined(DOUBLE)
    y1 = mpfr_get_d(mp_rop, rnd);
#endif

    mpfr_clears (mpx, mpy, mp_rop, (mpfr_ptr) 0);

    return y1;
}

