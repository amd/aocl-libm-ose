/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include "precision.h"


#if defined(FLOAT)
#define FUNC_ERFCINV alm_mp_erfcinvf

#elif defined(DOUBLE)
#define FUNC_ERFCINV alm_mp_erfcinv

#else
#error
#endif

#include <mpfr.h>
#include <math.h>
#include "erfinv.h"

/*
 * MPFR does not provide mpfr_erfcinv; Solve erfc(y) = x.
 * y_{n+1} = y_n - (erfc(y_n) - x) / ( -(2/sqrt(pi)) * exp(-y_n^2) )
 * using Newton-Raphson iteration.
 */

static int mpfr_erfcinv(mpfr_ptr y, mpfr_srcptr x, mpfr_rnd_t rnd) {
    mpfr_t one, two, zero;
    mpfr_init2(one, mpfr_get_prec(x));
    mpfr_init2(two, mpfr_get_prec(x));
    mpfr_init2(zero, mpfr_get_prec(x));
    mpfr_set_d(one, 1.0, MPFR_RNDN);
    mpfr_set_d(two, 2.0, MPFR_RNDN);
    mpfr_set_d(zero, 0.0, MPFR_RNDN);

    /* Check domain: x must be in (0, 2) */
    /* erfcinv(0) = +inf (both +0 and -0) */
    if (mpfr_zero_p(x)) {
        mpfr_set_inf(y, 1);  // +infinity
        mpfr_clear(one); mpfr_clear(two); mpfr_clear(zero);
        return 0;
    }
    
    /* erfcinv(x < 0) = NaN with invalid exception */
    if (mpfr_sgn(x) < 0) {
        mpfr_set_nan(y);  // +NaN (default)
        mpfr_clear(one); mpfr_clear(two); mpfr_clear(zero);
        return -1;  // Domain error
    }
    
    /* erfcinv(2) = -inf */
    if (mpfr_cmp(x, two) == 0) {
        mpfr_set_inf(y, -1);  // -infinity
        mpfr_clear(one); mpfr_clear(two); mpfr_clear(zero);
        return 0;
    }
    
    /* erfcinv(x > 2) = NaN with invalid exception */
    if (mpfr_cmp(x, two) > 0) {
        mpfr_set_nan(y);  // +NaN (default)
        mpfr_clear(one); mpfr_clear(two); mpfr_clear(zero);
        return -1;  // Domain error
    }

    /* erfcinv(1) = 0 */
    if (mpfr_cmp(x, one) == 0) {
        mpfr_set_d(y, 0.0, MPFR_RNDN);
        mpfr_clear(one); mpfr_clear(two); mpfr_clear(zero);
        return 0;
    }

    mpfr_clear(one); mpfr_clear(two); mpfr_clear(zero);

    /* Working precision: use at least the precision of y, or 256 bits minimum */
    mpfr_prec_t PREC = mpfr_get_prec(y);
    if (PREC < 256) PREC = 256;
    
    const int MAX_ITERS = 25;

    mpfr_t my, mf, md, mtmp, two_over_sqrtpi, delta;
    mpfr_init2(my, PREC);
    mpfr_init2(mf, PREC);
    mpfr_init2(md, PREC);
    mpfr_init2(mtmp, PREC);
    mpfr_init2(two_over_sqrtpi, PREC);
    mpfr_init2(delta, PREC);

    /* 2 / sqrt(pi) */
    mpfr_const_pi(mtmp, MPFR_RNDN);
    mpfr_sqrt(mtmp, mtmp, MPFR_RNDN);
    mpfr_set_d(two_over_sqrtpi, 2.0, MPFR_RNDN);
    mpfr_div(two_over_sqrtpi, two_over_sqrtpi, mtmp, MPFR_RNDN);

    /* Initial guess using high-precision MPFR operations */
    initial_guess_erfcinv(my, x);

    /* Store previous double-precision value for convergence check */
    double y_prev_double = 0.0;
    
    /* Newton-Raphson iteration */
    for (int iter = 0; iter < MAX_ITERS; ++iter) {
        /* mf = erfc(my) - x */
        mpfr_erfc(mf, my, MPFR_RNDN);
        mpfr_sub(mf, mf, x, MPFR_RNDN);

        /* Convergence check: Has the double-precision representation stabilized?
         * If y rounded to double hasn't changed, we've converged for double precision */
        double y_curr = mpfr_get_d(my, MPFR_RNDN);
        if (iter > 0 && y_curr == y_prev_double) {
            break;
        }
        y_prev_double = y_curr;

        /* derivative: -(2/sqrt(pi))*exp(-y^2) */
        mpfr_mul(mtmp, my, my, MPFR_RNDN);
        mpfr_neg(mtmp, mtmp, MPFR_RNDN);
        mpfr_exp(mtmp, mtmp, MPFR_RNDN);
        mpfr_mul(md, two_over_sqrtpi, mtmp, MPFR_RNDN);
        mpfr_neg(md, md, MPFR_RNDN);

        /* delta = mf / derivative */
        mpfr_div(delta, mf, md, MPFR_RNDN);

        /* y = y - delta */
        mpfr_sub(my, my, delta, MPFR_RNDN);
        
        /* Check for overflow/inf */
        if (mpfr_inf_p(my)) {
            break;
        }
    }

    /* Store result in output parameter with requested rounding mode */
    mpfr_set(y, my, rnd);

    mpfr_clear(my); mpfr_clear(mf); mpfr_clear(md);
    mpfr_clear(mtmp); mpfr_clear(two_over_sqrtpi); 
    mpfr_clear(delta);
    
    return 0;
}

REAL_L FUNC_ERFCINV(REAL x)
{
    REAL_L y;

    /* NaN handling outside MPFR:
     * mpfr_get_d/mpfr_get_ld may return an implementation-defined NaN
     * (sign/payload can change). Returning input preserves sign/payload. */
    if (isnan(x)) {
        return (REAL_L)x;
    }

    mpfr_rnd_t rnd = MPFR_RNDN;
    mpfr_t mpx, mp_rop;

    mpfr_inits2(ALM_MP_PRECI_BITS, mpx, mp_rop, (mpfr_ptr) 0);

#if defined(FLOAT)
    mpfr_set_d(mpx, x, rnd);
#elif defined(DOUBLE)
    mpfr_set_ld(mpx, x, rnd);
#endif

    mpfr_erfcinv(mp_rop, mpx, rnd);

#if defined(FLOAT)
    y = mpfr_get_d(mp_rop, rnd);
#elif defined(DOUBLE)
    y = mpfr_get_ld(mp_rop, rnd);
#endif

    mpfr_clears (mpx, mp_rop, (mpfr_ptr) 0);
    return y;
}
