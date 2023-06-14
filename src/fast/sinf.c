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
 * Fast version of sinf
 * This routine doesn't take care of special cases such as INFs and NaNs
 * Maximum ULP: 2.99
 *  
 * Signature:
 *   float sinf(float x)
 *
 **************************************************************************
 * Implementation Notes
 * ---------------------
 * Convert given x into the form,
 * |x| = N * pi + f where N is an integer and f lies in [-pi/2,pi/2]
 * N is obtained by : N = round(x/pi)
 * f is obtained by : f = abs(x)-N*pi
 * sinf(x) = sinf(N * pi + f) = sinf(N * pi)*cosf(f) + cosf(N*pi)*sinf(f)
 * sinf(x) = sign(x)*sinf(f)*(-1)^N
 *
 * The term sinf(f) can be approximated by using a polynomial
 ***************************************************************************
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

static struct {

    float invpi, pi1, pi2, pi3;
    float shift;
    uint32_t mask32, max_arg;
    float poly_sinf[5];

} fast_sinf_data = {

    .max_arg = 0x49800000,
    .mask32  = 0x7fffffff,
    .pi1     = -0x1.921fb6p1f,
    .pi2     = 0x1.777a5cp-24f,
    .pi3     = 0x1.ee59dap-49f,
    .invpi   = 0x1.45f306p-2f,
    .shift   = 0x1.8p23f,

    /*
      * Polynomial coefficients obtained using Remez algorithm from Sollya
    */
     .poly_sinf = {
         0x1.p0f,
         -0x1.555548p-3f,
         0x1.110e7cp-7f,
         -0x1.9f6446p-13f,
         0x1.5d38b6p-19f
     },

};

#define FAST_SINF_PI1     fast_sinf_data.pi1
#define FAST_SINF_PI2     fast_sinf_data.pi2
#define FAST_SINF_PI3     fast_sinf_data.pi3
#define FAST_SINF_INVPI   fast_sinf_data.invpi
#define ALM_SHIFT_SP      fast_sinf_data.shift
#define FAST_SINF_ARG_MAX fast_sinf_data.max_arg
#define ALM_SIGN_MASK32   fast_sinf_data.mask32

#define C1  fast_sinf_data.poly_sinf[0]
#define C3  fast_sinf_data.poly_sinf[1]
#define C5  fast_sinf_data.poly_sinf[2]
#define C7  fast_sinf_data.poly_sinf[3]
#define C9  fast_sinf_data.poly_sinf[4]

float
ALM_PROTO_FAST(sinf)(float x)
{
    float r, F, poly, result;
    uint32_t n,sign;

    uint32_t ux = asuint32(x);

    sign = ux & ~ALM_SIGN_MASK32;

    r = asfloat(ux & ALM_SIGN_MASK32);

    float dn = r * FAST_SINF_INVPI + ALM_SHIFT_SP;

    n = asuint32(dn);

    dn -= ALM_SHIFT_SP;

    F = r + dn * FAST_SINF_PI1;
    F = F + dn * FAST_SINF_PI2;
    F = F + dn * FAST_SINF_PI3;

    uint32_t odd =  n << 31;

    poly = POLY_EVAL_ODD_9(F, C1, C3, C5, C7, C9);
    
    result = asfloat(asuint32(poly) ^ sign ^ odd);

    return result;
}

strong_alias (__sinf_finite, ALM_PROTO_FAST(sinf))
weak_alias (amd_sinf, ALM_PROTO_FAST(sinf))
weak_alias (sinf, ALM_PROTO_FAST(sinf))