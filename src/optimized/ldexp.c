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

#include "fn_macros.h"
#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>

/*
 * double ldexp(double x, int n)
 *
 * Computes x * 2^n purely by adjusting the biased exponent of x, so the
 * result is always correctly rounded (ldexp is exact except on overflow /
 * underflow, where the sub-/super-normal scaling multiply rounds once).
 *
 * Special values (match glibc / C99 F.10.3.10):
 *   - x = +-0            -> x         (sign preserved)
 *   - n = 0              -> x         (exact no-op)
 *   - x = +-Inf          -> x
 *   - x = quiet NaN      -> x
 *   - x = signaling NaN  -> quiet NaN, FE_INVALID raised
 *   - overflow           -> +-Inf, raises FE_OVERFLOW | FE_INEXACT
 *   - underflow          -> subnormal or +-0, raises FE_UNDERFLOW | FE_INEXACT
 *
 * IEEE 754 double: x = (-1)^s * 2^(E - 1023) * 1.f,  E = biased exponent.
 * Let e = E + n be the target biased exponent:
 *   - e >  2046 : overflow  -> +-Inf
 *   - 1 <= e    : normal    -> write e into the exponent field
 *   - -52 <= e  : subnormal -> write (e + 53), then multiply by 2^-53 so the
 *                             hardware performs the single correct rounding
 *   - e <= -53  : underflow -> +-0
 * A subnormal input is first normalised (x * 2^53), which shifts its biased
 * exponent by -53; this is folded into 'base' so the tail is shared.
 */

/*
 * |n| beyond this always saturates regardless of x: the double biased
 * exponent spans [0, 2046] and subnormal normalisation adds at most 53, so
 * no finite non-underflowing result exists once |n| exceeds ~2099.  Clamping
 * here also guarantees 'base + n' below cannot overflow 'int'.
 */
#define LDEXP_SATURATE_N    (BIASEDEMAX_DP64 + MULTIPLIER_DP)

double ALM_PROTO_OPT(ldexp)(double x, int n)
{
    uint64_t ux  = asuint64(x);
    uint64_t uax = ux & ~SIGNBIT_DP64;          /* |x| bit pattern */

    /* Cold path: x is NaN or +-Inf (exponent field all ones). */
    if (unlikely(uax >= PINFBITPATT_DP64)) {
        if (uax == PINFBITPATT_DP64)            /* +-Inf */
            return x;
        /* signaling NaN -> quiet NaN + FE_INVALID */
        if (!(uax & QNAN_MASK_64))
            return __alm_handle_error(ux | QNAN_MASK_64, AMD_F_INVALID);
        return x;                               /* quiet NaN */
    }

    /* x == +-0, or nothing to do: exact, sign-preserving no-op. */
    if (uax == 0 || n == 0)
        return x;

    uint64_t sign = ux & SIGNBIT_DP64;

    if (unlikely(n > LDEXP_SATURATE_N))         /* certain overflow  -> +-Inf */
        return __alm_handle_error(sign | PINFBITPATT_DP64,
                                  AMD_F_INEXACT | AMD_F_OVERFLOW);
    if (unlikely(n < -LDEXP_SATURATE_N))        /* certain underflow -> +-0   */
        return __alm_handle_error(sign, AMD_F_INEXACT | AMD_F_UNDERFLOW);

    /* biased exponent [0 .. 2046] */
    int base = (int)(uax >> EXPSHIFTBITS_DP64);

    if (unlikely(base == 0)) {                  /* subnormal input: normalise */
        uax  = asuint64(asdouble(uax) * VAL_2PMULTIPLIER_DP); /* x * 2^53 */
        base = (int)(uax >> EXPSHIFTBITS_DP64) - MULTIPLIER_DP;
    }

    int e = base + n;                           /* target biased exponent */

    if (unlikely(e > BIASEDEMAX_DP64))          /* overflow  -> +-Inf */
        return __alm_handle_error(sign | PINFBITPATT_DP64,
                                  AMD_F_INEXACT | AMD_F_OVERFLOW);

    if (unlikely(e <= -MULTIPLIER_DP))          /* underflow -> +-0   */
        return __alm_handle_error(sign, AMD_F_INEXACT | AMD_F_UNDERFLOW);

    /*
     * Normal result (e >= 1): write e into the exponent field.  Subnormal
     * result (e < 1): write e + 53 and multiply by 2^-53 for a single correct
     * rounding (may raise FE_UNDERFLOW/FE_INEXACT).  The rare e < 1 branch is
     * cheaper than an always-on branchless post-scale multiply.
     */
    uint64_t r = sign | (uax & MANTBITS_DP64);
    if (e < 1) {                                /* subnormal result */
        r |= ((uint64_t)(e + MULTIPLIER_DP) << EXPSHIFTBITS_DP64);
        return asdouble(r) * VAL_2PMMULTIPLIER_DP; /* * 2^-53 (rounds once) */
    }

    /* normal input, normal result */
    r |= ((uint64_t)e << EXPSHIFTBITS_DP64);
    return asdouble(r);
}
