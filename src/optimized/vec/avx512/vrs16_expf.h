/*
 * Copyright (C) 2019-2026, Advanced Micro Devices. All rights reserved.
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
 * Shared vrs16 expf core: a branch-free, domain-check-free fast path plus a
 * separate special-case mask. The public amd_vrs16_expf (see vrs16_expf.c)
 * composes them; other in-range callers (e.g. vrs16_coshf) reuse the fast path
 * directly to avoid a redundant out-of-range check.
 */

#ifndef __VRS16_EXPF_H__
#define __VRS16_EXPF_H__

#include <libm_macros.h>
#include <libm/compiler.h>
#include <libm_util_amd.h>
#include <libm/types.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly-vec.h>

/*
 * FMA contract: the polynomial here is evaluated with POLY_EVAL_5 from
 * <libm/poly-vec.h>, whose mul_add() expands to a fused multiply-add when
 * AMD_LIBM_FMA_USABLE is defined at include time and to a separate multiply
 * then add otherwise.  The public amd_vrs16_expf is built without
 * AMD_LIBM_FMA_USABLE (non-FMA variant), and its accuracy/bit pattern is
 * defined against that expansion.  Callers that reuse this fast path (e.g.
 * vrs16_coshf) must include this header in the same non-FMA configuration;
 * defining AMD_LIBM_FMA_USABLE before including it would silently change the
 * rounding and produce results that differ bit-for-bit from amd_vrs16_expf.
 */

/*
 * Small scalar constants stay file-scope static const (broadcast immediates the
 * compiler folds into the fastpath); keeping them inline avoids the measured
 * throughput loss from routing them through an external object.  Each is a
 * single splatted value, so the per-TU copies are tiny and mergeable, unlike
 * larger tables that could cause duplication concerns.
 */
static const struct {
    v_f32x16_t   tblsz_byln2;
    v_f32x16_t   ln2_tbl_head, ln2_tbl_tail;
    v_f32x16_t   huge;
    v_u32x16_t   arg_max;
    v_u32x16_t   mask;
    v_i32x16_t   expf_bias;
} v_expf_data = {
    .tblsz_byln2  =  _MM512_SET1_PS16(0x1.71547652b82fep+0f),
    .ln2_tbl_head =  _MM512_SET1_PS16(0x1.63p-1f),
    .ln2_tbl_tail =  _MM512_SET1_PS16(-0x1.bd0104p-13f),
    .huge         =  _MM512_SET1_PS16(0x1.8p+23f) ,
    .arg_max      =  _MM512_SET1_U32x16((uint32_t)0x42AE0000),
    .mask         =  _MM512_SET1_U32x16((uint32_t)0x7FFFFFFF),
    .expf_bias    =  _MM512_SET1_I32x16(127),
};

/*
 * The degree-5 polynomial coefficient table is the large const datum.  It is
 * declared extern here and DEFINED exactly once in vrs16_expf.c so it is not
 * duplicated into every translation unit that includes this header
 * (vrs16_expf.c and vrs16_coshf.c).  The symbol is arch-suffixed via
 * ALM_PROTO_OPT so each per-Zen-generation build keeps its own single
 * definition instead of one shared global that would collide when all arch
 * objects are linked into libalm.
 */
extern const v_f32x16_t ALM_PROTO_OPT(v16_expf_poly)[5];
#define V16_EXPF_POLY ALM_PROTO_OPT(v16_expf_poly)

#define V16_EXPF_TBL_LN2         v_expf_data.tblsz_byln2
#define V16_EXPF_LN2_TBL_H       v_expf_data.ln2_tbl_head
#define V16_EXPF_LN2_TBL_T       v_expf_data.ln2_tbl_tail
#define V16_EXPF_BIAS            v_expf_data.expf_bias
#define V16_EXPF_HUGE            v_expf_data.huge
#define V16_EXPF_ARG_MAX         v_expf_data.arg_max
#define V16_EXPF_MASK            v_expf_data.mask

// Coefficients for 5-degree polynomial
#define V16_EXPF_A0 V16_EXPF_POLY[0]
#define V16_EXPF_A1 V16_EXPF_POLY[1]
#define V16_EXPF_A2 V16_EXPF_POLY[2]
#define V16_EXPF_A3 V16_EXPF_POLY[3]
#define V16_EXPF_A4 V16_EXPF_POLY[4]

/*
 * Lanes that need the scalar expf fallback: |x| outside the valid range
 * (also catches inf/nan, whose abs bit pattern exceeds any finite value).
 */
static inline ALM_ALWAYS_INLINE v_u32x16_t
vrs16_expf_special_mask(v_f32x16_t _x)
{
    v_u32x16_t vx = as_v16_u32_f32(_x) & V16_EXPF_MASK;

    return (vx > V16_EXPF_ARG_MAX);
}

/*
 * Branch-free fast path, no domain checks: e^x = 2^(x/ln2) = 2^N * 2^r.
 * Valid only for in-range |x|; callers must route out-of-range/inf/nan
 * lanes to scalar expf (public expf) or otherwise handle them (coshf).
 */
static inline ALM_ALWAYS_INLINE v_f32x16_t
vrs16_expf_fastpath(v_f32x16_t _x)
{
    v_f32x16_t z, dn;
    v_u32x16_t n;

    /* x * (1/ln(2)) */
    z   = _x * V16_EXPF_TBL_LN2;

    dn  = z + V16_EXPF_HUGE;

    /* n = int(z) */
    n   = as_v16_u32_f32(dn);

    /* dn = double(n) */
    dn  = dn - V16_EXPF_HUGE;

    /* r = x - (dn * (ln(2))) */
    v_f32x16_t r1, r2, r;
    r1  = _x - ( dn * V16_EXPF_LN2_TBL_H);
    r2  = dn * V16_EXPF_LN2_TBL_T;
    r   = r1 - r2;

    /* m = 2^n via biased exponent */
    v_u32x16_t m = (n + V16_EXPF_BIAS) << 23;

    /*
     * POLY_EVAL_5(r, c0..c5) evaluates c0 + c1*r + c2*r^2 + c3*r^3 + c4*r^4 +
     * c5*r^5.  Passing (A0, A0, A1, A2, A3, A4) yields
     * poly = A0 + A0*r + A1*r^2 + A2*r^3 + A3*r^4 + A4*r^5.
     */
    v_f32x16_t poly = POLY_EVAL_5(r, V16_EXPF_A0, V16_EXPF_A0, V16_EXPF_A1,
                                  V16_EXPF_A2, V16_EXPF_A3, V16_EXPF_A4);

    // result = polynomial * 2^n
    return poly * as_v16_f32_u32(m);
}

#endif /* __VRS16_EXPF_H__ */
