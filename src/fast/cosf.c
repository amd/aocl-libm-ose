/*
 * Copyright (C) 2023 Advanced Micro Devices, Inc. All rights reserved.
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
 * Fast version of cosf function
 * This implementation does not take care of special cases such as NaNs/INFs
 * Maximum ULP: 2.84 
 * Signature:
 *   float cosf(float x)
 *
 ******************************************
 * Implementation Notes
 * ---------------------
 * To compute cosf(float x)
 *
 * Using the identity,
 * cosf(x) = sinf(x + pi/2)           (1)
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
 *      Hence, cosf(x) = sinf(x + pi/2) = (-1)^N * sinf(f)
 *
 */

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>
#include <libm/alm_special.h>

static struct 
{
    float poly_cosf[5];
    float half;
    float half_pi, inv_pi, pi_head, pi_tail1, pi_tail2;
    float alm_huge;
    uint32_t mask_32, arg_max;
}fast_cosf_data = {
        .half = 0x1p-1f,
        .half_pi = 0x1.921fb6p0f,
        .inv_pi = 0x1.45f306p-2f,
        .pi_head = -0x1.921fb6p1f,
        .pi_tail1 = 0x1.777a5cp-24f,
        .pi_tail2 = 0x1.ee59dap-49f,
        .alm_huge = 0x1.8p23f,
        .mask_32 = 0x7FFFFFFF,
        .arg_max = 0x4A989680,
        .poly_cosf = {
                        0x1.p0f,
                        -0x1.555548p-3f,
                        0x1.110df4p-7f,
                        -0x1.9f42eap-13f,
                        0x1.5b2e76p-19f,
                     },
        
};

#define FAST_COSF_HALF       fast_cosf_data.half
#define FAST_COSF_HALF_PI    fast_cosf_data.half_pi
#define FAST_COSF_INV_PI     fast_cosf_data.inv_pi
#define FAST_COSF_PI_HEAD    fast_cosf_data.pi_head
#define FAST_COSF_PI_TAIL1   fast_cosf_data.pi_tail1
#define FAST_COSF_PI_TAIL2   fast_cosf_data.pi_tail2
#define FAST_COSF_MASK_32    fast_cosf_data.mask_32
#define FAST_COSF_ARG_MAX    fast_cosf_data.arg_max
#define ALM_HUGE             fast_cosf_data.alm_huge

#define C0 fast_cosf_data.poly_cosf[0]
#define C1 fast_cosf_data.poly_cosf[1]
#define C2 fast_cosf_data.poly_cosf[2]
#define C3 fast_cosf_data.poly_cosf[3]
#define C4 fast_cosf_data.poly_cosf[4]

float
ALM_PROTO_FAST(cosf)(float x)
{

    float dinput, frac, poly, result;

    uint32_t ux = asuint32(x);

    /* Get absolute value of input*/   
    dinput = asfloat(ux & FAST_COSF_MASK_32);

    /* Get n = int ((x + pi/2) /pi) - 0.5 */
    float dn = ((dinput * FAST_COSF_INV_PI) + FAST_COSF_HALF) + ALM_HUGE;
    uint32_t n = asuint32(dn);
    dn = dn - ALM_HUGE;
    dn = dn - FAST_COSF_HALF;

    /* frac = x - (n*pi) */
    frac = dinput + (dn * FAST_COSF_PI_HEAD);
    frac = frac + (dn * FAST_COSF_PI_TAIL1);
    frac = frac + (dn * FAST_COSF_PI_TAIL2);

    /* Check if n is odd or not */
    uint32_t odd = n << 31;

    /* Compute sin(f) using the polynomial
     * x*(1+C1*x^2+C2*x^4+C3*x^6+C4*x^8)
    */
    poly = POLY_EVAL_ODD_9(frac, C0, C1, C2, C3, C4);
    
    /* If n is odd, result is negative */
    result = asfloat(asuint32(poly) ^ odd);
    
    return result;

}

strong_alias (__cosf_finite, ALM_PROTO_FAST(cosf))
weak_alias (amd_cosf, ALM_PROTO_FAST(cosf))
weak_alias (cosf, ALM_PROTO_FAST(cosf))
