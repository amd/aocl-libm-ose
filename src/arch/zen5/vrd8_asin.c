/*
 * Copyright (C) 2024 Advanced Micro Devices, Inc. All rights reserved.
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
 *   v_f64x8_t vrd8_asin(v_f64x8_t x)
 *
 * Contains implementation of v_f64x8_t vrd8_asin(v_f64x8_t x)
 *
 * The input domain should be in the [-1, +1] else a domain error is displayed
 *
 * asin(-x) = -asin(x)
 * asin(x) = pi/2-2*asin(sqrt(1/2*(1-x)))  when x > 1/2
 *
 * y = abs(x)
 * asinf(y) = asinf(g)  when y <= 0.5,  where g = y*y
 *          = pi/2-asinf(g)  when y > 0.5, where g = 1/2*(1-y), y = -2*sqrt(g)
 * The term asin(f) is approximated by using a polynomial where the inputs lie in the interval [0 1/2]
 *
 * MAX ULP of current implementation : 2
 */

#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>
#include "kern/sqrt_pos.c"
#include <libm/arch/zen5.h>

static struct {
    v_f64x8_t HALF, ONE, poly_asin[13];
    v_u64x8_t mask_64;
    double pi_by_two, two, one, half;
} v8_asin_data = {
    .mask_64 = _MM512_SET1_U64x8(0x7FFFFFFFFFFFFFFFUL),
    .HALF = _MM512_SET1_PD8(0.5),
    .ONE = _MM512_SET1_PD8(0x1p0),
    // Polynomial coefficients obtained using fpminimax algorithm from Sollya
    .poly_asin = {
        // polynomial coeffs are obtained from vrd8_asin.sollya
        _MM512_SET1_PD8(0x1.ffffffffdecd3p-1),
        _MM512_SET1_PD8(0x1.55555555552aap-3),
        _MM512_SET1_PD8(0x1.333333337cbaep-4),
        _MM512_SET1_PD8(0x1.6db6db3c0984p-5),
        _MM512_SET1_PD8(0x1.f1c72dd86cbafp-6),
        _MM512_SET1_PD8(0x1.6e89d3ff33aa4p-6),
        _MM512_SET1_PD8(0x1.1c6d83ae664b6p-6),
        _MM512_SET1_PD8(0x1.c6e1568b90518p-7),
        _MM512_SET1_PD8(0x1.8f6a58977fe49p-7),
        _MM512_SET1_PD8(0x1.a6ab10b3321bp-8),
        _MM512_SET1_PD8(0x1.43305ebb2428fp-6),
        _MM512_SET1_PD8(-0x1.0e874ec5e3157p-6),
        _MM512_SET1_PD8(0x1.06eec35b3b142p-5),
    },
    // Values of factors of pi required to calculate asin
    .pi_by_two = 0x1.921fb54442d18p-1,
    .two = 0x1p1,
    .one = 0x1p0,
    .half = 0x1p-1,
};

#define V8_ASIN_HALF       v8_asin_data.HALF
#define V8_ASIN_ONE        v8_asin_data.ONE

#define V8_ASIN_MASK_64    v8_asin_data.mask_64

#define V8_PI_BY_TWO    v8_asin_data.pi_by_two
#define TWO          v8_asin_data.two
#define F_ONE        v8_asin_data.one
#define F_HALF       v8_asin_data.half

#define C0 v8_asin_data.poly_asin[0]
#define C1 v8_asin_data.poly_asin[1]
#define C2 v8_asin_data.poly_asin[2]
#define C3 v8_asin_data.poly_asin[3]
#define C4 v8_asin_data.poly_asin[4]
#define C5 v8_asin_data.poly_asin[5]
#define C6 v8_asin_data.poly_asin[6]
#define C7 v8_asin_data.poly_asin[7]
#define C8 v8_asin_data.poly_asin[8]
#define C9 v8_asin_data.poly_asin[9]
#define C10 v8_asin_data.poly_asin[10]
#define C11 v8_asin_data.poly_asin[11]
#define C12 v8_asin_data.poly_asin[12]

#define ZERO                0
#define VECTOR_LENGTH       8

v_f64x8_t
ALM_PROTO_ARCH_ZN5(vrd8_asin)(v_f64x8_t x)
{
    v_f64x8_t r, poly, result, G = _MM512_SET1_PD8(0.0), n =  _MM512_SET1_PD8(V8_PI_BY_TWO);
    v_u64x8_t ux = as_v8_u64_f64(x);
    v_u64x8_t sign = ux & ~V8_ASIN_MASK_64;
    r  = as_v8_f64_u64(ux & V8_ASIN_MASK_64);
    v_u64x8_t cmp = (v_u64x8_t)((r) > (V8_ASIN_HALF));

    for (int i = ZERO; i < VECTOR_LENGTH; i++) {
        if (cmp[i]) {
            G[i] = F_HALF * (F_ONE - r[i]);
            r[i] = -TWO * ALM_PROTO_KERN(sqrt)(G[i]);
        } else {
            n[i] = 0.0;
            G[i] = r[i] * r[i];
        }
    }

    poly = r + r * G * POLY_EVAL_12(G,C1,C2,C3,C4,C5,C6,C7,C8,C9,C10,C11,C12); 
    poly =  (n + poly) + n;

    result = as_v8_f64_u64(as_v8_u64_f64(poly) ^ sign);

    return result;
}
