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
 *   double cbrt(double x)
 *
 * Algorithm:
 *   cbrt(x) = cbrt(2^n * m) = 2^(n/3) * cbrt(m)  where m in [0.5, 1)
 *           = 2^quotient * 2^(rem/3) * cbrt(F_k) * (1+r)^(1/3)
 *
 * F_k is the 9-bit table center for the mantissa (InverseTable).
 * CbrtTable stores cbrt(2^rem * F_k) as head+tail (true double-double,
 * full 52-bit precision heads) for rem in {-2,-1,0,1,2}.
 * This eliminates the runtime cbrt(F_k) * cbrt(2^rem) multiplication.
 *
 */

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <immintrin.h>

#include <libm_macros.h>
#include <libm/types.h>

#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/alm_special.h>
#include <cbrt_data.h>

#define CBRT_EXP_COEFF_1    3.33333333333333314829616256247E-1  // 0x3fd5555555555555
#define CBRT_EXP_COEFF_2   -1.11111111111111104943205418749E-1  // 0xbfbc71c71c71c71c
#define CBRT_EXP_COEFF_3    6.17283950617283916351141215273E-2  // 0x3faf9add3c0ca458
#define CBRT_EXP_COEFF_4   -4.11522633744855967363740489873E-2  // 0xbfa511e8d2b3183b
#define CBRT_EXP_COEFF_5    3.01783264746227734842687340233E-2  // 0x3f9ee7113506ac13
#define CBRT_EXP_COEFF_6   -2.34720317024843770636888251602E-2  // 0xbf98090d6221a247

double
ALM_PROTO_OPT(cbrt)(double x) {
    uint64_t ix  = asuint64(x);
    uint64_t ixe = (EXPBITS_DP64 & ix) >> EXPSHIFTBITS_DP64;
    uint64_t ixm = MANTBITS_DP64 & ix;
    int64_t expn = (int64_t)ixe - EXPBIAS_DP64;

    if (unlikely(ixe - 1 >= BIASEDEMAX_DP64)) {
        if (unlikely((ixe != 0) || (ixm == 0))) {
            return x + x; // Zero, Inf or NaN: raise FE_INVALID for sNaN
        }
        // Subnormal: multiply by 2^54 (a multiple of 3) to normalize
        // Subtract 54 from expn to compensate
        ix   = asuint64(x * 0x1p54);
        ixe  = (ix & EXPBITS_DP64) >> EXPSHIFTBITS_DP64;
        ixm  = ix & MANTBITS_DP64;
        expn = (int64_t)ixe - EXPBIAS_DP64 - 54;
    }

    // Signed divide-by-3 via multiply-shift (faster than idiv for bounded expn)
    // 0x55555556 ~= 2^32/3; high 32 bits of signed product = floor(expn/3)
    // Subtract (expn >> 63) to convert floor to C truncation-toward-zero
    int64_t quotient = ((expn * 0x55555556LL) >> 32) - (expn >> 63);
    int64_t rem      = expn - quotient * 3;

    // Build scale = 2^quotient as a double via integer shift
    // Adding EXPBIAS_DP64 to the 32-bit quotient before shifting avoids
    // a movabsq for the 64-bit constant 0x3FF0000000000000.
    uint64_t quotientScale = (uint64_t)(quotient + EXPBIAS_DP64) << EXPSHIFTBITS_DP64;

    // Reduced mantissa in [0.5, 1)
    double rdu_d = asdouble(ixm | HALFEXPBITS_DP64);

    // 9-bit table index: upper 9 mantissa bits, rounded to nearest
    // Bit 43 is the rounding bit; bits 44..52 are the index base
    uint64_t mant_idx = ((ixm >> 43) & 1) + ((ixm >> 44) | 0x100);

    // Issue table loads before the polynomial to hide cache-miss latency
    // CbrtTable[rem+2] stores cbrt(2^rem * F_k) as tail+head pairs,
    // with full 52-bit precision heads (true double-double).
    uint64_t tidx = (mant_idx - 256) << 1;
    double bT = asdouble(CbrtTable[rem + 2][tidx]);
    double bH = asdouble(CbrtTable[rem + 2][tidx + 1]);

    // Convert mant_idx to double without vcvtsi2sd via the OR-subtract trick
    double midx_d = asdouble(mant_idx | EXP_VAL_52_DP64);
    double mant_d = asdouble(InverseTable[mant_idx - 256]);
    double      r = mant_d * (rdu_d - (midx_d - 0x1p52) * 0x1p-9);

    double r2 = r * r;
    double r3 = r2 * r;
    double r4 = r2 * r2;
    double r5 = r4 * r;
    double r6 = r3 * r3;

    double poly = CBRT_EXP_COEFF_1 * r;
    poly += CBRT_EXP_COEFF_2 * r2;
    poly += CBRT_EXP_COEFF_3 * r3;
    poly += CBRT_EXP_COEFF_4 * r4;
    poly += CBRT_EXP_COEFF_5 * r5;
    poly += CBRT_EXP_COEFF_6 * r6;

    double ans = (poly * bT) + bT + (poly * bH) + bH;

    return copysign(ans * asdouble(quotientScale), x);
}
