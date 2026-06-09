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
#define FUNC_CDFNORMINV alm_mp_cdfnorminvf

#elif defined(DOUBLE)
#define FUNC_CDFNORMINV alm_mp_cdfnorminv

#else
#error
#endif

#include <mpfr.h>
#include <math.h>
#include "erfinv.h"

/*
 * MPFR does not provide mpfr_cdfnorminv; Solve Φ(y) = x for y.
 * y_{n+1} = y_n - (Φ(y_n) - x) / φ(y_n)
 * where φ(y) = (1/√(2π)) * exp(-y²/2) is the derivative of cdfnorm Φ(y)
 * using Newton-Raphson iteration.
 */
static struct
{
    const double sqrt_2;
    const double one_over_sqrt_2pi;
    const double sqrt_2pi;

} cdfnorminv_mpfr_data = {
    .sqrt_2 = 0x1.6a09e667f3bcdp+0,              /* √2 */
    .one_over_sqrt_2pi = 0x1.9884533d43651p-2,   /* 1/√(2π) */
    .sqrt_2pi = 0x1.40d931ff62705p+1             /* √(2π) */
};

#define SQRT_2 cdfnorminv_mpfr_data.sqrt_2
#define ONE_OVER_SQRT_2PI cdfnorminv_mpfr_data.one_over_sqrt_2pi
#define SQRT_2PI cdfnorminv_mpfr_data.sqrt_2pi

/* Helper function: Compute cdfnorm using MPFR */
static int mpfr_cdfnorm(mpfr_ptr rop, mpfr_srcptr x, mpfr_rnd_t rnd) {
    if (mpfr_nan_p(x)) {
        mpfr_set(rop, x, rnd);
        return 0;
    }
    
    mpfr_prec_t prec = mpfr_get_prec(rop);
    mpfr_t x_div_sqrt2, sqrt2, half, one, tmp;
    
    mpfr_init2(x_div_sqrt2, prec);
    mpfr_init2(sqrt2, prec);
    mpfr_init2(half, prec);
    mpfr_init2(one, prec);
    mpfr_init2(tmp, prec);
    
    mpfr_set_d(half, 0.5, rnd);
    mpfr_set_ui(one, 1u, rnd);
    mpfr_sqrt_ui(sqrt2, 2u, rnd);
    
    mpfr_div(x_div_sqrt2, x, sqrt2, rnd);
    
    if (mpfr_sgn(x) < 0) {
        mpfr_neg(x_div_sqrt2, x_div_sqrt2, rnd);
        mpfr_erfc(rop, x_div_sqrt2, rnd);
        mpfr_mul(rop, rop, half, rnd);
    } else {
        mpfr_erf(tmp, x_div_sqrt2, rnd);
        mpfr_add(rop, tmp, one, rnd);
        mpfr_mul(rop, rop, half, rnd);
    }
    
    mpfr_clear(x_div_sqrt2);
    mpfr_clear(sqrt2);
    mpfr_clear(half);
    mpfr_clear(one);
    mpfr_clear(tmp);
    
    return 0;
}

/* Provide initial guess for cdfnorminv :
 * Region-based initial guess
 * Domain: p ∈ (0, 1)
 * 
 * Strategy:
 * - Lower tail (p < 1/1024): cdfnorminv(p) = -√2 * erfcinv(2p)
 * - Mid-range (1/1024 ≤ p ≤ 1023/1024): cdfnorminv(p) = √2 * erfinv(2p - 1)
 *   For most of the range, erfinv gives good results as it uses higher FP density around 0
 * - Upper tail (p > 1023/1024): cdfnorminv(p) = -√2 * erfcinv(2p)
 * - Very close to 0.5: Linear approximation
 */
static void initial_guess_cdfnorminv(mpfr_ptr y, mpfr_srcptr p) {
    mpfr_prec_t prec = mpfr_get_prec(y);
    mpfr_t half, threshold_low, threshold_high, threshold_linear, tmp, p_diff;
    
    mpfr_init2(half, prec);
    mpfr_init2(threshold_low, prec);
    mpfr_init2(threshold_high, prec);
    mpfr_init2(threshold_linear, prec);
    mpfr_init2(tmp, prec);
    mpfr_init2(p_diff, prec);
    
    mpfr_set_d(half, 0.5, MPFR_RNDN);
    mpfr_set_d(threshold_low, 0.0009765625, MPFR_RNDN);
    mpfr_set_d(threshold_high, 0.9990234375, MPFR_RNDN);
    mpfr_set_d(threshold_linear, 1e-8, MPFR_RNDN);
    
    /* cdfnorminv(0.5) = 0 */
    if (mpfr_cmp(p, half) == 0) {
        mpfr_set_d(y, 0.0, MPFR_RNDN);
        goto cleanup;
    }
    
    /* For very small |p - 0.5| (p near 0.5): 
     * Linear approximation: cdfnorminv(p) ≈ √(2π) * (p - 0.5) */
    mpfr_sub(p_diff, p, half, MPFR_RNDN);
    mpfr_abs(tmp, p_diff, MPFR_RNDN);
    if (mpfr_cmp(tmp, threshold_linear) < 0) {
        mpfr_set_d(tmp, SQRT_2PI, MPFR_RNDN);
        mpfr_mul(y, tmp, p_diff, MPFR_RNDN);
        goto cleanup;
    }
    
    /* Lower tail: p < 1/1024
     * Use cdfnorminv(p) = -√2 * erfcinv(2p) */
    if (mpfr_cmp(p, threshold_low) < 0) {
        mpfr_t two_p, erfcinv_guess, sqrt2;
        mpfr_init2(two_p, prec);
        mpfr_init2(erfcinv_guess, prec);
        mpfr_init2(sqrt2, prec);
        
        mpfr_mul_d(two_p, p, 2.0, MPFR_RNDN);
        initial_guess_erfcinv(erfcinv_guess, two_p);
        
        mpfr_set_d(sqrt2, SQRT_2, MPFR_RNDN);
        mpfr_mul(y, sqrt2, erfcinv_guess, MPFR_RNDN);
        mpfr_neg(y, y, MPFR_RNDN);
        
        mpfr_clear(two_p);
        mpfr_clear(erfcinv_guess);
        mpfr_clear(sqrt2);
        goto cleanup;
    }
    
    /* Upper tail: p > 1023/1024
     * Use cdfnorminv(p) = -√2 * erfcinv(2p) */
    if (mpfr_cmp(p, threshold_high) > 0) {
        mpfr_t two_p, erfcinv_guess, sqrt2;
        mpfr_init2(two_p, prec);
        mpfr_init2(erfcinv_guess, prec);
        mpfr_init2(sqrt2, prec);
        
        mpfr_mul_d(two_p, p, 2.0, MPFR_RNDN);
        initial_guess_erfcinv(erfcinv_guess, two_p);
        
        mpfr_set_d(sqrt2, SQRT_2, MPFR_RNDN);
        mpfr_mul(y, sqrt2, erfcinv_guess, MPFR_RNDN);
        mpfr_neg(y, y, MPFR_RNDN);
        
        mpfr_clear(two_p);
        mpfr_clear(erfcinv_guess);
        mpfr_clear(sqrt2);
        goto cleanup;
    }
    
    /* Mid-range: 1/1024 ≤ p ≤ 1023/1024
     * Use cdfnorminv(p) = √2 * erfinv(2p - 1) */
    {
        mpfr_t two_p_minus_1, erfinv_guess, sqrt2, two;
        mpfr_init2(two_p_minus_1, prec);
        mpfr_init2(erfinv_guess, prec);
        mpfr_init2(sqrt2, prec);
        mpfr_init2(two, prec);
        
        mpfr_set_d(two, 2.0, MPFR_RNDN);
        mpfr_mul(tmp, p, two, MPFR_RNDN);
        mpfr_sub_ui(two_p_minus_1, tmp, 1u, MPFR_RNDN);
        
        initial_guess_erfinv(erfinv_guess, two_p_minus_1);
        
        mpfr_set_d(sqrt2, SQRT_2, MPFR_RNDN);
        mpfr_mul(y, sqrt2, erfinv_guess, MPFR_RNDN);
        
        mpfr_clear(two_p_minus_1);
        mpfr_clear(erfinv_guess);
        mpfr_clear(sqrt2);
        mpfr_clear(two);
    }
    
cleanup:
    mpfr_clear(half);
    mpfr_clear(threshold_low);
    mpfr_clear(threshold_high);
    mpfr_clear(threshold_linear);
    mpfr_clear(tmp);
    mpfr_clear(p_diff);
}

/* Implement cdfnorminv using Newton-Raphson iteration */
static int mpfr_cdfnorminv(mpfr_ptr y, mpfr_srcptr p, mpfr_rnd_t rnd) {
    mpfr_t zero, one, half;
    mpfr_init2(zero, mpfr_get_prec(p));
    mpfr_init2(one, mpfr_get_prec(p));
    mpfr_init2(half, mpfr_get_prec(p));
    mpfr_set_d(zero, 0.0, MPFR_RNDN);
    mpfr_set_d(one, 1.0, MPFR_RNDN);
    mpfr_set_d(half, 0.5, MPFR_RNDN);
    
    if (mpfr_zero_p(p) || mpfr_sgn(p) < 0) {
        if (mpfr_zero_p(p)) {
            mpfr_set_inf(y, -1);
        } else {
            mpfr_set_nan(y);
        }
        mpfr_clear(zero); mpfr_clear(one); mpfr_clear(half);
        return mpfr_sgn(p) < 0 ? -1 : 0;
    }
    
    if (mpfr_cmp(p, one) == 0) { /* cdfnorminv(1) = +inf */
        mpfr_set_inf(y, 1);
        mpfr_clear(zero); mpfr_clear(one); mpfr_clear(half);
        return 0;
    }
    
    if (mpfr_cmp(p, one) > 0) { /* cdfnorminv(p > 1) = NaN */
        mpfr_set_nan(y);
        mpfr_clear(zero); mpfr_clear(one); mpfr_clear(half);
        return -1;
    }
    
    if (mpfr_cmp(p, half) == 0) { /* cdfnorminv(0.5) = 0 */
        mpfr_set_d(y, 0.0, MPFR_RNDN);
        mpfr_clear(zero); mpfr_clear(one); mpfr_clear(half);
        return 0;
    }
    
    mpfr_clear(zero); mpfr_clear(one); mpfr_clear(half);
    
    /* Working precision: use at least the precision of y, or 256 bits minimum */
    mpfr_prec_t PREC = mpfr_get_prec(y);
    if (PREC < 256) PREC = 256;
    
    const int MAX_ITERS = 25;
    
    mpfr_t my, mf, md, mtmp, one_over_sqrt_2pi, delta;
    mpfr_init2(my, PREC);
    mpfr_init2(mf, PREC);
    mpfr_init2(md, PREC);
    mpfr_init2(mtmp, PREC);
    mpfr_init2(one_over_sqrt_2pi, PREC);
    mpfr_init2(delta, PREC);
    
    mpfr_set_d(one_over_sqrt_2pi, ONE_OVER_SQRT_2PI, MPFR_RNDN);
    
    initial_guess_cdfnorminv(my, p);
    
    double y_prev_double = 0.0;
    
    for (int iter = 0; iter < MAX_ITERS; ++iter) {
        /* mf = Φ(my) - p */
        mpfr_cdfnorm(mf, my, MPFR_RNDN);
        mpfr_sub(mf, mf, p, MPFR_RNDN);
        
        /* Convergence check */
        double y_curr = mpfr_get_d(my, MPFR_RNDN);
        if (iter > 0 && y_curr == y_prev_double) {
            break;
        }
        y_prev_double = y_curr;
        
        /* Derivative: φ(y) = (1/√(2π)) * exp(-y²/2) */
        mpfr_mul(mtmp, my, my, MPFR_RNDN);
        mpfr_div_d(mtmp, mtmp, 2.0, MPFR_RNDN);
        mpfr_neg(mtmp, mtmp, MPFR_RNDN);
        mpfr_exp(mtmp, mtmp, MPFR_RNDN);
        mpfr_mul(md, one_over_sqrt_2pi, mtmp, MPFR_RNDN);
        
        /* delta = mf / md */
        mpfr_div(delta, mf, md, MPFR_RNDN);
        
        /* y = y - delta */
        mpfr_sub(my, my, delta, MPFR_RNDN);
        
        /* Check for overflow/inf */
        if (mpfr_inf_p(my)) {
            break;
        }
    }
    
    mpfr_set(y, my, rnd);
    
    mpfr_clear(my); mpfr_clear(mf); mpfr_clear(md);
    mpfr_clear(mtmp); mpfr_clear(one_over_sqrt_2pi);
    mpfr_clear(delta);
    
    return 0;
}

REAL_L FUNC_CDFNORMINV(REAL x)
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

    mpfr_cdfnorminv(mp_rop, mpx, rnd);

#if defined(FLOAT)
    y = mpfr_get_d(mp_rop, rnd);
#elif defined(DOUBLE)
    y = mpfr_get_ld(mp_rop, rnd);
#endif

    mpfr_clears (mpx, mp_rop, (mpfr_ptr) 0);
    return y;
}
