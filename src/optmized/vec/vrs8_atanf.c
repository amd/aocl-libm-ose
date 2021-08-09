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
 * Reduce the given x, so that every element of x falls into the interval [-(2-sqrt(3)),+(2-sqrt(3))]
 * Use the following identities to evalute atan and reduce the range of every element
 * atan(x) = pi/2 - atan(1/x)				when x > 1
 * 		   = pi/6 + atan(f) 				when f > (2-sqrt(3))
 * where f = (sqrt(3)*x-1)/(x+sqrt(3))
 *
 * All elements are approximated by using a polynomial
 */
#include <stdint.h>
#include <libm_util_amd.h>
#include <libm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>
static struct {
    v_f32x8_t poly_atanf[9];
    v_u32x8_t mask_32;
    float P[4], Range, sqrt3, unit;
} v8_atanf_data = {
    .sqrt3 = 0x1.bb67aep0,
    .Range = 0x1.126146p-2,
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
        _MM_SET1_PS8(-0x1.555556p-2f),
        _MM_SET1_PS8(0x1.99999ap-3f),
        _MM_SET1_PS8(-0x1.24924ap-3f),
        _MM_SET1_PS8(0x1.c71c7p-4f),
        _MM_SET1_PS8(-0x1.745cd2p-4f),
        _MM_SET1_PS8(0x1.3b0aeap-4f),
        _MM_SET1_PS8(-0x1.1061c6p-4f),
        _MM_SET1_PS8(0x1.d1242ap-5f),
        _MM_SET1_PS8(-0x1.3a3c92p-5f),
    },
};
#define THEEPS v8_atanf_data.THEEPS
#define SQRT3 v8_atanf_data.sqrt3
#define Range v8_atanf_data.Range
#define P v8_atanf_data.P
#define ALM_V8_ATANF_MASK_32  v8_atanf_data.mask_32
#define Unit v8_atanf_data.unit
#define C0 v8_atanf_data.poly_atanf[0]
#define C1 v8_atanf_data.poly_atanf[1]
#define C2 v8_atanf_data.poly_atanf[2]
#define C3 v8_atanf_data.poly_atanf[3]
#define C4 v8_atanf_data.poly_atanf[4]
#define C5 v8_atanf_data.poly_atanf[5]
#define C6 v8_atanf_data.poly_atanf[6]
#define C7 v8_atanf_data.poly_atanf[7]
#define C8 v8_atanf_data.poly_atanf[8]
/*
 ********************************************
 * Implementation Notes
 * ---------------------
 * sign = sign(xi)
 * xi = |xi|
 *
 * Reduce every element(xi) from x into f, so that it falls into the interval [-(2-sqrt(3)),+(2-sqrt(3))]
 * Use the following identities for every xi to finally evaluate vrs8_atanf and reduce the range of xi
 *
 * 1. If xi > 1,
 *      atan(xi) = pi/2 - atan(1/xi)
 *
 * 2. If f > (2-sqrt(3)),
 *      atan(x) = pi/6 + atan(f)
 *      where f = (sqrt(3)*xi-1)/(xi+sqrt(3))
 *
 *      atan(f) is calculated using the polynomial,
 *      f + C0*xi^3 + C1*xi^5 + C2*xi^7 + C3*xi^9 + C4*xi^11+
 *      C5*xi^13 + C6*xi^15 + C7*xi^17 + C8*xi^19
 *
 */
v_f32x8_t
ALM_PROTO_OPT(vrs8_atanf)(v_f32x8_t x)
{
    v_f32x8_t poly, aux, result;
    v_u32x8_t sign;
    v_u32x8_t n={0};
    v_u32x8_t  ux = as_v8_u32_f32 (x);
    /* Get sign of the input value */
    sign = ux & ~ALM_V8_ATANF_MASK_32;
    /* Get absolute value of input */
    aux  = as_v8_f32_u32(ux & ALM_V8_ATANF_MASK_32);
    for(int i=0;i<8;i++){
        if(aux[i]>Unit){
            aux[i]=Unit/aux[i];
            n[i]=2;
        }
        if(aux[i]>Range){
            aux[i]=(aux[i]*SQRT3-Unit)/(SQRT3+aux[i]);
            n[i]++;
        }
    }
    poly = POLY_EVAL_ODD_19(aux, C0, C1, C2, C3, C4, C5, C6, C7, C8);
    for(int i=0;i<8;i++){
        if (n[i] > 1) poly[i] = -poly[i];
            result[i] = P[n[i]]+poly[i];
    }
    return as_v8_f32_u32(as_v8_u32_f32(result) ^ sign);
}
