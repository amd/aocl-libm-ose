/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
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
 *   v_f64x8_t vrd8_acos(v_f64x8_t x)
 *
 * See vrd4_acos.c. This is the 8-wide (AVX-512) variant.
 *
 * acos is reduced to asin on [0, 0.5]:
 *
 *   1. |x| >  0.5 : asin argument r = -2*sqrt((1 - |x|)/2), g = (1 - |x|)/2
 *   2. |x| <= 0.5 : asin argument r =  |x|,                 g = |x|*|x|
 *
 * poly = r + r*g*P(g) approximates the underlying asin; the region
 * half-angles below then map it back to acos:
 *
 *      x >= 0 : acos(x) = (K - poly) + K   (== 2*K - poly)
 *      x <  0 : acos(x) = (K + poly) + K   (== 2*K + poly)
 *
 * K is half of the target angle, so the compensated "(K +/- poly) + K" form
 * doubles it while limiting rounding error:
 *
 *      |x| >  0.5, x >= 0 : K = 0      (doubled -> 0)
 *      |x| >  0.5, x <  0 : K = pi/2   (doubled -> pi)
 *      |x| <= 0.5         : K = pi/4   (doubled -> pi/2), for either sign
 *
 * The per-lane region split is done with AVX-512 masks: _mm512_sqrt_pd on
 * the |x| > 0.5 lanes and _mm512_mask_blend_pd to select each path, so there
 * is no scalar loop (and no software kern sqrt). Lanes with |x| >= 1 (domain
 * edges and +/-Inf) fall back to scalar acos; NaN propagates through the
 * polynomial path unchanged.
 *
 * Max ULP of this implementation: 1.5
 */

#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>

static struct {
    v_f64x8_t half, max_arg, minus_two;
    v_f64x8_t piby4, pi;
    v_f64x8_t poly_asin[12];
    v_u64x8_t mask_64;
} v8_acos_data = {
    .half      = _MM512_SET1_PD8(0x1p-1),                   /* 0.5 */
    .max_arg   = _MM512_SET1_PD8(0x1p0),                    /* 1.0 (acos domain edge |x| = 1) */
    .minus_two = _MM512_SET1_PD8(-0x1p1),                   /* -2.0 */
    .mask_64   = _MM512_SET1_U64x8(0x7FFFFFFFFFFFFFFFUL),   /* abs-value mask: clears the sign bit to give |x| */
    /*
     * Region half-angle for the non-negative result, and pi for the
     * reflection acos(-|x|) = pi - acos(|x|).
     */
    .piby4     = _MM512_SET1_PD8(0x1.921fb54442d18p-1),     /* pi/4 = 0.7853981633974483 (doubled -> pi/2) */
    .pi        = _MM512_SET1_PD8(0x1.921fb54442d18p1),      /* pi = 3.1415926535897931 */
    /*
     * asin minimax polynomial coefficients (fpminimax, tools/sollya/
     * vrd8_asin.sollya) over the reduced interval [0, 0.5];
     * asin(r) ~ r + r*g*P(g), g = r*r. The leading terms track the Taylor
     * series (1/6, 3/40, 15/336, 105/3456) with a minimax-tuned tail for
     * the target ULP. These are coefficients 1..12 of the vrd8_asin
     * table, which additionally carries a leading 0x1.ffffffffdecd3p-1.
     */
    .poly_asin = {
        _MM512_SET1_PD8(0x1.55555555552aap-3),    /* 0.1666666666666477      ~ 1/6      */
        _MM512_SET1_PD8(0x1.333333337cbaep-4),    /* 0.0750000000041797      ~ 3/40     */
        _MM512_SET1_PD8(0x1.6db6db3c0984p-5),     /* 0.04464285678140856     ~ 15/336   */
        _MM512_SET1_PD8(0x1.f1c72dd86cbafp-6),    /* 0.03038196065035564     ~ 105/3456 */
        _MM512_SET1_PD8(0x1.6e89d3ff33aa4p-6),    /* 0.022371727970318958               */
        _MM512_SET1_PD8(0x1.1c6d83ae664b6p-6),    /* 0.01736009463784135                */
        _MM512_SET1_PD8(0x1.c6e1568b90518p-7),    /* 0.013881842859634605               */
        _MM512_SET1_PD8(0x1.8f6a58977fe49p-7),    /* 0.012189191110336799               */
        _MM512_SET1_PD8(0x1.a6ab10b3321bp-8),     /* 0.006449405266899452               */
        _MM512_SET1_PD8(0x1.43305ebb2428fp-6),    /* 0.01972588778568479                */
        _MM512_SET1_PD8(-0x1.0e874ec5e3157p-6),   /* -0.01651175205874841               */
        _MM512_SET1_PD8(0x1.06eec35b3b142p-5),    /* 0.0320962729982477                 */
    },
};

#define ALM_V8_ACOS_HALF      v8_acos_data.half
#define ALM_V8_ACOS_MAX_ARG   v8_acos_data.max_arg
#define ALM_V8_ACOS_MINUS_TWO v8_acos_data.minus_two
#define ALM_V8_ACOS_PIBY4     v8_acos_data.piby4
#define ALM_V8_ACOS_PI        v8_acos_data.pi
#define ALM_V8_ACOS_MASK_64   v8_acos_data.mask_64

#define C1  v8_acos_data.poly_asin[0]
#define C2  v8_acos_data.poly_asin[1]
#define C3  v8_acos_data.poly_asin[2]
#define C4  v8_acos_data.poly_asin[3]
#define C5  v8_acos_data.poly_asin[4]
#define C6  v8_acos_data.poly_asin[5]
#define C7  v8_acos_data.poly_asin[6]
#define C8  v8_acos_data.poly_asin[7]
#define C9  v8_acos_data.poly_asin[8]
#define C10 v8_acos_data.poly_asin[9]
#define C11 v8_acos_data.poly_asin[10]
#define C12 v8_acos_data.poly_asin[11]

static inline v_f64x8_t
acos_specialcase(v_f64x8_t _x, v_f64x8_t result, v_u64x8_t cond)
{
    return call_v8_f64(ALM_PROTO_OPT(acos), _x, result, cond);
}

v_f64x8_t
ALM_PROTO_OPT(vrd8_acos)(v_f64x8_t x)
{
    v_f64x8_t poly;
    v_f64x8_t aux;
    v_u64x8_t ux;
    v_u64x8_t outofrange;

    ux = as_v8_u64_f64(x);

    /* |x| */
    aux = as_v8_f64_u64(ux & ALM_V8_ACOS_MASK_64);

    /*
     * |x| >= 1 -> scalar fallback (the domain edge |x| = 1 and +/-Inf).
     * NaN is not selected here, as the comparison is false for it; NaN
     * lanes propagate through the polynomial path and come out as NaN.
     */
    outofrange = (v_u64x8_t)(aux >= ALM_V8_ACOS_MAX_ARG);

    /* |x| > 0.5 selects the sqrt (double-angle) path */
    __mmask8 big = _mm512_cmp_pd_mask(aux, ALM_V8_ACOS_HALF, _CMP_GT_OQ);

    /*
     * large-|x| path: g = 0.5*(1 - |x|), r = -2*sqrt(g) (hardware sqrt).
     * The 0.5 and -2 factors are exact powers of two, so g is exact and r
     * incurs a single rounding, the sqrt. The algebraically-equal form
     * r = -sqrt(2)*sqrt(1 - |x|) is deliberately NOT used: it replaces the
     * exact scaling with an irrational-constant multiply plus an extra
     * rounding, which pushes this kernel past its 1.5 ULP target.
     */
    v_f64x8_t one_minus_aux = _mm512_sub_pd(ALM_V8_ACOS_MAX_ARG, aux);
    v_f64x8_t g_big = _mm512_mul_pd(ALM_V8_ACOS_HALF, one_minus_aux);
    v_f64x8_t r_big = _mm512_mul_pd(ALM_V8_ACOS_MINUS_TWO,
                                    _mm512_sqrt_pd(g_big));

    /* small-|x| path: g = x^2, r = |x| */
    v_f64x8_t g_small = _mm512_mul_pd(aux, aux);

    v_f64x8_t g = _mm512_mask_blend_pd(big, g_small, g_big);
    v_f64x8_t r = _mm512_mask_blend_pd(big, aux, r_big);

    /* asin(r) ~ r + r*g*P(g) */
    poly = POLY_EVAL_12(g, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12);
    poly = r + r * g * poly;

    /* half-angle: 0 on the big path, pi/4 on the small one */
    v_f64x8_t kpos = _mm512_mask_blend_pd(big, ALM_V8_ACOS_PIBY4,
                                          _mm512_setzero_pd());

    /*
     * acos(|x|) = (K - poly) + K. The split into a subtract followed by an
     * add of K keeps the low-order bits that a single 2*K - poly would drop.
     */
    v_f64x8_t res_pos = _mm512_add_pd(_mm512_sub_pd(kpos, poly), kpos);

    /* acos(-|x|) = pi - acos(|x|), so the negative case reuses res_pos */
    v_f64x8_t res_neg = _mm512_sub_pd(ALM_V8_ACOS_PI, res_pos);

    /* select by sign bit of x */
    __mmask8 neg = _mm512_movepi64_mask(_mm512_castpd_si512(x));
    v_f64x8_t result = _mm512_mask_blend_pd(neg, res_pos, res_neg);

    if (unlikely(any_v8_u64_avx512(outofrange)))
        return acos_specialcase(x, result, outofrange);

    return result;
}
