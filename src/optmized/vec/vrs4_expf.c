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
 * C implementation of exp single precision 128-bit vector version (v4s)
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

#include <libm/poly.h>

static const struct {
    v_f64x4_t   tblsz_byln2;
    v_f64x4_t   huge;
    v_u32x4_t   arg_max;
    v_u32x4_t   mask;
    v_f64x4_t   poly_expf[6];
    v_f32x4_t   expf_max, expf_min;
    v_i32x4_t   infinity;
} v_expf_data = {
    .tblsz_byln2 =  _MM_SET1_PD4(0x1.71547652b82fep+0),
    .huge        =  _MM_SET1_PD4(0x1.8p+52) ,
    .arg_max     =  _MM_SET1_I32(0x42AE0000),
    .mask        =  _MM_SET1_I32(0x7fffffff),
    .infinity    =  _MM_SET1_I32(0x7f800000),
    .expf_min    =  _MM_SET1_PS4(-0x1.9fe368p6f),
    .expf_max    =  _MM_SET1_PS4(88.7228393f),

    /*
     * Polynomial coefficients obtained using Remez algorithm
     */
    .poly_expf = {
        _MM_SET1_PD4(0x1.0000014439a91p0),
        _MM_SET1_PD4(0x1.62e43170e3344p-1),
        _MM_SET1_PD4(0x1.ebf906bc4c115p-3),
        _MM_SET1_PD4(0x1.c6ae2bb88c0c8p-5),
        _MM_SET1_PD4(0x1.3d1079db4ef69p-7),
        _MM_SET1_PD4(0x1.5f8905cb0cc4ep-10),
    },
};

#define TBL_LN2      v_expf_data.tblsz_byln2
#define EXPF_HUGE    v_expf_data.huge
#define ARG_MAX      v_expf_data.arg_max
#define ARG_MIN      v_expf_data.arg_min
#define EXPF_MAX     v_expf_data.expf_max
#define EXPF_MIN     v_expf_data.expf_min
#define MASK         v_expf_data.mask
#define INF          v_expf_data.infinity

#define OFF          ARG_MAX - ARG_MIN

#define C1 v_expf_data.poly_expf[0]
#define C2 v_expf_data.poly_expf[1]
#define C3 v_expf_data.poly_expf[2]
#define C4 v_expf_data.poly_expf[3]
#define C5 v_expf_data.poly_expf[4]
#define C6 v_expf_data.poly_expf[5]

#define SCALAR_EXPF ALM_PROTO_OPT(expf)

v_f32x4_t
ALM_PROTO_OPT(vrs4_expf)(v_f32x4_t _x)
{

    v_u32x4_t vx = as_v4_u32_f32(_x);

    // Get absolute value of vx
    vx = vx & MASK;

    // Check if -103 < vx < 88
    v_u32x4_t cond = ((vx > ARG_MAX));

    // Convert _x to double precision
    v_f64x4_t x = cvt_v4_f32_to_f64(_x);

    // x * (64.0/ln(2))
    v_f64x4_t z = x * TBL_LN2;

    v_f64x4_t dn = z + EXPF_HUGE;

    // n = int (z)
    v_u64x4_t n = as_v4_u64_f64(dn);

    // dn = double(n)
    dn = dn - EXPF_HUGE;

    // r = z - dn
    v_f64x4_t r = z - dn;

    v_f64x4_t poly = POLY_EVAL_6(r, C1, C2, C3, C4, C5, C6);

    // result = (float)[poly + (n << 52)]
    v_u64x4_t q = as_v4_u64_f64(poly) + (n << 52);

    v_f64x4_t result = as_v4_f64_u64(q);

    v_f32x4_t ret = cvt_v4_f64_to_f32(result);

    if(unlikely(any_v4_u32_loop(cond))) {

        v_i32x4_t inf_condition = _x > EXPF_MAX;

        v_i32x4_t zero_condition = _x < EXPF_MIN;

        v_32x4 vy = {.f32x4 = ret};

        //Zero out the elements that have to be set to infinity
        vy.i32x4 = vy.i32x4 & (~inf_condition);

        inf_condition = inf_condition & INF;

        vy.i32x4 = vy.i32x4 | inf_condition;

        //Zero to be set for elements with x < EXPF_MIN
        vy.i32x4 = vy.i32x4 & (~zero_condition);

        return vy.f32x4;
    }


    return ret;

}
