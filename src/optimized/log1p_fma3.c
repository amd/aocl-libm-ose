/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
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
 *     Compute log1p(x) = log(1 + x), FMA3 entry point __amd_fma3_log1p.
 *
 *     This is a portable C re-implementation of the scalar double-precision
 *     log1p routine that was previously hand-written in assembly
 *     (isa/avx2/gas/log1p_fma3.S, isa/avx2/masm/log1p_fma3.asm). The assembly
 *     was authored for the SysV ABI where every XMM register is caller-saved
 *     and therefore clobbered the caller's non-volatile XMM6-XMM15 on the
 *     Windows x64 ABI (github amd/aocl-libm-ose issue #33). Compiling the
 *     routine as C makes the toolchain honour each platform's calling
 *     convention automatically while preserving the original algorithm and
 *     data tables, so accuracy is unchanged.
 *
 *     Algorithm:
 *
 *     if(exp(-1/16)-1 < x < exp(1/16)-1)
 *         u = x / (x + 2.0);
 *         corr = x * u;
 *         log(1 + x) = x + (2 * u * u * u * (D1 + u*u*(D2 + u*u*(D3 + u*u*D4)))) - corr;
 *     else
 *         x = x + 1
 *         x = 2^m(F + f)
 *         u = f / (F * 0.5 + f);
 *         v = u * u;
 *         log(1+x) = m * log(2) + log(F) + log(v);
 */

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/poly.h>

#include "log1p_data.h"

static const struct {
    uint64_t sign_mask, minus_one, epsilon;
    uint64_t index_mask1,index_mask2, index_mask3, one;
    double threshold_low, threshold_high;
    double index_constant1;
    int32_t index_constant2;
    double ln2_lead, ln2_tail;

    double ALIGN(16) poly_1[3];
    double ALIGN(16) poly_2[4];
} log1p_fma3_data = {
      .sign_mask = 0x7fffffffffffffff,
      .minus_one = 0xBFF0000000000000U,
      .epsilon = 0x3ca0000000000000U,
      .index_mask1 = 0x000fc00000000000U,
      .index_mask2 = 0x0000200000000000U,
      .index_mask3 = 0x0010000000000000U,
      .one = 0x3ff0000000000000,
      .threshold_low = -0x1.f0540438fd5c4p-5,
      .threshold_high = 0x1.082b577d34ed8p-4,
      .index_constant1 = 0x1p-6,
      .index_constant2 = 64,
      .ln2_lead = 0x1.62e42e0000000p-1,
      .ln2_tail = 0x1.efa39ef35793cp-25,
      .poly_1 = {
                       0x1.5555555555557p-4,
                       0x1.9999999865edep-7,
                       0x1.249423bd94741p-9
       },
      .poly_2 = {
                       0x1.55555555554e6p-4,
                       0x1.9999999bac6d4p-7,
                       0x1.2492307f1519fp-9,
                       0x1.c8034c85dfffp-12,
              },

};

#define C1 log1p_fma3_data.poly_1[0]
#define C2 log1p_fma3_data.poly_1[1]
#define C3 log1p_fma3_data.poly_1[2]

#define D1 log1p_fma3_data.poly_2[0]
#define D2 log1p_fma3_data.poly_2[1]
#define D3 log1p_fma3_data.poly_2[2]
#define D4 log1p_fma3_data.poly_2[3]

#define SIGN_MASK       log1p_fma3_data.sign_mask
#define MINUS_ONE       log1p_fma3_data.minus_one
#define EPSILON         log1p_fma3_data.epsilon
#define LN2_HEAD        log1p_fma3_data.ln2_lead
#define LN2_TAIL        log1p_fma3_data.ln2_tail
#define THRESHOLD_LOW   log1p_fma3_data.threshold_low
#define THRESHOLD_HIGH  log1p_fma3_data.threshold_high
#define ONE             log1p_fma3_data.one
#define INDEX_MASK1     log1p_fma3_data.index_mask1
#define INDEX_MASK2     log1p_fma3_data.index_mask2
#define INDEX_MASK3     log1p_fma3_data.index_mask3
#define INDEX_CONSTANT1 log1p_fma3_data.index_constant1
#define INDEX_CONSTANT2 log1p_fma3_data.index_constant2

double
ALM_PROTO_FMA3(log1p)(double x) {

    double u, v, r1, r2, f, f1, f2, poly;

    int32_t exp, index;

    uint64_t ux, aux, ux1, uf;

    ux = asuint64(x);

    aux = ux & SIGN_MASK;

    if(unlikely(aux >= EXPBITS_DP64)) {

        /* x is infinity or NaN */
        if(ux  == POS_INF_F64) {

            return x;

        }
        /* NaN or -inf: return a quiet NaN via alm_log_special. */
        if(ux > POS_INF_F64) {

            return alm_log_special(asdouble(ux | QNANBITPATT_DP64), ALM_E_IN_X_NAN);

        }

    }

    if(unlikely(ux >= MINUS_ONE)) {

        /* x <= -1.0 (bitwise compare against the encoding of -1.0) */
        if(ux == MINUS_ONE) {

            return alm_log_special(asdouble(NINFBITPATT_DP64), AMD_F_DIVBYZERO);

        }

        return alm_log_special(asdouble(ux | QNANBITPATT_DP64), ALM_E_IN_X_NAN);

    }

    if(unlikely(aux < EPSILON)) {

        return x;

    }

    if((x >= THRESHOLD_LOW) && (x <= THRESHOLD_HIGH)) {

        u = x / (x + 2.0);

        double corr = x * u;

        u = u + u;

        v = u * u;

        r1 = (u * v * POLY_EVAL_HORNER_4(v, D1, D2, D3, D4)) - corr;

        return x + r1;

    }

    double x1 = x + 1.0;

    ux1 = asuint64(x1);

    exp = (int32_t)((ux1 & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EMAX_DP64;

    index = (int32_t)((((ux1 & INDEX_MASK1) | INDEX_MASK3) >> 46) + ((ux1 & INDEX_MASK2) >> 45));

    f1 = (double)index * INDEX_CONSTANT1;

    index -= INDEX_CONSTANT2;

    ux1 = ux1 & MANTBITS_DP64;

    uf = ux1 | ONE;

    f = asdouble(uf);

    if((exp <= -2) || exp >= 60) {

        f2 = f - f1;

    }
    else {

        ux = ((uint64_t)(0x3ff - exp)) << EXPSHIFTBITS_DP64;

        double m = asdouble(ux);

        f2 = x * m + (m - f1);

    }

    u = f2 / (f2 * 0.5 + f1);

    v = u * u;

    poly = (v * (C1 + v * (C2 + v * C3)));

    double z = log1p_table[index].tail + (u + u * poly);

    double dexp = (double)exp;

    r1 = dexp * LN2_HEAD + log1p_table[index].lead;

    r2 = dexp * LN2_TAIL + z;

    return r1 + r2;

}
