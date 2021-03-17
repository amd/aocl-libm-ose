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
 *   double exp2(double x)
 *
 * Spec:
 *   exp2(1) = 2
 *   exp2(x) = 1           if x ∈ F and exp2(x) ≠ 2^x and
 *         log2(1 - (epsilon/(2 * r))) < x  and
 *                         x < log2(1+(epsilon/2)
 *   exp2(x) = 1           if x = -0
 *   exp2(x) = +inf        if x = +inf
 *   exp2(x) = 0           if x = -inf
 *   exp2(x) = 2^x
 *
 *   exp2(x) overflows     if (approximately) x > ln(DBL_MAX). (709.782712893384)
 */

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/poly.h>

#include <libm/typehelper.h>
#include <libm/compiler.h>

/*
 * N defines the precision with which we deal with 'x'
 * I.O.W (1 << N) is the size  of the look up table
 */

#define EXP2_N 6
#define ALM_EXP2_TBL_SZ          (1 << EXP2_N)

#define ALM_EXP2_MAX_DEG 8

struct exp_table {
  double main, head, tail;
};

static const struct {
        double           one_by_tbl_sz;
				double					 ln2;
        double           huge;
        double ALIGN(16) poly[ALM_EXP2_MAX_DEG];
        struct exp_table table[ALM_EXP2_TBL_SZ];
} exp2_data = {
#if EXP2_N == 7
        .one_by_tbl_sz     = 0x1.0p-7,
#elif EXP2_N == 6
        .one_by_tbl_sz     = 0x1.0p-6,
#else
#error "EXP2_N not defined"
#endif
				.huge              = 0x1.8p+52,
				.ln2						   = 0x1.62e42fefa39efp-1, /* log(2) */
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
#if EXP2_N == 6
#include "data/_exp_tbl_64_interleaved.data"

#elif EXP2_N == 7
#include "data/_exp_tbl_128_interleaved.data"
#endif
        },
};

/* C1 = 1! = 1 */
#define C2  exp2_data.poly[0]
#define C3  exp2_data.poly[1]
#define C4  exp2_data.poly[2]
#define C5  exp2_data.poly[3]
#define C6  exp2_data.poly[4]
#define C7  exp2_data.poly[5]
#define C8  exp2_data.poly[6]


#define ALM_EXP2_HUGE           exp2_data.huge
#define ALM_EXP2_LN2						exp2_data.ln2
#define ALM_EXP2_1_BY_TBL_SZ    exp2_data.one_by_tbl_sz
#define ALM_EXP2_TBL_DATA       exp2_data.table


#if 0
#define UMAX_X      0x4090000000000000
#define UMIN_X      0xc090c80000000000
#endif

#define ALM_EXP2_ARG_MAX        0x1.000p+10
#define ALM_EXP2_ARG_MIN       -0x1.0c8p+10

#define DENORMAL_LOW   -0x1.74046dfefd9d0p+9
#define DENORMAL_MIN    0x0000000000000001

static inline uint32_t top12(double x)
{
    return (uint32_t)(asuint64(x) >> 52);
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
double
FN_PROTOTYPE_OPT(exp2)(double x)
{
    double_t    r, q, dn, y;
    int64_t     n, j, m;
    flt64_t     q1 = {.i = 0,};

    /* Top 11 bits, ignoring sign bit (with BIAS)*/
    uint32_t exponent = top12(x) & 0x7ff;

    /* Comparing 11-bit 'exponent' with 12-bit unsigned value */
    if (unlikely (exponent - top12(0x1p-54) >= top12(512.0) - top12(0x1p-54))) {
        if (exponent - top12(0x1p-54) >= 0x80000000)
            return 1.0;

        if (x >= ALM_EXP2_ARG_MAX) {
            if (isnan(x))
                return  alm_exp2_special(x, asdouble(QNANBITPATT_DP64), ALM_E_OUT_INF);

            return  alm_exp2_special(x, asdouble(PINFBITPATT_DP64), ALM_E_IN_X_NAN);
        }

        if (x <= ALM_EXP2_ARG_MIN) {
            if (asuint64(x) == NINFBITPATT_DP64)
                return  alm_exp2_special(x, x, ALM_E_OUT_ZERO);

            return alm_exp2_special(x, 0.0, ALM_E_OUT_ZERO);
        }
	// flag de-normals to process at the end
        exponent = 0xfff;

    }

    double a = x * ALM_EXP2_TBL_SZ;

#define FAST_INTEGER_CONVERSION 1
#if FAST_INTEGER_CONVERSION
    q1.d = a + ALM_EXP2_HUGE;
    n    = q1.i;
    dn   = q1.d - ALM_EXP2_HUGE;
#else
    double_t a = x * ALM_EXP2_TBL_SZ;
    n    = cast_double_to_i64(a);
    dn   = cast_i64_to_double(n);
#endif

    r =  x - (dn * ALM_EXP2_1_BY_TBL_SZ);

    r *= ALM_EXP2_LN2;

    /* table index, for lookup, truncated */
    j = n % ALM_EXP2_TBL_SZ;

    /*
     * n-j/TABLE_SIZE, TABLE_SIZE = 1<<N
     *    and finally
     * m = m << 52
     */
    m = (n - j) << (52 - EXP2_N);

#if 1
		/* q = r + r*r*(1/2.0 + r*(1/6.0+ r*(1/24.0 + r*(1/120.0 + r*(1/720.0))))); */
    q = r * (1 + r * (C2 + r * (C3 + r * (C4 + r * (C5 + r * C6)))));
    /* q = POLY_EVAL_6(r, 1, C2, C3, C4, C5, C6); */
#else
		/* Estrins parallel evaluation  - XXX has huge ULPs */
		double r2 = r * r;									/* r^2 */
		double r4 = r2 * r2;

    q = r + (r2 * (C2  + r * C3));
		q += (r2 * r2) *  C4;								/* r^4 * C4 */
		q += r4 * (C4 + r*C5);
		q += r4 * (C4 + r * (C5 + r* (C6 + r*C7)));
#endif

    /* f(j)*q + f1 + f2 */
    const struct exp_table *tbl = &((const struct exp_table*)ALM_EXP2_TBL_DATA)[j];
    q = q * tbl->main + tbl->head + tbl->tail;

    /* Processing denormals */
    if (unlikely(exponent == 0xfff)) {
        int64_t m1 = (n - j) >> EXP2_N;
        if (m1 <= -1022)
            if (m1 < -1022 || q < 1.0) {
                /* Process true de-normals */
                m1 += 1074;
                flt64u_t tmp = {.i = (1ULL << (uint8_t)m1) };
                y = q * tmp.d;
                goto out;
            }
    }

    y = asdouble((uint64_t)m + asuint64(q));

out:
    return y;
}
