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
#include <libm/compiler.h>
#include <libm/amd_funcs_internal.h>

#define AMD_LIBM_FMA_USABLE 1
#include <libm/poly.h>

extern double  __two_to_jby64_table[];

static const struct {

    v_f64x4_t   ln2by_tblsz;
    v_f64x4_t   tblsz_byln2;
    v_f64x4_t   one_by_two;
    v_f64x4_t   one_by_six;
    v_f64x4_t   huge;
    v_i32x4_t   arg_min;
    v_i32x4_t   arg_max;
    v_i32x4_t   expf_tbl_sz;

}v_expf_data = {
    .ln2by_tblsz =  _MM_SET1_PD4(0x1.62e42fefa39efp-7),
    .tblsz_byln2 =  _MM_SET1_PD4(0x1.71547652b82fep6),
    .one_by_two  =  _MM_SET1_PD4(0x1p-1),
    .one_by_six  =  _MM_SET1_PD4(0x1.5555555555555p-3),
    .arg_min     =  _MM_SET1_I32(-0x1.9fe368p6f) ,
    .arg_max     =  _MM_SET1_I32(0x1.62e42ep6f) ,
    .huge        =  _MM_SET1_PD4(0x1.8p+52) ,
    .expf_tbl_sz =  _MM_SET1_I32(0x3f),
};

#define LN2_TBL       v_expf_data.ln2by_tblsz
#define TBL_LN2       v_expf_data.tblsz_byln2
#define EXPF_HUGE     v_expf_data.huge
#define C0            v_expf_data.one_by_two
#define C1            v_expf_data.one_by_six
#define ARG_MAX       v_expf_data.arg_max
#define ARG_MIN       v_expf_data.arg_min
#define V_EXPF_TBL_SZ v_expf_data.expf_tbl_sz

#define EXPF_TBL (double const*)__two_to_jby64_table
#define EXPF_N  6

static inline v_i32x4_t
v_to_i32_f32(v_f32x4_t _xf32)
{
    return (v_i32x4_t){_xf32[0], _xf32[1], _xf32[2], _xf32[3]};
}

static inline v_f64x4_t
v_to_f32_f64(v_f32x4_t _xf32)
{
    return _mm256_cvtps_pd(_xf32);
}

static inline v_f32x4_t
v_to_f64_f32(v_f64x4_t _xf64)
{
    return _mm256_cvtpd_ps(_xf64);
}

static inline v_i32x4_t
v_to_f64_i32(v_f64x4_t _xf64)
{
    return (v_i32x4_t)_mm256_cvtpd_epi32(_xf64);
}

static inline v_i64x4_t
v_to_i32_i64(v_i32x4_t _xi32)
{
    return (v_i64x4_t)_mm256_cvtepi32_epi64((__m128i)_xi32);
}

static inline v_f64x4_t
v_gather(v_i64x4_t index, int scale)
{
    return _mm256_i64gather_pd(EXPF_TBL, (__m256i)index, scale);
}

static inline v_i32x4_t top12f(v_f32x4_t _xf32)
{
    v_i32x4_t f = v_to_i32_f32(_xf32);
    return f >> 20;
}

v_f32x4_t
FN_PROTOTYPE_OPT(vrs4_expf)(v_f32x4_t _x)
{

    // Convert _x to double precision
    v_f64x4_t x = v_to_f32_f64(_x);

    // x * (64.0/ln(2))
    v_f64x4_t fpart = x * TBL_LN2;

    // int( x * (64)/ln(2))
    v_i32x4_t ipart = v_to_f64_i32(fpart);

    // double(ipart)
    v_f64x4_t dpart = fpart + EXPF_HUGE;
    dpart -= EXPF_HUGE;

    // r = x - dpart * ln(2)/64
    v_f64x4_t r = x - (dpart * LN2_TBL);

    // q = r + r*r[1/2 + (r * 1/6)]
    v_f64x4_t q = POLY_EVAL_1(r, C0, C1);

    // j = n & 0x3f
    v_i32x4_t j = ipart & V_EXPF_TBL_SZ;

    // Get values from look-up table
    v_i64x4_t vindex = v_to_i32_i64(j);
    v_f64x4_t f = v_gather(vindex, 8);

    f = f * q + f;

    // m = (n - j)/64
    v_i64x4_t m = v_to_i32_i64((ipart >> EXPF_N));

    // 2^m
    m = m << 52;

    // result = 2 ^m * (f + (f*q))
    m += as_v_i64x4(f);
    v_f32x4_t result = v_to_f64_f32(as_v_f64x4(m));

    return result;

}
