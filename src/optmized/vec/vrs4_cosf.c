/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

/*
 * Signature:
 *   v_f32x4_t cosf(v_f32x4_t x)
 *
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 * To compute cosf(float x)
 * Using the identity,
 * cos(x) = sin(x + pi/2)           (1)
 *
 * 1. Argument Reduction
 *      Adding pi/2 to x, x is now x + pi/2
 *      Now, let x be represented as,
 *          |x| = N * pi + f        (2) | N is an integer,
 *                                        -pi/2 <= f <= pi/2
 *
 *      From (2), N = int(x / pi)
 *                f = |x| - (N * pi)
 *
 * 2. Polynomial Evaluation
 *       From (1) and (2),sin(f) can be calculated using a polynomial
 *       sin(f) = f*(1 + C1*f^2 + C2*f^4 + C3*f^6 + c4*f^8)
 *
 * 3. Reconstruction
 *      Hence, cos(x) = sin(x + pi/2) = (-1)^N * sin(f)
 *
 * MAX ULP of current implementation : 1
 */

#include <stdint.h>
#include <emmintrin.h>

#include <libm_util_amd.h>
#include <libm_special.h>
#include <libm_macros.h>

#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>
#include <libm/poly.h>

static struct {

            v_f64x4_t poly_cosf[5];
            v_f64x4_t half_pi, inv_pi, pi_head, pi_tail;
            v_f64x4_t alm_huge;
            v_u32x4_t mask_32, infinity;
            v_u64x4_t sign_mask;
            } cosf_data = {
                            .half_pi   = _MM_SET1_PD4(0x1.921fb54442d18p0),
                            .inv_pi    = _MM_SET1_PD4(0x1.45f306dc9c883p-2),
                            .pi_head   = _MM_SET1_PD4(0x1.921fb50000000p1),
                            .pi_tail   = _MM_SET1_PD4(0x1.110b4611a6263p-25),
                            .alm_huge  = _MM_SET1_PD4(0x1.8p+52),
                            .sign_mask = _MM_SET1_I64(0x7FFFFFFFFFFFFFFF),
                            .mask_32   = _MM_SET1_I32(0x7FFFFFFF),
                            .infinity  = _MM_SET1_I32(0x7f800000),
                            .poly_cosf = {
                                            _MM_SET1_PD4(0x1p0),
                                            _MM_SET1_PD4(-0x1.55554d018df8bp-3),
                                            _MM_SET1_PD4(0x1.110f0293a5dcbp-7),
                                            _MM_SET1_PD4(-0x1.9f781a0aebdb9p-13),
                                            _MM_SET1_PD4(0x1.5e2a3e7550c85p-19),
                                         },
            };

#define HALF_PI   cosf_data.half_pi
#define INV_PI    cosf_data.inv_pi
#define PI_HEAD   cosf_data.pi_head
#define PI_TAIL   cosf_data.pi_tail
#define ALM_HUGE  cosf_data.alm_huge
#define SIGN_MASK cosf_data.sign_mask
#define MASK_32   cosf_data.mask_32
#define ARG_MAX   cosf_data.infinity

#define C0 cosf_data.poly_cosf[0]
#define C1 cosf_data.poly_cosf[1]
#define C2 cosf_data.poly_cosf[2]
#define C3 cosf_data.poly_cosf[3]
#define C4 cosf_data.poly_cosf[4]

float ALM_PROTO(cosf)(float);

static inline v_f32x4_t
cosf_specialcase(v_f32x4_t _x, v_f32x4_t result, v_i32x4_t cond)
{
    return call_v4_f32(ALM_PROTO(cosf), _x, result, cond);
}

v_f32x4_t
ALM_PROTO_OPT(vrs4_cosf)(v_f32x4_t x)
{

    v_f64x4_t dinput, frac, poly, result;
    v_u64x4_t ixd;

    v_u32x4_t ux = as_v4_u32_f32(x);

    /* Check for special cases */
    v_u32x4_t cond = (ux & MASK_32) > ARG_MAX;

    /* Convert input to double precision */
    dinput = cast_v4_f32_to_f64(x);
    ixd = as_v4_u64_f64(dinput);

    /* Remove sign from input */
    dinput = as_v4_f64_u64(ixd & SIGN_MASK);

    /* x + pi/2 */
    dinput = dinput + HALF_PI;

    /* Get n = int (x/pi) */
    v_f64x4_t dn = (dinput * INV_PI) + ALM_HUGE;
    v_u64x4_t n = cast_v4_f64_to_i64(dn);
    dn = dn - ALM_HUGE;

    /* frac = x - (n*pi) */
    frac = dinput - (dn * PI_HEAD);
    frac = frac - (dn * PI_TAIL);

    /* Check if n is odd or not */
    v_u64x4_t odd = n << 63;

    /*
     * Compute sin(f) using the polynomial
     * x*(1+C1*x^2+C2*x^4+C3*x^6+C4*x^8)
     */
    poly = POLY_EVAL_ODD_9(frac, C0, C1, C2, C3, C4);

    /* If n is odd, result is negative */
    for (int i =0; i<4; i++)
    {
        if(odd[i])
            result[i] = -poly[i];
        else
            result[i] = poly[i];
    }

    /* If any of the input values are greater than ARG_MAX,
     * call scalar cosf
     */
    if(unlikely(any_v4_u32(cond)))
        return cosf_specialcase(x, cvt_v4_f64_to_f32(result), cond);

    return cvt_v4_f64_to_f32(result);
}
