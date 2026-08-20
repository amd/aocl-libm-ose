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

#include "fn_macros.h"
#include "libm_util_amd.h"
#include <libm/amd_funcs_internal.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>

#define EMAX_DP64_VAL 0x1p1023
#define EMIN_DP64_VAL 0x1p-1022
#define LDEXP_MIN_EXP -55
#define LDEXP_MAX_EXP 52

/*
 * ldexp(x, exp) = x * 2^exp
 *
 * Fast path: when exp is in [EMIN_DP64, EMAX_DP64], construct 2^exp as
 * (EXPBIAS_DP64 + exp) << EXPSHIFTBITS_DP64.  Using EXPBIAS_DP64 + exp
 * directly avoids a 64-bit immediate (movabsq).  IEEE 754 multiplication
 * correctly handles x = 0, +-Inf, NaN, and subnormal x, and naturally
 * raises FE_OVERFLOW or FE_UNDERFLOW.
 *
 * Else path: exp outside [EMIN_DP64, EMAX_DP64].  Peel off up to two
 * EMAX_DP64 or EMIN_DP64 steps by multiplying by 2^EMAX or 2^EMIN, then
 * finish with the remaining exp in the fast-path range.  This handles
 * exp in [-(BIASEDEMAX_DP64 + EXPSHIFTBITS_DP64 + 1),
 *          BIASEDEMAX_DP64 + EXPSHIFTBITS_DP64]
 * = [-2099, 2098], covering all inputs that don't overflow or underflow
 * to infinity/zero before the final multiply.
 */
double ALM_PROTO_REF(ldexp)(double x, int exp)
{
    // Unlikely branch only taken if exp is outside of [EMIN_DP64, EMAX_DP64]
    if (unlikely((unsigned)(exp - EMIN_DP64) >= BIASEDEMAX_DP64)) {
        if (exp >= 0) {
            // exp > 1023; multiply x by 2^1023 and subtract 1023 from exp
            x *= EMAX_DP64_VAL;
            exp -= EMAX_DP64;

            if (unlikely(exp > EMAX_DP64)) {
                // exp > 1023; multiply x by 2^1023 and subtract 1023 from exp
                x *= EMAX_DP64_VAL;
                exp -= EMAX_DP64;

                // If remaining exp > 52, clip exp at 52 since exp >= 52 is
                // sufficient to cause overflow and raise FE_OVERFLOW.
                if (exp > LDEXP_MAX_EXP) {
                    exp = LDEXP_MAX_EXP;
                }
            }
        } else {
            // exp < -1022; multiply x by 2^-1022 and add 1022 to exp
            x *= EMIN_DP64_VAL;
            exp -= EMIN_DP64;

            if (unlikely(exp < EMIN_DP64)) {
                // exp < -1022; multiply x by 2^-1022 and add 1022 to exp
                x *= EMIN_DP64_VAL;
                exp -= EMIN_DP64;

                // If remaining exp < -55, clip exp at -55 since exp <= -55
                // is sufficient to cause underflow and raise FE_UNDERFLOW.
                if (exp < LDEXP_MIN_EXP) {
                    exp = LDEXP_MIN_EXP;
                }
            }
        }
    }

    // exp is in [EMIN_DP64, EMAX_DP64]. Compute x * 2^exp.
    x *= asdouble((uint64_t)(exp + EXPBIAS_DP64) << EXPSHIFTBITS_DP64);

    return x;
}
