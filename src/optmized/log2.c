/*
 * Copyright (C) 2019-2021, Advanced Micro Devices. All rights reserved.
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
 *   double log2(double x)
 *
 * Spec:
 *   log2(x) = nearest(log2(x))   if x ∈ F and x > 0
 *   log2(x) = +inf        if x = {-0, 0}
 *   log2(x) = +inf        if x = +inf
 *   log2(x) = no_result   otherwise
 *
 */

#include <libm_util_amd.h>
#include <libm/alm_special.h>

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/poly.h>

#include <libm/typehelper.h>
#include <libm/compiler.h>


#define N              8
#define LOG2_TABLE_SIZE (1 << N)
#define MAX_POLYDEGREE 8


static struct {
    double   log2e;
    double   poly[MAX_POLYDEGREE];
} log2_data = {
    .log2e      = 0x1.71547652b82fep+0, /* log2(e) */

    /*
     * Polynomial constants, for taylor series
     */
    .poly = {
        1.0,
        0x1.0000000000000p-1,          /* -1/2 */
        0x1.5555555555555p-2,           /* 1/3 */
        0x1.0000000000000p-2,          /* -1/4 */
        0x1.999999999999ap-3,           /* 1/5 */
        0x1.5555555555555p-3,          /* -1/6 */
        0x1.2492492492492p-3,           /* 1/7 */
        0x1.0000000000000p-3,          /* -1/8 */
    },
};

#define C1  log2_data.poly[0]
#define C2	log2_data.poly[1]
#define C3	log2_data.poly[2]
#define C4	log2_data.poly[3]
#define C5	log2_data.poly[4]
#define C6	log2_data.poly[5]
#define C7	log2_data.poly[6]
#define C8	log2_data.poly[7]

#define ALM_LOG2E       log2_data.log2e
#define ALM_LOG2E_LEAD  log2_data.log2e_lead
#define ALM_LOG2E_TAIL  log2_data.log2e_tail

/* Mask for first N bits of mantissa */
#define MANT_MASK_N    (((1ULL << N) - 1) << (EXPSHIFTBITS_DP64 - N))
/* Mask for N+1th bit */
#define MANT_MASK_N1   (1ULL << (EXPSHIFTBITS_DP64 - N - 1))

struct log2_table {
    double lead, tail;
};

extern struct log2_table log2_table_256[];
extern        double     log_f_inv_256[];
#define TAB_F_INV      log_f_inv_256
#define TAB_LOG2       log2_table_256

static inline uint64_t top12(double x)
{
    /* 12 are the exponent bits */
    return asuint64(x) >> (64 - 12);
}

static inline double
log2_near_one(double x)
{
    double  u, q, r;
    double  A1, A2, B1, B2, R1;
    double  u3, u7, u2;

    static const struct {
        double   log2e_lead, log2e_tail;
        double   poly[5];
    } near_one_data = {
        .log2e_lead = 0x1.7000000000000p+0,
        .log2e_tail = 0x1.547652b82fd9cp-8,
        .poly = {
            0x1.55555555554e6p-4,               // 1/2^2 * 3
            0x1.9999999bac6d4p-7,               // 1/2^4 * 5
            0x1.2492307f1519fp-9,               // 1/2^6 * 7
            0x1.c8034c85dfff0p-12               // 1/2^8 * 9
        },
    };
#define LOG2E_LEAD near_one_data.log2e_lead
#define LOG2E_TAIL near_one_data.log2e_tail
#define CA1 near_one_data.poly[0]
#define CA2 near_one_data.poly[1]
#define CA3 near_one_data.poly[2]
#define CA4 near_one_data.poly[3]

    r = x - 1.0;
    flt64_t fx = {.d = r};

    double u_by_2 = r / (2.0 + r);

    q = u_by_2 * r;  /* correction */

    u = u_by_2 + u_by_2;

    u2 = u * u;

    A1 = CA2 * u2 + CA1;
    A2 = CA4 * u2 + CA3;

    u3 = u2 * u;
    B1 =  u3 * A1;

    u7 = u * (u3 * u3);
    B2 = u7 * A2;

    R1 = B1 + B2;
    double poly = R1 - q;

    fx.u &= 0xffffffff00000000;
    double u_high = fx.d;
    double u_low  = r - u_high;

    double s = poly + u_low;

    double ans  = (s * LOG2E_TAIL) + (u_high * LOG2E_TAIL);
    ans        += (s * LOG2E_LEAD) + (u_high * LOG2E_LEAD);

    return ans;
}

/*
 * Implementation Details:
 * -----------------------
 *   'x' very close to 1.0 are handled differently,
 *
 *   Otherwise:
 *        x   =   (2^m)*(A)
 *            =   (2^m)*(G+g)                   where 1<=(1+G)<=2  and g <= (2^-9)
 *
 *     let F = G and f = g
 *            =   (2^m) * 2 * (F/2 + f/2)        0.5<=F<=1  and f <= 2^-10
 *
 *              => A    * B *  C        --------------(1)
 *
 *     let Y = 1/2 * [2^-m * 2^m * A]             0.5<= Y <=1
 *
 *     let F = 0x100 + (top-8-bits of mantissa) + (9th bit)
 *           range of F is: 256 <= F <=512
 *         F = F / 512      0.5 <= F <= 1
 *
 *   From (1):
 *        A = log2(2^m)   = m
 *
 *        B = log2(2)     = 1
 *
 *        C = log2(F/2 + f/2)
 *           => log2(F/2 * (1 + f/F))
 *           => log2(F/2) + log2(1+f/F)
 *
 *      log2(x) = m + 1 + log(C)
 *
 *      log2(F/2) => log2(F) - log2(2)  => log2(F) - 1
 *
 *
 *      log2(r) = ln(r) / ln(2)
 *              = ln(r) / ( ln(2) / ln(e) )      as ln(e) is 1
 *              = ln(r) * ln(e)/ln(2)
 *              = ln(r) * log2(e)
 *
 *     log2(x) = m + log2(F) + ln(1+r) * log2(e)   where r = f/F
 *
 */

double
ALM_PROTO_OPT(log2)(double x)
{
    double    q, r, dexpo, j_times_half;
    uint64_t  ux   = asuint64(x);
    uint64_t  expo = top12(x);

    if (unlikely (expo >= 0x7ff)) {
        if (ux == PINFBITPATT_DP64)
            return x;

        if (isinf(x))
            return alm_log2_special(x, asdouble(PINFBITPATT_DP64), ALM_E_OUT_INF);

        if (!(ux & QNANBITPATT_DP64))
            return alm_log2_special(x, asdouble(ux | QNANBITPATT_DP64), ALM_E_IN_X_NAN);
    }

    if (unlikely (x <= 0.0)) {
        if (x == 0.0)
            return alm_log2_special(x, asdouble(PINFBITPATT_DP64), ALM_E_IN_X_ZERO);

        return alm_log2_special(x, asdouble(QNANBITPATT_DP64), ALM_E_IN_X_NEG);
    }

    flt64_t mant  = {.u = ux & MANTBITS_DP64};
    dexpo         = cast_i64_to_double((int64_t) expo);

    /* Denormals : adjust mantissa */
    if (unlikely (dexpo < -1023.0)) {
        mant.u   |= ONEEXPBITS_DP64;
        mant.d   -= 1.0;
        expo      = (mant.u >> EXPSHIFTBITS_DP64) - 2045;
        mant.u   &= MANTBITS_DP64;
        ux        = mant.u;
    }

    expo    -= EXPBIAS_DP64;
    dexpo    = cast_i64_to_double((int64_t) expo);

    /* Values very close to 1, x in [0.9375, 1.625] */

#define NEAR_ONE_LO asuint64(1 - 0x1.0p-4)
#define NEAR_ONE_HI asuint64(1 + 0x1.1p-4)

    if (unlikely(ux - NEAR_ONE_LO < NEAR_ONE_HI - NEAR_ONE_LO)) {
        return log2_near_one(x);
    }

    uint64_t mant_n   = ux & MANT_MASK_N;
    uint64_t mant_n1  = ux & MANT_MASK_N1;
    uint64_t j        = (mant_n + (mant_n1 << 1));

    mant.i        |= HALFEXPBITS_DP64;               /* F */
    j_times_half   = asdouble(HALFEXPBITS_DP64 | j); /* Y */

    j >>= (52 - N);

    /* f = -( Y - F) */
    double f = j_times_half - mant.d;

    r = f * TAB_F_INV[j];

    struct log2_table *tb_entry = &((struct log2_table*)TAB_LOG2)[j];

    double poly = r * (C1 + r* (C2 + r* (C3 + r * (C4 + r * (C5 + r * C6)))));

    /* -poly as f = -(Y-F) */
    double s = dexpo + tb_entry->lead;
    double z = tb_entry->tail - poly * ALM_LOG2E;

    q = s + z;

    return q;
}
