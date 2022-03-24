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
 *     Compute log1pf(x) = logf(1 + x)
 *
 *     IEEE SPEC:
 *     If the argument is ±0, it is returned unmodified
 *     If the argument is -1, -∞ is returned and FE_DIVBYZERO is raised.
 *     If the argument is less than -1, NaN is returned and FE_INVALID is raised.
 *     If the argument is +∞, +∞ is returned
 *     If the argument is NaN, NaN is returned
 *
 *
 *     Algorithm:
 *
 *     if(exp(1/16)-1 < x < exp(-1/16)-1)
 *         u = x / (x + 2.0);
 *         corr = x * u;
 *         log(1 + p) =  x +(2 * u * u * u * (D1 + u * u * (D2 + u * u * (D3 +  u * u * D4)))) - corr;
 *     else
 *         x = x + 1
 *         1 <= 2^-m . X <= 2  (i.e., 1 <= (1/2^m).x <= 2)
 *         Determine 'f' and 'F' such that,
 *         x = 2^m(F + f)
 *         F = 1 + j.2^(-6)   j = 0,1,2,...,2^(7)
 *         f <= 2^(-8)
 *         x = 2^m(F + f)
 *         u = f / (F * 0.5 + f);
 *         v = u * u;
 *
 *         log(1+x) = m * log(2) + log(F) + log(v);
 *
 *
 *
 */


#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>

#if !defined(__clang__) && !defined(ENABLE_DEBUG)
#pragma GCC push_options
#pragma GCC optimize ("O2")
#endif  /* !DEBUG */

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>

#include "log1p_data.h"

static struct {
    uint32_t sign_mask, minus_one, epsilon;
    uint64_t index_mask1,index_mask2, index_mask3, one;
    double threshold_low, threshold_high;
    double index_constant1;
    int32_t index_constant2;
    double ln2_lead, ln2_tail;

    double ALIGN(16) poly_1[3];
    double ALIGN(16) poly_2[4];
} log1p_data = {
      .sign_mask = 0x7FFFFFFF,
      .minus_one = 0xBF800000,
      .epsilon = 0x33800000,
      .index_mask1 = 0x000fc00000000000U,
      .index_mask2 = 0x0000200000000000U,
      .index_mask3 = 0x0010000000000000U,
      .one = 0x3ff0000000000000,
      .threshold_low = -6.05869371865242201114e-02,
      .threshold_high = 6.44944589178594318568e-02,
      .index_constant1 = 1.5625E-2,
      .index_constant2 = 64,
      .ln2_lead = 0x1.62e42e0000000p-1,
      .ln2_tail = 0x1.efa39ef35793cp-25,
      .poly_1 = {
                       8.33333333333333593622e-02,
                       1.24999999978138668903e-02,
                       2.23219810758559851206e-03
       },
      .poly_2 = {
                       8.33333333333317923934e-02,
                       1.25000000037717509602e-02,
                       2.23213998791944806202e-03,
                       4.34887777707614552256e-04,
              },

};

#define C1 log1p_data.poly_1[0]
#define C2 log1p_data.poly_1[1]
#define C3 log1p_data.poly_1[2]

#define D1 log1p_data.poly_2[0]
#define D2 log1p_data.poly_2[1]
#define D3 log1p_data.poly_2[2]
#define D4 log1p_data.poly_2[3]

#define SIGN_MASK       log1p_data.sign_mask
#define MINUS_ONE       log1p_data.minus_one
#define EPSILON         log1p_data.epsilon
#define LN2_HEAD        log1p_data.ln2_lead
#define LN2_TAIL        log1p_data.ln2_tail
#define THRESHOLD_LOW   log1p_data.threshold_low
#define THRESHOLD_HIGH  log1p_data.threshold_high
#define ONE             log1p_data.one
#define INDEX_MASK1     log1p_data.index_mask1
#define INDEX_MASK2     log1p_data.index_mask2
#define INDEX_MASK3     log1p_data.index_mask3
#define INDEX_CONSTANT1 log1p_data.index_constant1
#define INDEX_CONSTANT2 log1p_data.index_constant2

float
ALM_PROTO_OPT(log1pf)(float x) {

    double xd, u, v, r1, r2, f, f1, f2, poly;

    int32_t exp, index;

    uint32_t ux, aux;

    uint64_t uxd, uf;

    ux = asuint32(x);

    aux = ux & SIGN_MASK;

    if(unlikely(aux >= EXPBITS_SP32)) {

        /* x ix infinity or nan */
        if(ux == POS_INF_F32) {

            return x;

        }
        /*NaN or negative infinity should return NaN here */

        if( (ux & QNANBITPATT_SP32) == QNANBITPATT_SP32) {

            return x;

        }

        return alm_logf_special(asfloat(QNANBITPATT_SP32), ALM_E_IN_X_NAN);

    }

    if(unlikely(ux >= MINUS_ONE)) {

        /* x is negative or zero */
        if(ux == MINUS_ONE) {

            return alm_logf_special(asfloat(NINFBITPATT_SP32), AMD_F_DIVBYZERO);

        }

        return alm_logf_special(asfloat(ux | QNANBITPATT_SP32), ALM_E_IN_X_NAN);

    }

    if(unlikely(aux < EPSILON)) {

        return x;

    }

    xd = (double)x;

    if((xd >= THRESHOLD_LOW) && (xd <= THRESHOLD_HIGH)) {

        u = xd / (xd + 2.0);

        double corr = xd * u;

        u = u + u;

        v = u * u;

        r1 = (u * v * (D1 + v * (D2 + v * (D3 +  v * D4)))) - corr;

        return (float)(xd + r1);

    }

    double x1 = xd + 1.0;

    uxd = asuint64(x1);

    exp = (int32_t)((uxd & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EMAX_DP64;

    index = (int32_t)((((uxd & INDEX_MASK1) | INDEX_MASK3) >> 46) + ((uxd & INDEX_MASK2) >> 45));

    f1 = (double)index * INDEX_CONSTANT1;

    index -= INDEX_CONSTANT2;

    uxd = uxd & MANTBITS_DP64;

    uf = uxd | ONE;

    f = asdouble(uf);

    if((exp <= -2) || exp >= 60) {

        f2 = f - f1;

    }
    else {

        uxd = ((uint64_t)(0x3ff - exp)) << EXPSHIFTBITS_DP64;

        double m = asdouble(uxd);

        f2 = xd * m + (m - f1);

    }

    u = f2 / (f2 * 0.5 + f1);

    v = u * u;

    poly = (v * (C1 + v * (C2 + v * C3)));

    double z = log1p_table[index].tail + (u + u * poly);

    double dexp = (double)exp;

    r1 = dexp * LN2_HEAD + log1p_table[index].lead;

    r2 = dexp * LN2_TAIL + z;

    return (float)(r1 + r2);

}

