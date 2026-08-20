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

#ifndef __VRD4_EXP_KERNEL_H__
#define __VRD4_EXP_KERNEL_H__

/*
 * Branch-free, domain-check-free core of the 4-wide double exp (v4d),
 * tableless degree-11 minimax reconstruction.  vrd4_exp() composes this
 * with the out-of-range scalar fixup; other kernels (e.g. vrd4_cosh) that
 * already guard the input range call the fastpath directly.
 */

#include <libm/types.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/poly-vec.h>

#define VRD4_EXP_KERNEL_BIAS 1023

static const struct {
    v_f64x4_t tblsz_ln2;
    v_f64x4_t ln2_tblsz_head, ln2_tblsz_tail;
    v_f64x4_t huge;
    v_i64x4_t exp_bias;
    v_i64x4_t mask;
    v_f64x4_t poly[12];
    } v4_exp_kernel_data = {
                .tblsz_ln2      = _MM_SET1_PD4(0x1.71547652b82fep+0),
                .ln2_tblsz_head = _MM_SET1_PD4(0x1.63p-1),
                .ln2_tblsz_tail = _MM_SET1_PD4(-0x1.bd0105c610ca8p-13),
                .huge           = _MM_SET1_PD4(0x1.8000000000000p+52),
                .exp_bias       = _MM_SET1_I64(VRD4_EXP_KERNEL_BIAS),
                .mask           = _MM_SET1_I64(0x7FFFFFFFFFFFFFFF),
                .poly           = {
                                _MM_SET1_PD4(0x1.0p0),
                                _MM_SET1_PD4(0x1.000000000001p-1),
                                _MM_SET1_PD4(0x1.55555555554a2p-3),
                                _MM_SET1_PD4(0x1.555555554f37p-5),
                                _MM_SET1_PD4(0x1.1111111130dd6p-7),
                                _MM_SET1_PD4(0x1.6c16c1878111dp-10),
                                _MM_SET1_PD4(0x1.a01a011057479p-13),
                                _MM_SET1_PD4(0x1.a01992d0fe581p-16),
                                _MM_SET1_PD4(0x1.71df4520705a4p-19),
                                _MM_SET1_PD4(0x1.28b311c80e499p-22),
                                _MM_SET1_PD4(0x1.ad661ce7af3e3p-26),
                },
    };

static inline ALM_ALWAYS_INLINE v_f64x4_t
vrd4_exp_fastpath(v_f64x4_t x)
{
    /* x * (64.0/ln(2)) */
    v_f64x4_t z = x * v4_exp_kernel_data.tblsz_ln2;

    v_f64x4_t dn = z + v4_exp_kernel_data.huge;

    /* n = int (z) */
    v_i64x4_t n = as_v4_i64_f64(dn);

    /* dn = double(n) */
    dn = dn - v4_exp_kernel_data.huge;

    /* r = x - (dn * (ln(2)/64)), ln(2)/64 split into Head and Tail */
    v_f64x4_t r1 = x - (dn * v4_exp_kernel_data.ln2_tblsz_head);
    v_f64x4_t r2 = dn * v4_exp_kernel_data.ln2_tblsz_tail;
    v_f64x4_t r = r1 - r2;

    /* Calculate 2^m */
    v_i64x4_t m = (n + v4_exp_kernel_data.exp_bias) << 52;

    v_f64x4_t poly = POLY_EVAL_11(r,
        v4_exp_kernel_data.poly[0], v4_exp_kernel_data.poly[0],
        v4_exp_kernel_data.poly[1], v4_exp_kernel_data.poly[2],
        v4_exp_kernel_data.poly[3], v4_exp_kernel_data.poly[4],
        v4_exp_kernel_data.poly[5], v4_exp_kernel_data.poly[6],
        v4_exp_kernel_data.poly[7], v4_exp_kernel_data.poly[8],
        v4_exp_kernel_data.poly[9], v4_exp_kernel_data.poly[10]);

    /* result = poly * 2^m */
    return poly * as_v4_f64_i64(m);
}

#endif /* __VRD4_EXP_KERNEL_H__ */
