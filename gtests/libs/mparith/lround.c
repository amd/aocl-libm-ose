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


#include "precision.h"


#if defined(FLOAT)
#define FUNC_LROUND alm_mp_lroundf

#elif defined(DOUBLE)
#define FUNC_LROUND alm_mp_lround

#else
#error
#endif
/*
| Rounding Mode             | Macro         | Description                                                  |
|---------------------------|---------------|--------------------------------------------------------------|
| Round to nearest (even)   | `MPFR_RNDN`   | Rounds to the nearest value; ties go to the even number.     |
| Round toward zero         | `MPFR_RNDZ`   | Truncates fractional part; rounds toward zero.               |
| Round toward +∞           | `MPFR_RNDU`   | Always rounds upward (toward positive infinity).             |
| Round toward −∞           | `MPFR_RNDD`   | Always rounds downward (toward negative infinity).           |
| Round away from zero      | `MPFR_RNDA`   | Rounds away from zero; halfway cases go to greater magnitude.|
| Faithful rounding         | `MPFR_RNDF`   | Returns one of the two nearest values; no guarantee which.   |
| Nearest, away from zero   | `MPFR_RNDNA`  | Rounds to nearest; halfway cases go away from zero.          |
|---------------------------|---------------|--------------------------------------------------------------|
*/

/*
 * MPFR oracle for lround()/lroundf() -- the IEEE-754 convertToIntegerTiesToAway
 * operation with a 'long int' destination.
 *
 *   - Round to nearest, halfway cases away from zero, independent of the
 *     current rounding mode.  MPFR_RNDNA does exactly this and never consults
 *     the hardware rounding mode, so the oracle is rounding-mode independent.
 *   - FE_INEXACT is never raised: caller flags are snapshotted before any MPFR
 *     work and restored afterwards, so MPFR's internal FP arithmetic cannot
 *     leak a spurious inexact/underflow.  Only FE_INVALID may be added.
 *   - NaN, +-Inf, or a rounded result outside [LONG_MIN, LONG_MAX] raise
 *     FE_INVALID and return an unspecified value; we return the x86/glibc
 *     "integer indefinite" (most-negative long) for every such case and either
 *     sign -- e.g. lround(LONG_MAX + 1.5) == LONG_MIN.
 *
 * mpfr_fits_slong_p() is the exact range oracle: false for NaN/+-Inf and any
 * out-of-range value, and -- crucially -- true for a negative input that rounds
 * to exactly LONG_MIN, which is in range and must not raise (matching glibc's
 * 'x > (double)LONG_MIN - 0.5' boundary on 32-bit long).
 *
 * The result is returned directly as 'lint_t' (long int); the test harness
 * compares it against the integer the implementation returns.
 */

#include <fenv.h>
#include <limits.h>
#include <mpfr.h>

lint_t FUNC_LROUND(REAL x)
{
    lint_t y;
    long si = 0;
    int fits;
    fexcept_t saved_flags;

    mpfr_t mpx;

    /*
     * Snapshot the caller's FP exception flags so MPFR's internal arithmetic
     * cannot leak a spurious FE_INEXACT/underflow: lround may add FE_INVALID
     * and nothing else.
     */
    fegetexceptflag(&saved_flags, FE_ALL_EXCEPT);

    mpfr_init2(mpx, ALM_MP_PRECI_BITS);

    /*
     * float -> double is exact, and double stays double, so mpfr_set_d loads
     * the operand exactly.  Using double (not long double) keeps behaviour
     * identical across the Linux 80-bit vs Windows 64-bit long double split.
     */
    mpfr_set_d(mpx, (double)x, MPFR_RNDN);

    /* IEEE-754 roundTiesToAway, independent of the FPU rounding mode. */
    mpfr_rint(mpx, mpx, MPFR_RNDNA);

    /* Exact range check: false for NaN/+-Inf/out-of-range; true at LONG_MIN. */
    fits = mpfr_fits_slong_p(mpx, MPFR_RNDZ);
    if (fits)
        si = mpfr_get_si(mpx, MPFR_RNDZ);

    mpfr_clear(mpx);

    /* Drop any flags MPFR raised internally, leaving the caller's untouched. */
    fesetexceptflag(&saved_flags, FE_ALL_EXCEPT);

    if (fits) {
        y = (lint_t)si;
    } else {
        /* Invalid/out-of-range: raise FE_INVALID, return integer indefinite. */
        feraiseexcept(FE_INVALID);
        y = (lint_t)LONG_MIN;
    }

    return y;
}
