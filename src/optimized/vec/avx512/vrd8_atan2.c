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

/* Contains implementation of v_f64x8_t vrd8_atan2(v_f64x8_t y, v_f64x8_t x)
 *
 * Note : Relation to atan implementation
 * atan2(y, x) is built from atan(min(|x|,|y|)/max(|x|,|y|)), which lives on
 * [0, pi/4], plus a quadrant reconstruction.  Because the ratio r = num/den is
 * always in [0, 1], the degree-19 argument reduction of vrd8_atan collapses to
 * a single region boundary at RANGE = 2-sqrt(3):
 *
 *      r <= RANGE :  atan(r) = poly(r)
 *      r >  RANGE :  atan(r) = pi/6 + poly( (r*sqrt3 - 1)/(sqrt3 + r) )
 *
 * Both the region test and the reduced argument are formed without ever
 * computing r, so only ONE divide is needed and the whole kernel is branchless:
 *
 *      r > RANGE              <=>  num > RANGE*den            (den > 0)
 *      (r*sqrt3-1)/(sqrt3+r)   =  (num*sqrt3 - den)/(sqrt3*den + num)
 *
 * The reduced argument rr lives in [-(2-sqrt3), 2-sqrt3] and is approximated by
 * a degree-17 odd minimax polynomial.
 */

#include <immintrin.h>
#include <stdint.h>

#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/constants.h>
#include <libm/poly.h>
#include <libm/typehelper-vec.h>

#define ATAN2_VRD
#include "../../atan2_data.h"

static struct {
    double    sqrt3_ps, prescale, range, pi6, pi2, pi, scale_up;
    v_f64x8_t poly_atan[8];
} v8_atan2_data = {
    .sqrt3_ps = ATAN2_VRD_SQRT3_PS,
    .prescale = ATAN2_VRD_PRESCALE,
    .range    = ATAN2_VRD_RANGE,
    .pi6      = ATAN2_VRD_PI6,
    .pi2      = ATAN2_VRD_PI2,
    .pi       = ATAN2_VRD_PI,
    .scale_up = ATAN2_VRD_SCALE_UP,
    .poly_atan = {
        _MM512_SET1_PD8(ATAN2_VRD_POLY_C0),
        _MM512_SET1_PD8(ATAN2_VRD_POLY_C1),
        _MM512_SET1_PD8(ATAN2_VRD_POLY_C2),
        _MM512_SET1_PD8(ATAN2_VRD_POLY_C3),
        _MM512_SET1_PD8(ATAN2_VRD_POLY_C4),
        _MM512_SET1_PD8(ATAN2_VRD_POLY_C5),
        _MM512_SET1_PD8(ATAN2_VRD_POLY_C6),
        _MM512_SET1_PD8(ATAN2_VRD_POLY_C7),
    },
};

#define SQRT3_PS v8_atan2_data.sqrt3_ps
#define PRESCALE v8_atan2_data.prescale
#define RANGE    v8_atan2_data.range
#define PI6      v8_atan2_data.pi6
#define PI2      v8_atan2_data.pi2
#define PI       v8_atan2_data.pi
#define SCALE_UP v8_atan2_data.scale_up
#define C0 v8_atan2_data.poly_atan[0]
#define C1 v8_atan2_data.poly_atan[1]
#define C2 v8_atan2_data.poly_atan[2]
#define C3 v8_atan2_data.poly_atan[3]
#define C4 v8_atan2_data.poly_atan[4]
#define C5 v8_atan2_data.poly_atan[5]
#define C6 v8_atan2_data.poly_atan[6]
#define C7 v8_atan2_data.poly_atan[7]

#define ABS_MASK 0x7FFFFFFFFFFFFFFFLL
#define SIGN_BIT 0x8000000000000000ULL

v_f64x8_t
ALM_PROTO_OPT(vrd8_atan2)(v_f64x8_t y, v_f64x8_t x)
{
    const v_u64x8_t abs_mask = _MM512_SET1_U64x8((uint64_t)ABS_MASK);
    const v_u64x8_t sign_bit = _MM512_SET1_U64x8((uint64_t)SIGN_BIT);

    v_u64x8_t uxi = as_v8_u64_f64(x);
    v_u64x8_t uyi = as_v8_u64_f64(y);

    /* sign of y for the final copysign */
    v_u64x8_t ysign = uyi & sign_bit;

    /* raw magnitudes |x|, |y| */
    v_u64x8_t auxi = uxi & abs_mask;
    v_u64x8_t auyi = uyi & abs_mask;

    /* min/max swap handled in int as compare is still valid
     * and for any NaN/Inf, resulting amax is at or above INF
     * making it suitable for special case check. */
    v_u64x8_t amax = max_v8_u64(auxi, auyi);

    /*
     * Special case handling for :
     *   amax == 0    the 0/0 case, there is no ratio to form;
     *   amax >= INF  at least one operand is Inf/NaN.
     * Both detected with a single check.
     * Check intentionally placed here as vector compute
     * for lanes with these values raise undesired exceptions
     * and accounting for them adds compute overhead.
     */
    if (unlikely(cmpge_v8_u64(amax - _MM512_SET1_U64x8((uint64_t)1),
                              _MM512_SET1_U64x8((uint64_t)ALM_F64_INF - 1)))) {
        return call2_v8_f64(ALM_PROTO_OPT(atan2), y, x, y,
                            (v_i64x8_t)V8_ALL_ONES_U64);
    }

    v_u64x8_t amin = min_v8_u64(auxi, auyi);
    v_mask8_t swapped = cmpgt_v8_u64(auyi, auxi);

    /* scale small lanes up so that rr compute keeps full precision;
     * proportional scaling; done only when both x, y are below SMALL_LIM */
    v_mask8_t sub = cmplt_v8_u64(amax, _MM512_SET1_U64x8(ATAN2_VRD_SMALL_LIM));
    const v_f64x8_t scale = _MM512_SET1_PD8(SCALE_UP);
    v_f64x8_t num = as_v8_f64_u64(amin);
    v_f64x8_t den = as_v8_f64_u64(amax);
    num = mask_mul_v8_f64(num, sub, num, scale);
    den = mask_mul_v8_f64(den, sub, den, scale);

    /* r = num/den > RANGE  <=>  num > RANGE*den */
    v_mask8_t red = cmpgt_v8_f64(num, RANGE * den);

    /* single divide feeding the odd polynomial:
     *   rr = red ? (num*sqrt3 - den)/(den*sqrt3 + num) : num/den
     * the reduced terms carry an exact 1/4 so that den*sqrt3 + num stays
     * finite for operands near DBL_MAX; rr is unaffected by the scale */
    v_f64x8_t pnum = blend_v8_f64(red, num * SQRT3_PS - den * PRESCALE, num);
    v_f64x8_t pden = blend_v8_f64(red, den * SQRT3_PS + num * PRESCALE, den);
    v_f64x8_t rr = pnum / pden;

    /* atan(rr) in [0, pi/12] via the degree-17 odd minimax polynomial */
    v_f64x8_t poly = POLY_EVAL_ODD_17_BAL(rr, C0, C1, C2, C3, C4, C5, C6, C7);

    /* atan(min/max) in [0, pi/4]: add pi/6 in the reduced region */
    v_f64x8_t atan_base = mask_add_v8_f64(poly, red, poly, _MM512_SET1_PD8(PI6));

    /* quadrant reconstruction: 
     *   swapped | xneg | flip | based       | offset | res            | range
     *   --------+------+------+-------------+--------+----------------+-----------
     *      0    |  0   |  0   |  +atan_base |  0     | atan_base      | [0,   pi/4]
     *      1    |  0   |  1   |  -atan_base | pi/2   | pi/2-atan_base | [pi/4,pi/2]
     *      1    |  1   |  0   |  +atan_base | pi/2   | pi/2+atan_base | [pi/2,3pi/4]
     *      0    |  1   |  1   |  -atan_base | pi     | pi  -atan_base | [3pi/4,pi]
     */
    v_mask8_t xneg = movepi_v8_u64(uxi);
    v_mask8_t flip = swapped ^ xneg;
    v_f64x8_t neg_base = as_v8_f64_u64(as_v8_u64_f64(atan_base) ^ sign_bit);
    v_f64x8_t based = blend_v8_f64(flip, neg_base, atan_base);

    v_f64x8_t offset = _MM512_SET1_PD8(0.0);
    offset = blend_v8_f64(xneg, _MM512_SET1_PD8(PI), offset);
    offset = blend_v8_f64(swapped, _MM512_SET1_PD8(PI2), offset);
    v_f64x8_t res = based + offset;

    /* copysign(res, y) */
    return as_v8_f64_u64((as_v8_u64_f64(res) & abs_mask) | ysign);
}
