/*
 * Copyright (C) 2025 Advanced Micro Devices, Inc. All rights reserved.
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

/*
 * Signature:
 *   v_f64x2_t vrd2_asin(v_f64x2_t x)
 *
 * Spec:
 *   asin(0)    = 0
 *   asin(1)    = pi/2
 *   asin(-1)   = -pi/2
 *   asin(+inf) = NaN
 *   asin(-inf) = NaN
 *   asin(x)    = NaN if |x| > 1
 *
 ********************************************
 * Implementation Notes
 * ---------------------
 * To compute vrd2_asin(v_f64x2_t x)
 *
 * Based on the scalar asin implementation from asin.c:
 *
 * 1. For abs(x) <= 0.5:
 *      asin(x) = x + x^3*R(x^2)
 *      where R(x^2) is a rational minimax approximation to
 *      (asin(x) - x)/x^3.
 *
 * 2. For abs(x) > 0.5:
 *      asin(x) = pi/2 - 2*asin(sqrt((1-|x|)/2))
 *      using the identity and careful reconstruction.
 *
 * Uses the same polynomial coefficients as the scalar version.
 * Special cases are handled by calling the scalar asin function.
 */

#include <libm_util_amd.h>
#include <libm_macros.h>
#include <libm/typehelper-vec.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>
#include <stdbool.h>
#include <math.h>


/* Use the same polynomial coefficients as scalar asin.c */
static struct {
    v_f64x2_t piby2_tail, hpiby2_head, piby2;
    v_f64x2_t poly_asin_num[6];
    v_f64x2_t poly_asin_deno[5];
} v2_asin_data = {
    .piby2_tail     = _MM_SET1_PD2(6.1232339957367660e-17),  /* 0x3c91a62633145c07 */
    .hpiby2_head    = _MM_SET1_PD2(7.8539816339744831e-01), /* 0x3fe921fb54442d18 */
    .piby2          = _MM_SET1_PD2(1.5707963267948965e+00), /* 0x3ff921fb54442d18 */
    
    /* Rational polynomial coefficients - numerator */
    .poly_asin_num = {
        _MM_SET1_PD2(0.227485835556935010735943483075),
        _MM_SET1_PD2(-0.445017216867635649900123110649),
        _MM_SET1_PD2(0.275558175256937652532686256258),
        _MM_SET1_PD2(-0.0549989809235685841612020091328),
        _MM_SET1_PD2(0.00109242697235074662306043804220),
        _MM_SET1_PD2(0.0000482901920344786991880522822991),
    },
    
    /* Rational polynomial coefficients - denominator */
    .poly_asin_deno = {
        _MM_SET1_PD2(1.36491501334161032038194214209),
        _MM_SET1_PD2(-3.28431505720958658909889444194),
        _MM_SET1_PD2(2.76568859157270989520376345954),
        _MM_SET1_PD2(-0.943639137032492685763471240072),
        _MM_SET1_PD2(0.105869422087204370341222318533),
    },
};

#define PIBY2_TAIL  v2_asin_data.piby2_tail
#define PIBY2       v2_asin_data.piby2
#define HPIBY2_HEAD v2_asin_data.hpiby2_head

#define C1 v2_asin_data.poly_asin_num[0]
#define C2 v2_asin_data.poly_asin_num[1]
#define C3 v2_asin_data.poly_asin_num[2]
#define C4 v2_asin_data.poly_asin_num[3]
#define C5 v2_asin_data.poly_asin_num[4]
#define C6 v2_asin_data.poly_asin_num[5]

#define D1 v2_asin_data.poly_asin_deno[0]
#define D2 v2_asin_data.poly_asin_deno[1]
#define D3 v2_asin_data.poly_asin_deno[2]
#define D4 v2_asin_data.poly_asin_deno[3]
#define D5 v2_asin_data.poly_asin_deno[4]

#define SCALAR_ASIN ALM_PROTO(asin)

v_f64x2_t
ALM_PROTO_OPT(vrd2_asin)(v_f64x2_t x)
{
    v_f64x2_t u, v, poly_num, poly_deno;
    v_f64x2_t result = _MM_SET1_PD2(0.0);
    v_f64x2_t s = _MM_SET1_PD2(0.0);
    v_f64x2_t r = _MM_SET1_PD2(0.0);
    v_f64x2_t y = _MM_SET1_PD2(0.0);
    v_u64x2_t ux, aux, xneg;
    int32_t xexp[2];
    int transform[2] = {0, 0};
    
    ux = as_v2_u64_f64(x);
    
    /* Get absolute value and sign for each element */
    aux = ux & ~SIGNBIT_DP64;
    xneg = ux & SIGNBIT_DP64;
    
    /* Extract exponent for each element */
    xexp[0] = (int32_t)((ux[0] & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
    xexp[1] = (int32_t)((ux[1] & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
    
    /* Check if ANY element is a special case (0, 1, NaN, Inf, etc.) */
    if (aux[0] > PINFBITPATT_DP64 || xexp[0] < -28 || xexp[0] >= 0 ||
        aux[1] > PINFBITPATT_DP64 || xexp[1] < -28 || xexp[1] >= 0) {
        /* Special case: call scalar asin for both elements */
        result[0] = SCALAR_ASIN(x[0]);
        result[1] = SCALAR_ASIN(x[1]);
        return result;
    }
    
    /* All elements are normal cases - process vectorized */
    y = as_v2_f64_u64(aux); /* Get absolute value */

    for (int i = 0; i < 2; i++) {
        transform[i] = (xexp[i] >= -1); /* abs(x) >= 0.5 */
        if (transform[i]) {
            /* Transform: y = sqrt((1-|x|)/2) */
            r[i] = 0.5 * (1.0 - y[i]);
        } else {
            /* Direct case: r = y^2 */
            r[i] = y[i] * y[i];
        }
    }

    // Compute square roots required for Transform case only
    s = _mm_sqrt_pd(r);
        
    /* Compute polynomial approximation for all elements */
    poly_num = POLY_EVAL_6(r, C1, C2, C3, C4, C5, C6);
    poly_deno = POLY_EVAL_5(r, D1, D2, D3, D4, D5);
    u = r * poly_num / poly_deno;
    
    /* Reconstruct final result for all elements */
    for (int i = 0; i < 2; i++) {
        if (transform[i]) {
            /* Transform case: careful reconstruction as in scalar version */
            double c, s1, p, q;
            uint64_t us;
            
            us = asuint64(s[i]);
            s1 = asdouble(0xffffffff00000000ULL & us);
            
            c = (r[i] - s1 * s1) / (s[i] + s1);
            p = 2.0 * s[i] * u[i] - (PIBY2_TAIL[i] - 2.0 * c);
            q = HPIBY2_HEAD[i] - 2.0 * s1;
            v[i] = HPIBY2_HEAD[i] - (p - q);
        } else {
            /* Direct case */
#ifdef WINDOWS
            /* Use a temporary variable to prevent VC++ rearranging */
            /* y + y*u into y * (1 + u) and getting incorrectly rounded result */
            double tmp;
            tmp = y[i] * u[i];
            v[i] = y[i] + tmp;
#else
            v[i] = y[i] + y[i] * u[i];
#endif
        }
        
        /* Apply sign */
        if (xneg[i]) {
            result[i] = -v[i];
        } else {
            result[i] = v[i];
        }
    }
    
    return result;
}
