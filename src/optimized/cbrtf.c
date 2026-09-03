/*
 * Copyright (C) 2008-2026 Advanced Micro Devices, Inc. All rights reserved.
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
 *   float cbrtf(float x)
 *
 * Algorithm:
 *   cbrtf(x) = cbrt(m * 2^n)
 *            = cbrt(2^rem * m_k) * cbrt(1 + r) * 2^quotient
 *
 * where n = expn = 3*quotient + rem, rem in {-2,-1,0,1,2},
 * m_k = 1/recip[k], k = top 8 bits of mantissa,
 * r = m * recip[k] - 1 in [-1/256, 1/256).
 *
 * CbrtfTable[rem+2] stores {recip[k], cbrt(2^rem * m_k)} interleaved
 * as uint64 bit patterns, correctly rounded to nearest double.
 * cbrt(1+r) ~ 1 + r/3 - r^2/9 + 5*r^3/81  (3-term Taylor).
 * All intermediate arithmetic is double; only the final cast rounds.
 */

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>

#include <libm_macros.h>
#include <libm/types.h>

#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <cbrtf_data.h>

float
ALM_PROTO_OPT(cbrtf)(float x)
{
    uint32_t ix  = asuint32(x);
    uint32_t ixe = (EXPBITS_SP32 & ix) >> EXPSHIFTBITS_SP32;
    uint32_t ixm = MANTBITS_SP32 & ix;
    int32_t expn = (int32_t)ixe - EXPBIAS_SP32;

    if (unlikely(ixe - 1 >= BIASEDEMAX_SP32)) {
        // Zero, Inf, or NaN: x+x quiets sNaN and raises FE_INVALID
        if (unlikely((ix & POS_BITSET_F32) - 1 >= POS_INF_F32 - 1))
            return x + x;
        // Subnormal: multiply by 2^24 (= 8^8, a multiple of 3) to normalize
        // Subtract 24 from expn to compensate
        ix   = asuint32(x * 0x1p24f);
        ixe  = (EXPBITS_SP32 & ix) >> EXPSHIFTBITS_SP32;
        ixm  = MANTBITS_SP32 & ix;
        expn = (int32_t)ixe - EXPBIAS_SP32 - 24;
    }

    int32_t quotient = expn / 3;
    int32_t rem      = expn - quotient * 3;

    // 8-bit table index from top 8 bits of the 23-bit mantissa
    uint32_t k     = ixm >> 15;
    uint32_t tidx  = k * 2;

    // Select the table row for this remainder; load recip and cbrt(2^rem*m_k)
    const uint64_t *row = CbrtfTable[rem + 2];
    double recip = asdouble(row[tidx]);
    double cbrtEntry = asdouble(row[tidx + 1]);

    // Scale = 2^quotient, built from integer quotient
    float  scale_f = asfloat((uint32_t)(quotient + EXPBIAS_SP32) << EXPSHIFTBITS_SP32);

    // Mantissa m in [1, 2) as double
    double m = asdouble((uint64_t)ixm << 29 | 0x3FF0000000000000ULL);

    // r = m * recip - 1, small residual in [-1/256, 1/256)
    double r  = fma(m, recip, -1.0);

    // 3-term Taylor: cbrt(1+r) - 1 ~= r/3 - r^2/9 + 5*r^3/81
    double r2 = r * r;
    double t  = r * fma(r, -0x1.c71c71c71c71cp-4, 0x1.5555555555555p-2);
    t = fma(r2 * r, 0x1.f9add3c0ca458p-5, t);

    // Result = cbrtEntry * (1 + t) * 2^quotient
    double result = fma(t, cbrtEntry, cbrtEntry) * scale_f;

    return copysignf((float)result, x);
}
