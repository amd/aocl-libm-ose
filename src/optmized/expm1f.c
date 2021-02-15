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

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm_special.h>

#if !defined(DEBUG) && defined(__GNUC__) && !defined(__clang__)
#pragma GCC push_options
#pragma GCC optimize ("O3")
#endif  /* !DEBUG */

#include "expm1f_data.h"

#define DATA expm1f_v2_data

/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   float expm1f(float x)
 *
 * Description:
 *        Calculates closest approximation to (exp(x) - 1); where, exp(x) = e^x.
 *
 * Spec:
 *   expm1f(x)
 *          = expm1f(x)         if x ∈ F and |x| >= FLT_MIN
 *          = x                 if x ∈ F, and |x| < FLT_MIN
 *          = +/-x              if x = {-0, 0}, preserve sign
 *          = -0                if x = -0
 *          = -1                if x = -inf
 *          = +inf              if x = +inf
 *          = NaN               otherwise
 *
 *    expm1f(x) will overflow approximately when x > ln(FLT_MAX)
 *
 * Implementation Notes:
 *
 */
float expm1f_special(float x, float y, U32 code);

#define FTHRESH_LO -0x1.269622p-2
#define FTHRESH_HI  0x1.c8ff7ep-3f

#define THRESH_LO  0xBE934B11U	/* log(1 - 1/4) = -0.287682 */
#define THRESH_HI  0x3E647FBFU	/* log(1 + 1/4) =  0.223144 */
/* HI - LO = 0x7fd134ae */
/* LO - HI = 0x802ecb52 */

#define ARG_MIN 0xC18AA122U              /* ~= -17.32867 */
#define ARG_MAX 0x42B19999U              /* ~=  88.79999 */
/* MIN - MAX =  0x7ed9078a */
/* MAX - MIN =  0x8126f877 */

#define FARG_MIN -0x1.154244p+4f
#define FARG_MAX  0x1.633332p+6f

#define THRESH_LO_NOSIGN 0x3E9e4B11U
#define ARG_MIN_NOSIGN   0x418AA122U


/* this macro may be deleted later*/
float	FN_PROTOTYPE_OPT(expm1f_v2)	(float x);

float
FN_PROTOTYPE_OPT(expm1f_v2)(float x)
{
    flt64_t q1;
    double  dx, dn, q, r, f;
    int     j, n, m;

    if (unlikely (x <= DATA.x.min || x > DATA.x.max)) {

        if (x > DATA.x.max)
            return asfloat(PINFBITPATT_SP32);

        if (x < DATA.x.min)
            return -1.0;

        return asfloat(QNANBITPATT_SP32);
    }

    /*
     * Treat the near 0 values separately to avoid catastrophic
     * cancellation
     */
    if (unlikely (x <= FTHRESH_HI && (double)x >= FTHRESH_LO)) {
        double dx2;

#define A1 DATA.poly[0]
#define A2 DATA.poly[1]
#define A3 DATA.poly[2]
#define A4 DATA.poly[3]
#define A5 DATA.poly[4]

        dx  = (double)x;
        dx2 = dx * dx;
        q   = dx2 * dx * ((double)A1 + dx * ((double)A2
                        + dx*((double)A3 + dx*((double)A4
                        + dx*((double)A5)))));
        q  += (dx2 * 0.5) + dx;
        return (float)q;
    }

    dx  = eval_as_double((double)x * DATA._64_by_ln2);

#define FAST_INTEGER_CONVERSION 1
#if FAST_INTEGER_CONVERSION
    q   = eval_as_double(dx + DATA.Huge);
    n   = (int)asuint64(q);
    dn  = q - DATA.Huge;
#else
    n   = cast_float_to_i32(dx);
    dn  = cast_i32_to_float(n);
#endif

    r  = (double)x - dn * DATA.ln2_by_64;

    j  = n & 0x3f;

    m  = (n - j) >> 6;

#define C1 1/2.0
#define C2 1/6.0
    q  = r + r * r * (C1 + (C2 * r));

    f  = DATA.tab[j];

    q1.i = (int64_t)((1023ULL - (unsigned long long)m) << 52);

    q1.d = f * q + (f - q1.d);

    j    = n >> EXPM1F_N;

    q = asdouble((uint64_t)q1.i + ((uint64_t)j << 52));

    return (float)q;
}

#if !defined(DEBUG) && defined(__GNUC__) && !defined(__clang__)
#pragma GCC pop_options
#endif
