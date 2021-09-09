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
/* Contains implementation of v_f32x8_t vrs8_atanf(v_f32x8_t x)
 *
 * sign = sign(x)
 * x = abs(x)
 *
 * Argument Reduction for every x into the interval [-(2-sqrt(3)),+(2-sqrt(3))]
 *
 * Use the following identities
 * atan(x) = pi/2 - atan(1/x)               when x > 1
 *         = pi/6 + atan(f)                 when f > (2-sqrt(3))
 * where f = (sqrt(3)*x-1)/(x+sqrt(3))
 *
 * All elements are approximated by using polynomial of degree 7
 */
#include <libm_util_amd.h>
#include <libm_special.h>
#include <libm_macros.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>
#include <stdbool.h>

static struct {
    v_f32x8_t poly_atanf[3];
    v_u32x8_t mask_32;
    float P[4], range, sqrt3, unit;
} v8_atanf_data = {
    .sqrt3 = 0x1.bb67aep0,
    .range = 0x1.126146p-2,
    .mask_32 = _MM256_SET1_I32(0x7FFFFFFF),
    .unit= 1.0f,
    // Values of factors of pi required to calculate atanf
    .P = {
        0.0,
        0x1.0c1524p-1,
        0x1.921fb6p0,
        0x1.0c1524p0,
    },
    // Polynomial coefficients obtained using fpminimax algorithm from Sollya
    .poly_atanf = {
        _MM_SET1_PS8(-0x1.5552f2p-2f),
        _MM_SET1_PS8(0x1.9848ap-3f),
        _MM_SET1_PS8(-0x1.066ac8p-3f),
    },
};

#define SQRT3 v8_atanf_data.sqrt3
#define RANGE v8_atanf_data.range
#define PI v8_atanf_data.P
#define ALM_V8_ATANF_MASK_32  v8_atanf_data.mask_32
#define UNIT v8_atanf_data.unit
#define C0 v8_atanf_data.poly_atanf[0]
#define C1 v8_atanf_data.poly_atanf[1]
#define C2 v8_atanf_data.poly_atanf[2]

#define POS 0x0
#define NEG 0x80000000
/*
 ********************************************
 * Implementation Notes
 * ---------------------
 * sign = sign(xi)
 * xi = |xi|
 *
 * Argument reduction: Use the following identities
 *
 * 1. If xi > 1,
 *      atan(xi) = pi/2 - atan(1/xi)
 *
 * 2. If f > (2-sqrt(3)),
 *      atan(x) = pi/6 + atan(f)
 *      where f = (sqrt(3)*xi-1)/(xi+sqrt(3))
 *
 *      atan(xi) is calculated using the polynomial,
 *      xi + C0*xi^3 + C1*xi^5 + C2*xi^7
 *
 */
 /*
static inline bool
all_v8_u32_loop(v_u32x8_t cond)
{
    for (int i = 0; i < 8; i++) {
        if (!cond[i]) {
            return 0;
        }
    }
    return 1;
}
*/
v_f32x8_t
ALM_PROTO_OPT(vrs8_atanf)(v_f32x8_t x)
{
    v_f32x8_t aux, result, pival;
    v_u32x8_t sign, polysign;
    v_u32x8_t  ux = as_v8_u32_f32 (x);
    /* Get sign of the input value */
    sign = ux & ~ALM_V8_ATANF_MASK_32;
    /* Get absolute value of input */
    aux  = as_v8_f32_u32(ux & ALM_V8_ATANF_MASK_32);

    for(int i = 0;i < 8; ++i){
        unsigned int n = 0;
        if(aux[i] > UNIT){
            aux[i] = UNIT / aux[i];
            n = 2;
        }
        if(aux[i] > RANGE){
            aux[i] = (aux[i] * SQRT3 - UNIT) / (SQRT3 + aux[i]);
            ++n;
        }
        pival[i] = PI[n];
        polysign[i] = n > 1 ? NEG : POS;
    }
    v_f32x8_t poly = POLY_EVAL_ODD_7(aux, C0, C1, C2);
    poly = as_v8_f32_u32(as_v8_u32_f32(poly) ^ polysign);

    result = pival + poly;

    return as_v8_f32_u32(as_v8_u32_f32(result) ^ sign);
}