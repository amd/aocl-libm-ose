/*
 * Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
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
 * C implementation of exp single precision 256-bit vector version (v8s)
 *
 * Implementation Notes
 * ----------------------
 * 1. Argument Reduction:
 *      e^x = 2^(x/ln2)                          --- (1)
 *
 *      Let x/ln(2) = z                          --- (2)
 *
 *      Let z = n + r , where n is an integer    --- (3)
 *                      |r| <= 1/2
 *
 *     From (1), (2) and (3),
 *      e^x = 2^z
 *          = 2^(N+r)
 *          = (2^N)*(2^r)                        --- (4)
 *
 * 2. Polynomial Evaluation
 *   From (4),
 *     r   = z - N
 *     2^r = C1 + C2*r + C3*r^2 + C4*r^3 + C5 *r^4 + C6*r^5
 *
 * 4. Reconstruction
 *      Thus,
 *        e^x = (2^N) * (2^r)
 *
 *
 */

#include <libm_macros.h>
#include <libm/compiler.h>
#include <libm_util_amd.h>
#include <libm/types.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly-vec.h>

#include <libm/arch/zen5.h>


static const struct {
    v_f32x16_t   tblsz_byln2;
    v_f32x16_t   ln2_tbl_head, ln2_tbl_tail;
    v_f32x16_t   huge;
    v_u32x16_t   arg_max;
    v_u32x16_t   mask;
    v_i32x16_t   expf_bias;
    v_f32x16_t   poly_expf_5[5];
    v_f32x16_t   poly_expf_7[7];
} v_expf_data ={
    .tblsz_byln2  =  _MM512_SET1_PS16(0x1.71547652b82fep+0f),
    .ln2_tbl_head =  _MM512_SET1_PS16(0x1.63p-1f),
    .ln2_tbl_tail =  _MM512_SET1_PS16(-0x1.bd0104p-13f),
    .huge         =  _MM512_SET1_PS16(0x1.8p+23f) ,
    .arg_max      =  _MM512_SET1_U32x16((uint32_t)0x42AE0000),
    .mask         =  _MM512_SET1_U32x16((uint32_t)0x7FFFFFFF),
    .expf_bias    =  _MM512_SET1_I32x16(127),

    // Polynomial coefficients obtained using Remez algorithm
    .poly_expf_5 = {
        _MM512_SET1_PS16(0x1p0f),
        _MM512_SET1_PS16(0x1.fffdc4p-2f),
        _MM512_SET1_PS16(0x1.55543cp-3f),
        _MM512_SET1_PS16(0x1.573aecp-5f),
        _MM512_SET1_PS16(0x1.126bb6p-7f),
    },

    .poly_expf_7 = {
        _MM512_SET1_PS16(0x1p0f),
        _MM512_SET1_PS16(0x1p-1f),
        _MM512_SET1_PS16(0x1.555554p-3f),
        _MM512_SET1_PS16(0x1.555468p-5f),
        _MM512_SET1_PS16(0x1.1112fap-7f),
        _MM512_SET1_PS16(0x1.6da4acp-10f),
        _MM512_SET1_PS16(0x1.9eb724p-13f),
    },

};


#define V16_TBL_LN2         v_expf_data.tblsz_byln2
#define V16_LN2_TBL_H       v_expf_data.ln2_tbl_head
#define V16_LN2_TBL_T       v_expf_data.ln2_tbl_tail
#define V16_EXPF_BIAS       v_expf_data.expf_bias
#define V16_EXP_HUGE        v_expf_data.huge
#define V16_ARG_MAX         v_expf_data.arg_max
#define V16_MASK            v_expf_data.mask

// Coefficients for 5-degree polynomial
#define A0 v_expf_data.poly_expf_5[0]
#define A1 v_expf_data.poly_expf_5[1]
#define A2 v_expf_data.poly_expf_5[2]
#define A3 v_expf_data.poly_expf_5[3]
#define A4 v_expf_data.poly_expf_5[4]

// Coefficients for 7-degree polynomial
#define C0 v_expf_data.poly_expf_7[0]
#define C1 v_expf_data.poly_expf_7[1]
#define C2 v_expf_data.poly_expf_7[2]
#define C3 v_expf_data.poly_expf_7[3]
#define C4 v_expf_data.poly_expf_7[4]
#define C5 v_expf_data.poly_expf_7[5]
#define C6 v_expf_data.poly_expf_7[6]

#define SCALAR_EXPF ALM_PROTO_OPT(expf)


v_f32x16_t
ALM_PROTO_ARCH_ZN5(vrs16_expf)(v_f32x16_t _x)
{
    v_f32x16_t z, dn;
    v_u32x16_t vx, n;
    // vx = int(x)
    vx = as_v16_u32_f32(_x);

    vx = vx & V16_MASK;

    /* Check if -103 < vx < 88 */
    v_u32x16_t cond = (vx > V16_ARG_MAX);

    /* x * (64.0/ln(2)) */
    z   = _x * V16_TBL_LN2;

    dn  = z + V16_EXP_HUGE;

    /* n = int(z) */
    n   = as_v16_u32_f32(dn);

    /* dn = double(n) */
    dn  = dn - V16_EXP_HUGE;

    /* r = x - (dn * (ln(2)/64)) */
    v_f32x16_t r1, r2, r;
    r1  = _x - ( dn * V16_LN2_TBL_H);
    r2  = dn * V16_LN2_TBL_T;
    r   = r1 - r2;

    /* m = (n - j)/64, Calculating 2^m */
    v_u32x16_t m = (n + V16_EXPF_BIAS) << 23;

    /* poly = A1 + A2*r + A3*r^2 + A4*r^3 + A5*r^4 + A6*r^5
     *      = (A1 + A2*r) + r^2(A3 + A4*r) + r^4(A5 + A6*r)
     */
    v_f32x16_t poly = POLY_EVAL_5(r, A0, A0, A1, A2, A3, A4);

    // result = polynomial * 2^m
    v_f32x16_t result = poly * as_v16_f32_u32(m);

    /*
     * If input value is outside valid range, call scalar expf(value)
     * Else, return the above computed result
     */

    if(unlikely(any_v16_u32(cond))) {

        result = call_v16_f32(SCALAR_EXPF, _x, result, cond);

    }

    return result;

}

