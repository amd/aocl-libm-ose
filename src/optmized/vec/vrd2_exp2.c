/*
 * Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly-vec.h>


static const struct {
    v_i64x2_t   mask;
    v_i64x2_t   arg_max;

#ifdef EXPERIMENTAL
    v_f64x2_t   sixtyfour_byln2;
    v_f64x2_t   ln2by_64_head, ln2by_64_tail;
#else
    v_f64x2_t   sixtyfour;
    v_f64x2_t   oneby_64;
#endif
    v_i64x2_t   bias;
    v_f64x2_t   huge;
    v_f64x2_t   ln2;
    v_f64x2_t   poly[12];
    } exp2_data = {
    .mask           = _MM_SET1_I64x2(0x7FFFFFFFFFFFFFFF),
    .arg_max        = _MM_SET1_I64x2(0x4086000000000000),
    .sixtyfour      = _MM_SET1_PD2(0x1.0p+6),
#ifdef EXPERIMENTAL
    .ln2by_64_head  = _MM_SET1_PD2(0x1.63p-1),
    .ln2by_64_tail  = _MM_SET1_PD2(-0x1.bd0105c610ca8p-13),
#else
    .oneby_64       = _MM_SET1_PD2(0x1.0p-6),
#endif
    .huge           = _MM_SET1_PD2(0x1.8p+52),
    .ln2            = _MM_SET1_PD2(0x1.62e42fefa39efp-1),
    .bias           = _MM_SET1_I64x2(EMAX_DP64), /* 1023 */
    .poly           = {
        _MM_SET1_PD2(0x1.0p0),
        _MM_SET1_PD2(0x1.0p-1),
        _MM_SET1_PD2(0x1.5555555555555p-3),
        _MM_SET1_PD2(0x1.5555555555555p-5),
        _MM_SET1_PD2(0x1.1111111111111p-7),
        _MM_SET1_PD2(0x1.6c16c16c16c17p-10),
        _MM_SET1_PD2(0x1.a01a01a01a019p-13),
        _MM_SET1_PD2(0x1.a01a019ffbbebp-16),
        _MM_SET1_PD2(0x1.71de3a55b4df2p-19),
        _MM_SET1_PD2(0x1.27e567106f599p-22),
        _MM_SET1_PD2(0x1.ae63d211b9bc7p-26),
    },
};

#define ALM_V2_EXP2_64               exp2_data.sixtyfour
#define ALM_V2_EXP2_BIAS             exp2_data.bias
#define ALM_V2_EXP2_ONEBY_64         exp2_data.oneby_64
#define ALM_V2_EXP2_LN2BY_64_HEAD    exp2_data.ln2by_64_head
#define ALM_V2_EXP2_LN2BY_64_TAIL    exp2_data.ln2by_64_tail
#define ALM_V2_EXP2_HUGE             exp2_data.huge
#define ALM_V2_EXP2_LN2              exp2_data.ln2
#define ALM_V2_EXP2_ARG_MAX          exp2_data.arg_max
#define ALM_V2_EXP2_MASK             exp2_data.mask

#define C1  exp2_data.poly[0]
#define C2  C1
#define C3  exp2_data.poly[1]
#define C4  exp2_data.poly[2]
#define C5  exp2_data.poly[3]
#define C6  exp2_data.poly[4]
#define C7  exp2_data.poly[5]
#define C8  exp2_data.poly[6]
#define C9  exp2_data.poly[7]
#define C10 exp2_data.poly[8]
#define C11 exp2_data.poly[9]
#define C12 exp2_data.poly[10]

#define SCALAR_EXP2  ALM_PROTO(exp2)


/*
 * Implementation Notes
 * ----------------------
 * 0. Choosing N = 64
 *
 * 1. Argument Reduction:
 *      2^(x)   =   2^(x*N/N)                --- (1)
 *
 *      Choose 'n' and 'f', such that
 *      x * N = n + f                        --- (2)
 *                | n is integer and |f| <= 0.5
 *
 *     Choose 'm' such that,
 *      n = (N * m)                          --- (3)
 *
 *     From (1), (2) and (3),
 *      2^x = 2^((N*m + f)/N)
 *          = (2^m) * 2^(f/N)
 *
 * 2. Polynomial Evaluation
 *   From (2),
 *     f  = x*N - n
 *   Let,
 *     r  = f/N = (x*N - n)/N == x - n/N
 *
 *   Taylor series only exist for e^x
 *        2^x = e^(x*ln2)
 *     => 2^r = e^(r*ln2)
 *
 * 3. Reconstruction
 *      Thus,
 *        2^x = 2^m * 2^r
 *
 */

v_f64x2_t
ALM_PROTO_OPT(vrd2_exp2)(v_f64x2_t input)
{
    v_f64x2_t   r, z, dn;
    v_i64x2_t   vx, n, m, cond;

    /* Get absolute value */
    vx   = as_v2_i64_f64(input);
    vx   = vx & ALM_V2_EXP2_MASK;

    /* Check if -709 < vx < 709 */
    cond = ((vx) > ALM_V2_EXP2_ARG_MAX);

    /* x * 64.0 */
    z    = input * ALM_V2_EXP2_64;

    dn   = z + ALM_V2_EXP2_HUGE;

    /* n = int (z) */
    n    = as_v2_i64_f64(dn) & 0xfff;

    /* dn = double(n) */
    dn   = dn - ALM_V2_EXP2_HUGE;

#ifdef EXPERIMENTAL
    /*
     * r = x - (dn * 1/64))
     *     where 1/64 is split into Head and Tail values
     */
    v_f64x2_t r1, r2;
    r1  = input - (dn * ALM_V2_EXP2_LN2BY_64_HEAD);
    r2  = dn * ALM_V2_EXP2_LN2BY_64_TAIL;
    r   = r1 + r2;
#else
    r = input - (dn * ALM_V2_EXP2_ONEBY_64);
    r *= ALM_V2_EXP2_LN2;
#endif

    /* m = n/64, and Calculate m = 2^m  */
    m = n << (52 - 6);

    /* poly = C1 + C2*r + C3*r^2 + C4*r^3 + C5*r^4 + C6*r^5 +
     *          C7*r^6 + C8*r^7 + C9*r^8 + C10*r^9 + C11*r^10 + C12*r^11
     *      = (C1 + C2*r) + r^2(C3 + C4*r) + r^4(C5 + C6*r) +
     *           r^6(C7 + C8*r) + r^8(C9 + C10*r) + r^10(C11 + C12*r)
     */
    v_f64x2_t poly = POLY_EVAL_11(r,
                                  C1, C1, C3, C4,  C5,  C6,
                                  C7, C8, C9, C10, C11, C12);

    /* result = poly * 2^m */
    v_f64x2_t ret = as_v2_f64_i64(as_v2_i64_f64(poly) + m);
    //v_f64x2_t ret = poly * as_v2_f64_i64(m);

    if (unlikely(any_v2_u64_loop(cond))) {
        ret = (v_f64x2_t) {
            (cond[0]) ? SCALAR_EXP2(input[0]) : ret[0],
            (cond[1]) ? SCALAR_EXP2(input[1]) : ret[1],
        };
    }

    return ret;
}
