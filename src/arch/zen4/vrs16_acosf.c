/*
 * Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   v_f32x16_t vrs16_acosf(v_f32x16_t x)
 *
 * Spec:
 *   acosf(0)    = pi/2
 *   acosf(1)    = 0
 *   acosf(-1)   = pi
 *   acosf(+inf) = NaN
 *   acosf(-inf) = NaN
 *
 *
 ********************************************
 * Implementation Notes
 * ---------------------
 * To compute vrs16_acosf(v_f32x16_t x)
 *
 * Based on the value of x, acosf(x) is calculated as,
 *
 * 1. If x > 0.5
 *      acosf(x) = 2 * asinf(sqrt((1 - x) / 2))
 *
 * 2. If x < -0.5
 *      acosf(x) = pi - 2asinf(sqrt((1 + x) / 2))
 *
 * 3. If x <= |0.5|
 *      acosf(x) = pi / 2 - asinf(x)
 *
 * 4. 	acos(-x) = 1 / 2 * pi + asin(x)
 *
 * acosf(x) is calculated using the polynomial,
 *      x + C1*x^3 + C2*x^5 + C3*x^7 + C4*x^9 + C5*x^11
 *
 * Max ULP of current implementation: 1.5
 *
 */

#include <stdint.h>
#include <emmintrin.h>

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>

#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>
#include <libm/arch/zen4.h>

#include <stdbool.h>

static struct {
    v_f32x16_t piby2, pi;
    v_f32x16_t half, max_arg;
    v_f32x16_t a[2], b[2], poly_asinf[5];
    v_i32x16_t mask_32;
} v16_asinf_data = {
    .piby2      = _MM512_SET1_PS16(1.5707963705e+00f),
    .pi         = _MM512_SET1_PS16(3.1415926535897933e+00f),
    .half       = _MM512_SET1_PS16(0x1p-1f),
    .max_arg    = _MM512_SET1_PS16(0x1p0f),
    .mask_32    = _MM512_SET1_I32x16(0x7FFFFFFF),
    .a          = {
                   _MM512_SET1_PS16(0.0f),
                   _MM512_SET1_PS16(0x1.921fb6p-1f),
                  },
    .b          = {
                   _MM512_SET1_PS16(0x1.921fb6p0f),
                   _MM512_SET1_PS16(0x1.921fb6p-1f),
                  },
    /* Polynomial coefficients*/
    .poly_asinf = {
                   _MM512_SET1_PS16(0x1.5555fcp-3f),
                   _MM512_SET1_PS16(0x1.32f8d8p-4f),
                   _MM512_SET1_PS16(0x1.7525aap-5f),
                   _MM512_SET1_PS16(0x1.86e46ap-6f),
                   _MM512_SET1_PS16(0x1.5d456cp-5f),
                  },
};

#define ALM_V16_ACOSF_PIBY2    v16_asinf_data.piby2
#define ALM_V16_ACOSF_PI       v16_asinf_data.pi
#define ALM_V16_ACOSF_HALF     v16_asinf_data.half
#define ALM_V16_ACOSF_MASK_32  v16_asinf_data.mask_32
#define ALM_V16_ACOSF_MAX_ARG  v16_asinf_data.max_arg

#define ALM_ACOSF_HALF        0x1p-1f
#define ALM_ACOSF_ONE         0x1p0f
#define ALM_ACOSF_TWO         0x1p1f

#define A v16_asinf_data.a
#define B v16_asinf_data.b

#define C1 v16_asinf_data.poly_asinf[0]
#define C2 v16_asinf_data.poly_asinf[1]
#define C3 v16_asinf_data.poly_asinf[2]
#define C4 v16_asinf_data.poly_asinf[3]
#define C5 v16_asinf_data.poly_asinf[4]

static inline bool
all_v16_u32_loop(v_u32x16_t cond)
{
    for (int i = 0; i < 16; i++) {
        if (!cond[i]) {
            return 0;
        }
    }
    return 1;
}

static inline v_f32x16_t
acosf_specialcase(v_f32x16_t _x, v_f32x16_t result, v_u32x16_t cond)
{
    return call_v16_f32(ALM_PROTO(acosf), _x, result, cond);
}

v_f32x16_t
ALM_PROTO_ARCH_ZN4(vrs16_acosf)(v_f32x16_t x)
{
    v_f32x16_t  z, poly, result, aux;
    v_u32x16_t  ux, sign, outofrange, cond1, cond2;

    int32_t n = 0;

    ux   = as_v16_u32_f32 (x);

    /* Get absolute value of input */
    aux  = as_v16_f32_u32(ux & ALM_V16_ACOSF_MASK_32);

    /* Get sign of the input value */
    sign = ux & ~ALM_V16_ACOSF_MASK_32;

    /* Check for special case */
    /* if |x| >= 1 */
    outofrange = aux >= ALM_V16_ACOSF_MAX_ARG;

    /* if |x| > 0.5 */
    cond1      = aux >  ALM_V16_ACOSF_HALF;

    /* if |x| <= 0.5 */
    cond2      = aux <= ALM_V16_ACOSF_HALF;

    if(all_v16_u32_loop(cond1)) {

        z = ALM_V16_ACOSF_HALF * (ALM_V16_ACOSF_MAX_ARG - aux);

        aux = _mm512_mul_ps(_mm512_set1_ps(-ALM_ACOSF_TWO),
                                            _mm512_sqrt_ps(z));

    } else if (all_v16_u32_loop(cond2)) {

        n = 1;
        z = aux * aux;

    } else {

        outofrange = cond1 | outofrange;

        for (int i = 0; i < 16; i++) {

            if (aux[i] > ALM_ACOSF_HALF) {
                z[i]   = ALM_ACOSF_HALF * (ALM_ACOSF_ONE - aux[i]);
                aux[i] = -ALM_ACOSF_TWO * sqrtf(z[i]);

            } else {
                n = 1;
                z[i] = aux[i] * aux[i];

            }
        }
    }

    /*
     *  Compute acosf(x) using the polynomial
     *  x + C1*x^3 + C2*x^5 + C3*x^7 + C4*x^9 + C5*x^11
     */
    poly = POLY_EVAL_5(z, C1, C2, C3, C4, C5);
    poly = aux + aux * z * poly;

    for (int i = 0; i < 16; i++) {
        if (sign[i])
            result[i] = (B[n][i] + poly[i]) + B[n][i];
        else
            result[i] = (A[n][i] - poly[i]) + A[n][i];
    }

    /*
     *  Fall back to scalar acosf implementation for
     *  special cases
     */
    if(unlikely(any_v16_u32_loop(outofrange)))
        return acosf_specialcase(x, result, outofrange);

    return result;
}
