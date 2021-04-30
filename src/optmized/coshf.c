/*
 * Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   float coshf(float x)
 *
 * Spec:
 *   coshf(-x) = coshf(x) if x < 0
 *   coshf(x) = inf       if x = +inf
 *   coshf(x) = inf       if x = -inf
 *
 *   coshf(x) overflows   if (approximately) x > log(FLT_MAX)  (89.415985107421875)
 */

#include "libm_util_amd.h"
#include "libm_inlines_amd.h"
#include "libm_special.h"

#include <stdint.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/poly.h>
#include <libm/amd_funcs_internal.h>

static struct {
    uint32_t arg_max, infinity;
    float logV, invV2, halfV, halfVm1;
    float theeps, xc, ybar, wmax;
    float poly_coshf[4];
    float half, t1;
} coshf_data = {
    .arg_max    = 0x42B2D4FC,           /* 89.415985107421875 */
    .infinity   = PINFBITPATT_SP32,
    .logV       = 0x1.62e6p-1,
    .invV2      = 0x1.fffc6p-3,
    .halfVm1    = 0x1.d0112ep-17,
    .halfV      = 0x1.0000e8p0,
    .theeps     = 0x1p-11,
    .xc         = 0x1.62e43p-2,
    .ybar       = 0x1.62e42ep6,
    .wmax       = 0x1.62e0f2p6,
    .half       = 0x1p-1,
    .poly_coshf = {
        0x1p+0,
        0x1p-1,
        0x1.555466p-5,
        0x1.6da5e2p-10,
    },
};

#define C0 coshf_data.poly_coshf[0]
#define C1 coshf_data.poly_coshf[1]
#define C2 coshf_data.poly_coshf[2]
#define C3 coshf_data.poly_coshf[3]

#define LOGV    coshf_data.logV
#define INVV2   coshf_data.invV2
#define HALFVM1 coshf_data.halfVm1
#define HALFV   coshf_data.halfV
#define THEEPS  coshf_data.theeps
#define XC      coshf_data.xc
#define YBAR    coshf_data.ybar
#define WMAX    coshf_data.wmax
#define HALF    coshf_data.half
#define INF     coshf_date.infinity
#define ARG_MAX coshf_data.arg_max


/*
 * Implementation Notes
 * ---------------------
 *
 *        cosh(x) = (exp(x) + exp(-x))/2
 *        cosh(-x) = +cosh(x)
 *
 *
 *                      +-
 *                      | v                         1
 *        coshf(x)   =  |--- * (exp(x - log(v)) + ------  * exp(-(x-log(v))))
 *                      | 2                        (v*v)
 *                      +-
 *
 *        Let z = exp(x - log(v))         ; where log(v) = 0.6931610107421875
 *
 *        coshf(x)   = v/2 * (z + 1/v^2 * 1/z)   ; exp(-z) = 1/z
 *
 *        To avoid division, we calculate coshf(x) as piece-wise function
 *
 *                   +------
 *                   | 1 + 1/2*x^2        if 0 <= x < EPS,  where EPS = 2*2^(-t/2), 
 *                   |                                           t is no of bits in significand
 *                   |
 *                   | poly(x)            if EPS <= x < XC  where XC = 1/2*ln(2)
 *        coshf(x) = |
 *                   | 1/2*(Z+1/Z)        if XC <= x < 8.5  where Z = exp(x)
 *                   |
 *                   | 1/2*Z              if 8.5 <= x < YBAR, where YBAR = ln(FLT_MAX)
 *                   |
 *                   | v/2*Z              if YBAR <= x < WMAX, where Z = exp(x - ln(v))
 *                   |                                               WMAX = min(YBAR, ln(FLT_MAX) - ln(v)+0.69)
 *                   | INFINITY           Otherwise
 *                   +-------
 *
 */

float
ALM_PROTO_OPT(coshf)(float x)
{
    float    y, w, z, res;
    uint32_t ux, sign;

    ux   = asuint32(x);
    sign = ux & SIGNBIT_SP32;

    y = asfloat(ux);

    if (sign) {             /* x is -ve */
        ux = ux ^ sign;
        y = x;
    }

    if (unlikely(ux > ARG_MAX)) {
        if (ux > PINFBITPATT_SP32)      /* |x| is a NaN? */
            return x + x;
        else                            /* x is infinity */
            return x / 0.0f;
    }

    if (ux <= asuint32(THEEPS))
        return (1.0f + HALF * x * x);

    if (ux > asuint32(8.5f)) {
        if (y > YBAR) {
            w = y - LOGV;
            z = ALM_PROTO(expf)(w);
            return HALFV * z;
        }

        z   = ALM_PROTO(expf)(y);
        res =  HALF * z;
    }
    else {
        /* y > THEEPS */
        if (y > XC) {
            z = ALM_PROTO(expf)(y);
            return HALF * (z + (1.0f / z));
        }

        /* coshf(x) = C0 + y^2*(C1 + y^2*(C2 + y^2*C3)) */
        float y2 = y * y;

        res = C0 + y2 * (C1 + y2 * (C2 + y2 * C3));
    }

    return res;
}

