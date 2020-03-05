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

#include <libm/poly.h>

static const struct {
    v_f64x4_t   tblsz_byln2;
    v_f64x4_t   huge;
    v_i32x4_t   arg_min;
    v_i32x4_t   arg_max;
    v_i32x4_t   mask;
    v_f64x4_t   poly_expf[6];
} v_expf_data ={
              .tblsz_byln2 =  _MM_SET1_PD4(0x1.71547652b82fep+0),
              .huge        =  _MM_SET1_PD4(0x1.8p+52) ,
              .arg_min     =  _MM_SET1_I32(0xFFFFFF99),
              .arg_max     =  _MM_SET1_I32(0x00000058),
              .mask        =  _MM_SET1_I32(0x7fffffff),
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
#define MASK         v_expf_data.mask
#define OFF          ARG_MAX - ARG_MIN

#define C1 v_expf_data.poly_expf[0]
#define C2 v_expf_data.poly_expf[1]
#define C3 v_expf_data.poly_expf[2]
#define C4 v_expf_data.poly_expf[3]
#define C5 v_expf_data.poly_expf[4]
#define C6 v_expf_data.poly_expf[5]

#define SCALAR_EXPF FN_PROTOTYPE(expf)

static inline int
v_any_u32(v_i32x4_t cond)
{
    int ret = 0;

    for (int i = 0; i < 4; i++)
    {
        if(cond[i] !=0)
            ret= 1;
    }
    return ret;
}

static inline v_i32x4_t
v_to_f32_i32(v_f32x4_t _xf32)
{
    return (v_i32x4_t){_xf32[0], _xf32[1], _xf32[2], _xf32[3]};
}


v_f32x4_t
FN_PROTOTYPE_OPT(vrs4_expf)(v_f32x4_t _x)
{
    // vx = int(_x)
    v_i32x4_t vx = v_to_f32_i32(_x);

    // Get absolute value of vx
    vx = vx & MASK;

    // Check if -103 < vx < 88
    v_i32x4_t cond = ((vx - ARG_MIN) >= OFF);

    // Convert _x to double precision
    v_f64x4_t x = v_to_f32_f64(_x);

    // x * (64.0/ln(2))
    v_f64x4_t z = x * TBL_LN2;

    v_f64x4_t dn = z + EXPF_HUGE;

    // n = int (z)
    v_u64x4_t n = as_v_u64x4(dn);

    // dn = double(n)
    dn = dn - EXPF_HUGE;

    // r = z - dn
    v_f64x4_t r = z - dn;

    /* Compute polynomial
       poly = C1 + C2*r + C3*r^2 + C4*r^3 + C5 *r^4 + C6*r^5
            = (C1 + C2*r) + r^2(C3 + C4*r) + r^4(C4 + C6*r)
    */
    /*
    v_f64x4_t qtmp1 = C1 + C2 * r;
    v_f64x4_t qtmp2 = C3 + C4 * r;
    v_f64x4_t r2 = r * r;
    v_f64x4_t qtmp3 = C5 + C6 * r;
    v_f64x4_t q =  qtmp1 + r2 * qtmp2;
    v_f64x4_t poly = q + r2 * r2  * qtmp3;
    */

    v_f64x4_t poly = POLY_EVAL_6(r, C1, C2, C3, C4, C5, C6);
    // result = (float)[poly + (n << 52)]
    v_f32x4_t  ret = v_to_f64_f32(as_v_f64(as_v_u64x4(poly) + (n << 52)));

    // If input value is outside valid range, call scalar expf(value)
    // Else, return the above computed result
    if(unlikely(v_any_u32(cond))) {
    return (v_f32x4_t) {
         cond[0] ? SCALAR_EXPF(_x[0]) : ret[0],
         cond[1] ? SCALAR_EXPF(_x[1]) : ret[1],
         cond[2] ? SCALAR_EXPF(_x[2]) : ret[2],
         cond[3] ? SCALAR_EXPF(_x[3]) : ret[3],
     };

    }

    return ret;
}
