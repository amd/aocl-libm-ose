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

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>

#include <libm_macros.h>
#include <libm/types.h>

#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/alm_special.h>

#define EXP10F_N 6

#define EXP10F_TABLE_SIZE (1 << EXP10F_N)
#define EXPF_MAX_POLY_DEGREE 4

#define EXP10_Y_ZERO      2
#define EXP10_Y_INF       3


#include "expf_data.h"


static struct {
    double log10of2by_tblsz, tblsz_bylog10of2, Huge, poly[2];
    const double *table_v3;
} exp10f_v2_data = {
    .log10of2by_tblsz = 0x1.34413509f79ffp-8,
    .tblsz_bylog10of2 = 0x1.a934f0979a371p7,
    .Huge = 0x1.8000000000000p52,
    .table_v3 = __two_to_jby64,

    .poly = {
        0x1.0000000000000p-1, /* 1/2! = 1/2    */
        0x1.5555555555555p-3, /* 1/3! = 1/6    */
    },
};

#define C1  exp10f_v2_data.poly[0]
#define C2  exp10f_v2_data.poly[1]

#define EXP10F_LOG10of2_BY_TBLSZ         exp10f_v2_data.log10of2by_tblsz
#define EXP10F_TBLSZ_BY_LOG10of2         exp10f_v2_data.tblsz_bylog10of2
#define EXP10F_HUGE                      exp10f_v2_data.Huge
#define EXP10F_TABLE                     exp10f_v2_data.table_v3

#define EXP10F_FARG_MIN -0x1.66d3e8p5f    /*  ~= -44.85 */
#define EXP10F_FARG_MAX  0x1.344136p5f    /*  ~=   38.53  */

#define REAL_LN10                  0x1.26bb1bbb55516p1 //ln(10)


static uint32_t
top12f(float x) {
    flt32_t f = {.f = x};
    return f.u >> 20;
}


#undef EXP10F_N
#define EXP10F_N 6

#undef EXP10F_TABLE_SIZE
#define EXP10F_TABLE_SIZE (1 << EXP10F_N)

float
ALM_PROTO_OPT(exp10f)(float x) {
    double_t  q, dn, r, z;
    uint64_t n, j;
    uint32_t top = top12f(x);

    if (unlikely(top > top12f(38.0f))) {
        if (x != x) {
            return x;
        }

        if (asuint32(x) == asuint32(-INFINITY)) {
            return 0.0f;
        }

        if (x > EXP10F_FARG_MAX) {
            if (asuint32(x) == PINFBITPATT_SP32) {
                return asfloat(PINFBITPATT_SP32);
            }

            /* Raise FE_OVERFLOW, FE_INEXACT */
            return __alm_handle_errorf(POS_INF_F32, AMD_F_OVERFLOW|AMD_F_INEXACT);
        }

        if (x < EXP10F_FARG_MIN) {
            return __alm_handle_errorf(POS_ZERO_F32, AMD_F_UNDERFLOW|AMD_F_INEXACT);
        }
    }

    z = (double_t)x * EXP10F_TBLSZ_BY_LOG10of2;
    /*
     * n  = (int) scale(x)
     * dn = (double) n
     */
#undef FAST_INTEGER_CONVERSION
#define FAST_INTEGER_CONVERSION 1
#if FAST_INTEGER_CONVERSION
    dn = z + EXP10F_HUGE;
    n    = asuint64(dn);
    dn  -=  EXP10F_HUGE;
#else
    n = z;
    dn = cast_i32_to_float(n);
#endif
    r  = (double_t)x - dn * EXP10F_LOG10of2_BY_TBLSZ;         // x-(x*64/log10to2)log10to2/64
    r  = r * REAL_LN10;
    j  = n % EXP10F_TABLE_SIZE;
    double_t qtmp  = C1 + (C2 * r);
    double_t r2 = r * r;
    double_t tbl = asdouble(asuint64(EXP10F_TABLE[j]) + (n << (52 - EXP10F_N)));
    q  = r  + (r2 * qtmp);
    double_t result = tbl + tbl * q;
    return (float_t)(result);
}
