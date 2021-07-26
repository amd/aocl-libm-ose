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
    v_f32x4_t poly_atanf[9];
    v_u32x4_t mask_32;
    float P[4], Range, sqrt3, unit;
} v4_atanf_data = {
    .sqrt3 = 0x1.bb67aep0,
    .Range = 0x1.126146p-2,
    .mask_32 = _MM_SET1_I32(0x7FFFFFFF),
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
        _MM_SET1_PS4(-0x1.555556p-2f),
        _MM_SET1_PS4(0x1.99999ap-3f),
        _MM_SET1_PS4(-0x1.24924ap-3f),
        _MM_SET1_PS4(0x1.c71c7p-4f),
        _MM_SET1_PS4(-0x1.745cd2p-4f),
        _MM_SET1_PS4(0x1.3b0aeap-4f),
        _MM_SET1_PS4(-0x1.1061c6p-4f),
        _MM_SET1_PS4(0x1.d1242ap-5f),
        _MM_SET1_PS4(-0x1.3a3c92p-5f),
    },
};

#define THEEPS v4_atanf_data.THEEPS
#define SQRT3 v4_atanf_data.sqrt3
#define Range v4_atanf_data.Range
#define P v4_atanf_data.P
#define ALM_V4_ATANF_MASK_32  v4_atanf_data.mask_32
#define Unit v4_atanf_data.unit
#define C0 v4_atanf_data.poly_atanf[0]
#define C1 v4_atanf_data.poly_atanf[1]
#define C2 v4_atanf_data.poly_atanf[2]
#define C3 v4_atanf_data.poly_atanf[3]
#define C4 v4_atanf_data.poly_atanf[4]
#define C5 v4_atanf_data.poly_atanf[5]
#define C6 v4_atanf_data.poly_atanf[6]
#define C7 v4_atanf_data.poly_atanf[7]
#define C8 v4_atanf_data.poly_atanf[8]

v_f32x4_t
ALM_PROTO_OPT(vrs4_atanf)(v_f32x4_t x)
{
    v_f32x4_t poly, aux, result;
    v_u32x4_t sign;
    v_u32x4_t n={0};
    v_u32x4_t  ux = as_v4_u32_f32 (x);

    /* Get sign of the input value */
    sign = ux & ~ALM_V4_ATANF_MASK_32;

    /* Get absolute value of input */
    aux  = as_v4_f32_u32(ux & ALM_V4_ATANF_MASK_32);

    for(int i=0;i<4;i++){
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

    for(int i=0;i<4;i++){
        if (n[i] > 1)
            poly[i] = -poly[i];
        result[i] = P[n[i]]+poly[i];
    }

    return as_v4_f32_u32(as_v4_u32_f32(result) ^ sign);
}
