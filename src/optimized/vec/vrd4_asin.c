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
 *   v_f64x4_t vrd4_asin(v_f64x4_t x)
 *
 * Spec:
 *   asin(0)    = 0
 *   asin(1)    = pi/2
 *   asin(-1)   = -pi/2
 *   asin(+inf) = NaN
 *   asin(-inf) = NaN
 *   asin(x)    = NaN if |x| > 1
 *
 *
 ********************************************
 * Implementation Notes
 * ---------------------
 * To compute vrd4_asin(v_f64x4_t x)
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

/* Polynomial coefficients and constants for vrd4_asin */
static struct {
    v_f64x4_t piby2_tail, hpiby2_head, piby2;
    v_f64x4_t poly_asin_num[6];
    v_f64x4_t poly_asin_deno[5];
} v4_asin_data = {
    .piby2_tail = _MM_SET1_PD4(6.1232339957367660e-17),  /* 0x3c91a62633145c07 */
    .hpiby2_head = _MM_SET1_PD4(7.8539816339744831e-01), /* 0x3fe921fb54442d18 */
    .piby2       = _MM_SET1_PD4(1.5707963267948965e+00), /* 0x3ff921fb54442d18 */

    /* Rational polynomial coefficients - numerator */
    .poly_asin_num = {
        _MM_SET1_PD4(0.227485835556935010735943483075),
        _MM_SET1_PD4(-0.445017216867635649900123110649),
        _MM_SET1_PD4(0.275558175256937652532686256258),
        _MM_SET1_PD4(-0.0549989809235685841612020091328),
        _MM_SET1_PD4(0.00109242697235074662306043804220),
        _MM_SET1_PD4(0.0000482901920344786991880522822991),
    },

    /* Rational polynomial coefficients - denominator */
    .poly_asin_deno = {
        _MM_SET1_PD4(1.36491501334161032038194214209),
        _MM_SET1_PD4(-3.28431505720958658909889444194),
        _MM_SET1_PD4(2.76568859157270989520376345954),
        _MM_SET1_PD4(-0.943639137032492685763471240072),
        _MM_SET1_PD4(0.105869422087204370341222318533),
    },
};

#define PIBY2_TAIL  v4_asin_data.piby2_tail
#define PIBY2       v4_asin_data.piby2
#define HPIBY2_HEAD v4_asin_data.hpiby2_head

#define C1 v4_asin_data.poly_asin_num[0]
#define C2 v4_asin_data.poly_asin_num[1]
#define C3 v4_asin_data.poly_asin_num[2]
#define C4 v4_asin_data.poly_asin_num[3]
#define C5 v4_asin_data.poly_asin_num[4]
#define C6 v4_asin_data.poly_asin_num[5]

#define D1 v4_asin_data.poly_asin_deno[0]
#define D2 v4_asin_data.poly_asin_deno[1]
#define D3 v4_asin_data.poly_asin_deno[2]
#define D4 v4_asin_data.poly_asin_deno[3]
#define D5 v4_asin_data.poly_asin_deno[4]

#define SCALAR_ASIN ALM_PROTO(asin)

v_f64x4_t
ALM_PROTO_OPT(vrd4_asin)(v_f64x4_t x)
{
    v_f64x4_t u, v, poly_num, poly_deno;
    v_f64x4_t result = _MM_SET1_PD4(0.0);
    v_f64x4_t s = _MM_SET1_PD4(0.0);
    v_f64x4_t r = _MM_SET1_PD4(0.0);
    v_f64x4_t y = _MM_SET1_PD4(0.0);
    v_u64x4_t ux, aux, xneg;
    int transform[4] = {0, 0, 0, 0};

    ux = as_v4_u64_f64(x);

    aux = ux & ~SIGNBIT_DP64;
    xneg = ux & SIGNBIT_DP64;
   
    /* Extract exponent for each element - vectorized */
    v_u64x4_t exp_mask = {EXPBITS_DP64, EXPBITS_DP64, EXPBITS_DP64, EXPBITS_DP64};
    v_u64x4_t exp_bits = ux & exp_mask;
    
    /* Shift and convert to individual elements */
    v_i32x4_t xexp = {(int32_t)((exp_bits[0]) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64, 
                  (int32_t)((exp_bits[1]) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64, 
                  (int32_t)((exp_bits[2]) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64, 
                  (int32_t)((exp_bits[3]) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64};

    /* Vectorized special case detection */
    v_u64x4_t pinf_mask = {PINFBITPATT_DP64, PINFBITPATT_DP64, PINFBITPATT_DP64, PINFBITPATT_DP64};
    v_i32x4_t exp_min = {-28, -28, -28, -28};
    v_i32x4_t exp_zero = {0, 0, 0, 0};

    v_u64x4_t aux_gt_pinf = aux > pinf_mask;
    v_i32x4_t exp_lt_min = xexp < exp_min;
    v_i32x4_t exp_ge_zero = xexp >= exp_zero;

    /* Optimized special case check using vector OR operations */

    v_i32x4_t special_case = exp_lt_min | exp_ge_zero;

    if (special_case[0] || special_case[1] || special_case[2] || special_case[3] ||
        aux_gt_pinf[0] || aux_gt_pinf[1] || aux_gt_pinf[2] || aux_gt_pinf[3]) {
        result[0] = SCALAR_ASIN(x[0]);
        result[1] = SCALAR_ASIN(x[1]);
        result[2] = SCALAR_ASIN(x[2]);
        result[3] = SCALAR_ASIN(x[3]);
        return result;
    }

    y = as_v4_f64_u64(aux);

    for (int i = 0; i < 4; i++) {
        transform[i] = (xexp[i] >= -1);
        if (transform[i]) {
            r[i] = 0.5 * (1.0 - y[i]);
        } else {
            r[i] = y[i] * y[i];
        }
    }

    s = _mm256_sqrt_pd(r);

    poly_num = POLY_EVAL_6(r, C1, C2, C3, C4, C5, C6);
    poly_deno = POLY_EVAL_5(r, D1, D2, D3, D4, D5);
    u = r * poly_num / poly_deno;
    
    /* Reconstruct final result for all elements */
    for (int i = 0; i < 4; i++) {
        if (transform[i]) {
            /* Transform case: careful reconstruction */
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

        result[i] = xneg[i] ? -v[i] : v[i];
    }

    return result;
}
