/*
 * Copyright (C) 2021-2022, Advanced Micro Devices. All rights reserved.
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
 *
 */

#include <libm_util_amd.h>
#include <libm/alm_special.h>

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <emmintrin.h>

#define AMD_LIBM_FMA_USABLE 1           /* needed for poly.h */
#include <libm/poly-vec.h>

#define VRS8_LOG10F_MAX_POLY_SIZE 14

static const struct {
    v_u32x8_t v_min, v_max, v_mask, v_off;
    v_f32x8_t v_one;
    v_f32x8_t ln2_10;
    v_f32x8_t poly[VRS8_LOG10F_MAX_POLY_SIZE];
} v_log10f_data = {
    .v_min  = _MM256_SET1_I32(0x00800000),
    .v_max  = _MM256_SET1_I32(0x7f800000),
    .v_mask = _MM256_SET1_I32(MANTBITS_SP32),
    .v_off  = _MM256_SET1_I32(0x3f2aaaab),
    .v_one  = _MM256_SET1_PS8(1.0f),
    .ln2_10    = _MM256_SET1_PS8(0x1.344136p-2f), /* log10(2) */
    .poly = {
            _MM256_SET1_PS8(0.0f),
            _MM256_SET1_PS8(0x1.bcb7b2p-2f),
            _MM256_SET1_PS8(-0x1.bcb7b2p-3f),
            _MM256_SET1_PS8(0x1.287a8p-3f),
            _MM256_SET1_PS8(-0x1.bcb7e8p-4f),
            _MM256_SET1_PS8(0x1.63c118p-4f),
            _MM256_SET1_PS8(-0x1.286fbcp-4f),
            _MM256_SET1_PS8(0x1.fd7686p-5f),
            _MM256_SET1_PS8(-0x1.beb002p-5f),
            _MM256_SET1_PS8(0x1.797fe2p-5f),
            _MM256_SET1_PS8(-0x1.4c5f34p-5f),
            _MM256_SET1_PS8(0x1.bc558ap-5f),
            _MM256_SET1_PS8(-0x1.adad4cp-5f),
    },
};


#define V_MIN     v_log10f_data.v_min
#define V_MAX     v_log10f_data.v_max
#define V_MASK    v_log10f_data.v_mask
#define V_OFF     v_log10f_data.v_off
#define V_ONE     v_log10f_data.v_one
#define LN2_BY_10 v_log10f_data.ln2_10


/*
 * Short names for polynomial coefficients
 */
#define C0       v_log10f_data.poly[0]
#define C1       v_log10f_data.poly[1]
#define C2       v_log10f_data.poly[2]
#define C3       v_log10f_data.poly[3]
#define C4       v_log10f_data.poly[4]
#define C5       v_log10f_data.poly[5]
#define C6       v_log10f_data.poly[6]
#define C7       v_log10f_data.poly[7]
#define C8       v_log10f_data.poly[8]
#define C9       v_log10f_data.poly[9]
#define C10      v_log10f_data.poly[10]
#define C11      v_log10f_data.poly[11]
#define C12      v_log10f_data.poly[12]

#define V_I32(x) x.i32x4
#define V_F32(x) x.f32x4

/*
 * Signature:
 *   v_f32x8_t vrs4_log10f(v_f32x8_t x)
 *
 * Spec:
 *   log2f(x)
 *          = log2f(x)           if x ∈ F and x > 0
 *          = x                 if x = qNaN
 *          = 0                 if x = 1
 *          = -inf              if x = (-0, 0}
 *          = NaN               otherwise
 *
 * Assumptions/Expectations
 *      - Maximum ULP is observed to be at 4.29
 *      - Some FPU Exceptions may not be available
 *      - Performance is at least 3x
 *
 * Implementation Notes:
 *  1. Range Reduction:
 *      x = 2^n*(1+f)                                          .... (1)
 *         where n is exponent and is an integer
 *             (1+f) is mantissa ∈ [1,2). i.e., 1 ≤ 1+f < 2    .... (2)
 *
 *    From (1), taking log10 on both sides
 *      log10(x) = log10(2^n * (1+f))
 *             = n*log10(2) + log10(1+f)                           .... (3)
 *
 *      let z = 1 + f
 *             log10(z) = log10(k) + log10(z) - log10(k)
 *             log10(z) = log10(kz) - log10(k)
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
 *     More information refer to tools/sollya/vrs4_log10f.sollya
 *
 *
 */


static inline v_f32x8_t
log10f_specialcase(v_f32x8_t _x,
                 v_f32x8_t result,
                 v_u32x8_t cond)
{
    return call_v8_f32(ALM_PROTO(log10f), _x, result, cond);
}


v_f32x8_t
ALM_PROTO_OPT(vrs8_log10f)(v_f32x8_t _x)
{

    v_f32x8_t poly, r, n, result;

    v_u32x8_t vx = as_v8_u32_f32(_x);

    v_u32x8_t cond = (vx - V_MIN >= V_MAX - V_MIN);

    vx -= V_OFF;

    n = cast_v8_i32_to_f32(((v_i32x8_t)vx) >> 23);

    vx &= V_MASK;

    vx += V_OFF;

    r = as_v8_f32_u32(vx) - V_ONE ;

    /* poly = C0 + r*(C1 + r*(C2 + r*(C3 + r*(C4 + r*C5 + r*(C6 + r*C7 + r*(C8 + r*(C9 + r*(C10 + r*(C11 + r*C12)))))))))*/
    poly = POLY_EVAL_12(r, C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12);

    result = n * LN2_BY_10 + poly;

    if (unlikely(any_v8_u32_loop(cond))) {
        return log10f_specialcase(_x, result, cond);
    }

    return result;
}


