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


/*
 * ISO-IEC-10967-2: Elementary Numerical Functions
 * Signature:
 *   float exp2f(float x)
 *
 * Spec:
 *   exp2f(1) = 2
 *   exp2f(x) = 1           if x ∈ F and exp2f(x) ≠ 2^x and
 *         log2(1 - (epsilon/(2 * r))) < x  and
 *                         x < log2(1+(epsilon/2)
 *   exp2f(x) = 1           if x = -0
 *   exp2f(x) = +inf        if x = +inf
 *   exp2f(x) = 0           if x = -inf
 *   exp2f(x) = 2^x
 *
 *   exp2f(x) overflows     if (approximately) x > log2(FLT_MAX)
 */

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>

#include <libm_macros.h>
#include <libm/types.h>

#include <libm/typehelper.h>
#include <libm/compiler.h>


#define EXP2F_N             6
#define EXP2F_TABLE_SIZE    (1 << EXP2F_N)

extern double __two_to_jby64[EXP2F_TABLE_SIZE];

static
struct expf_data {
    double        oneby_tblsz;
    double        ln2;
    double        huge;
    double        *table;
    double        poly[3];
    double        two_pow_6;
} exp2f_data = {
    .oneby_tblsz  = 0x1.0p-6,
    .ln2          = 0x1.62e42fefa39efp-1, /* log(2) */
    .huge         = 0x1.8p+52 / EXP2F_TABLE_SIZE,
    .two_pow_6    = 0x1.0p6,
    .table        = __two_to_jby64,
    .poly         = {
        0x1.0000000000000p-1,           /* 1/2! = 1/2    */
        0x1.5555555555555p-3,           /* 1/3! = 1/6    */
        0x1.5555555555555p-5,           /* 1/4! = 1/24   */
    },
};

#define EXP2F_ONEBY_TBLSZ   exp2f_data.oneby_tblsz
#define EXP2F_LN2           exp2f_data.ln2
#define EXP2F_HUGE          exp2f_data.huge
#define EXP2F_TABLE         exp2f_data.table
#define EXP2F_TWO_POW_6     exp2f_data.two_pow_6

#define EXP2F_FARG_MIN          -0x1.2ap7f
#define EXP2F_FARG_MAX           0x1.00p7f

#define C2    exp2f_data.poly[0]
#define C3    exp2f_data.poly[1]
#define C4    exp2f_data.poly[2]

static inline uint32_t
top12f(float x)
{
    flt32_t f = {.f = x};
    return f.u >> 20;
}

/*
 * Implementation Notes
 * ----------------------
 * 0. Choosing N = 64
 *
 * 1. Argument Reduction:
 *      2^(x)   =   2^(x*N/N)                --- (1)
 *
 *      Choose 'n' and 'f', such that
 *      x * N = n + f                        --- (2)
 *                | n is integer and |f| <= 0.5
 *
 *     Choose 'm' and 'j' such that,
 *      n = (N * m) + j                      --- (3)
 *
 *     From (1), (2) and (3),
 *      2^x = 2^((N*m + j + f)/N)
 *          = (2^m) * (2^(j/N)) * 2^(f/N)
 *
 *
 * 2. Table Lookup
 *      Values of (2^(j/N)) are precomputed, j = 0, 1, 2, 3 ... 63
 *
 * 3. Polynomial Evaluation
 *   From (2),
 *     f  = x*N - n
 *   Let,
 *     r  = f/N = (x*N - n)/N == x - n/N
 *
 *   Taylor series only exist for e^x
 *     2^r = e^(r*ln2)
 *
 * 4. Reconstruction
 *      Thus,
 *        2^x = (2^m) * (2^(j/N)) * 2^r
 *
 */
float
ALM_PROTO_OPT(exp2f)(float x)
{
    double      q, dn, r;
    uint64_t    n, j, m;

    uint32_t top = top12f(x) & 0x7ff;

    if (unlikely (top > top12f(128.0f))) {
        if(x != x) /* check if x is a NAN */
            return x;

        if (asuint32(x) == asuint32(-INFINITY))
            return 0.0f;

        if (x > EXP2F_FARG_MAX) {
            if(asuint32(x) == PINFBITPATT_SP32)
                return asfloat(PINFBITPATT_SP32);

            /* Raise FE_OVERFLOW, FE_INEXACT */
            return alm_exp2f_special(x, asfloat(PINFBITPATT_SP32), ALM_E_OUT_INF);
        }

        if (x < EXP2F_FARG_MIN) {
            return alm_exp2f_special(x, 0.0, ALM_E_OUT_ZERO);
        }
    }

    double dx = (double)x;

    /*
     * n  = (int) scale(x)
     * dn = (double) n
     */
#undef FAST_INTEGER_CONVERSION
#define FAST_INTEGER_CONVERSION 1
#if FAST_INTEGER_CONVERSION
    /*To eliminate decimal digits after -1/2N */
    dn   = dx + EXP2F_HUGE;
    n    = asuint64(dn);
    dn  -=  EXP2F_HUGE;

#else

    n = dx;
    dn = cast_i32_to_float(n);

#endif

    r  = dx - dn;

    r *=  EXP2F_LN2;

    j  = n % EXP2F_TABLE_SIZE;

    double poly  = C2 + (C3 * r);

    m    = n << (52 - EXP2F_N);

    double r2 = r * r;

    double tbl = asdouble(asuint64(EXP2F_TABLE[j]) + m);

    q  = r  + (r2 * poly);

    double result = tbl + tbl* q;

    return (float)result;

}
