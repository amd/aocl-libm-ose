/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/constants.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/poly-vec.h>

extern float _tanf_special(float);

/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   float tanf(float x)
 *
 * Spec:
 *   tanf(n· 2π + π/4)  = 1       if n∈Z and |n· 2π + π/4|   <= big_angle_F
 *   tanf(n· 2π + 3π/4) = −1      if n∈Z and |n· 2π + 3π/4|  <= big angle rF
 *   tanf(x) = x                  if x ∈ F^(2·π) and tanf(x) != tan(x)
 *                                               and |x| < sqrt(epsilonF/rF)
 *   tanf(−x) = −tanf(x)          if x ∈ F^(2·π)
 */

static const struct {
    v_u32x4_t    arg_max;
    v_u32x4_t    sign_mask;
    v_f32x4_t    huge;
    v_f32x4_t    invhalfpi, halfpi1, halfpi2, halfpi3;
    v_f32x4_t    poly_tanf[7];
} v4_tanf_data = {
    .sign_mask = _MM_SET1_I32(1U<<31),
    .arg_max   = _MM_SET1_I32(0x49800000), /* 0x1p20f */
    .huge      = _MM_SET1_PS4(0x1.80000000p23f),
    .invhalfpi = _MM_SET1_PS4(0x1.45f306p-1f),
    .halfpi1   = _MM_SET1_PS4(-0x1.921fb6p0f),
    .halfpi2   = _MM_SET1_PS4(0x1.777a5cp-25f),
    .halfpi3   = _MM_SET1_PS4(0x1.ee59dap-50f),

    // Polynomial coefficients obtained using Remez algorithm from Sollya
    .poly_tanf = {
        _MM_SET1_PS4(0x1.555566p-2f),
        _MM_SET1_PS4(0x1.110cdp-3f),
        _MM_SET1_PS4(0x1.baf34p-5f),
        _MM_SET1_PS4(0x1.5bf38ep-6f),
        _MM_SET1_PS4(0x1.663acap-7f),
        _MM_SET1_PS4(-0x1.07c6f4p-16f),
        _MM_SET1_PS4(0x1.21cedap-8f),
    },

};

#define V4_SIMD_WIDTH        4
#define ALM_TANF_HUGE_VAL    v4_tanf_data.huge
#define ALM_TANF_HALFPI      v4_tanf_data.halfpi
#define ALM_TANF_PI_HIGH     v4_tanf_data.pihi
#define ALM_TANF_PI_LOW      v4_tanf_data.pilow
#define ALM_TANF_INVHALFPI   v4_tanf_data.invhalfpi
#define ALM_TANF_ARG_MAX     v4_tanf_data.arg_max
#define ALM_TANF_SIGN_MASK32 v4_tanf_data.sign_mask

#define  ALM_TANF_HALFPI1 v4_tanf_data.halfpi1
#define  ALM_TANF_HALFPI2 v4_tanf_data.halfpi2
#define  ALM_TANF_HALFPI3 v4_tanf_data.halfpi3

#define C1 v4_tanf_data.poly_tanf[0]
#define C2 v4_tanf_data.poly_tanf[1]
#define C3 v4_tanf_data.poly_tanf[2]
#define C4 v4_tanf_data.poly_tanf[3]
#define C5 v4_tanf_data.poly_tanf[4]
#define C6 v4_tanf_data.poly_tanf[5]
#define C7 v4_tanf_data.poly_tanf[6]

extern float tanf_specialcase(float);
#if 0
extern float tanf_oddcase(float);

static inline v_f32x4_t
vrs4_tanf_oddcase(v_f32x4_t _x, v_f32x4_t result, v_u32x4_t odd)
{
    /* We send the result in this case as we have already computed them */
    return call_v4_f32(tanf_oddcase, result, _x, odd);
}
#endif

static inline v_f32x4_t
vrs4_tanf_specialcase(v_f32x4_t _x, v_f32x4_t result, v_u32x4_t cond)
{
    return call_v4_f32(tanf_specialcase, _x, result, cond);
}

/*
 * Implementation Notes:
 *
 * float tanf(float x)
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

__m128
ALM_PROTO_OPT(vrs4_tanf)(__m128 xf32x4)
{
    v_f32x4_t   F, xx;
    v_f32x4_t   poly;
    int32_t i;
    v_u32x4_t   sign = {0}, n;
    v_u32x4_t   ux = as_v4_u32_f32(xf32x4);

    v_u32x4_t  cond = (ux  & ~ALM_TANF_SIGN_MASK32) > ALM_TANF_ARG_MAX;

	sign = ux & ALM_TANF_SIGN_MASK32;

    /* fabs(x) */
    xx = as_v4_f32_u32(ux & ~ALM_TANF_SIGN_MASK32);

    /*
     * dn = x * (2/π)
     * would turn to fma
     */

    v_f32x4_t nn =  xx * ALM_TANF_INVHALFPI + ALM_TANF_HUGE_VAL;

    // n = (int)dn /
    n   = as_v4_u32_f32(nn);

    nn -= ALM_TANF_HUGE_VAL;
/*
     v_f32x4_t nn =  xx * ALM_TANF_INVHALFPI;
    for(int i =0;i<4;i++) {
        n[i] = (nn[i] < 0)?(nn[i] - 0.5):(nn[i] + 0.5);
        nn[i] = (float)n[i];
    }
*/
    /* F = x - (n * π/2) */
    F = xx + nn * ALM_TANF_HALFPI1;
    F = F + nn * ALM_TANF_HALFPI2;
    F = F + nn * ALM_TANF_HALFPI3;

    v_u32x4_t odd = n << 31;

    /*
     * Calculate the polynomial approximation
     *					x * (C1 + C2*x^2 + C3*x^4 + C4*x^6 + \
     *									C5*x^8 + C6*x^10 + C7*x^12 + C8*x^14)
     * polynomial is approximated as x*P(x^2)
     */
    poly = POLY_EVAL_ODD_15(F, C1, C2, C3, C4, C5, C6, C7);

    v_f32x4_t result = as_v4_f32_u32(as_v4_u32_f32(poly) ^ sign);

    /* if n is odd, result = -1.0/result */
    for(i = 0; i < V4_SIMD_WIDTH; i++) {

        result[i] = odd[i] ? (-1.0f / result[i]) : result[i];

    }

    if (any_v4_u32_loop(cond)) {
        result = vrs4_tanf_specialcase(xf32x4, result, cond);
    }

    return result;
}
