/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 */
#include <stdint.h>
#include <libm_util_amd.h>
#include <libm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/poly.h>


/*
 * Signature:
 *    v_f32x8_t amd_vrs8_sinf(v_f32x8_t x)
 *
 *
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 *
 * Convert given x into the form
 * |x| = N * pi + f where N is an integer and f lies in [-pi/2,pi/2]
 * N is obtained by : N = round(x/pi)
 * f is obtained by : f = abs(x)-N*pi
 * sin(x) = sin(N * pi + f) = sin(N * pi)*cos(f) + cos(N*pi)*sin(f)
 * sin(x) = sign(x)*sin(f)*(-1)**N
 *
 * The term sin(f) can be approximated by using a polynomial
 *
 ******************************************
*/

static struct {
    v_f64x4_t invpi, pi, pi1, pi2;
    v_f64x4_t shift;
    v_u32x8_t infinity, mask32;
    v_u64x4_t sign_mask;
    v_f64x4_t poly_sinf[5];
 } sinf_data = {
     .sign_mask = _MM_SET1_I64(0x7FFFFFFFFFFFFFFF),
     .mask32    = _MM256_SET1_I32(0x7fffffff),
     .infinity  = _MM256_SET1_I32(0x7f800000),
     .pi    = _MM_SET1_PD4(0x1.921fb54442d18p1),
     .pi1   = _MM_SET1_PD4(0x1.921fb50000000p1),
     .pi2   = _MM_SET1_PD4(0x1.110b4611a6263p-25),
     .invpi = _MM_SET1_PD4(0x1.45f306dc9c883p-2),
     .shift = _MM_SET1_PD4(0x1.8p+52),
     /*
      * Polynomial coefficients obtained using Remez algorithm from Sollya
      */
     .poly_sinf = {
         _MM_SET1_PD4(1.0),
         _MM_SET1_PD4(-0x1.55554d018df8bp-3),
         _MM_SET1_PD4(0x1.110f0293a5dcbp-7),
         _MM_SET1_PD4(-0x1.9f781a0aebdb9p-13),
         _MM_SET1_PD4(0x1.5e2a3e7550c85p-19)
     },
};


#define pi      sinf_data.pi
#define pi1     sinf_data.pi1
#define pi2     sinf_data.pi2
#define invpi   sinf_data.invpi
#define ALM_SHIFT   sinf_data.shift
#define ARG_MAX sinf_data.infinity

#define C1  sinf_data.poly_sinf[0]
#define C3  sinf_data.poly_sinf[1]
#define C5  sinf_data.poly_sinf[2]
#define C7  sinf_data.poly_sinf[3]
#define C9  sinf_data.poly_sinf[4]

#define SIGN_MASK sinf_data.sign_mask
#define SIGN_MASK32 sinf_data.mask32

float ALM_PROTO(sinf)(float);


static inline v_f32x8_t
sinf_specialcase(v_f32x8_t _x,
                 v_f32x8_t result,
                 v_i32x8_t cond)
{
    return call_v8_f32(ALM_PROTO(sinf), _x, result, cond);
}

v_f32x8_t
ALM_PROTO_OPT(vrs8_sinf)(v_f32x8_t x)
{

    v_f64x4_t xd, F, poly, result;

    v_u64x4_t n;

    v_u64x4_t sign, uxd;

    v_u32x8_t ux = as_v8_u32_f32(x);

    v_u32x8_t cmp = (ux & SIGN_MASK32) > (ARG_MAX);

    v_f32x4_t _x[2];

    v_f32x4_t ret_array[2];

    _x[0] = _mm256_extractf128_ps(x, 0);

    _x[1] = _mm256_extractf128_ps(x, 1);

    for(int lane = 0; lane < 2; lane++) {

        xd = cast_v4_f32_to_f64(_x[lane]);

        uxd = as_v4_u64_f64(xd);

        sign = uxd & (~SIGN_MASK);

        xd  = as_v4_f64_u64(uxd & (SIGN_MASK));

        v_f64x4_t dn =  xd * invpi + ALM_SHIFT;

        n = cast_v4_f64_to_i64(dn);

        dn -= ALM_SHIFT;

        F = xd - dn*pi1;

        F = F - dn*pi2;

        v_u64x4_t odd =  n << 63;

        /*
        *  Calculate the polynomial approximation x*(1+C1*x^2+C2*x^4+C3*x^6+C4*x^8)
        *
        *
        */

        poly = POLY_EVAL_ODD_9(F, C1, C3, C5, C7, C9);

        result = as_v4_f64_u64(as_v4_u64_f64(poly) ^ sign ^ odd);

        ret_array[lane] = cvt_v4_f64_to_f32(result);

    }

    v_f32x8_t ret =  _mm256_setr_m128(ret_array[0], ret_array[1]);

    if(unlikely(any_v8_u32_loop(cmp))) {
        return sinf_specialcase(x, ret, cmp);
    }

    return ret;
}
