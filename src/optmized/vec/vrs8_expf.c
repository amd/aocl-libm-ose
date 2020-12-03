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

#include <stdint.h>
#include <emmintrin.h>

#include <libm_util_amd.h>
#include <libm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>

#include <libm/poly-vec.h>

static const struct {
    v_f32x8_t   tblsz_byln2;
    v_f32x8_t   ln2_tbl_head, ln2_tbl_tail;
    v_f32x8_t   huge;
    v_u32x8_t   arg_max;
    v_i32x8_t   mask;
    v_i32x8_t   expf_bias;
    v_f32x8_t   poly_expf_5[5];
    v_f32x8_t   poly_expf_7[7];
} v_expf_data ={
              .tblsz_byln2 =  _MM256_SET1_PS8(0x1.71547652b82fep+0),
              .ln2_tbl_head = _MM256_SET1_PS8(0x1.63p-1),
              .ln2_tbl_tail = _MM256_SET1_PS8(-0x1.bd0104p-13),
              .huge        =  _MM256_SET1_PS8(0x1.8p+23) ,
              .arg_max     =  _MM256_SET1_I32(0x42AE0000),
              .mask        =  _MM256_SET1_I32(0x7FFFFFFF),
              .expf_bias   =  _MM256_SET1_I32(127),

             // Polynomial coefficients obtained using Remez algorithm

              .poly_expf_5 = {
                              _MM256_SET1_PS8(0x1p0),
                              _MM256_SET1_PS8(0x1.fffdc4p-2),
                              _MM256_SET1_PS8(0x1.55543cp-3),
                              _MM256_SET1_PS8(0x1.573aecp-5),
                              _MM256_SET1_PS8(0x1.126bb6p-7),
              },

              .poly_expf_7 = {
                              _MM256_SET1_PS8(0x1p0),
                              _MM256_SET1_PS8(0x1p-1),
                              _MM256_SET1_PS8(0x1.555554p-3),
                              _MM256_SET1_PS8(0x1.555468p-5),
                              _MM256_SET1_PS8(0x1.1112fap-7),
                              _MM256_SET1_PS8(0x1.6da4acp-10),
                              _MM256_SET1_PS8(0x1.9eb724p-13),
              },

};


#define TBL_LN2   v_expf_data.tblsz_byln2
#define LN2_TBL_H v_expf_data.ln2_tbl_head
#define LN2_TBL_T v_expf_data.ln2_tbl_tail
#define EXPF_BIAS v_expf_data.expf_bias
#define EXP_HUGE      v_expf_data.huge
#define ARG_MAX   v_expf_data.arg_max
#define MASK      v_expf_data.mask

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

/*
    Implementation with 7-degree polynomial

    Performance numbers:
    GCC - 731 MOPS
    AOCC - 833 MOPS

    Max ULP error : 1.7
*/
v_f32x8_t
ALM_PROTO_OPT(vrs8_expf_experimental)(v_f32x8_t _x)
{

    // vx = int(x)
    v_u32x8_t vx = as_v8_u32_f32(_x);

    // Get absolute value of vx
    vx = vx & MASK;

    // Check if -103 < vx < 88
    v_u32x8_t cond = (vx > ARG_MAX);

    // x * (64.0/ln(2))
    v_f32x8_t z = _x * TBL_LN2;

    v_f32x8_t dn = z + EXP_HUGE;

    // n = int(z)
    v_u32x8_t n = as_v8_u32_f32(dn);

    // dn = double(n)
    dn = dn - EXP_HUGE;

    // r = x - (dn * (ln(2)/64))
    // where ln(2)/64 is split into Head and Tail values
    v_f32x8_t r1 = _x - ( dn * LN2_TBL_H);
    v_f32x8_t r2 = dn * LN2_TBL_T;
    v_f32x8_t r = r1 - r2;

    // m = (n - j)/64
    // Calculate 2^m
    v_u32x8_t m = (n + EXPF_BIAS) << 23;

    // Compute polynomial
    /* poly = C1 + C2*r + C3*r^2 + C4*r^3 + C5*r^4 + C6*r^5
            = (C1 + C2*r) + r^2(C3 + C4*r) + r^4(C5 + C6*r)
    */
    v_f32x8_t poly = POLY_EVAL_7(r, C0, C0, C1, C2, C3, C4, C5, C6);

    // result = polynomial * 2^m
    v_f32x8_t result = poly * as_v8_f32_u32(m);

    // If input value is outside valid range, call scalar expf(value)
    // Else, return the above computed result
    if(unlikely(any_v8_u32_loop(cond))) {
    return (v_f32x8_t) {
         cond[0] ? SCALAR_EXPF(_x[0]) : result[0],
         cond[1] ? SCALAR_EXPF(_x[1]) : result[1],
         cond[2] ? SCALAR_EXPF(_x[2]) : result[2],
         cond[3] ? SCALAR_EXPF(_x[3]) : result[3],
         cond[4] ? SCALAR_EXPF(_x[4]) : result[4],
         cond[5] ? SCALAR_EXPF(_x[5]) : result[5],
         cond[6] ? SCALAR_EXPF(_x[6]) : result[6],
         cond[7] ? SCALAR_EXPF(_x[7]) : result[7],

     };
    }

    return result;

}

/*
    Implementation with 5-degree polynomial

    Performance numbers:
    GCC - 773 MOPS
    AOCC - 958 MOPS

    Max ULP - 3.3
*/
v_f32x8_t
ALM_PROTO_OPT(vrs8_expf)(v_f32x8_t _x)
{

    // vx = int(x)
    v_u32x8_t vx = as_v8_u32_f32(_x);

    // Get absolute value of vx
    vx = vx & MASK;

    // Check if -103 < vx < 88
    v_u32x8_t cond = (vx > ARG_MAX);

    // x * (64.0/ln(2))
    v_f32x8_t z = _x * TBL_LN2;

    v_f32x8_t dn = z + EXP_HUGE;

    // n = int(z)
    v_u32x8_t n = as_v8_u32_f32(dn);

    // dn = double(n)
    dn = dn - EXP_HUGE;

    // r = x - (dn * (ln(2)/64))
    // where ln(2)/64 is split into Head and Tail values
    v_f32x8_t r1 = _x - ( dn * LN2_TBL_H);
    v_f32x8_t r2 = dn * LN2_TBL_T;
    v_f32x8_t r = r1 - r2;

    // m = (n - j)/64
    // Calculate 2^m
    v_u32x8_t m = (n + EXPF_BIAS) << 23;

    // Compute polynomial
    /* poly = A1 + A2*r + A3*r^2 + A4*r^3 + A5*r^4 + A6*r^5
            = (A1 + A2*r) + r^2(A3 + A4*r) + r^4(A5 + A6*r)
    */
    v_f32x8_t poly = POLY_EVAL_5(r, A0, A0, A1, A2, A3, A4);

    // result = polynomial * 2^m
    v_f32x8_t result = poly * as_v8_f32_u32(m);

    // If input value is outside valid range, call scalar expf(value)
    // Else, return the above computed result
    if(unlikely(any_v8_u32_loop(cond))) {
    return (v_f32x8_t) {
         cond[0] ? SCALAR_EXPF(_x[0]) : result[0],
         cond[1] ? SCALAR_EXPF(_x[1]) : result[1],
         cond[2] ? SCALAR_EXPF(_x[2]) : result[2],
         cond[3] ? SCALAR_EXPF(_x[3]) : result[3],
         cond[4] ? SCALAR_EXPF(_x[4]) : result[4],
         cond[5] ? SCALAR_EXPF(_x[5]) : result[5],
         cond[6] ? SCALAR_EXPF(_x[6]) : result[6],
         cond[7] ? SCALAR_EXPF(_x[7]) : result[7],

     };
    }

    return result;

}


