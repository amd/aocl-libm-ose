/* Correctly-rounded hyperbolic tangent function for binary32 value.

Copyright (c) 2022, Alexei Sibidanov.
Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.

This file was originally developed as part of the CORE-MATH project
(https://core-math.gitlabpages.inria.fr/).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
 * Signature:
 *   v_f32x8_t vrs8_tanhf(v_f32x8_t x)
 *
 ******************************************
 * Implementation Notes
 * ----------------------
 * To compute vrs8_tanhf(v_f32x8_t x)
 * 
 * If 0x39000000 <= |x| < 0x9.02cb2ffffd19d464063fp0f (approx 2.98e-8 <= |x| < 9.01):
 *    In this case, tanhf(x) is approximated as the ratio of two polynomials of degree 8.
 *
 * For other cases, call scalar tanhf()
 *
 * Max ULP of current implementation: 0.5
 *
 */

#include <stdint.h>
#include <emmintrin.h>

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>

#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>

static struct
{
    v_u32x8_t arg_max, sign_mask, small_arg;
    v_f64x4_t cn[8], cd[8];
} v8_tanhf_data = {
    .arg_max = _MM256_SET1_I32(0x41102cb3u),
    .sign_mask = _MM256_SET1_I32(0x7FFFFFFF),
    .small_arg = _MM256_SET1_I32(0x39000000),
    .cn = {
        _MM_SET1_PD4(0x1p+0),
        _MM_SET1_PD4(0x1.30877b8b72d33p-3),
        _MM_SET1_PD4(0x1.694aa09ae9e5ep-8),
        _MM_SET1_PD4(0x1.4101377abb729p-14),
        _MM_SET1_PD4(0x1.e0392b1db0018p-22),
        _MM_SET1_PD4(0x1.2533756e546f7p-30),
        _MM_SET1_PD4(0x1.d62e5abe6ae8ap-41),
        _MM_SET1_PD4(0x1.b06be534182dep-54)
        },
    .cd = {
        _MM_SET1_PD4(0x1p+0),
        _MM_SET1_PD4(0x1.ed99131b0ebeap-2),
        _MM_SET1_PD4(0x1.0d27ed6c95a69p-5),
        _MM_SET1_PD4(0x1.7cbdaca0e9fccp-11),
        _MM_SET1_PD4(0x1.b4e60b892578ep-18),
        _MM_SET1_PD4(0x1.a6f707c5c71abp-26),
        _MM_SET1_PD4(0x1.35a8b6e2cd94cp-35),
        _MM_SET1_PD4(0x1.ca8230677aa01p-47)
        },
};

#define V8_TANHF_ARG_MAX v8_tanhf_data.arg_max
#define V8_TANHF_SIGN_MASK v8_tanhf_data.sign_mask
#define V8_TANHF_SMALL_ARG v8_tanhf_data.small_arg
#define V8_TANHF_ONE v8_tanhf_data.one
#define V8_TANHF_TWO v8_tanhf_data.two

#define CN0 v8_tanhf_data.cn[0]
#define CN1 v8_tanhf_data.cn[1]
#define CN2 v8_tanhf_data.cn[2]
#define CN3 v8_tanhf_data.cn[3]
#define CN4 v8_tanhf_data.cn[4]
#define CN5 v8_tanhf_data.cn[5]
#define CN6 v8_tanhf_data.cn[6]
#define CN7 v8_tanhf_data.cn[7]

#define CD0 v8_tanhf_data.cd[0]
#define CD1 v8_tanhf_data.cd[1]
#define CD2 v8_tanhf_data.cd[2]
#define CD3 v8_tanhf_data.cd[3]
#define CD4 v8_tanhf_data.cd[4]
#define CD5 v8_tanhf_data.cd[5]
#define CD6 v8_tanhf_data.cd[6]
#define CD7 v8_tanhf_data.cd[7]

static inline v_f32x8_t
tanhf_specialcase(v_f32x8_t _x, v_f32x8_t result, v_u32x8_t cond, v_u32x8_t sign)
{
    return call_v8_f32(ALM_PROTO(tanhf), _x, result, cond);
}

v_f32x8_t
ALM_PROTO_OPT(vrs8_tanhf)(v_f32x8_t x)
{

    /* Get sign of input argument */
    v_u32x8_t ux = as_v8_u32_f32(x);
    v_u32x8_t sign = ux & (~V8_TANHF_SIGN_MASK);

    /* Get absolute value of input argument */
    ux = ux & V8_TANHF_SIGN_MASK;

    /* Check for special cases */
    v_u32x8_t cond = ux >= V8_TANHF_ARG_MAX;
    cond |= ux <= V8_TANHF_SMALL_ARG;

    /* Approximate tanhf using ratio of two polynomials of degree 8 */
    v_f32x4_t x1 = _mm256_extractf128_ps(x, 0);
    v_f32x4_t x2 = _mm256_extractf128_ps(x, 1);

    v_f64x4_t z1 = _mm256_cvtps_pd(x1);
    v_f64x4_t z2 = _mm256_cvtps_pd(x2);

    v_f64x4_t zs1 = z1 * z1, z41 = zs1 * zs1, z81 = z41 * z41;
    v_f64x4_t zs2 = z2 * z2, z42 = zs2 * zs2, z82 = z42 * z42;

    v_f64x4_t n01 = CN0 + zs1 * CN1, n21 = CN2 + zs1 * CN3, n41 = CN4 + zs1 * CN5, n61 = CN6 + zs1 * CN7;
    v_f64x4_t n02 = CN0 + zs2 * CN1, n22 = CN2 + zs2 * CN3, n42 = CN4 + zs2 * CN5, n62 = CN6 + zs2 * CN7;

    n01 += z41 * n21;
    n41 += z41 * n61;
    n01 += z81 * n41;

    n02 += z42 * n22;
    n42 += z42 * n62;
    n02 += z82 * n42;

    v_f64x4_t d01 = CD0 + zs1 * CD1, d21 = CD2 + zs1 * CD3, d41 = CD4 + zs1 * CD5, d61 = CD6 + zs1 * CD7;
    v_f64x4_t d02 = CD0 + zs2 * CD1, d22 = CD2 + zs2 * CD3, d42 = CD4 + zs2 * CD5, d62 = CD6 + zs2 * CD7;

    d01 += z41 * d21;
    d41 += z41 * d61;
    d01 += z81 * d41;

    d02 += z42 * d22;
    d42 += z42 * d62;
    d02 += z82 * d42;

    v_f64x4_t r1 = z1 * n01 / d01;
    v_f64x4_t r2 = z2 * n02 / d02;

    v_f32x8_t result = _mm256_setr_m128(_mm256_cvtpd_ps(r1), _mm256_cvtpd_ps(r2));
    ;

    /* If any of the input values are greater than ARG_MAX or smaller than SMALL_ARG,
     * call scalar tanhf
     */
    if (unlikely(any_v8_u32_loop(cond)))
        result = tanhf_specialcase(x, result, cond, sign);

    return result;
}
