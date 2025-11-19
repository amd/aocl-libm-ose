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

/*
 * MPFR does not provide mpfr_erfcinv; Solve erfc(y) = x.
 * y_{n+1} = y_n - (erfc(y_n) - x) / ( -(2/sqrt(pi)) * exp(-y_n^2) )
 * using Newton-Raphson iteration.
 */
static struct
{
    const double sqrt_pi_by_2;
    const double log_sqrt_pi;

} erfcinv_mpfr_data = {
    .sqrt_pi_by_2 = 0x1.c5bf891b4ef6bp-1,
    .log_sqrt_pi = 0x1.250d048e7a1bdp-1
};

#define SQRT_PI_BY_2 erfcinv_mpfr_data.sqrt_pi_by_2
#define LOG_SQRT_PI erfcinv_mpfr_data.log_sqrt_pi

/* Provide initial guess for erfcinv */
static void initial_guess_erfcinv(mpfr_ptr y, mpfr_srcptr x) {
    /* Region-based initial guess
     * Domain: x ∈ (0, 2)
     * erfcinv(1) = 0
     * For x near 1 : Linear approximation
     * For x near 0 or 2 : Asymptotic approximation
     * Rest : Winitzki rational approximation
     */
    mpfr_prec_t prec = mpfr_get_prec(y);
    mpfr_t one, two, tmp, tmp2, x_diff;
    mpfr_t threshold_low, threshold_high, threshold_linear;
    
    mpfr_init2(one, prec);
    mpfr_init2(two, prec);
    mpfr_init2(tmp, prec);
    mpfr_init2(tmp2, prec);
    mpfr_init2(x_diff, prec);
    mpfr_init2(threshold_low, prec);
    mpfr_init2(threshold_high, prec);
    mpfr_init2(threshold_linear, prec);
    
    mpfr_set_d(one, 1.0, MPFR_RNDN);
    mpfr_set_d(two, 2.0, MPFR_RNDN);
    mpfr_set_d(threshold_low, 0.01, MPFR_RNDN);
    mpfr_set_d(threshold_high, 1.99, MPFR_RNDN);
    mpfr_set_d(threshold_linear, 1e-8, MPFR_RNDN);
    
    /* erfcinv(1) = 0 */
    if (mpfr_cmp(x, one) == 0) {
        mpfr_set_d(y, 0.0, MPFR_RNDN);
        goto cleanup_all;
    }
    
    /* Tail region near x = 0: erfcinv → +∞
     * Asymptotic expansion working directly with x to avoid cancellation
     * s = √(-ln(x/2)), y ≈ s - (ln(s) + ln(√π)) / (2s) */
    if (mpfr_cmp(x, threshold_low) < 0) {
        mpfr_t s, corr;
        mpfr_init2(s, prec);
        mpfr_init2(corr, prec);
        
        // s = sqrt(-ln(x/2))
        mpfr_div_d(tmp, x, 2.0, MPFR_RNDN);
        mpfr_log(tmp, tmp, MPFR_RNDN);
        mpfr_neg(tmp, tmp, MPFR_RNDN);
        mpfr_sqrt(s, tmp, MPFR_RNDN);
        
        // corr = (ln(s) + LOG_SQRT_PI) / (2s)
        mpfr_log(tmp, s, MPFR_RNDN);
        mpfr_add_d(tmp, tmp, LOG_SQRT_PI, MPFR_RNDN);
        mpfr_mul_d(tmp2, s, 2.0, MPFR_RNDN);
        mpfr_div(corr, tmp, tmp2, MPFR_RNDN);
        
        // y = s - corr
        mpfr_sub(y, s, corr, MPFR_RNDN);
        
        mpfr_clear(s); mpfr_clear(corr);
        goto cleanup_all;
    }
    
    /* Tail region near x = 2: erfcinv → -∞
     * Asymptotic expansion working directly with (2-x) to avoid cancellation
     * s = √(-ln((2-x)/2)), y ≈ -(s - (ln(s) + ln(√π)) / (2s)) */
    if (mpfr_cmp(x, threshold_high) > 0) {
        mpfr_t s, corr, x_complement;
        mpfr_init2(s, prec);
        mpfr_init2(corr, prec);
        mpfr_init2(x_complement, prec);
        
        // x_complement = 2 - x
        mpfr_sub(x_complement, two, x, MPFR_RNDN);
        
        // s = sqrt(-ln((2-x)/2))
        mpfr_div_d(tmp, x_complement, 2.0, MPFR_RNDN);
        mpfr_log(tmp, tmp, MPFR_RNDN);
        mpfr_neg(tmp, tmp, MPFR_RNDN);
        mpfr_sqrt(s, tmp, MPFR_RNDN);
        
        // corr = (ln(s) + LOG_SQRT_PI) / (2s)
        mpfr_log(tmp, s, MPFR_RNDN);
        mpfr_add_d(tmp, tmp, LOG_SQRT_PI, MPFR_RNDN);
        mpfr_mul_d(tmp2, s, 2.0, MPFR_RNDN);
        mpfr_div(corr, tmp, tmp2, MPFR_RNDN);
        
        // y = -(s - corr)
        mpfr_sub(y, s, corr, MPFR_RNDN);
        mpfr_neg(y, y, MPFR_RNDN);
        
        mpfr_clear(s); mpfr_clear(corr); mpfr_clear(x_complement);
        goto cleanup_all;
    }
    
    /* For very small |x-1| (x near 1): 
     * Linear approximation using 1st term of Taylor series
     * erfcinv(x) = erfinv(1-x), and for small z=1-x:
     * erfinv(z) ≈ (√π/2) * z
     * So: erfcinv(x) ≈ (√π/2) * (1-x) */
    mpfr_sub(x_diff, one, x, MPFR_RNDN);
    mpfr_abs(tmp, x_diff, MPFR_RNDN);
    if (mpfr_cmp(tmp, threshold_linear) < 0) {
        mpfr_set_d(tmp2, SQRT_PI_BY_2, MPFR_RNDN);
        mpfr_mul(y, tmp2, x_diff, MPFR_RNDN);  // y = (√π/2) * (1-x)
        goto cleanup_all;
    }
    
    /* Mid-range: Winitzki rational approximation
     * For erfcinv(x), use z = 1-x (converting to erfinv domain)
     * ln1 = ln(1 - z²)
     * a = 0.147
     * A = 2/(π*a) + ln1/2
     * B = ln1/a
     * y ≈ sign(z) * √(-A + √(-B + A^2)) */
    {
        mpfr_t z, a_const, pi_const, A, B, z_squared;
        mpfr_init2(z, prec);
        mpfr_init2(a_const, prec);
        mpfr_init2(pi_const, prec);
        mpfr_init2(A, prec);
        mpfr_init2(B, prec);
        mpfr_init2(z_squared, prec);
        
        // z = 1 - x
        mpfr_sub(z, one, x, MPFR_RNDN);
        int sign = mpfr_sgn(z);
        
        // z_squared = z^2
        mpfr_mul(z_squared, z, z, MPFR_RNDN);
        
        // ln1 = ln(1 - z^2)
        mpfr_sub(tmp, one, z_squared, MPFR_RNDN);
        mpfr_log(tmp, tmp, MPFR_RNDN);
        
        // a = 0.147
        mpfr_set_d(a_const, 0.147, MPFR_RNDN);
        
        // A = 2/(pi*a) + ln1/2
        mpfr_const_pi(pi_const, MPFR_RNDN);
        mpfr_mul(tmp2, pi_const, a_const, MPFR_RNDN);
        mpfr_d_div(A, 2.0, tmp2, MPFR_RNDN);
        mpfr_div_d(tmp2, tmp, 2.0, MPFR_RNDN);
        mpfr_add(A, A, tmp2, MPFR_RNDN);
        
        // B = ln1/a
        mpfr_div(B, tmp, a_const, MPFR_RNDN);
        
        // y = sqrt(sqrt(A^2 - B) - A)
        mpfr_mul(tmp2, A, A, MPFR_RNDN);
        mpfr_sub(tmp2, tmp2, B, MPFR_RNDN);
        if (mpfr_cmp_d(tmp2, 0.0) < 0) {
            mpfr_set_d(tmp2, 0.0, MPFR_RNDN);
        }
        mpfr_sqrt(tmp, tmp2, MPFR_RNDN);
        mpfr_sub(tmp, tmp, A, MPFR_RNDN);
        mpfr_sqrt(y, tmp, MPFR_RNDN);
        
        // Apply sign
        if (sign < 0) mpfr_neg(y, y, MPFR_RNDN);
        
        mpfr_clear(z); mpfr_clear(a_const); mpfr_clear(pi_const);
        mpfr_clear(A); mpfr_clear(B); mpfr_clear(z_squared);
    }
    
cleanup_all:
    mpfr_clear(one); mpfr_clear(two); mpfr_clear(tmp); mpfr_clear(tmp2);
    mpfr_clear(x_diff); mpfr_clear(threshold_low); mpfr_clear(threshold_high);
    mpfr_clear(threshold_linear);
}

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
