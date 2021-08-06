/*
 * Copyright (C) 2019-2020, Advanced Micro Devices. All rights reserved.
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

#include <libm_util_amd.h>
#include <libm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>

#include <libm/poly.h>

static const
struct {
    //v_f64x4_t   tblsz_byln2;
    v_f32x8_t   huge;
    v_f32x8_t   ln2;
    v_u32x8_t   arg_min;
    v_u32x8_t   arg_max;
    v_i32x8_t   mask;
    v_f32x8_t   exp2f_max, exp2f_min;
    v_i32x8_t   infinity;
    v_i32x8_t   bias;
    v_f32x8_t   poly_exp2f[9];
} v_exp2f_data ={
    //.tblsz_byln2 =  _MM_SET1_PD4(0x1.71547652b82fep+0),
    .huge        =  _MM256_SET1_PS8(0x1.8p+23f),
    .arg_min     =  _MM256_SET1_I32(0xFFFFFF99),
    .arg_max     =  _MM256_SET1_I32(0x42fe0000),
    .mask        =  _MM256_SET1_I32(0x7fffffff),
    .infinity    =  _MM256_SET1_I32(0x7f800000),
    .exp2f_min   =  _MM_SET1_PS8(-0x1.9fe368p6f),
    .bias        =  _MM256_SET1_I32(127),
    .exp2f_max   =  _MM_SET1_PS8(88.7228393f),
    .ln2         =  _MM_SET1_PS8(0x1.62e42fefa39efp-1f),

    /*
     * Polynomial coefficients obtained using Remez algorithm
     */
    .poly_exp2f = {
        _MM_SET1_PS8(0x1p0f),
        _MM_SET1_PS8(0x1p-1f),
        _MM_SET1_PS8(0x1.5555555555555p-3f),
        _MM_SET1_PS8(0x1.5555555555556p-5f),
        _MM_SET1_PS8(0x1.111111111110fp-7f),
        _MM_SET1_PS8(0x1.6c16c16bee4d5p-10f),
        _MM_SET1_PS8(0x1.a01a01a093e4ep-13f),
        _MM_SET1_PS8(0x1.a01aba8ed59e3p-16f),
        _MM_SET1_PS8(0x1.71ddb3d62728ep-19f),
    },
};




#define SCALAR_EXP2F ALM_PROTO_OPT(exp2f)

/*
 * FIXME: use correct ARG_MAX/ARG_MIN
 *            as well as EXP2F_MAX/EXP2F_MIN
 */
//#define ALM_V8_EXP2F_TBL_LN2      v_exp2f_data.tblsz_byln2
#define ALM_V8_EXP2F_HUGE         v_exp2f_data.huge
#define ALM_V8_EXP2F_MASK         v_exp2f_data.mask
#define ALM_V8_EXP2F_INF          v_exp2f_data.infinity
#define ALM_V8_EXP2F_MAX          v_exp2f_data.exp2f_max
#define ALM_V8_EXP2F_MIN          v_exp2f_data.exp2f_min
#define ALM_V8_EXP2F_ARG_MAX      v_exp2f_data.arg_max
#define ALM_V8_EXP2F_ARG_MIN      v_exp2f_data.arg_min
#define ALM_V8_EXP2F_BIAS         v_exp2f_data.bias
#define ALM_V8_EXP2F_LN2          v_exp2f_data.ln2
#define ALM_V8_EXP2F_OFF          ARG_MAX - ARG_MIN

#define C1 v_exp2f_data.poly_exp2f[0]
#define C2 v_exp2f_data.poly_exp2f[1]
#define C3 v_exp2f_data.poly_exp2f[2]
#define C4 v_exp2f_data.poly_exp2f[3]
#define C5 v_exp2f_data.poly_exp2f[4]
#define C6 v_exp2f_data.poly_exp2f[5]
#define C7 v_exp2f_data.poly_exp2f[6]
#define C8 v_exp2f_data.poly_exp2f[7]
#define C9 v_exp2f_data.poly_exp2f[8]

/*
 * C implementation of exp single precision 128-bit vector version (v8s)
 *
 * Implementation Notes
 * ----------------------
 * 1. Argument Reduction:
 *      2^x                                      --- (1)
 *
 *      Let z  = x                               --- (2)
 *
 *      Let z = n + r , where n is an integer    --- (3)
 *                      |r| <= 1/2
 *
 *     From (1), (2) and (3),
 *      e^x = 2^z
 *          = 2^(n+r)
 *          = (2^n)*(2^r)                        --- (4)
 *
 * 2. Polynomial Evaluation
 *   From (4),
 *     r   = z - n
 *     2^r = C1 + C2*r + C3*r^2 + C4*r^3 + C5 *r^4 + C6*r^5
 *
 * 4. Reconstruction
 *      Thus,
 *        e^x = (2^n) * (2^r)
 *
 *
 */

v_f32x8_t
ALM_PROTO_OPT(vrs8_exp2f)(v_f32x8_t _x)
{
    /* vx = int(x) */
    v_u32x8_t vx = as_v8_u32_f32(_x);

    /* Get absolute value of vx */
    vx = vx & ALM_V8_EXP2F_MASK;

    /* Check if -127 < vx < 127 */
    v_u32x8_t cond = (vx > ALM_V8_EXP2F_ARG_MAX);

    /* x * (64.0/ln(2)) */
    //v_f32x8_t z = _x * TBL_LN2;

    v_f32x8_t dn = _x + ALM_V8_EXP2F_HUGE;

    /* n = int(z) */
    v_u32x8_t n = as_v8_u32_f32(dn);

    /* dn = double(n) */
    dn = dn - ALM_V8_EXP2F_HUGE;

    v_f32x8_t r = _x - dn;

    r *= ALM_V8_EXP2F_LN2;

    /* 2^m;  m = (n - j)/64 */
    v_u32x8_t m = (n + ALM_V8_EXP2F_BIAS) << 23;

    /* Compute polynomial
     * poly = A1 + A2*r + A3*r^2 + A4*r^3 + A5*r^4 + A6*r^5
     *       = (A1 + A2*r) + r^2(A3 + A4*r) + r^4(A5 + A6*r)
		 */
    v_f32x8_t poly = 1.0f + POLY_EVAL_9(r, C1, C2, C3, C4, C5, C6, C7, C8, C9);

    /* result = polynomial * 2^m */
    v_f32x8_t result = poly * as_v8_f32_u32(m);

    /* If input value is outside valid range, call scalar expf(value)
     * Else, return the above computed result
		 */
    if(unlikely(any_v8_u32_loop(cond))) {
        result = (v_f32x8_t) {
            cond[0] ? SCALAR_EXP2F(_x[0]) : result[0],
            cond[1] ? SCALAR_EXP2F(_x[1]) : result[1],
            cond[2] ? SCALAR_EXP2F(_x[2]) : result[2],
            cond[3] ? SCALAR_EXP2F(_x[3]) : result[3],
            cond[4] ? SCALAR_EXP2F(_x[4]) : result[4],
            cond[5] ? SCALAR_EXP2F(_x[5]) : result[5],
            cond[6] ? SCALAR_EXP2F(_x[6]) : result[6],
            cond[7] ? SCALAR_EXP2F(_x[7]) : result[7],

        };
    }

    return result;
}
