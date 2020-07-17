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
 *    v_f32x4_t amd_vrs4_sinf(v_f32x4_t x)
 *
 *
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 *
 * Special cases handled by scalar sinf().
 *
 * 1. Argument reduction
 * Convert given x into the form
 * |x| = N * pi + f where N is an integer and f lies in [-pi/2,pi/2]
 * N is obtained by : N = round(x/pi)
 * f is obtained by : f = abs(x)-N*pi
 * sin(x) = sin(N * pi + f) = sin(N * pi)*cos(f) + cos(N*pi)*sin(f)
 * sin(x) = sign(x)*sin(f)*(-1)**N
 * f = abs(x) - N * pi
 *
 * 2. Polynomial approximation
 * The term sin(f) where [-pi/2 < f < pi/2] can be approximated by
 * using a polynomial computed using sollya using the Remez
 * algorithm to determine the coeffecients and the maximum ulp is 1.

 *
 ******************************************
*/

static struct {
    v_f64x4_t poly_sinf[5];
    v_f64x4_t pi, invpi, pi1, pi2;
    v_f64x4_t shift;
    v_u32x4_t infinity, mask32;
    v_u64x4_t sign_mask;
 } sinf_data = {
     .sign_mask = _MM_SET1_I64(0x7FFFFFFFFFFFFFFF),
     .mask32    = _MM_SET1_I32(0x7fffffff),
     .infinity  = _MM_SET1_I32(0x7f800000),
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
#define SHIFT   sinf_data.shift
#define ARG_MAX sinf_data.infinity

#define C1  sinf_data.poly_sinf[0]
#define C3  sinf_data.poly_sinf[1]
#define C5  sinf_data.poly_sinf[2]
#define C7  sinf_data.poly_sinf[3]
#define C9  sinf_data.poly_sinf[4]

#define SIGN_MASK sinf_data.sign_mask
#define SIGN_MASK32 sinf_data.mask32

float ALM_PROTO(sinf)(float);

static inline v_f32x4_t
sinf_specialcase(v_f32x4_t _x,
                 v_f32x4_t result,
                 v_i32x4_t cond)
{
    return v_call_f32(ALM_PROTO(sinf), _x, result, cond);
}

v_f32x4_t
ALM_PROTO_OPT(vrs4_sinf)(v_f32x4_t x)
{

    v_f64x4_t xd, F, poly, result;

    v_u64x4_t n;

    v_u64x4_t sign, ixd;

    v_u32x4_t ux = as_v4_u32_f32(x);

    v_u32x4_t cmp = (ux & SIGN_MASK32) > (ARG_MAX);

    xd = cast_v4_f32_to_f64(x);

    ixd = as_v4_u64_f64(xd);

    sign = ixd & (~SIGN_MASK);

    xd  = as_v4_f64_u64(ixd & (SIGN_MASK));

    v_f64x4_t dn =  xd * invpi + SHIFT;

    n = cast_v4_f64_to_i64(dn);

    dn -= SHIFT;

    F = xd - dn * pi1;

    F = F - dn * pi2;

    v_u64x4_t odd =  n << 63;

   /*
    *  Calculate the polynomial approximation x*(1+C1*x^2+C2*x^4+C3*x^6+C4*x^8)
    *
    */

    poly = POLY_EVAL_ODD_9(F, C1, C3, C5, C7, C9);

    result = as_v4_f64_u64(as_v4_u64_f64(poly) ^ sign ^ odd);

    v_f32x4_t ret = cvt_v4_f64_to_f32(result);

    if(unlikely(any_v4_u32_loop(cmp))) {
        return sinf_specialcase(x, ret, cmp);
    }

    return ret;
}
