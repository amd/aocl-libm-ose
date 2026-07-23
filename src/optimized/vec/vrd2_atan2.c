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

/* Contains implementation of v_f64x2_t vrd2_atan2(v_f64x2_t y, v_f64x2_t x)
 *
 * Note : Relation to atan implementation
 * atan2(y, x) is built from atan(min(|x|,|y|)/max(|x|,|y|)), which lives on
 * [0, pi/4], plus a quadrant reconstruction.  Because the ratio r = num/den is
 * always in [0, 1], the argument reduction of vrd2_atan collapses to a single
 * region boundary at RANGE = 2-sqrt(3):
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
 *
 * This is the SSE (128-bit, 2-lane) counterpart of vrd4_atan2 / vrd8_atan2.
 * Predication uses full-width vector masks (0/-1 per lane) and variable blends
 * (VBLENDVPD); merge-masked arithmetic is emulated as compute-then-blend.
 */

#include <immintrin.h>
#include <stdint.h>

#include <libm/amd_funcs_internal.h>
#include <libm/poly.h>
#include <libm/typehelper-vec.h>

#define ATAN2_VRD
#include "../atan2_data.h"

static struct {
    double    sqrt3, range, pi6, pi2, pi, scale_up;
    v_f64x2_t poly_atan[8];
} v2_atan2_data = {
    .sqrt3    = ATAN2_VRD_SQRT3,
    .range    = ATAN2_VRD_RANGE,
    .pi6      = ATAN2_VRD_PI6,
    .pi2      = ATAN2_VRD_PI2,
    .pi       = ATAN2_VRD_PI,
    .scale_up = ATAN2_VRD_SCALE_UP,
    .poly_atan = {
        _MM_SET1_PD2(ATAN2_VRD_POLY_C0),
        _MM_SET1_PD2(ATAN2_VRD_POLY_C1),
        _MM_SET1_PD2(ATAN2_VRD_POLY_C2),
        _MM_SET1_PD2(ATAN2_VRD_POLY_C3),
        _MM_SET1_PD2(ATAN2_VRD_POLY_C4),
        _MM_SET1_PD2(ATAN2_VRD_POLY_C5),
        _MM_SET1_PD2(ATAN2_VRD_POLY_C6),
        _MM_SET1_PD2(ATAN2_VRD_POLY_C7),
    },
};

#define SQRT3    v2_atan2_data.sqrt3
#define RANGE    v2_atan2_data.range
#define PI6      v2_atan2_data.pi6
#define PI2      v2_atan2_data.pi2
#define PI       v2_atan2_data.pi
#define SCALE_UP v2_atan2_data.scale_up
#define C0 v2_atan2_data.poly_atan[0]
#define C1 v2_atan2_data.poly_atan[1]
#define C2 v2_atan2_data.poly_atan[2]
#define C3 v2_atan2_data.poly_atan[3]
#define C4 v2_atan2_data.poly_atan[4]
#define C5 v2_atan2_data.poly_atan[5]
#define C6 v2_atan2_data.poly_atan[6]
#define C7 v2_atan2_data.poly_atan[7]

#define ABS_MASK 0x7FFFFFFFFFFFFFFFLL
#define SIGN_BIT 0x8000000000000000ULL

v_f64x2_t
ALM_PROTO_OPT(vrd2_atan2)(v_f64x2_t y, v_f64x2_t x)
{
    const uint64_t abs_mask = (uint64_t)ABS_MASK;
    const uint64_t sign_bit = (uint64_t)SIGN_BIT;

    v_u64x2_t uxi = as_v2_u64_f64(x);
    v_u64x2_t uyi = as_v2_u64_f64(y);

    /* sign of y for the final copysign */
    v_u64x2_t ysign = uyi & sign_bit;

    /* |x|, |y| and their raw magnitudes */
    v_u64x2_t auxi = uxi & abs_mask;
    v_u64x2_t auyi = uyi & abs_mask;
    v_f64x2_t ax = as_v2_f64_u64(auxi);
    v_f64x2_t ay = as_v2_f64_u64(auyi);
    v_u64x2_t orv = auxi | auyi;

    /* scale subnormal lanes up so that rr compute keeps full precision;
     * proportional scaling; done only when both x, y are subnormal */
    v_u64x2_t sub = (v_u64x2_t)(orv < ATAN2_VRD_SUBNORM_LIM);
    ax = mask_mul_v2_f64(ax, sub, ax, SCALE_UP);
    ay = mask_mul_v2_f64(ay, sub, ay, SCALE_UP);

    /* num = min(|x|,|y|), den = max(|x|,|y|), swapped when |y| > |x| */
    v_f64x2_t num = min_v2_f64(ax, ay);
    v_f64x2_t den = max_v2_f64(ax, ay);
    v_u64x2_t swapped = (v_u64x2_t)(ay > ax);

    /* fold the all-zero case (0/0) to rr = 0 (den forced to 1). Ensuring rr = 0
     * for this case produces correct results for all +/- 0 input combinations*/
    v_u64x2_t zero = (v_u64x2_t)(orv == (uint64_t)0);
    den = mask_mov_v2_f64(den, zero, _MM_SET1_PD2(1.0));
    num = mask_mov_v2_f64(num, zero, _MM_SET1_PD2(0.0));

    /* r = num/den > RANGE  <=>  num > RANGE*den */
    v_u64x2_t red = (v_u64x2_t)(num > RANGE * den);

    /* single divide feeding the odd polynomial:
     *   rr = red ? (num*sqrt3 - den)/(den*sqrt3 + num) : num/den */
    v_f64x2_t pnum = blend_v2_f64(red, num * SQRT3 - den, num);
    v_f64x2_t pden = blend_v2_f64(red, den * SQRT3 + num, den);
    v_f64x2_t rr = pnum / pden;

    /* atan(rr) in [0, pi/12] via the degree-17 odd minimax polynomial */
    v_f64x2_t poly = POLY_EVAL_ODD_17_BAL(rr, C0, C1, C2, C3, C4, C5, C6, C7);

    /* atan(min/max) in [0, pi/4]: add pi/6 in the reduced region */
    v_f64x2_t atan_base = mask_add_v2_f64(poly, red, poly, PI6);

    /* quadrant reconstruction:
     *   swapped | xneg | flip | based       | offset | res            | range
     *   --------+------+------+-------------+--------+----------------+-----------
     *      0    |  0   |  0   |  +atan_base |  0     | atan_base      | [0,   pi/4]
     *      1    |  0   |  1   |  -atan_base | pi/2   | pi/2-atan_base | [pi/4,pi/2]
     *      1    |  1   |  0   |  +atan_base | pi/2   | pi/2+atan_base | [pi/2,3pi/4]
     *      0    |  1   |  1   |  -atan_base | pi     | pi  -atan_base | [3pi/4,pi]
     */
    v_u64x2_t xneg = (v_u64x2_t)((v_i64x2_t)uxi < 0);
    v_u64x2_t flip = swapped ^ xneg;
    v_f64x2_t neg_base = as_v2_f64_u64(as_v2_u64_f64(atan_base) ^ sign_bit);
    v_f64x2_t based = blend_v2_f64(flip, neg_base, atan_base);

    v_f64x2_t offset = _MM_SET1_PD2(0.0);
    offset = blend_v2_f64(xneg, _MM_SET1_PD2(PI), offset);
    offset = blend_v2_f64(swapped, _MM_SET1_PD2(PI2), offset);
    v_f64x2_t res = based + offset;

    /* copysign(res, y) */
    return as_v2_f64_u64((as_v2_u64_f64(res) & abs_mask) | ysign);
}
