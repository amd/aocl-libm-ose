/*
 * Copyright (C) 2008-2024 Advanced Micro Devices, Inc. All rights reserved.
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
 * Signature:
 *    double expm1(double x)
 *
 * Computes exp(x)-1
 *
 * Max ULP error = 0.54
 ********************************************
 * Implementation Notes
 * ---------------------
 * The algorithm is improved version of its corresponding assembly variant.
 *
 * This variant also handles the IEEE exceptions accordingly.
 *
 * The performance of this C variant is slightly lower, compared to ASM variant.
 * It is due to the additional overhead of handling exceptions.
 *
 */

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm_util_amd.h>
#include <libm/typehelper.h>
#include <libm/alm_special.h>

/* Constants used in expm1 computation */

#define MAX_EXPM1_ARG  0x1.62e6666666666p9
#define MIN_EXPM1_ARG -0x1.2b708872320e1p5

#define LOG_ONE_MINUS_ONE_BY_FOUR -0x1.269621134db93p-2 // log(1-1/4)
#define LOG_ONE_PLUS_ONE_BY_FOUR   0x1.c8ff7c79a9a22p-3 // log(1+1/4)

#define LOG2_BY_64_MTAIL -0x1.cf79abc9e3b39p-46
#define LOG2_BY_64_MHEAD -0x1.62e42fefap-7

#define REAL_1_BY_720 0x1.6c16c16c16c17p-10 // 1/720
#define REAL_1_BY_120 0x1.1111111111111p-7  // 1/120
#define REAL_1_BY_24  0x1.5555555555555p-5  // 1/24
#define REAL_1_BY_6   0x1.5555555555555p-3  // 1/6
#define REAL_1_BY_2   0x1p-1                // 1/2

/* Polynomial Coefficients */
#define B1 0x1.5555555555549p-3
#define B2 0x1.55555555554b6p-5
#define B3 0x1.111111111a9f3p-7
#define B4 0x1.6c16c16ce14c6p-10
#define B5 0x1.a01a01159dd2dp-13
#define B6 0x1.a019f635825c4p-16
#define B7 0x1.71e14bfe3db59p-19
#define B8 0x1.28295484734eap-22
#define B9 0x1.a2836aa646b96p-26

#define REAL_64_BY_LOG_2        0x1.71547652b82fep6 // 64/ln(2)

#define REAL_X_NEAR_0_THRESHOLD 0x1p-63

#define REAL_TWO_POWER_MINUS_7  0x1p-7

/* External tables used by expm1 for reference */

extern const double __two_to_jby64_table[64];
const double *tab = __two_to_jby64_table;

extern const double __two_to_jby64_head_table[64];
const double *tab_head = __two_to_jby64_head_table;

extern const double __two_to_jby64_tail_table[64];
const double *tab_tail = __two_to_jby64_tail_table;

double
ALM_PROTO_OPT(expm1) (double x)
{
    flt64_t q1 = {.i = 0,};
    int64_t m, n, j;
    double d_n;

    if(unlikely(x > MAX_EXPM1_ARG))
    {
        return __alm_handle_error(POS_INF_F64, AMD_F_OVERFLOW|AMD_F_INEXACT);
    }
    if(unlikely(x < MIN_EXPM1_ARG))
    {
        return -1.0;
    }

    if( (x > LOG_ONE_MINUS_ONE_BY_FOUR) && (x < LOG_ONE_PLUS_ONE_BY_FOUR) )
    {
        /* x is a Small Argument (-0.28 < x < 0.22) */
        double abs_x = asdouble(asuint64(x) & POS_BITSET_DP64);
        if(abs_x == 0.0)
        {
            return x;
        }
        else if(abs_x < REAL_X_NEAR_0_THRESHOLD)
        {
            return __alm_handle_error(asuint64(x), AMD_F_UNDERFLOW|AMD_F_INEXACT);
        }

        q1.u = (0x01E0000000000000 + asuint64(x)); // # 0x01E0000000000000 -> 30 in exponents place
        double u = x + q1.d;
        u = u - q1.d;

        double v = x - u;
        double y = (u*u) * REAL_1_BY_2;

        double q = x*(x*x)*(B1 + x*(B2 + x*(B3 + x*(B4 + x*(B5 + x*(B6 + x*(B7 + x*(B8 + x*(B9)))))))));
        double z = v * (x + u) * REAL_1_BY_2;

        if(y < REAL_TWO_POWER_MINUS_7)
        {
            x = x + (y + (q + z));
            return x;
        }
        double temp = q + (v + z);
        u = u + y;
        temp = temp + u;
        return temp;
    }

    /* x is a Not a Small Argument, Normal Flow */
    d_n = REAL_64_BY_LOG_2 * x;
    n = (int64_t)d_n;
    d_n = (double)n;
    n = (int64_t)d_n; // Although redundant, this re-typecasting seems to improve performance with GCC compiler! No change in perf observed with AOCC compiler.

    double r1 = x + d_n * LOG2_BY_64_MHEAD;
    double r2 = d_n * LOG2_BY_64_MTAIL;
    double r = r1 + r2;

    j = n & 63;

    /* m = (n-j)/64 */
    m = ((n-j) >> 6);

    /* q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720))))) */
    double q = r + (r*r)*(REAL_1_BY_2 + r*(REAL_1_BY_6+ r*(REAL_1_BY_24 + r*(REAL_1_BY_120 + r*(REAL_1_BY_720)))));

    double f = tab[j];
    double f1 = tab_head[j];
    double f2 = tab_tail[j];

    q1.i = (int64_t)((1023ULL - (unsigned long long)m) << 52);

    if(m > 52)
    {
        if(m == 1024)
        {
            q1.d = f1 + (f*q) + f2;
            q1.d = asdouble(q1.u + TWOEXPBITS_DP64);

            if((q1.u & POS_INF_F64) == POS_INF_F64)
            {
                return __alm_handle_error(POS_INF_F64, AMD_F_OVERFLOW|AMD_F_INEXACT);
            }
            return q1.d;
        }
        q1.d = (f1 + (f*q+(f2 - q1.d)));
        q1.d =  asdouble(((uint64_t)m << 52) + q1.u);
        return q1.d;
    }

    else if(m < -7)
    {
        q1.d = (f1 + (f*q + f2));
        q1.d = asdouble(q1.u + ((uint64_t)m << 52));
        return q1.d - 1.0;
    }
    else
    {
        double temp = f1 - q1.d;
        q1.d = f1*q + f2*(1+q);
        q1.d = q1.d + temp;
        q1.d = asdouble(q1.u + ((uint64_t)m << 52));
        return q1.d;
    }
    return 0;
}
