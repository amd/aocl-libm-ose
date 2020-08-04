/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */


#include <stdint.h>

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/constants.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/poly-vec.h>

#define  ALM_SIGN_MASK   ~(1UL<<63)
#define  ALM_SIGN_MASK32 ~(1U<<31)

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
    v_u32x4_t    infinity;
    v_f64x4_t    huge;
    v_f64x4_t    halfpi, invhalfpi;
    v_f64x4_t    poly_tanf[8];
} tanf_data = {
    .infinity  = _MM_SET1_I32(0x7f80000),
    .huge      = _MM_SET1_PD4(0x1.8000000000000p52),
    .halfpi    = _MM_SET1_PD4(0x1.921fb54442d18469899p0),
    .invhalfpi = _MM_SET1_PD4(0x1.45f306dc9c882a53f85p-1),
    // Polynomial coefficients obtained using Remez algorithm from Sollya
    .poly_tanf = {
        _MM_SET1_PD4(0x1.ffffff99ac0468p-1),
        _MM_SET1_PD4(0x1.55559193ecf2bp-2),
        _MM_SET1_PD4(0x1.1106bf4ba8f408p-3),
        _MM_SET1_PD4(0x1.bbafbb6650308p-5),
        _MM_SET1_PD4(0x1.561638922df5fp-6),
        _MM_SET1_PD4(0x1.7f3a033a88788p-7),
        _MM_SET1_PD4(-0x1.ba0d41d26961f8p-11),

        _MM_SET1_PD4(0x1.3952b4eff28ac8p-8),
    },

};

#define ALM_HUGE_VAL    tanf_data.huge
#define ALM_HALFPI      tanf_data.halfpi
#define ALM_PI_HIGH     tanf_data.pihi
#define ALM_PI_LOW      tanf_data.pilow
#define ALM_INVHALFPI   tanf_data.invhalfpi
#define ALM_ARG_MAX     tanf_data.infinity

#define C1 tanf_data.poly_tanf[0]
#define C2 tanf_data.poly_tanf[1]
#define C3 tanf_data.poly_tanf[2]
#define C4 tanf_data.poly_tanf[3]
#define C5 tanf_data.poly_tanf[4]
#define C6 tanf_data.poly_tanf[5]
#define C7 tanf_data.poly_tanf[6]
#define C8 tanf_data.poly_tanf[7]


float ALM_PROTO(tanf)(float);

static inline v_f32x4_t
tanf_specialcase(v_f32x4_t _x, v_f32x4_t result, v_i32x4_t cond)
{
    return v_call_f32(ALM_PROTO(tanf), _x, result, cond);
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
    v_f64x4_t   F, xd, tanx;
    v_f64x4_t   poly;
    v_u64x4_t   sign, uxd, n;
    v_u32x4_t   ux = as_v4_u32_f32(xf32x4);

    v_i32x4_t  cond = (ux  & ALM_SIGN_MASK32) > ALM_ARG_MAX;

    xd = cast_v4_f32_to_f64(xf32x4);

    uxd = as_v4_u64_f64(xd);

    sign = uxd & (~ALM_SIGN_MASK);

    /* fabs(x) */
    xd = as_v4_f64_u64(uxd & ALM_SIGN_MASK);

    /*
     * dn = x * (2/π)
     * would turn to fma
     */
    v_f64x4_t dn =  xd * ALM_INVHALFPI + ALM_HUGE_VAL;

    /* n = (int)dn */
    n   = as_v4_u64_f64(dn);

    dn -= ALM_HUGE_VAL;

    /* F = xd - (n * π/2) */
    F = xd - dn * ALM_HALFPI;

    v_i64x4_t odd = (n << 63);

    /*
     * Calculate the polynomial approximation
     *					x * (C1 + C2*x^2 + C3*x^4 + C4*x^6 + \
		 *									C5*x^8 + C6*x^10 + C7*x^12 + C8*x^14)
     * polynomial is approximated as x*P(x^2)
     */
    poly = POLY_EVAL_EVEN_15(F, C1, C2, C3, C4, C5, C6, C7, C8);

    tanx = as_v4_f64_u64(as_v4_u64_f64(poly) ^ sign);

    v_f32x4_t result = cast_v4_f64_to_f32(tanx);

    if (unlikely(any_v4_u32(cond)))
        return tanf_specialcase(xf32x4, result, cond);

    if (any_v4_u64(odd)) {
            result = tanf_specialcase(xf32x4,
                                     result,
                                     cast_v4_u64_to_u32(odd));
    }

    return result;
}
