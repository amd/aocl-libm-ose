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

/* Common helper functions for erfinv, erfcinv and cdfnorminv */

#ifndef __MPFR_ERFINV_H__
#define __MPFR_ERFINV_H__

#include <mpfr.h>

/* Constants used by initial guess functions */
#ifndef SQRT_PI_BY_2
#define SQRT_PI_BY_2 0x1.c5bf891b4ef6bp-1  /* √π/2 */
#endif

#ifndef LOG_SQRT_PI
#define LOG_SQRT_PI  0x1.250d048e7a1bdp-1  /* ln(√π) */
#endif

/* Provide initial guess for erfinv */
static inline void initial_guess_erfinv(mpfr_ptr y, mpfr_srcptr x)
{
  /* Region-based initial guess
   * Domain: x ∈ (-1, 1)
   * erfinv(0) = 0
   * For x near 0       : Linear Approximation
   * For x near -1 or 1 : Asymptotic Approximation
   * Others             : Winitzki Rational Approximation
   */
  mpfr_prec_t prec = mpfr_get_prec(y);
  mpfr_t one, tmp, tmp2, abs_x;
  mpfr_t threshold_low, threshold_high, threshold_linear;

  mpfr_init2(one, prec);
  mpfr_init2(tmp, prec);
  mpfr_init2(tmp2, prec);
  mpfr_init2(abs_x, prec);
  mpfr_init2(threshold_low, prec);
  mpfr_init2(threshold_high, prec);
  mpfr_init2(threshold_linear, prec);

  mpfr_set_d(one, 1.0, MPFR_RNDN);
  mpfr_set_d(threshold_low, 0.99, MPFR_RNDN);   /* |x| > 0.99 for asymptotic */
  mpfr_set_d(threshold_high, 0.005, MPFR_RNDN); /* |x| < 0.005 for asymptotic tail */
  mpfr_set_d(threshold_linear, 1e-8, MPFR_RNDN);

  mpfr_abs(abs_x, x, MPFR_RNDN);

  /* erfinv(0) = 0 */
  if (mpfr_zero_p(x)) {
    mpfr_set_d(y, 0.0, MPFR_RNDN);
    goto cleanup_all;
  }

  /* For very small |x| near 0:
   *   Linear approximation using 1st term of Taylor series
   *   erfinv(x) ≈ (√π/2) * x
   */
  if (mpfr_cmp(abs_x, threshold_linear) < 0) {
    mpfr_set_d(tmp, SQRT_PI_BY_2, MPFR_RNDN);
    mpfr_mul(y, tmp, x, MPFR_RNDN);  /* y = ( (√π/2) * x ) */
    goto cleanup_all;
  }

  /* Tail region near x = 1:  erfinv → +∞
   *   Asymptotic expansion for large positive arguments
   *   For x → 1⁻, let w = 1-x, then:
   *   s = √(-ln(w)), y ≈ s - (ln(s) + ln(√π)) / (2s)
   */
  if (mpfr_cmp(abs_x, threshold_low) > 0 && mpfr_sgn(x) > 0) {
    mpfr_t s, corr, w;
    mpfr_init2(s, prec);
    mpfr_init2(corr, prec);
    mpfr_init2(w, prec);

    /* w = (1 - x) */
    mpfr_sub(w, one, x, MPFR_RNDN);

    /* s = sqrt( -ln(w) ) */
    mpfr_log(tmp, w, MPFR_RNDN);
    mpfr_neg(tmp, tmp, MPFR_RNDN);
    mpfr_sqrt(s, tmp, MPFR_RNDN);

    /* corr = ( ln(s) + LOG_SQRT_PI ) / (2s) */
    mpfr_log(tmp, s, MPFR_RNDN);
    mpfr_add_d(tmp, tmp, LOG_SQRT_PI, MPFR_RNDN);
    mpfr_mul_d(tmp2, s, 2.0, MPFR_RNDN);
    mpfr_div(corr, tmp, tmp2, MPFR_RNDN);

    /* y = (s - corr) */
    mpfr_sub(y, s, corr, MPFR_RNDN);

    mpfr_clear(s); mpfr_clear(corr); mpfr_clear(w);
    goto cleanup_all;
  }

  /* Tail region near x = -1:  erfinv → -∞
   *   Asymptotic expansion for large negative arguments
   *   For x → -1⁺, let w = 1+x, then:
   *   s = √(-ln(w)), y ≈ -( s - (ln(s) + ln(√π)) / (2s) )
   */
  if (mpfr_cmp(abs_x, threshold_low) > 0 && mpfr_sgn(x) < 0) {
    mpfr_t s, corr, w;
    mpfr_init2(s, prec);
    mpfr_init2(corr, prec);
    mpfr_init2(w, prec);

    /* w = 1+x (Here, x is negative) */
    mpfr_add(w, one, x, MPFR_RNDN);

    /* s = sqrt(-ln(w)) */
    mpfr_log(tmp, w, MPFR_RNDN);
    mpfr_neg(tmp, tmp, MPFR_RNDN);
    mpfr_sqrt(s, tmp, MPFR_RNDN);

    /* corr = ( ln(s) + LOG_SQRT_PI ) / (2s) */
    mpfr_log(tmp, s, MPFR_RNDN);
    mpfr_add_d(tmp, tmp, LOG_SQRT_PI, MPFR_RNDN);
    mpfr_mul_d(tmp2, s, 2.0, MPFR_RNDN);
    mpfr_div(corr, tmp, tmp2, MPFR_RNDN);

    /* y = -(s - corr) */
    mpfr_sub(y, s, corr, MPFR_RNDN);
    mpfr_neg(y, y, MPFR_RNDN);

    mpfr_clear(s); mpfr_clear(corr); mpfr_clear(w);
    goto cleanup_all;
  }

  /* Mid-range: Winitzki Rational Approximation
   *   For erfinv(x), use z=x directly
   *   ln1 = ln(1-z²)
   *   a = 0.147
   *   A = 2/(π*a) + ln1/2
   *   B = ln1/a
   *   y ≈ sign(z) * √(-A + √(A² - B))
   */
  {
    mpfr_t z, a_const, pi_const, A, B, z_squared;
    mpfr_init2(z, prec);
    mpfr_init2(a_const, prec);
    mpfr_init2(pi_const, prec);
    mpfr_init2(A, prec);
    mpfr_init2(B, prec);
    mpfr_init2(z_squared, prec);

    /* z = x */
    mpfr_set(z, x, MPFR_RNDN);
    int sign = mpfr_sgn(z);

    /* z_squared = z^2 */
    mpfr_mul(z_squared, z, z, MPFR_RNDN);

    /* ln1 = ln(1 - z^2) */
    mpfr_sub(tmp, one, z_squared, MPFR_RNDN);
    mpfr_log(tmp, tmp, MPFR_RNDN);

    /* a = 0.147 */
    mpfr_set_d(a_const, 0.147, MPFR_RNDN);

    /* A = 2/(pi*a) + ln1/2 */
    mpfr_const_pi(pi_const, MPFR_RNDN);
    mpfr_mul(tmp2, pi_const, a_const, MPFR_RNDN);
    mpfr_d_div(A, 2.0, tmp2, MPFR_RNDN);
    mpfr_div_d(tmp2, tmp, 2.0, MPFR_RNDN);
    mpfr_add(A, A, tmp2, MPFR_RNDN);

    /* B = ln1/a */
    mpfr_div(B, tmp, a_const, MPFR_RNDN);

    /* y = sqrt(sqrt(A^2 - B) - A) */
    mpfr_mul(tmp2, A, A, MPFR_RNDN);
    mpfr_sub(tmp2, tmp2, B, MPFR_RNDN);
    if (mpfr_cmp_d(tmp2, 0.0) < 0) {
      mpfr_set_d(tmp2, 0.0, MPFR_RNDN);
    }

    mpfr_sqrt(tmp, tmp2, MPFR_RNDN);
    mpfr_sub(tmp, tmp, A, MPFR_RNDN);
    mpfr_sqrt(y, tmp, MPFR_RNDN);

    /* Apply sign */
    if (sign < 0)
      mpfr_neg(y, y, MPFR_RNDN);

    mpfr_clear(z); mpfr_clear(a_const); mpfr_clear(pi_const);
    mpfr_clear(A); mpfr_clear(B); mpfr_clear(z_squared);
  }

cleanup_all:
  mpfr_clear(one); mpfr_clear(tmp); mpfr_clear(tmp2);
  mpfr_clear(abs_x); mpfr_clear(threshold_low); mpfr_clear(threshold_high);
  mpfr_clear(threshold_linear);
}


/* Provide initial guess for erfcinv */
static inline void initial_guess_erfcinv(mpfr_ptr y, mpfr_srcptr x) {
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

#endif /* __MPFR_ERFINV_H__ */
