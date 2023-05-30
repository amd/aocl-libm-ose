/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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
 *   coshf(x) overflows   if (approximately) x > ln(2*FLT_MAX)  (89.41598609556996)
 */

#include <libm_macros.h>
#include <libm_util_amd.h>
#include <libm/types.h>
#include <libm/compiler.h>

#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>

static struct {
    float half,
        log_v,
        half_v,
        very_small_x,
        small_x,
        exp_max,
        cosh_max;
    float poly_coshf[4];
} coshf_data = {
    .half         = 0x1p-1,
    .log_v        = 0x1.62e6p-1,
    .half_v       = 0x1.0000e8p0,
    .very_small_x = 0x1p-11,
    .small_x      = 0x1.62e43p-2,
    .cosh_max     = 0x1.65a9f84p6f,  /* ln(2*FLT_MAX) = 89.41598609556996 */
    .exp_max      = 0x1.62e42fep6f,  /* ln(FLT_MAX)   = 88.72283891501002 */
    .poly_coshf   = {
        0x1p+0,
        0x1p-1,
        0x1.555466p-5,
        0x1.6da5e2p-10,
    },
};

#define A0 coshf_data.poly_coshf[0]
#define A1 coshf_data.poly_coshf[1]
#define A2 coshf_data.poly_coshf[2]
#define A3 coshf_data.poly_coshf[3]

#define LOGV    coshf_data.log_v
#define HALFV   coshf_data.half_v
#define VERY_SMALL_X  coshf_data.very_small_x
#define SMALL_X      coshf_data.small_x
#define EXP_MAX    coshf_data.exp_max
#define COSH_MAX    coshf_data.cosh_max
#define HALF    coshf_data.half

/*
 * Since we are sending single precision value to
 * double precision kernel, large ULP is acceptible
 */
#include "kern/expf.c"

/* Implementation Notes
 * ---------------------
 *
 *        cosh(x) = (e^x + e^-x)/2
 *        cosh(-x) = +cosh(x)
 *
 *
 *                      +-
 *                      | v    ,--                 1                   --.
 *        coshf(x)   =  |--- * |e^(x - ln(v)) +  ------  * e^(-(x-ln(v)) |
 *                      | 2    |                  (v*v)                  |
 *                      +-     `--                                     --'
 *
 *        Let z = exp(x - ln(v))         ; where ln(v) = 0.6931610107421875
 *
 *        coshf(x)   = v/2 * (z + 1/v^2 * 1/z)   ; e^-j = 1/e^j
 *
 *        To avoid division, we calculate coshf(x) as piece-wise function
 *
 *                   +------                                               (-t/2)
 *                   | 1 + 1/2*x^2        if 0 <= x < EPS,  where EPS = 2*2^,
 *                   |                                           t is bits in significand
 *                   |
 *                   | poly(x)            if EPS <= x < SMALL_X  where SMALL_X = 1/2*ln(2)
 *        coshf(x) = |
 *                   | 1/2*(Z+1/Z)        if SMALL_X <= x < 8.5  where Z = exp(x)
 *                   |
 *                   | 1/2*Z              if 8.5 <= x < EXP_MAX, where EXP_MAX = ln(FLT_MAX)
 *                   |
 *                   | v/2*Z              if EXP_MAX <= x < COSH_MAX, where Z = exp(x - ln(v))
 *                   |                                               COSH_MAX = min(EXP_MAX, ln(FLT_MAX) - ln(v)+0.69)
 *                   | INFINITY           Otherwise
 *                   +-------
 *
 *        poly(x) is Taylor series of degree 2.
 *
 *        More information in docs/internal/cosh.md
 */

static float
coshf_expf_kern(float x)
{
    return ALM_PROTO_KERN(expf)(x);
}

float
ALM_PROTO_OPT(coshf)(float x)
{
    float    y, w, z, res;
    uint32_t ux;

    ux   = asuint32(x) & ~(SIGNBIT_SP32);

    y = asfloat(ux);

    if (unlikely(ux > asuint32(COSH_MAX))) {
        if (x != x)      /* |x| is a NaN? */
            return x + x;
        else                            /* x is infinity */
        #if ( defined _WIN32 || defined _WIN64 )
            return __alm_handle_errorf(PINFBITPATT_SP32, AMD_F_OVERFLOW);
        #else
            return(FLT_MAX * FLT_MAX);
        #endif
    }

    if (ux <= asuint32(VERY_SMALL_X))   /* x in (0, EPS] */
        return (1.0f + HALF * x * x);

    if (ux > asuint32(8.5f)) {          /* x in (8.5, COSH_MAX] */
        if (y > EXP_MAX) {              /* x in (EXP_MAX, COSH_MAX] */
            w = y - LOGV;
            z = coshf_expf_kern(w);
            return HALFV * z;
        }
        /* x in (8.5, EXP_MAX] Call exp() as expf() will return infintiy for y > logf(FLT_MAX)*/
        double t   = exp((double)y);
        res =  (float)(0.5 * t);
    }
    else {                              /* x in (VERY_SMALL_X, 8.5] */
        if (y > SMALL_X) {              /* x in (SMALL_X, 8.5] */
            z = coshf_expf_kern(y);
            return HALF * (z + (1.0f / z));
        }

        /* x in (VERY_SMALL_X, SMALL_X] */
        /* coshf(x) = C0 + y^2*(C1 + y^2*(C2 + y^2*C3)) */
        float y2 = y * y;

        res = A0 + y2 * (A1 + y2 * (A2 + y2 * A3));
    }

    return res;
}

