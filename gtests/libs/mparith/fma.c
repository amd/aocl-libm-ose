#include "precision.h"


#if defined(FLOAT)
#define FUNC_FMA alm_mp_fmaf

#elif defined(DOUBLE)
#define FUNC_FMA alm_mp_fma

#else
#error
#endif

#include <mpfr.h>

REAL_L FUNC_FMA(REAL x, REAL y, REAL z)
{
    REAL_L r;

    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t mpx, mpy, mpz, mp_rop;

    mpfr_inits2(ALM_MP_PRECI_BITS, mpx, mpy, mpz, mp_rop, (mpfr_ptr) 0);

#if defined(FLOAT)
    mpfr_set_d(mpx, x, rnd);
    mpfr_set_d(mpy, y, rnd);
    mpfr_set_d(mpz, z, rnd);
#elif defined(DOUBLE)
    mpfr_set_ld(mpx, x, rnd);
    mpfr_set_ld(mpy, y, rnd);
    mpfr_set_ld(mpz, z, rnd);
#endif

    mpfr_fma(mp_rop, mpx, mpy, mpz, rnd);

#if defined(FLOAT)
    r = mpfr_get_d(mp_rop, rnd);
#elif defined(DOUBLE)
    r = mpfr_get_ld(mp_rop, rnd);
#endif

    mpfr_clears (mpx, mpy, mpz, mp_rop, (mpfr_ptr) 0);
    return r;
}


