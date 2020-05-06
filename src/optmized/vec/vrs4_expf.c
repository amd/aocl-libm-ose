/*
 * Copyright (C) 2019-2020, AMD. All rights reserved.
 *
 * C implementation of exp single precision 128-bit vector version (v4s)
 *
 */

/*
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
#include <libm_amd.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>

#include <libm/poly-vec.h>

static const struct {
    v_f32x4_t   tblsz_byln2;
    v_f32x4_t   ln2_tbl_head, ln2_tbl_tail;
    v_f32x4_t   huge;
    v_i32x4_t   arg_min;
    v_i32x4_t   arg_max;
    v_i32x4_t   mask;
    v_i32x4_t   expf_bias;
    v_f32x4_t   poly_expf_5[5];
    v_f32x4_t   poly_expf_7[7];
} v_expf_data ={

              .tblsz_byln2  = _MM_SET1_PS4(0x1.715478p0f),
              .ln2_tbl_head = _MM_SET1_PS4(0x1.63p-1f),
              .ln2_tbl_tail = _MM_SET1_PS4(-0x1.bd0104p-13f),
              .huge        =  _MM_SET1_PS4(0x1.8p+23f) ,
              .arg_min     =  _MM_SET1_I32(-86),
              .arg_max     =  _MM_SET1_I32(88),
              .mask        =  _MM_SET1_I32(0x7fffffff),
              .expf_bias   =  _MM_SET1_I32(127),
              /*
               * Polynomial coefficients obtained using Remez algorithm
               */
              .poly_expf_5 = {
                              _MM_SET1_PS4(0x1p0f),
                              _MM_SET1_PS4(0x1.fffdc4p-2f),
                              _MM_SET1_PS4(0x1.55543cp-3f),
                              _MM_SET1_PS4(0x1.573aecp-5f),
                              _MM_SET1_PS4(0x1.126bb6p-7f),
              },

              .poly_expf_7 = {
                              _MM_SET1_PS4(0x1p0f),
                              _MM_SET1_PS4(0x1p-1f),
                              _MM_SET1_PS4(0x1.555554p-3f),
                              _MM_SET1_PS4(0x1.555468p-5f),
                              _MM_SET1_PS4(0x1.1112fap-7f),
                              _MM_SET1_PS4(0x1.6da4acp-10f),
                              _MM_SET1_PS4(0x1.9eb724p-13f),
              },
};

#define TBL_LN2      v_expf_data.tblsz_byln2
#define LN2_TBL_H    v_expf_data.ln2_tbl_head
#define LN2_TBL_T    v_expf_data.ln2_tbl_tail
#define EXPF_BIAS    v_expf_data.expf_bias
#define EXPF_HUGE    v_expf_data.huge
#define ARG_MAX      v_expf_data.arg_max
#define ARG_MIN      v_expf_data.arg_min
#define MASK         v_expf_data.mask
#define OFF          ARG_MAX - ARG_MIN

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

#define SCALAR_EXPF FN_PROTOTYPE(expf)

/*
    Implementation with 7-degree polynomial

    Performance numbers:
    GCC - 550 MOPS
    AOCC -475 MOPS

    Max ULP - 1.7

*/

v_f32x4_t
FN_PROTOTYPE_OPT(vrs4_expf_experimental)(v_f32x4_t _x)
{
    // vx = int(_x)
    v_i32x4_t vx = v4_to_f32_i32(_x);

    // Get absolute value of vx
    vx = vx & MASK;

    // Check if -103 < vx < 88
    v_u32x4_t cond = ((vx - ARG_MIN) >= OFF);

    // x * (64.0/ln(2))
    v_f32x4_t z = _x * TBL_LN2;

    v_f32x4_t dn = z + EXPF_HUGE;

    // n = int (z)
    v_u32x4_t n = as_v_u32x4(dn);

    // dn = double(n)
    dn = dn - EXPF_HUGE;

    // r = x - (dn * (ln(2)/64))
    // where ln(2)/64 is split into Head and Tail values
    v_f32x4_t r1 = _x - ( dn * LN2_TBL_H);
    v_f32x4_t r2 = dn * LN2_TBL_T;
    v_f32x4_t r = r1 - r2;

    // m = (n - j)/64
    // Calculate 2^m
    v_i32x4_t m = (n + EXPF_BIAS) << 23;

    /* Compute polynomial
       poly = C1 + C2*r + C3*r^2 + C4*r^3 + C5 *r^4 + C6*r^5
            = (C1 + C2*r) + r^2(C3 + C4*r) + r^4(C5 + C6*r)
    */
    v_f32x4_t poly = POLY_EVAL_7(r, C0, C0, C1, C2, C3, C4, C5, C6);

    // result = polynomial * 2^m
    v_f32x4_t result = poly * as_f32x4(m);

    // If input value is outside valid range, call scalar expf(value)
    // Else, return the above computed result
    if(unlikely(v4_any_u32_loop(cond))) {
    return (v_f32x4_t) {
         cond[0] ? SCALAR_EXPF(_x[0]) : result[0],
         cond[1] ? SCALAR_EXPF(_x[1]) : result[1],
         cond[2] ? SCALAR_EXPF(_x[2]) : result[2],
         cond[3] ? SCALAR_EXPF(_x[3]) : result[3],
     };

    }

    return result;
}

/*
    Implementation with 5-degree polynomial

    Performance numbers:
    GCC - 550 MOPS
    AOCC - 492 MOPS

    Max ULP - 3.3
*/
v_f32x4_t
FN_PROTOTYPE_OPT(vrs4_expf)(v_f32x4_t _x)
{
    // vx = int(x)
    v_i32x4_t vx = v4_to_f32_i32(_x);

    // Get absolute value of vx
    vx = vx & MASK;

    // Check if -103 < vx < 88
    v_u32x4_t cond = ((vx - ARG_MIN) >= OFF);

    // x * (64.0/ln(2))
    v_f32x4_t z = _x * TBL_LN2;

    v_f32x4_t dn = z + EXPF_HUGE;

    // n = int(z)
    v_u32x4_t n = as_v_u32x4(dn);

    // dn = double(n)
    dn = dn - EXPF_HUGE;

    // r = x - (dn * (ln(2)/64))
    // where ln(2)/64 is split into Head and Tail values
    v_f32x4_t r1 = _x - ( dn * LN2_TBL_H);
    v_f32x4_t r2 = dn * LN2_TBL_T;
    v_f32x4_t r = r1 - r2;

    // m = (n - j)/64
    // Calculate 2^m
    v_i32x4_t m = (n + EXPF_BIAS) << 23;

    // Compute polynomial
    /* poly = A1 + A2*r + A3*r^2 + A4*r^3 + A5*r^4 + A6*r^5
            = (A1 + A2*r) + r^2(A3 + A4*r) + r^4(A5 + A6*r)
    */
    v_f32x4_t poly = POLY_EVAL_5(r, A0, A0, A1, A2, A3, A4);

    // result = polynomial * 2^m
    v_f32x4_t result = poly * as_f32x4(m);

    // If input value is outside valid range, call scalar expf(value)
    // Else, return the above computed result
    if(unlikely(v4_any_u32_loop(cond))) {
    return (v_f32x4_t) {
         cond[0] ? SCALAR_EXPF(_x[0]) : result[0],
         cond[1] ? SCALAR_EXPF(_x[1]) : result[1],
         cond[2] ? SCALAR_EXPF(_x[2]) : result[2],
         cond[3] ? SCALAR_EXPF(_x[3]) : result[3],
     };
    }
    return result;
}

