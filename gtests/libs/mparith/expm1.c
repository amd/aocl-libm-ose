#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_EXPM1 alm_mp_expm1f
#define FUNC_EXPM1_ULP alm_mp_expm1f_ULP
#elif defined(DOUBLE)
#define FUNC_EXPM1 alm_mp_expm1
#define FUNC_EXPM1_ULP alm_mp_expm1_ULP
#else
#error
#endif

#include <mpfr.h>

REAL FUNC_EXPM1(REAL x)
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

    mpfr_expm1(mp_rop, mpx, rnd);

#if defined(FLOAT)
    y = mpfr_get_flt(mp_rop, rnd);
#elif defined(DOUBLE)
    y = mpfr_get_d(mp_rop, rnd);
#endif

    mpfr_clears (mpx, mp_rop, (mpfr_ptr) 0);
    return y;
}

