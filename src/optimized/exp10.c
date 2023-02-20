/*
 * Copyright (C) 2021-2022 Advanced Micro Devices, Inc. All rights reserved.
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
 *   double exp10(double x)
 *
 * Spec:
 *   exp10(x) = 10ˣ
 *   exp10(1) = 10
 *   exp10(0) = 1
 *   exp(inf) = +inf
 *   exp(-inf) = 0
 *   exp(+/-NAN) = +/- NAN
 *   
 *
 *   exp10(x) overflows     
 *   if (approximately) x > log10(FLT_MAX) i.e., 38.53
 * 
 *   exp10(x) underflows
 *   if (approximately) x < log10(-FLT_MAX) i.e., -44.85
 */

/*
 * Implementation Notes
 * ----------------------
 * 0. Choosing N = 64
 *
 * 1. Argument Reduction:
 *      10^(x)   =   2^(x/log10(2)) = 2^(x*(N/log10(2))/N)      --- (1)
 *
 *      Choose 'n' and 'f', such that
 *      x * N/log10(2) = n + f                              --- (2)
 *      | n is integer and |f| <= 1/2N
 *
 *     Choose 'm' and 'j' such that,
 *      n = (N * m) + j                                 --- (3)
 *
 *     From (1), (2) and (3),
 *      2^x = 2^((N*m + j + f)/N)
 *          = (2^m) * (2^(j/N)) * 2^(f/64)
 *          = (2^m) * (2^(j/N)) * e^(f*(log10(2)/N))
 *
 *
 * 2. Table Lookup
 *      Values of (2^(j/N)) are precomputed, j = 0, 1, 2, 3 ... 63
 *
  * 3. Polynomial Evaluation
 *   From (2),
 *     f = x*(64/log10(2)) - n
 *   Let,
 *     r  = f*(log10(2)/64) = x - n*(log10(2)/64)
 *
 * 4. Reconstruction
 *      Thus,
 *        10^x = (2^m) * (2^(j/64)) * 10^r
 *
 */

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/alm_special.h>

struct exp_table {
  double main, head, tail;
};

#define EXP10_N 6
#define ALM_EXP10_TBL_SZ  (1 << EXP10_N)

static const struct {
        double           sixtyfour_by_log2_base10, one_by_tbl_sz;
        double           ln10, log2_bas10_by_64_h;
        double           huge, log2_bas10_by_64_t;
        double ALIGN(16) poly[8];
        struct exp_table table[ALM_EXP10_TBL_SZ];
} exp10_data = {
#if EXP10_N == 7
        .one_by_tbl_sz     = 0x1.0p-7,
#elif EXP10_N == 6
        .one_by_tbl_sz     = 0x1.0p-6,
#else
#error "EXP2_N not defined"
#endif
  .sixtyfour_by_log2_base10 = 2.12603398072791179629348334856E2,
  .huge = 0x1.8p+52,
  .log2_bas10_by_64_h = -4.70359367318451404571533203125E-3,
  .log2_bas10_by_64_t = -9.06519212949933755076654542886E-12,
  .ln10               = 2.30258509299404590109361379291E0,
        /*
         * Polynomial constants, 1/x! (reciprocal of factorial(x))
         * To make better use of cache line, we dont store 0! and 1!
         */
        .poly = { /* skip for 0! and 1! */
                0x1.0000000000000p-1,   /* 1/2! = 1/2    */
                0x1.5555555555555p-3,   /* 1/3! = 1/6    */
                0x1.5555555555555p-5,   /* 1/4! = 1/24   */
                0x1.1111111111111p-7 ,  /* 1/5! = 1/120  */
                0x1.6c16c16c16c17p-10,  /* 1/6! = 1/720  */
                0x1.a01a01a01a01ap-13,  /* 1/7! = 1/5040 */
                0x1.a01a01a01a01ap-16,  /* 1/8! = 1/40320*/
                0x1.71de3a556c734p-19,  /* 1/9! = 1/322880*/

        },

        .table = {
#if EXP10_N == 6
#include "data/_exp_tbl_64_interleaved.data"

#elif EXP10_N == 7
#include "data/_exp_tbl_128_interleaved.data"
#endif
        },
};

/* C1 = 1! = 1 */
#define C2  exp10_data.poly[0]
#define C3  exp10_data.poly[1]
#define C4  exp10_data.poly[2]
#define C5  exp10_data.poly[3]
#define C6  exp10_data.poly[4]
#define C7  exp10_data.poly[5]
#define C8  exp10_data.poly[6]

#define ALM_EXP10_TBL_SZ            (1 << EXP10_N)
#define ALM_EXP10_SHIFT             exp10_data.huge
#define ALM_LOG2_BASE10_BY_64_HEAD  exp10_data.log2_bas10_by_64_h
#define ALM_LOG2_BASE10_BY_64_TAIL  exp10_data.log2_bas10_by_64_t
#define SIXTYFOUR_BY_LOG2_BASE10    exp10_data.sixtyfour_by_log2_base10
#define ALM_EXP10_TBL_DATA          exp10_data.table

#define ARG_MAX         0x4073300000000000U
#define ARG_MIN         0x3c00000000000000U
#define EXP10_ARG_MINF  -3.23306215343115809446317143738E2
#define EXP10_ARG_MAXF  3.0825471555991674676988623105E2
#define ALM_EXP10_LN2   exp10_data.ln10

static inline uint32_t top12(double x)
{
    return (uint32_t)(asuint64(x) >> 52);
}


double
ALM_PROTO_OPT(exp10)(double x) {

    double_t  q, dn, r, y;
    int64_t n, j, m;
    uint32_t exponent;

    uint64_t ux = asuint64(x);

    exponent = top12(x) & 0x7ff;

    if(unlikely(((ux & ~SIGNBIT_DP64) - ARG_MIN) >= (ARG_MAX - ARG_MIN))) {

        if (exponent - top12(0x1p-54) >= 0x80000000)
            return 1.0;

        if (x != x)  {/* check if x is a NAN */

            if((ux & QNANBITPATT_DP64) == QNANBITPATT_DP64) {

                return x;

            }

            return  alm_exp_special(asdouble(QNANBITPATT_DP64), ALM_E_IN_X_NAN);

        }

        if(ux == PINFBITPATT_DP64)
            return asdouble(PINFBITPATT_DP64);

        if(ux == NINFBITPATT_DP64)
            return 0.0;

        if(x >= EXP10_ARG_MAXF) {

            return  alm_exp_special(asdouble(PINFBITPATT_DP64), ALM_E_IN_X_INF);

        }

        if (x <= EXP10_ARG_MINF) {

            return alm_exp_special(0.0, ALM_E_IN_X_ZERO);

        }
        /* flag de-normals to process at the end */
        exponent = 0xfff;

        /*Avoid FAST INTEGER CONVERSION for potential denormal outputs */

        q = x * SIXTYFOUR_BY_LOG2_BASE10;

        n = cast_double_to_i64(q);

        dn = cast_i64_to_double(n);

    }
    else {

        q = x * SIXTYFOUR_BY_LOG2_BASE10;

        q += ALM_EXP10_SHIFT;

        n = (int64_t)asuint64(q);

        dn = q - ALM_EXP10_SHIFT;

    }

    r = x + dn * ALM_LOG2_BASE10_BY_64_HEAD + dn * ALM_LOG2_BASE10_BY_64_TAIL;

    r *= ALM_EXP10_LN2;

    /* table index, for lookup, truncated */
    j = n & (ALM_EXP10_TBL_SZ - 1);

    /*
     * (n-j) / TABLE_SIZE, TABLE_SIZE = 1<<N
     *    and finally
     * m = m << 52
     */
    m = (n - j) << (52 - EXP10_N);

    /* q = r + r*r*(1/2.0 + r*(1/6.0+ r*(1/24.0 + r*(1/120.0 + r*(1/720)))); */
    q = r * (1.0 + r * (C2 + r * (C3 + r * (C4 + r * (C5 + r * C6)))));

    /* f(j)*q + f1 + f2 */
    const struct exp_table *tbl = &((const struct exp_table*)ALM_EXP10_TBL_DATA)[j];

    q = q * tbl->main + tbl->head + tbl->tail;

    /* Processing denormals */
    if (unlikely(exponent == 0xfff)) {

        int64_t m1 = (int64_t)(n >> EXP10_N);

        if (m1 <= -1022)
            if (m1 < -1022 || q < 1.0) {
                /* Process true de-normals */
                m1 += 1074;

                flt64_t tmp = {.u = (1ULL << (uint8_t)m1) };

                y = q * tmp.d;

                return y;
            }
    }

    y = asdouble((uint64_t)m + asuint64(q));

    return y;

}
