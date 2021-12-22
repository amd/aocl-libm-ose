/*
 * Copyright (C) 2018-2021, Advanced Micro Devices. All rights reserved.
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

#include <libm_macros.h>
#include <libm/compiler.h>
#include <libm_util_amd.h>
#include <libm/types.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>

#define AMD_LIBM_FMA_USABLE 1           /* needed for poly.h */
#include <libm/poly-vec.h>

#include <libm/arch/zen4.h>

#define VRS4_LOGF_POLY_DEGREE 10

#define VRS4_LOGF_MAX_POLY_SIZE 12

static const struct {
    v_u32x16_t v_min, v_max, v_mask, v_off;
    v_f32x16_t v_one;
    v_f32x16_t ln2;
    v_f32x16_t poly[VRS4_LOGF_MAX_POLY_SIZE];
} v16_logf_data = {
    .v_min  = _MM512_SET1_U32x16((uint32_t)0x00800000),
    .v_max  = _MM512_SET1_U32x16((uint32_t)0x7f800000),
    .v_mask = _MM512_SET1_U32x16((uint32_t)MANTBITS_SP32),
    .v_off  = _MM512_SET1_U32x16((uint32_t)0x3f2aaaab),
    .v_one  = _MM512_SET1_PS16(1.0f),
    .ln2    = _MM512_SET1_PS16(0x1.62e43p-1f), /* ln(2) */
    .poly = {
            _MM512_SET1_PS16(0.0f),
            _MM512_SET1_PS16(0x1.0p0f),      /* 1.0 */
#if VRS4_LOGF_POLY_DEGREE == 7
            _MM512_SET1_PS16(-0x1.000478p-1f),
            _MM512_SET1_PS16(0x1.556906p-2f),
            _MM512_SET1_PS16(-0x1.fc0968p-3f),
            _MM512_SET1_PS16(0x1.93bf0cp-3f),
            _MM512_SET1_PS16(-0x1.923814p-3f),
            _MM512_SET1_PS16(0x1.689e5ep-3f),
#elif VRS4_LOGF_POLY_DEGREE == 8
            _MM512_SET1_PS16(-0x1.ffff9p-2f),
            _MM512_SET1_PS16(0x1.5561aep-2f),
            _MM512_SET1_PS16(-0x1.002598p-2f),
            _MM512_SET1_PS16(0x1.952ef4p-3f),
            _MM512_SET1_PS16(-0x1.4dfa02p-3f),
            _MM512_SET1_PS16(0x1.6023a8p-3f),
            _MM512_SET1_PS16(-0x1.460368p-3f),
#elif VRS4_LOGF_POLY_DEGREE == 10
            _MM512_SET1_PS16(-0x1.000004p-1f),
            _MM512_SET1_PS16(0x1.55545ep-2f),
            _MM512_SET1_PS16(-0x1.fffbp-3f),
            _MM512_SET1_PS16(0x1.99fc74p-3f),
            _MM512_SET1_PS16(-0x1.56061ep-3f),
            _MM512_SET1_PS16(0x1.1cba2ap-3f),
            _MM512_SET1_PS16(-0x1.ea79f2p-4f),
            _MM512_SET1_PS16(0x1.26f1fep-3f),
            _MM512_SET1_PS16(-0x1.17b714p-3f),
#else
#error "Unknown Polynomial degree"
#endif
    },
};


#define V16_LOGF_MIN   v16_logf_data.v_min
#define V16_LOGF_MAX   v16_logf_data.v_max
#define V16_LOGF_MASK  v16_logf_data.v_mask
#define V16_LOGF_OFF   v16_logf_data.v_off
#define V16_LOGF_ONE   v16_logf_data.v_one
#define V16_LN2        v16_logf_data.ln2


/*
 * Short names for polynomial coefficients
 */
#define C0       v16_logf_data.poly[0]
#define C1       v16_logf_data.poly[1]
#define C2       v16_logf_data.poly[2]
#define C3       v16_logf_data.poly[3]
#define C4       v16_logf_data.poly[4]
#define C5       v16_logf_data.poly[5]
#define C6       v16_logf_data.poly[6]
#define C7       v16_logf_data.poly[7]
#define C8       v16_logf_data.poly[8]
#define C9       v16_logf_data.poly[9]
#define C10      v16_logf_data.poly[10]
#define C11      v16_logf_data.poly[11]

#define V_I32(x) x.i32x16
#define V_F32(x) x.f32x16

/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   float logf(float x)
 *
 * Spec:
 *   logf(x)
 *          = logf(x)           if x ∈ F and x > 0
 *          = x                 if x = qNaN
 *          = 0                 if x = 1
 *          = -inf              if x = (-0, 0}
 *          = NaN               otherwise
 *
 * Assumptions/Expectations
 *      - ULP is derived to be << 4 (always)
 *	- Some FPU Exceptions may not be available
 *      - Performance is at least 3x
 *
 * Implementation Notes:
 *  1. Range Reduction:
 *      x = 2^n*(1+f)                                          .... (1)
 *         where n is exponent and is an integer
 *             (1+f) is mantissa ∈ [1,2). i.e., 1 ≤ 1+f < 2    .... (2)
 *
 *    From (1), taking log on both sides
 *      log(x) = log(2^n * (1+f))
 *             = log(2^n) + log(1+f)
 *             = n*log(2) + log(1+f)                           .... (3)
 *
 *      let z = 1 + f
 *             log(z) = log(k) + log(z) - log(k)
 *             log(z) = log(kz) - log(k)
 *
 *    From (2), range of z is [1, 2)
 *       by simply dividing range by 'k', z is in [1/k, 2/k)  .... (4)
 *       Best choice of k is the one which gives equal and opposite values
 *       at extrema        +-      -+
 *              1          | 2      |
 *             --- - 1 = - |--- - 1 |
 *              k          | k      |                         .... (5)
 *                         +-      -+
 *
 *       Solving for k, k = 3/2,
 *    From (4), using 'k' value, range is therefore [-0.3333, 0.3333]
 *
 *  2. Polynomial Approximation:
 *     More information refer to tools/sollya/vrs4_logf.sollya
 *
 *     7th Deg -   Error abs: 0x1.04c4ac98p-22   rel: 0x1.2216e6f8p-19
 *     6th Deg -   Error abs: 0x1.179e97d8p-19   rel: 0x1.db676c1p-17
 *
 */


static inline v_f32x16_t
logf_specialcase(v_f32x16_t _x,
                 v_f32x16_t result,
                 v_u32x16_t cond)
{
    v_f32x16_t ret;

    for (int i = 0; i < 16; i += 4) {

        v_f32x4_t _x1 = {_x[i], _x[i+1], _x[i+2], _x[i+3]};

        v_u32x4_t _cond1 = {cond[i], cond[i+1], cond[i+2], cond[i+3]};

        v_f32x4_t _res1 = {result[i], result[i+1], result[i+2], result[i+3]};

        _res1 = call_v4_f32(ALM_PROTO(logf), _x1, _res1, _cond1);

        ret[i]   = _res1[0];

        ret[i+1] = _res1[1];

        ret[i+2] = _res1[2];

        ret[i+3] = _res1[3];
    }

    return ret;
}

v_f32x16_t
ALM_PROTO_ARCH_ZN4(vrs16_logf)(v_f32x16_t _x)
{
    v_f32x16_t q, r, n;

    v_u32x16_t vx =  as_v16_u32_f32(_x);

    v_u32x16_t cond = (vx - V16_LOGF_MIN >= V16_LOGF_MAX - V16_LOGF_MIN);

    vx -= V16_LOGF_OFF;

    n = cast_v16_i32_to_f32(((v_i32x16_t)vx) >> 23);

    vx &= V16_LOGF_MASK;

    vx += V16_LOGF_OFF;

    r = as_v16_f32_u32(vx) - V16_LOGF_ONE;

#if VRS4_LOGF_POLY_DEGREE == 7
    /* n*ln2 + r + r2*(C1, + r*C2 + r2*(C3 + r*C4 + r2*(C5 + r*C6 + r2*(C7)))) */
    q = POLY_EVAL_7(r, C0, C1, C2, C3, C4, C5, C6, C7);
#elif VRS4_LOGF_POLY_DEGREE == 8
    q = POLY_EVAL_8(r, C0, C1, C2, C3, C4, C5, C6, C7, C8);
#elif VRS4_LOGF_POLY_DEGREE == 10
    q = POLY_EVAL_10(r, C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10);
#endif

    q = n * V16_LN2 + q;

    if (unlikely(any_v16_u32_loop(cond))) {
        return logf_specialcase(_x, q, cond);
    }

    return q;
}


