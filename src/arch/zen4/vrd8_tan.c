/*
 * Copyright (C) 2018-2020, Advanced Micro Devices, Inc. All rights reserved.
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
#include <libm/arch/zen4.h>


/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   double tan(double x)
 *
 * Spec:
 *   tan(n· 2π + π/4)  = 1       if n∈Z and |n· 2π + π/4|   <= big_angle_F
 *   tan(n· 2π + 3π/4) = −1      if n∈Z and |n· 2π + 3π/4|  <= big angle rF
 *   tan(x) = x                  if x ∈ F^(2·π) and tan(x) != tan(x)
 *                                               and |x| < sqrt(epsilonF/rF)
 *   tan(−x) = −tan(x)          if x ∈ F^(2·π)
 */

static struct {
    v_f64x8_t invhalfpi, huge;
    v_u64x8_t arg_max;
    v_f64x8_t halfpi1, halfpi2, halfpi3;
    v_f64x8_t poly_tan[14];
} tan_v8_data = {
    .arg_max   = _MM512_SET1_U64x8(0x4160000000000000UL), /* 10^6 */
    .huge      = _MM512_SET1_PD8(0x1.8000000000000p52),
    .invhalfpi = _MM512_SET1_PD8(0x1.45f306dc9c882a53f85p-1),
    .halfpi1   = _MM512_SET1_PD8(0x1.921fb54442d18p0),
    .halfpi2   = _MM512_SET1_PD8(0x1.1a62633145c07p-54),
    .halfpi3   = _MM512_SET1_PD8(-0x1.f1976b7ed8fbcp-110),

    /*
     * Polynomial coefficients obtained using
     * fpminimax algorithm from Sollya
     */
    .poly_tan = {
        _MM512_SET1_PD8(0x1.55555555554bep-2),
        _MM512_SET1_PD8(0x1.1111111119f2ap-3),
        _MM512_SET1_PD8(0x1.ba1ba1b38733cp-5),
        _MM512_SET1_PD8(0x1.664f49c8b63e3p-6),
        _MM512_SET1_PD8(0x1.226e0f7f17778p-7),
        _MM512_SET1_PD8(0x1.d6d989f491431p-9),
        _MM512_SET1_PD8(0x1.7d57d7c375c03p-10),
        _MM512_SET1_PD8(0x1.38148605a1756p-11),
        _MM512_SET1_PD8(0x1.d15fa298b8b17p-13),
        _MM512_SET1_PD8(0x1.20250b03ea768p-13),
        _MM512_SET1_PD8(-0x1.cd6072c36a433p-16),
        _MM512_SET1_PD8(0x1.7b1cbff8d88e6p-14),
        _MM512_SET1_PD8(-0x1.7c588d6a4c96ep-15),
        _MM512_SET1_PD8(0x1.5fc28759e55bap-16),
    },

};

#define ALM_TAN_V8_HUGE_VAL        tan_v8_data.huge
#define ALM_TAN_V8_HALFPI1         tan_v8_data.halfpi1
#define ALM_TAN_V8_HALFPI2         tan_v8_data.halfpi2
#define ALM_TAN_V8_HALFPI3         tan_v8_data.halfpi3
#define ALM_TAN_V8_INVHALFPI       tan_v8_data.invhalfpi
#define ALM_TAN_V8_ARG_MAX         tan_v8_data.arg_max
#define ALM_TAN_V8_SIGN_MASK       (1UL<<63)


#define C1  tan_v8_data.poly_tan[0]
#define C3  tan_v8_data.poly_tan[1]
#define C5  tan_v8_data.poly_tan[2]
#define C7  tan_v8_data.poly_tan[3]
#define C9  tan_v8_data.poly_tan[4]
#define C11 tan_v8_data.poly_tan[5]
#define C13 tan_v8_data.poly_tan[6]
#define C15 tan_v8_data.poly_tan[7]
#define C17 tan_v8_data.poly_tan[8]
#define C19 tan_v8_data.poly_tan[9]
#define C21 tan_v8_data.poly_tan[10]
#define C23 tan_v8_data.poly_tan[11]
#define C25 tan_v8_data.poly_tan[12]
#define C27 tan_v8_data.poly_tan[13]


extern double tan_specialcase(double x);
/*
 * Special case to handle
 * - out of range
 * - Inf/NaN
 */
static inline v_f64x8_t
vrd8_tan_specialcase(v_f64x8_t _x, v_f64x8_t result, v_u64x8_t cond)
{
    return call_v8_f64(tan_specialcase, _x, result, cond);
}

/*
 * Implementation Notes:
 *
 * v_f64x8_t vrd8_tan(v_f64x8_t x)
 *      A given x is reduced into the form:
 *
 *               |x| = (N * π/2) + F
 *
 *      Where N is an integer obtained using:
 *              N = round(x * 2/π)
 *      And F is a fraction part lying in the interval
 *              [-π/4, +π/4];
 *
 *      obtained as F = |x| - (N * π/2)
 *
 *      Thus tan(x) is given by
 *
 *              tan(x) = tan((N * π/2) + F) = tan(F)
 *              when N is even,
 *                     = -cot(F) = -1/tan(F)
 *              when N is odd, tan(F) is approximated using a polynomial
 *                      obtained from Remez approximation from Sollya.
 *
 */
v_f64x8_t
ALM_PROTO_ARCH_ZN4(vrd8_tan)(v_f64x8_t x)
{
    v_f64x8_t   F, poly, result, r;
    v_u64x8_t   n, sign;
    v_u64x8_t   ux = as_v8_u64_f64(x);

    v_u64x8_t cond = (ux & ~ALM_TAN_V8_SIGN_MASK) > ALM_TAN_V8_ARG_MAX;

    sign = ux & ALM_TAN_V8_SIGN_MASK;

    r = as_v8_f64_u64(ux & ~ALM_TAN_V8_SIGN_MASK);

    /*
     * dn = x * (2/π)
     * would turn to fma
     */
    v_f64x8_t dn =  r * ALM_TAN_V8_INVHALFPI + ALM_TAN_V8_HUGE_VAL;

    /* n = (int)dn */
    n   = as_v8_u64_f64(dn);

    dn -= ALM_TAN_V8_HUGE_VAL;

    /*
     * Get the fraction part
     *   F = xd - (n * π/2)
     */
    F = r - dn * ALM_TAN_V8_HALFPI1;  // F = x - n*pi1/2
    F = F - dn * ALM_TAN_V8_HALFPI2;  // F = F - n*pi2/2
    F = F - dn * ALM_TAN_V8_HALFPI3;  // F = F - n*pi3/2

    v_u64x8_t odd = (n << 63);

    /*
     * Calculate the polynomial approximation
     *  poly = x + C1*x^3 + C2*x^5 + C3*x^7 + C4*x^9 + C5*x^11 + \
     *          C6*x^13 + C7*x^15 + C8*x^17 + C9*x^19 + \
     *          C10*x^21 + C11*x^23 + C12*x^25 + C13*x^27 + C14*x^29;
     *
     *       = x + x * G*(C6 + G*(C7 + G*(C8 +
     *                  G*(C9 + G*(C10+ G*(C11 + G*(C12 +
     *                          G*(C13 + C14*G))))
     *                  ))))
     *
     * Polynomial is approximated as x+x*P(G)  where G = x^2
     */

    poly = POLY_EVAL_ODD_29(F, C1, C3, C5, C7, C9,
                            C11, C13, C15, C17, C19,
                            C21, C23, C25, C27);

    result = as_v8_f64_u64(as_v8_u64_f64(poly) ^ sign);

#define V8_SIMD_WIDTH 8

    for (int i = 0; i < V8_SIMD_WIDTH; i++) {
        result[i] = odd[i] ? (-1.0 / result[i]) : result[i];
    }

    if (any_v8_u64_loop(cond))
        result = vrd8_tan_specialcase(x, result, cond);

    return result;
}
