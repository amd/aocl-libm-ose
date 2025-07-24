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
 *   v_f64x4_t vrd4_acos(v_f64x4_t x)
 *
 * Spec:
 *   acos(0)    = pi/2
 *   acos(1)    = 0
 *   acos(-1)   = pi
 *   acos(+inf) = NaN
 *   acos(-inf) = NaN
 *
 *
 ********************************************
 * Implementation Notes
 * ---------------------
 * To compute vrd4_acos(v_f64x4_t x)
 *
 * Based on the value of x, acos(x) is calculated as,
 *
 * 1. If x > 0.5
 *      acos(x) = 2 * asin(sqrt((1 - x) / 2))
 *
 * 2. If x < -0.5
 *      acos(x) = pi - 2asin(sqrt((1 + x) / 2))
 *
 * 3. If x <= |0.5|
 *      acos(x) = pi / 2 - asin(x)
 *
 * 4. 	acos(-x) = 1 / 2 * pi + asin(x)
 *
 * asin(x) is calculated using the polynomial,
 *      x+C1*x^3+C2*x^5+C3*x^7+C4*x^9+C5*x^11+C6*x^13+C7*x^15+C8*x^17
 *       +C9*x^19+C10*x^21+C11*x^23+C12*x^25
 *
 * Max ULP of current implementation: 1.5
 *
 */


#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>

#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>

#include <stdbool.h>


static struct {
    v_f64x4_t half, max_arg, two;
    v_f64x4_t a[2], b[2], poly_asin[12];
    v_u64x4_t mask_64;
} v4_asin_data = {
    .half       = _MM_SET1_PD4(0x1p-1),
    .max_arg    = _MM_SET1_PD4(0x1p0),
    .two        = _MM_SET1_PD4(0x1p1),
    .mask_64    = {0x7FFFFFFFFFFFFFFFLL, 0x7FFFFFFFFFFFFFFFLL, 0x7FFFFFFFFFFFFFFFLL, 0x7FFFFFFFFFFFFFFFLL},
    .a          = {
                   _MM_SET1_PD4(0.0),
                   _MM_SET1_PD4(0x1.921fb54442d18p-1),
                  },
    .b          = {
                   _MM_SET1_PD4(0x1.921fb54442d18p0),
                   _MM_SET1_PD4(0x1.921fb54442d18p-1),
                  },
    /* Polynomial coefficients*/
    .poly_asin = {
                   _MM_SET1_PD4(0x1.55555555552aap-3),
                   _MM_SET1_PD4(0x1.333333337cbaep-4),
                   _MM_SET1_PD4(0x1.6db6db3c0984p-5),
                   _MM_SET1_PD4(0x1.f1c72dd86cbafp-6),
                   _MM_SET1_PD4(0x1.6e89d3ff33aa4p-6),
                   _MM_SET1_PD4(0x1.1c6d83ae664b6p-6),
                   _MM_SET1_PD4(0x1.c6e1568b90518p-7),
                   _MM_SET1_PD4(0x1.8f6a58977fe49p-7),
                   _MM_SET1_PD4(0x1.a6ab10b3321bp-8),
                   _MM_SET1_PD4(0x1.43305ebb2428fp-6),
                   _MM_SET1_PD4(-0x1.0e874ec5e3157p-6),
                   _MM_SET1_PD4(0x1.06eec35b3b142p-5)
                  },
};

#define ALM_V4_ACOS_HALF     v4_asin_data.half
#define ALM_V4_ACOS_MASK_64  v4_asin_data.mask_64
#define ALM_V4_ACOS_MAX_ARG  v4_asin_data.max_arg
#define ALM_ACOS_VEC_TWO     v4_asin_data.two

#define ALM_ACOS_HALF        0x1p-1
#define ALM_ACOS_ONE         0x1p0
#define ALM_ACOS_TWO         0x1p1


#define A v4_asin_data.a
#define B v4_asin_data.b

#define C1 v4_asin_data.poly_asin[0]
#define C2 v4_asin_data.poly_asin[1]
#define C3 v4_asin_data.poly_asin[2]
#define C4 v4_asin_data.poly_asin[3]
#define C5 v4_asin_data.poly_asin[4]
#define C6 v4_asin_data.poly_asin[5]
#define C7 v4_asin_data.poly_asin[6]
#define C8 v4_asin_data.poly_asin[7]
#define C9 v4_asin_data.poly_asin[8]
#define C10 v4_asin_data.poly_asin[9]
#define C11 v4_asin_data.poly_asin[10]
#define C12 v4_asin_data.poly_asin[11]


static inline bool
all_v4_u64_loop(v_u64x4_t cond)
{
    return !!(cond[0] & cond[1] & cond[2] & cond[3]);
}

static inline v_f64x4_t
acos_specialcase(v_f64x4_t _x, v_f64x4_t result, v_u64x4_t cond)
{
    return call_v4_f64(ALM_PROTO(acos), _x, result, cond);
}

v_f64x4_t
ALM_PROTO_OPT(vrd4_acos)(v_f64x4_t x)
{
    v_f64x4_t  z, poly, result, aux;
    v_u64x4_t  ux, sign, outofrange, cond1, cond2;

    int32_t coeff_index[4] = {0};

    ux   = as_v4_u64_f64 (x);

    /* Get absolute value of input */
    aux  = as_v4_f64_u64(ux & ALM_V4_ACOS_MASK_64);

    /* Get sign of the input value */
    sign = ux & ~ALM_V4_ACOS_MASK_64;

    /* Check for special case */
    /* if |x| >= 1 */
    outofrange = (v_u64x4_t)(aux >= ALM_V4_ACOS_MAX_ARG);

    /* if |x| > 0.5 */
    cond1      = (v_u64x4_t)(aux >  ALM_V4_ACOS_HALF);

    /* if |x| <= 0.5 */
    cond2      = (v_u64x4_t)(aux <= ALM_V4_ACOS_HALF);

    if(all_v4_u64_loop(cond1)) {

        z= ALM_V4_ACOS_HALF * (ALM_V4_ACOS_MAX_ARG - aux);

        aux[0] = -ALM_ACOS_TWO * sqrt(z[0]);
        aux[1] = -ALM_ACOS_TWO * sqrt(z[1]);
        aux[2] = -ALM_ACOS_TWO * sqrt(z[2]);
        aux[3] = -ALM_ACOS_TWO * sqrt(z[3]);

    } else if (all_v4_u64_loop(cond2)) {
        /* All elements <= 0.5: optimized vectorized direct path */
        coeff_index[0] = 1;
        coeff_index[1] = 1;
        coeff_index[2] = 1;
        coeff_index[3] = 1;
        z = aux * aux;  /* Vectorized x^2 computation */

    } else {

        outofrange = cond1 | outofrange;

        for (int i = 0; i < 4; i++) {

            if (aux[i] > ALM_ACOS_HALF) {
                z[i]   = ALM_ACOS_HALF * (ALM_ACOS_ONE - aux[i]);
                aux[i] = -ALM_ACOS_TWO * sqrt(z[i]);

            } else {
                /* For |x| <= 0.5: direct computation */
                coeff_index[i] = 1;  /* Use coefficient set 1 */
                z[i] = aux[i] * aux[i];
            }
        }
    }
    
    /*
     * Enhanced polynomial evaluation for better accuracy:
     * asin(x) ≈ x + C1*x^3 + C2*x^5 + C3*x^7 + C4*x^9 + C5*x^11 + C6*x^13 +
     *           C7*x^15 + C8*x^17 + C9*x^19 + C10*x^21 + C11*x^23 + C12*x^25
     *
     * Uses Horner's method for optimal numerical stability:
     * asin(x) ≈ x + x*z*(C1 + z*(C2 + z*(C3 + z*(C4 + z*(C5 + z*(C6 + 
     *           z*(C7 + z*(C8 + z*(C9 + z*(C10 + z*(C11 + z*C12)))))))))))
     * where z = x^2
     */
    poly = POLY_EVAL_12(z, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12);
    poly = aux + aux * z * poly;
    
    result[0] = sign[0] ? (B[coeff_index[0]][0] + poly[0]) + B[coeff_index[0]][0] : (A[coeff_index[0]][0] - poly[0]) + A[coeff_index[0]][0];
    result[1] = sign[1] ? (B[coeff_index[1]][1] + poly[1]) + B[coeff_index[1]][1] : (A[coeff_index[1]][1] - poly[1]) + A[coeff_index[1]][1];
    result[2] = sign[2] ? (B[coeff_index[2]][2] + poly[2]) + B[coeff_index[2]][2] : (A[coeff_index[2]][2] - poly[2]) + A[coeff_index[2]][2];
    result[3] = sign[3] ? (B[coeff_index[3]][3] + poly[3]) + B[coeff_index[3]][3] : (A[coeff_index[3]][3] - poly[3]) + A[coeff_index[3]][3];

    /*
     * Fall back to scalar acos implementation for special cases
     * This ensures perfect accuracy for edge cases like ±1, NaN, ±∞
     */
    if(unlikely(any_v4_u64_loop(outofrange)))
        return acos_specialcase(x, result, outofrange);

    return result;
}
