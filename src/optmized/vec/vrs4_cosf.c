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
 *      Now, let x be represented as,
 *          |x| = N * pi + f        (2) | N is an integer,
 *                                        -pi/2 <= f <= pi/2
 *
 *      From (2), N = int( (x + pi/2) / pi) - 0.5
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

            v_f32x4_t poly_cosf[5];
            v_f32x4_t half;
            v_f32x4_t half_pi, inv_pi, pi_head, pi_tail1, pi_tail2;
            v_f32x4_t alm_huge;
            v_u32x4_t mask_32, arg_max;
            } v4_cosf_data = {
                            .half      = _MM_SET1_PS4(0x1p-1f),
                            .half_pi   = _MM_SET1_PS4(0x1.921fb6p0f),
                            .inv_pi    = _MM_SET1_PS4(0x1.45f306p-2f),
                            .pi_head   = _MM_SET1_PS4(-0x1.921fb6p1f),
                            .pi_tail1  = _MM_SET1_PS4(0x1.777a5cp-24f),
                            .pi_tail2  = _MM_SET1_PS4(0x1.ee59dap-49f),
                            .alm_huge  = _MM_SET1_PS4(0x1.8p23f),
                            .mask_32   = _MM_SET1_I32(0x7FFFFFFF),
                            .arg_max   = _MM_SET1_I32(0x4A989680),
                            .poly_cosf = {
                                            _MM_SET1_PS4(0x1.p0f),
                                            _MM_SET1_PS4(-0x1.555548p-3f),
                                            _MM_SET1_PS4(0x1.110df4p-7f),
                                            _MM_SET1_PS4(-0x1.9f42eap-13f),
                                            _MM_SET1_PS4(0x1.5b2e76p-19f),
                                         },
            };

#define V4_COSF_HALF       v4_cosf_data.half
#define V4_COSF_HALF_PI    v4_cosf_data.half_pi
#define V4_COSF_INV_PI     v4_cosf_data.inv_pi
#define V4_COSF_PI_HEAD    v4_cosf_data.pi_head
#define V4_COSF_PI_TAIL1   v4_cosf_data.pi_tail1
#define V4_COSF_PI_TAIL2   v4_cosf_data.pi_tail2
#define V4_COSF_MASK_32    v4_cosf_data.mask_32
#define V4_COSF_ARG_MAX    v4_cosf_data.arg_max
#define ALM_HUGE           v4_cosf_data.alm_huge

#define C0 v4_cosf_data.poly_cosf[0]
#define C1 v4_cosf_data.poly_cosf[1]
#define C2 v4_cosf_data.poly_cosf[2]
#define C3 v4_cosf_data.poly_cosf[3]
#define C4 v4_cosf_data.poly_cosf[4]


float ALM_PROTO(cosf)(float);

static inline v_f32x4_t
cosf_specialcase(v_f32x4_t _x, v_f32x4_t result, v_i32x4_t cond)
{
    return call_v4_f32(ALM_PROTO(cosf), _x, result, cond);
}


v_f32x4_t
ALM_PROTO_OPT(vrs4_cosf)(v_f32x4_t x)
{

    v_f32x4_t dinput, frac, poly, result;

    v_u32x4_t ux = as_v4_u32_f32(x);

    /* Check for special cases */
    v_u32x4_t cond = (ux & V4_COSF_MASK_32) > V4_COSF_ARG_MAX;

    /* Remove sign from input */
    dinput = as_v4_f32_u32(ux & V4_COSF_MASK_32);

    /* Get n = int ((x + pi/2) /pi) - 0.5 */
    v_f32x4_t dn = ((dinput + V4_COSF_HALF_PI) * V4_COSF_INV_PI) + ALM_HUGE;
    v_u32x4_t n = as_v4_u32_f32(dn);
    dn = dn - ALM_HUGE;
    dn = dn - V4_COSF_HALF;

    /* frac = x - (n*pi) */
    frac = dinput + (dn * V4_COSF_PI_HEAD);
    frac = frac + (dn * V4_COSF_PI_TAIL1);
    frac = frac + (dn * V4_COSF_PI_TAIL2);

    /* Check if n is odd or not */
    v_u32x4_t odd = n << 31;

    /* Compute sin(f) using the polynomial
     * x*(1+C1*x^2+C2*x^4+C3*x^6+C4*x^8)
     */
    poly = POLY_EVAL_ODD_9(frac, C0, C1, C2, C3, C4);

    /* If n is odd, result is negative */
    result = as_v4_f32_u32(as_v4_u32_f32(poly) ^ odd);

    /* If any of the input values are greater than ARG_MAX,
     * call scalar cosf

     */
    if(unlikely(any_v4_u32_loop(cond)))
        return cosf_specialcase(x, result, cond);

    return result;
}

