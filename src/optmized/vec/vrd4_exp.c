/*
 * Copyright (C) 2019-2020, AMD. All rights reserved.
 *
 * C implementation of exp double precision 256-bit vector version (v4d)
 *
 */

 /*
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
#include <libm_amd.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>

#include <libm/poly-vec.h>

typedef struct {
    double head,tail;
} lookup_data;

#define N 10
#define TABLE_SIZE (1ULL << N)
#define DOUBLE_PRECISION_BIAS 1023
extern lookup_data exp_lookup[];

static const struct {
    v_f64x4_t tblsz_ln2;
    v_f64x4_t ln2_tblsz_head, ln2_tblsz_tail;
    v_f64x4_t Huge;
    v_i64x4_t one;
    v_i64x4_t exp_bias;
    v_i64x4_t exp_min;
    v_i64x4_t exp_max;
    v_i64x4_t mask;
    v_f64x4_t poly[5];
    }exp_data = {
                    .tblsz_ln2      = _MM_SET1_PD4(0x1.71547652b82fep10),
                    .ln2_tblsz_head = _MM_SET1_PD4(0x1.62e42f0000000p-11),
                    .ln2_tblsz_tail = _MM_SET1_PD4(0x1.DF473DE6AF279p-36),
                    .Huge           = _MM_SET1_PD4(0x1.8000000000000p+52),
                    .one            = _MM_SET1_I64(0x3ff0000000000000ULL),
                    .exp_bias       = _MM_SET1_I64(DOUBLE_PRECISION_BIAS),
                    .exp_min        = _MM_SET1_I64(-708),
                    .exp_max        = _MM_SET1_I64(710),
                    .mask           = _MM_SET1_I64(0x7FFFFFFFFFFFFFFF),
                    .poly           = {
                                        _MM_SET1_PD4(0x1.0000000000000p-1),
                                        _MM_SET1_PD4(0x1.5555555555555p-3),
                                        _MM_SET1_PD4(0x1.5555555555555p-5),
                                        _MM_SET1_PD4(0x1.1111111111111p-7),
                                        _MM_SET1_PD4(0x1.6c16c16c16c17p-10),
                                       },
    };

#define TWO_POWER_J_BY_N exp_lookup
#define DP64_BIAS        exp_data.exp_bias
#define LN2_HEAD         exp_data.ln2_tblsz_head
#define LN2_TAIL         exp_data.ln2_tblsz_tail
#define INVLN2           exp_data.tblsz_ln2
#define EXP_HUGE         exp_data.Huge
#define ONE              exp_data.one
#define MASK             exp_data.mask
#define ARG_MIN          exp_data.exp_min
#define ARG_MAX          exp_data.exp_max
#define OFF              ARG_MAX - ARG_MIN

#define C1 exp_data.poly[0]
#define C2 exp_data.poly[1]
#define C3 exp_data.poly[2]
#define C4 exp_data.poly[3]
#define C5 exp_data.poly[4]

#define SCALAR_EXP FN_PROTOTYPE(exp)

v_f64x4_t
FN_PROTOTYPE_OPT(vrd4_exp)(v_f64x4_t x)
{
    // vx = int(x)
    v_i64x4_t vx = v4_to_f64_i64(x);

    // Get absolute value of vx
    vx = vx & MASK;

    // Check if -709 < vx < 709
    v_i64x4_t cond = ((vx - ARG_MIN) >= OFF);

    // x * (64.0/ln(2))
    v_f64x4_t z = x * INVLN2;

    v_f64x4_t dn = z + EXP_HUGE;

    // n = int (z)
    v_i64x4_t n = as_u64x4(dn);

    // dn = double(n)
    dn = dn - EXP_HUGE;

    // j = n & 0x3f
    v_u64x4_t index = n & DP64_BIAS;

    // r = x - (dn * (ln(2)/64))
    // where ln(2)/64 is split into Head and Tail values
    v_f64x4_t r1 = x - ( dn * LN2_HEAD);
    v_f64x4_t r2 = dn * LN2_TAIL;
    v_f64x4_t r = r1 - r2;

    // m = (n - j)/64
    // Calculate 2^m
    v_i64x4_t m = ((n - index) << (52-N)) + ONE;

    // Compute polynomial
    // poly = C1 + C2*r + C3*r^2 + C4*r^3
    //      = (C1 + C2*r) + r^2(C3 + C4*r)
    r2 = r * r;
    v_f64x4_t poly = POLY_EVAL_1(r, C1, C2) + r2 * r2 * C3;

    v_f64x4_t j_by_N_head, j_by_N_tail;

    // Get head and tail values from look-up table
    for (int i = 0; i < 4; i++)
    {
        int32_t j = index[i];
        j_by_N_head[i] = TWO_POWER_J_BY_N[j].head;
        j_by_N_tail[i] = TWO_POWER_J_BY_N[j].tail;
    }

    // result = 2^m * (f + (f*q))
    v_f64x4_t q = j_by_N_tail + poly * j_by_N_tail;
    z = poly * j_by_N_head;
    r = j_by_N_head + (z + q);
    v_f64x4_t ret = r * as_f64x4(m);

    // If input value is outside valid range, call scalar exp(value)
    // Else, return the above computed result
    if(unlikely(v4_any_u64_loop(cond))) {
    return (v_f64x4_t) {
        cond[0] ? SCALAR_EXP(x[0]) : ret[0],
        cond[1] ? SCALAR_EXP(x[1]) : ret[1],
        cond[2] ? SCALAR_EXP(x[2]) : ret[2],
        cond[3] ? SCALAR_EXP(x[3]) : ret[3],
        };
    }

    return ret;
}
