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

#define EMAX_SP32_VAL  0x1p127f
#define EMIN_SP32_VAL  0x1p-126f
#define LDEXPF_MIN_EXP -26
#define LDEXPF_MAX_EXP 23

/*
 * ldexpf(x, exp) = x * 2^exp
 *
 * Fast path: when exp is in [EMIN_SP32, EMAX_SP32], construct 2^exp as
 * (EXPBIAS_SP32 + exp) << EXPSHIFTBITS_SP32.  IEEE 754 multiplication
 * correctly handles x = 0, +-Inf, NaN, and subnormal x.
 *
 * Else path: exp outside [EMIN_SP32, EMAX_SP32].  Peel off up to two
 * EMAX_SP32 or EMIN_SP32 steps by multiplying by 2^EMAX or 2^EMIN, then
 * finish with the remaining exp in the fast-path range.  This handles
 * exp in [-278, 277].
 */

float ALM_PROTO_REF(ldexpf)(float x, int exp)
{
    // Unlikely branch only taken if exp is outside of [EMIN_SP32, EMAX_SP32]
    if (unlikely((unsigned)(exp - EMIN_SP32) >= BIASEDEMAX_SP32)) {
        if (exp >= 0) {
            // exp > 127; multiply x by 2^127 and subtract 127 from exp
            x *= EMAX_SP32_VAL;
            exp -= EMAX_SP32;

            if (unlikely(exp > EMAX_SP32)) {
                // exp > 127; multiply x by 2^127 and subtract 127 from exp
                x *= EMAX_SP32_VAL;
                exp -= EMAX_SP32;

                // If remaining exp > 23, clip exp at 23 since exp >= 23 is
                // sufficient to cause overflow and raise FE_OVERFLOW.
                if (exp > LDEXPF_MAX_EXP) {
                    exp = LDEXPF_MAX_EXP;
                }
            }
        } else {
            // exp < -126; multiply x by 2^-126 and add 126 to exp
            x *= EMIN_SP32_VAL;
            exp -= EMIN_SP32;

            if (unlikely(exp < EMIN_SP32)) {
                // exp < -126; multiply x by 2^-126 and add 126 to exp
                x *= EMIN_SP32_VAL;
                exp -= EMIN_SP32;

                // If remaining exp < -26, clip exp at -26 since exp <= -26
                // is sufficient to cause underflow and raise FE_UNDERFLOW.
                if (exp < LDEXPF_MIN_EXP) {
                    exp = LDEXPF_MIN_EXP;
                }
            }
        }
    }

    // exp is in [EMIN_SP32, EMAX_SP32]. Compute x * 2^exp.
    x *= asfloat((uint32_t)(exp + EXPBIAS_SP32) << EXPSHIFTBITS_SP32);

    return x;
}
