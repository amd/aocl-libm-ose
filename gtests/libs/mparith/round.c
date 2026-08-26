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


#include "precision.h"


#if defined(FLOAT)
#define FUNC_ROUND alm_mp_roundf

#elif defined(DOUBLE)
#define FUNC_ROUND alm_mp_round

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

#include <mpfr.h>

void FUNC_ROUND(REAL x, mpfr_t result)
{

    mpfr_rnd_t rnd = MPFR_RNDNA;
    mpfr_t mpx, mp_rop;

    mpfr_inits2(ALM_MP_PRECI_BITS, mpx, mp_rop, (mpfr_ptr) 0);

#if defined(FLOAT)
    mpfr_set_flt(mpx, x, rnd);
#else
    mpfr_set_d(mpx, x, rnd);
#endif

    mpfr_round(mp_rop, mpx);

    mpfr_set(result, mp_rop, rnd);

    mpfr_clears(mpx, mp_rop, (mpfr_ptr) 0);

}

