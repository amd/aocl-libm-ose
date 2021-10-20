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
/* Contains implementation of v_f64x8_t vrd8_atan(v_f64x8_t x)
 *
 * sign = sign(x)
 * x = abs(x)
 *
 * Argument Reduction for every x into the interval [-(2-sqrt(3)),+(2-sqrt(3))]
 * Use the following identities
 * atan(x) = pi/2 - atan(1/x)                when x > 1
 *         = pi/6 + atan(f)                  when f > (2-sqrt(3))
 * where f = (sqrt(3)*x-1)/(x+sqrt(3))
 *
 * All elements are approximated by using polynomial of degree 19
 */
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>
#include <libm/arch/zen4.h>

static struct {
    double sqrt3, range, pi[4], unit;
    v_f64x8_t poly_atan[9];
    } v8_atan_data = {
        .sqrt3 = 0x1.bb67ae8584caap0,
        .range = 0x1.126145e9ecd56p-2,
        .unit= 1.0,
        .pi = {
            0,
            0x1.0c152382d7366p-1,
            0x1.921fb54442d18p0,
            0x1.0c152382d7366p0,
        },
        .poly_atan = {
            _MM512_SET1_PD8(-0x1.5555555555549p-2),
            _MM512_SET1_PD8(0x1.9999999996eccp-3),
            _MM512_SET1_PD8(-0x1.24924922b2972p-3),
            _MM512_SET1_PD8(0x1.c71c707163579p-4),
            _MM512_SET1_PD8(-0x1.745cd1358b0f1p-4),
            _MM512_SET1_PD8(0x1.3b0aea74b0a51p-4),
            _MM512_SET1_PD8(-0x1.1061c5f6997a6p-4),
            _MM512_SET1_PD8(0x1.d1242ae875135p-5),
            _MM512_SET1_PD8(-0x1.3a3c92f7949aep-5),
        },
    };
#define SQRT3  v8_atan_data.sqrt3
#define RANGE  v8_atan_data.range
#define PI     v8_atan_data.pi
#define UNIT v8_atan_data.unit
#define C0 v8_atan_data.poly_atan[0]
#define C1 v8_atan_data.poly_atan[1]
#define C2 v8_atan_data.poly_atan[2]
#define C3 v8_atan_data.poly_atan[3]
#define C4 v8_atan_data.poly_atan[4]
#define C5 v8_atan_data.poly_atan[5]
#define C6 v8_atan_data.poly_atan[6]
#define C7 v8_atan_data.poly_atan[7]
#define C8 v8_atan_data.poly_atan[8]
#define SIGN_MASK   0x7FFFFFFFFFFFFFFFUL
#define UNITBYRANGE UNIT/RANGE
#define POS 0x0
#define NEG 0x8000000000000000
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
 *      atan(xi) is calculated using the polynomial of degree 19,
 *
 */
v_f64x8_t
ALM_PROTO_ARCH_ZN4(vrd8_atan)(v_f64x8_t x)
{
    v_f64x8_t result, aux, pival;
    v_u64x8_t sign, polysign;
    v_u64x8_t ux = as_v8_u64_f64(x);
    /* Get sign of input value */
    sign = ux & (~SIGN_MASK);//
    /* Get absolute value of input */
    ux = ux & SIGN_MASK;
    aux = as_v8_f64_u64(ux);
    for(int i = 0; i < 8; ++i){
        unsigned int n = 0;
        if(aux[i] >= UNITBYRANGE){
            aux[i] = UNIT / aux[i];
            n = 2;
        }else if(aux[i] > UNIT){
            aux[i] = UNIT / aux[i];
            aux[i] = (aux[i] * SQRT3 - UNIT) / (SQRT3 + aux[i]);
            n = 3;
        }else if(aux[i] > RANGE){
            aux[i] = (aux[i] * SQRT3 - UNIT) / (SQRT3 + aux[i]);
            n = 1;
        }
        pival[i] = PI[n];
        polysign[i] = n > 1 ? NEG : POS;
    }
    v_f64x8_t poly = POLY_EVAL_ODD_19(aux, C0, C1, C2, C3, C4, C5, C6, C7, C8);
    poly = as_v8_f64_u64(as_v8_u64_f64(poly) ^ polysign);

    result = pival + poly;
    /* atan(-x) = -atan(x) */
    return as_v8_f64_u64(as_v8_u64_f64(result) ^ sign);
}
