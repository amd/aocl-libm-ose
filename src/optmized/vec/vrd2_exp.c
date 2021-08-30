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
  * C implementation of exp double precision 128-bit vector version (v2d)
  *
  * Implementation Notes
  * ----------------------
  * 1. Argument Reduction:
  *      e^x = 2^(x/ln2) = 2^(x*(64/ln(2))/64)     --- (1)
  *
  *      Choose 'n' and 'f', such that
  *      x * 64/ln2 = n + f                        --- (2) | n is integer
  *                            | |f| <= 0.5
  *     Choose 'm' and 'j' such that,
  *      n = (64 * m) + j                          --- (3)
  *
  *     From (1), (2) and (3),
  *      e^x = 2^((64*m + j + f)/64)
  *          = (2^m) * (2^(j/64)) * 2^(f/64)
  *          = (2^m) * (2^(j/64)) * e^(f*(ln(2)/64))
  *
  * 2. Table Lookup
  *      Values of (2^(j/64)) are precomputed, j = 0, 1, 2, 3 ... 63
  *
  * 3. Polynomial Evaluation
  *   From (2),
  *     f = x*(64/ln(2)) - n
  *   Let,
  *     r  = f*(ln(2)/64) = x - n*(ln(2)/64)
  *
  * 4. Reconstruction
  *      Thus,
  *        e^x = (2^m) * (2^(j/64)) * e^r
  *
  */


#include <stdint.h>
#include <emmintrin.h>

#include <libm_util_amd.h>
#include <libm_special.h>
#include <libm_macros.h>
//#include <libm_amd.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>

#include <libm/poly-vec.h>

#define DOUBLE_PRECISION_BIAS 1023


static const struct {
    v_f64x2_t tblsz_ln2;
    v_f64x2_t ln2_tblsz_head, ln2_tblsz_tail;
    v_f64x2_t Huge;
    v_i64x2_t exp_bias;
    v_i64x2_t exp_max;
    v_f64x2_t exp_maxd;
    v_f64x2_t exp_mind;
    v_i64x2_t mask;
    v_i64x2_t infinity;
    v_f64x2_t poly[12];
    double exp_min_value;
    }exp_data = {
                    .tblsz_ln2      = _MM_SET1_PD2(0x1.71547652b82fep+0),
                    .ln2_tblsz_head = _MM_SET1_PD2(0x1.63p-1),
                    .ln2_tblsz_tail = _MM_SET1_PD2(-0x1.bd0105c610ca8p-13),
                    .Huge           = _MM_SET1_PD2(0x1.8000000000000p+52),
                    .exp_bias       = _MM_SET1_I64x2(DOUBLE_PRECISION_BIAS),
                    .exp_maxd       = _MM_SET1_PD2(0x1.62e42fefa39efp+9),
                    .exp_mind       = _MM_SET1_PD2(-0x1.62e42fefa39efp+9),
                    .exp_max        = _MM_SET1_I64x2(0x4086200000000000),
                    .mask           = _MM_SET1_I64x2(0x7FFFFFFFFFFFFFFF),
                    .infinity       = _MM_SET1_I64x2(0x7ff0000000000000),
                    .exp_min_value  = -0x1.74910d52d3051p+9,
                    .poly           = {
                                        _MM_SET1_PD2(0x1p0),
                                        _MM_SET1_PD2(0x1.000000000001p-1),
                                        _MM_SET1_PD2(0x1.55555555554a2p-3),
                                        _MM_SET1_PD2(0x1.555555554f37p-5),
                                        _MM_SET1_PD2(0x1.1111111130dd6p-7),
                                        _MM_SET1_PD2(0x1.6c16c1878111dp-10),
                                        _MM_SET1_PD2(0x1.a01a011057479p-13),
                                        _MM_SET1_PD2(0x1.a01992d0fe581p-16),
                                        _MM_SET1_PD2(0x1.71df4520705a4p-19),
                                        _MM_SET1_PD2(0x1.28b311c80e499p-22),
                                        _MM_SET1_PD2(0x1.ad661ce7af3e3p-26),
                                       },
    };

#define DP64_BIAS        exp_data.exp_bias
#define LN2_HEAD         exp_data.ln2_tblsz_head
#define LN2_TAIL         exp_data.ln2_tblsz_tail
#define INVLN2           exp_data.tblsz_ln2
#define EXP_HUGE         exp_data.Huge
#define ARG_MAX          exp_data.exp_max
#define MASK             exp_data.mask
#define EXP_MAX          exp_data.exp_maxd
#define EXP_LOW          exp_data.exp_mind
#define INF              exp_data.infinity
#define EXP_MIN_VAL      exp_data.exp_min_value

#define C1  exp_data.poly[0]
#define C3  exp_data.poly[1]
#define C4  exp_data.poly[2]
#define C5  exp_data.poly[3]
#define C6  exp_data.poly[4]
#define C7  exp_data.poly[5]
#define C8  exp_data.poly[6]
#define C9  exp_data.poly[7]
#define C10 exp_data.poly[8]
#define C11 exp_data.poly[9]
#define C12 exp_data.poly[10]


#define SCALAR_EXP ALM_PROTO(exp)

v_f64x2_t
ALM_PROTO_OPT(vrd2_exp)(v_f64x2_t x)
{

    v_i64x2_t vx = as_v2_u64_f64(x);

    // Get absolute value
    vx = vx & MASK;

    // Check if -709 < vx < 709
    v_i64x2_t cond = (vx > ARG_MAX);

    // x * (64.0/ln(2))
    v_f64x2_t z = x * INVLN2;

    v_f64x2_t dn = z + EXP_HUGE;

    // n = int (z)
    v_i64x2_t n = as_v2_u64_f64(dn);

    // dn = double(n)
    dn = dn - EXP_HUGE;

    // r = x - (dn * (ln(2)/64))
    // where ln(2)/64 is split into Head and Tail values
    v_f64x2_t r1 = x - ( dn * LN2_HEAD);
    v_f64x2_t r2 = dn * LN2_TAIL;
    v_f64x2_t r = r1 - r2;

    // m = (n - j)/64
    // Calculate 2^m
    v_i64x2_t m = (n + DP64_BIAS) << 52;

    // Compute polynomial
    /* poly = C1 + C2*r + C3*r^2 + C4*r^3 + C5*r^4 + C6*r^5 +
              C7*r^6 + C8*r^7 + C9*r^8 + C10*r^9 + C11*r^10 + C12*r^11
            = (C1 + C2*r) + r^2(C3 + C4*r) + r^4(C5 + C6*r) +
              r^6(C7 + C8*r) + r^8(C9 + C10*r) + r^10(C11 + C12*r)
    */
    v_f64x2_t poly = POLY_EVAL_11(r, C1, C1, C3, C4, C5, C6,
                                  C7, C8, C9, C10, C11, C12);

    // result = polynomial * 2^m
    v_f64x2_t ret = poly * as_v2_f64_u64(m);

    if(unlikely(any_v2_u64_loop(cond))) {

        return (v_f64x2_t) {
            (cond[0]) ? SCALAR_EXP(x[0]):ret[0],
            (cond[1]) ? SCALAR_EXP(x[1]):ret[1],
        };

    }

    return ret;
}
