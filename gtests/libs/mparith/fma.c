#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_FMA alm_mp_fmaf
#define FUNC_FMA_ULP alm_mp_fmaf_ULP
#elif defined(DOUBLE)
#define FUNC_FMA alm_mp_fma
#define FUNC_FMA_ULP alm_mp_fma_ULP
#else
#error
#endif

#include <mpfr.h>

REAL FUNC_FMA(REAL x, REAL y, REAL z)
{
    REAL r;

    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t mpx, mpy, mpz, mp_rop;

    mpfr_inits2(ALM_MP_PRECI_BITS, mpx, mpy, mpz, mp_rop, (mpfr_ptr) 0);

#if defined(FLOAT)
    mpfr_set_flt(mpx, x, rnd);
    mpfr_set_flt(mpy, y, rnd);
    mpfr_set_flt(mpz, z, rnd);
#elif defined(DOUBLE)
    mpfr_set_d(mpx, x, rnd);
    mpfr_set_d(mpy, y, rnd);
    mpfr_set_d(mpz, z, rnd);
#endif

    mpfr_fma(mp_rop, mpx, mpy, mpz, rnd);

#if defined(FLOAT)
    r = mpfr_get_flt(mp_rop, rnd);
#elif defined(DOUBLE)
    r = mpfr_get_d(mp_rop, rnd);
#endif

    mpfr_clears (mpx, mpy, mpz, mp_rop, (mpfr_ptr) 0);
    return r;
}


