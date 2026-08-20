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

#ifndef __VRD2_EXP_KERNEL_H__
#define __VRD2_EXP_KERNEL_H__

/*
 * Branch-free, domain-check-free core of the 2-wide double exp (v2d),
 * table-based range reduction (64-entry double-double table) with a
 * degree-5 Sollya minimax polynomial.  vrd2_exp() composes this with the
 * out-of-range scalar/denormal fixup; other kernels (e.g. vrd2_cosh) that
 * already guard the input range call the fastpath directly.
 */

#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/poly-vec.h>

#define VRD2_EXP_KERNEL_TBL_N     6
#define VRD2_EXP_KERNEL_TBL_SIZE  (1 << VRD2_EXP_KERNEL_TBL_N)
#define VRD2_EXP_KERNEL_TBL_MASK  (VRD2_EXP_KERNEL_TBL_SIZE - 1)

struct vrd2_exp_kernel_tbl_entry {
    double head, tail;
};

/*
 * Small scalar constants only.  Every field is a compile-time constant that
 * the compiler folds into anonymous mergeable constant-pool entries
 * (.rodata.cstN), which the linker de-duplicates across translation units,
 * so keeping them inline costs nothing per extra consumer.  Contrast with the
 * 64-entry table below.
 */
static const struct {
    v_f64x2_t tblsz_byln2;
    v_f64x2_t ln2_by_tblsz_head, ln2_by_tblsz_tail;
    v_f64x2_t Huge;
    v_f64x2_t poly[4];
    } v2_exp_kernel_data = {
        .tblsz_byln2       = _MM_SET1_PD2(0x1.71547652b82fep+6),
        .ln2_by_tblsz_head = _MM_SET1_PD2(-0x1.62e42fefa0000p-7),
        .ln2_by_tblsz_tail = _MM_SET1_PD2(-0x1.cf79abc9e3b39p-46),
        .Huge              = _MM_SET1_PD2(0x1.8000000000000p+52),
        /*
         * Sollya minimax coefficients c2..c5 for degree-5 polynomial:
         *   exp(r) ~= 1 + r + c2*r^2 + c3*r^3 + c4*r^4 + c5*r^5
         * over [-ln(2)/128, +ln(2)/128]. Runtime evaluates via
         * r * POLY_EVAL_4(r, 1, c2, c3, c4, c5) for (exp(r) - 1).
         */
        .poly              = {
            _MM_SET1_PD2(0x1.fffffffffdbd9p-2),   /* c2 (Sollya minimax) */
            _MM_SET1_PD2(0x1.5555555557676p-3),   /* c3 (Sollya minimax) */
            _MM_SET1_PD2(0x1.555573c318398p-5),   /* c4 (Sollya minimax) */
            _MM_SET1_PD2(0x1.111108f3731afp-7),   /* c5 (Sollya minimax) */
        },
    };

/*
 * 64-entry double-double table for 2^(j/64).  Unlike the small scalar
 * constants above, this 1KB table lands in plain (non-mergeable) .rodata and
 * would be emitted once per translation unit that includes this header (and,
 * since the vec sources are compiled per micro-architecture, once per arch
 * variant too).  Give it a single (HIDDEN) definition in
 * src/optimized/data/vrd2_exp_data.c and only declare it here, so every
 * consumer (vrd2_exp, vrd2_cosh, ...) across all arch variants shares one
 * copy.  HIDDEN keeps intra-library references direct (no GOT indirection),
 * matching the folded-immediate fast path.
 */
extern const struct vrd2_exp_kernel_tbl_entry
    v2_exp_kernel_table[VRD2_EXP_KERNEL_TBL_SIZE] HIDDEN;

static inline ALM_ALWAYS_INLINE v_f64x2_t
vrd2_exp_fastpath(v_f64x2_t x)
{
    /* x * (64/ln2) */
    v_f64x2_t z = x * v2_exp_kernel_data.tblsz_byln2;

    v_f64x2_t dn = z + v2_exp_kernel_data.Huge;

    v_i64x2_t n = as_v2_i64_f64(dn);

    dn = dn - v2_exp_kernel_data.Huge;

    /* r = x - dn * (ln2/64), split into head+tail for precision */
    v_f64x2_t r = x + dn * v2_exp_kernel_data.ln2_by_tblsz_head;
    r = r + dn * v2_exp_kernel_data.ln2_by_tblsz_tail;

    /* j = n & 63 (table index) */
    v_i64x2_t tbl_mask = _MM_SET1_I64x2(VRD2_EXP_KERNEL_TBL_MASK);
    v_i64x2_t j = n & tbl_mask;

    /* m = (n - j) << 46: 2^m as IEEE 754 exponent bits (unsigned shift) */
    v_i64x2_t m = (v_i64x2_t)((v_u64x2_t)(n - j) << (52 - VRD2_EXP_KERNEL_TBL_N));

    int64_t j0 = j[0];
    int64_t j1 = j[1];
    v_f64x2_t t_head = (v_f64x2_t){v2_exp_kernel_table[j0].head,
                                   v2_exp_kernel_table[j1].head};
    v_f64x2_t t_tail = (v_f64x2_t){v2_exp_kernel_table[j0].tail,
                                   v2_exp_kernel_table[j1].tail};

    /*
     * Degree-5 Sollya minimax polynomial for e^r - 1:
     *   q = r * (1 + c2*r + c3*r^2 + c4*r^3 + c5*r^4)
     */
    v_f64x2_t poly = r * POLY_EVAL_4(r, _MM_SET1_PD2(1.0),
                                     v2_exp_kernel_data.poly[0],
                                     v2_exp_kernel_data.poly[1],
                                     v2_exp_kernel_data.poly[2],
                                     v2_exp_kernel_data.poly[3]);

    /* Reconstruction: head + fma(head, poly, tail) */
    v_f64x2_t inner = mul_add(t_head, poly, t_tail);
    v_f64x2_t q_result = t_head + inner;

    /* Apply 2^m by adding exponent bits to IEEE 754 representation */
    return as_v2_f64_u64(as_v2_u64_f64(q_result) + (v_u64x2_t)m);
}

#endif /* __VRD2_EXP_KERNEL_H__ */
