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

#include "libm_macros.h"
#include "libm_util_amd.h"

#include <libm/types.h>
#include <libm/alm_special.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>

#define MASK_MANT_ALL7   0x007f8000
#define MASK_MANT8       0x00008000
#define MASK_SIGN        0x7FFFFFFF
#define LOGF_N           8
#define LOGF_POLY_DEGREE 2

#include "logf_data.h"

/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   float log2f(float x)
 *
 * Spec:
 *   log2f(x)
 *          = log2f(x)           if x ∈ F and x > 0
 *          = x                 if x = qNaN
 *          = 0                 if x = 1
 *          = -inf              if x = (-0, 0}
 *          = NaN               otherwise
 *
 * Implementation Notes:
 *       x  =   2^m * (1 + A)     where 1 <= A < 2
 *
 *       x  =   2^m * (1 + (G + g)) where 1 <= 1+G < 2 and g < 2^(-8)
 *
 *    Let F = (1 + G) and f = g
 *       x  = 2^m*(F + f), so 1 <= F < 2, f < 2^(-8)
 *
 *       x  = (2^m) * 2 * (F/2+f/2)
 *               So, 0.5 <= F/2 < 1,   f/2 < 2^-9
 *
 *            => A        +   B     +       C
 *
 *       A =  log2(2^m) = m           .. (1)
 *       B =  log(2)                  .. (2) => Constant can be precalculated
 *       C =  log2(F/2*(1 + f/F))      .. (3)
 *         =>  log2(F/2*(1 + f/F))
 *         =>  log2(F/2) + log2(1 + f/F)
 *
 *               (from log(a) + log(b) <=> log(a*b))
 *
 *         => m + log2(F) + ln(1 + r) * log2(e) where r = f/F
 */

extern struct logf_table log2f_lookup[1 << LOGF_N];

static struct {
  float poly_log2f[2];
  float poly1[2];
  float log2e;
  float log2e_lead, log2e_tail;
} log2f_data = {
               /* Polynomial coefficients obtained using fpminimax algorithm from Sollya */
      .poly_log2f = {
         0.5f,
         3.33333313465e-1f,
              },

      .poly1 = {
         8.33333333333317923934e-02f,    /* A1 */
         1.25000000037717509602e-02f,    /* A2 */
      },

      .log2e = 1.44269502162933349609375E0f,

      .log2e_lead = 1.4375f,
      .log2e_tail = 0.0051950408889633f,
};

#define EXPSHIFTBITS_SP32 23  
#define C1 log2f_data.poly_log2f[0]
#define C2 log2f_data.poly_log2f[1]
#define C3 log2f_data.poly_log2f[2]
#define C4 log2f_data.poly_log2f[3]
#define C5 log2f_data.poly_log2f[4]
#define C6 log2f_data.poly_log2f[5]
#define C7 log2f_data.poly_log2f[6]
#define C8 log2f_data.poly_log2f[7]
#define C9 log2f_data.poly_log2f[8]

#define LOG2_E      log2f_data.log2e
#define LOG2_E_HEAD log2f_data.log2e_lead
#define LOG2_E_TAIL log2f_data.log2e_tail

static inline float_t
inline_log2_near_1f(float_t x)
{
    float_t r, r2, w, f, t, z;

    float_t rt, rtop, s;

    r = x - 1.0f;

    w = r / (2.0f + r);

    float_t correction = w * r;

    w += w;

    float_t w2 = w * w;

#define A1 log2f_data.poly1[0]
#define A2 log2f_data.poly1[1]

    r2 = (w  * w2 * (A1 + A2 * w2)) - correction;

    rtop = asfloat(asuint32(r) & 0xffff0000);

    rt = r - rtop;

    f = r2 + rt;

    s = LOG2_E_HEAD * f;

    z = LOG2_E_TAIL * rtop;

    t = LOG2_E_TAIL * f + z;

    w = s + t;

    return w + (rtop * LOG2_E_HEAD);

}

float
ALM_PROTO_OPT(log2f)(float x)
{

    uint32_t ux = asuint32(x);

    float  r, s, y, k, f;
    float  z, result, poly;
    int32_t n;
    uint32_t mantissa;

    if (unlikely (ux - 0x00800000 >= 0x7f800000 - 0x00800000)) {   /* x < 0x1p-126 or inf or nan. */

        uint32_t sign = ux & SIGNBIT_SP32;

        if (ux * 2 == 0) {                /* log2(0) = -inf */
            return alm_logf_special(x, asfloat(NINFBITPATT_SP32), ALM_E_DIV_BY_ZER0);
        }

        if (x != x)  {/* nan */
            if( (ux & QNANBITPATT_SP32) == QNANBITPATT_SP32)
                return x;

            return alm_logf_special(x, asfloat(QNANBITPATT_SP32), ALM_E_IN_X_NAN);
        }

        if (sign) {        /* x is -ve */
            return alm_logf_special(x, asfloat(QNANBITPATT_SP32), ALM_E_IN_X_NEG);
        }

        if ((ux & PINFBITPATT_SP32) == PINFBITPATT_SP32) {           /* log2(inf) = inf */
            return asfloat(PINFBITPATT_SP32);
        }

        /*
         * 'x' has to be denormal, Normalize it
         * there is a possibility that only the last (23'rd) bit is set
         * Hence multiply by 2^23 to bring to 1.xxxxx format.
         */
        ux = asuint32(x * 0x1p23f);

        ux -= 23 << 23;

    }

    /* Get the exponent */
    n = ((int32_t)ux >> EXPSHIFTBITS_SP32) - 127;

    k = (float)n;

#define NEAR_ONE_LO asuint32(1 - 0x1.0p-4)
#define NEAR_ONE_HI asuint32(1 + 0x1.1p-4)

    /* Values very close to 1, e^(-1/16) <= x <= e^(1/16)*/
    if (unlikely(ux - NEAR_ONE_LO < NEAR_ONE_HI - NEAR_ONE_LO)) {

        return inline_log2_near_1f(x);

    }

    mantissa = ux & 0x007FFFFF;

    uint32_t index = (ux & 0x007f0000);

    uint32_t j = index >> 16;

    y = asfloat(mantissa | 0x3f000000);

    f = asfloat(index | 0x3f000000);

    s = f - y;

    struct logf_table *tbl = &log2f_lookup[j];

    r = s * tbl->f_inv;

    poly = r * (1 + r * (C1 + r *C2));

    /*log_128_tail - poly * log2_e */
    z = tbl->f_128_tail - poly * LOG2_E;

    result = (k + tbl->f_128_head) + z;

    return result;

}


