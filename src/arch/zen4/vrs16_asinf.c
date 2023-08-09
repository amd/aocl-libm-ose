/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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
 *   v_f32x16_t vrs16_asinf(v_f32x16_t x)
 *
 * Contains implementation of v_f32x16_t vrs16_asinf(v_f32x16_t x)
 *
 * The input domain should be in the [-1, +1] else a domain error is displayed
 *
 * asin(-x) = -asin(x)
 * asin(x) = pi/2-2*asin(sqrt(1/2*(1-x)))  when x > 1/2
 *
 * y = abs(x)
 * asinf(y) = asinf(g)  when y <= 0.5,  where g = y*y
 *		    = pi/2-asinf(g)  when y > 0.5, where g = 1/2*(1-y), y = -2*sqrt(g)
 * The term asin(f) is approximated by using a polynomial where the inputs lie in the interval [0 1/2]
 *
 * MAX ULP of current implementation : 2
 */

#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>
#include "kern/sqrtf_pos.c" 
#include <libm/arch/zen4.h>

static struct {
    v_f32x16_t THEEPS, HALF, ONE, poly_asinf[5];
    v_u32x16_t mask_32;
    float pi_by_two, two, one, half;
} v16_asinf_data = {
    .THEEPS = _MM512_SET1_PS16(0x1.6a09e6p-12f),
    .HALF = _MM512_SET1_PS16(0x1p-1f),
    .ONE = _MM512_SET1_PS16(0x1p0f),
    // Polynomial coefficients obtained using fpminimax algorithm from Sollya
    .poly_asinf = {
        _MM512_SET1_PS16(0x1.5555fcp-3f),
        _MM512_SET1_PS16(0x1.32f8d8p-4f),
        _MM512_SET1_PS16(0x1.7525aap-5f),
        _MM512_SET1_PS16(0x1.86e46ap-6f),
        _MM512_SET1_PS16(0x1.5d456cp-5f),
    },
    .mask_32   = _MM512_SET1_U32x16(0x7FFFFFFFU),
    // Values of factors of pi required to calculate asin
    .pi_by_two = 0x1.921fb6p-1f,
    .two = 0x1p1f,
    .one = 0x1p0f,
    .half = 0x1p-1f,
};
#define V16_ASINF_THEEPS     v16_asinf_data.THEEPS
#define V16_ASINF_HALF       v16_asinf_data.HALF
#define V16_ASINF_ONE        v16_asinf_data.ONE

#define V16_ASINF_MASK_32    v16_asinf_data.mask_32

#define PI_BY_TWO    v16_asinf_data.pi_by_two
#define TWO          v16_asinf_data.two
#define F_ONE        v16_asinf_data.one
#define F_HALF       v16_asinf_data.half

#define C1 v16_asinf_data.poly_asinf[0]
#define C2 v16_asinf_data.poly_asinf[1]
#define C3 v16_asinf_data.poly_asinf[2]
#define C4 v16_asinf_data.poly_asinf[3]
#define C5 v16_asinf_data.poly_asinf[4]
#define C6 v16_asinf_data.poly_asinf[6]

#define VECTOR_LENGTH       16

v_f32x16_t
ALM_PROTO_ARCH_ZN4(vrs16_asinf)(v_f32x16_t x)
{
    v_f32x16_t r, poly, result, G, n = _MM512_SET1_PS16(PI_BY_TWO);
    v_u32x16_t ux = as_v16_u32_f32(x);
    v_u32x16_t sign = ux & ~V16_ASINF_MASK_32;
    r  = as_v16_f32_u32(ux & V16_ASINF_MASK_32);
    v_u32x16_t cmp = r > V16_ASINF_HALF;

    for (int i = 0; i < VECTOR_LENGTH; i++) {
        if (cmp[i]) {
            G[i] = F_HALF *(F_ONE - r[i]);
            r[i] = -TWO * ALM_PROTO_KERN(sqrtf)(G[i]);
        } else {
            n[i] = 0.0f;
            G[i] = r[i] * r[i];
        }
    }

    poly = r + r * G * POLY_EVAL_5(G,C1,C2,C3,C4,C5);
    poly =  (n + poly) + n;
    result = as_v16_f32_u32(as_v16_u32_f32(poly) ^ sign);

    return result;
}
