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
 *   double exp(double x)
 *
 * Spec:
 *   exp(1) = e
 *   exp(x) = 1           if x ∈ F and exp(x) ≠ eˣ
 *   exp(x) = 1           if x = -0
 *   exp(x) = +inf        if x = +inf
 *   exp(x) = 0           if x = -inf
 *   exp(x) = eˣ
 *
 *   exp(x) overflows     if (approximately) x > ln(FLT_MAX) i.e., 88.72..
 */

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>

#include <libm_macros.h>
#include <libm/types.h>

#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>


#define EXPF_N 6
#define EXPF_POLY_DEGREE 4

#define EXPF_TABLE_SIZE (1 << EXPF_N)
#define EXPF_MAX_POLY_DEGREE 4

/*
 * expf_data.h needs following to be defined before include
 *    - EXPF_N
 *    - EXPF_POLY_DEGREE
 *    - EXPF_TABLE_SIZE
 *    - EXPF_MAX_POLY_DEGREE
 */

#include "expf_data.h"

static const struct expf_data expf_v2_data = {
    .ln2by_tblsz = 0x1.62e42fefa39efp-7,
    .tblsz_byln2 = 0x1.71547652b82fep+6,
    .Huge        = 0x1.8p+52,
#if EXPF_N == 6
    .table_v3 = __two_to_jby64,
#elif EXPF_N == 5
    .table_v3 = &__two_to_jby32,
#endif

    .poly = {
        1.0,    /* 1/1! = 1 */
        0x1.0000000000000p-1,   /* 1/2! = 1/2    */
        0x1.5555555555555p-3,   /* 1/3! = 1/6    */
        0x1.cacccaa4ba57cp-5,   /* 1/4! = 1/24   */
    },
};

#define C1	expf_v2_data.poly[0]
#define C2	expf_v2_data.poly[1]
#define C3  expf_v2_data.poly[2]
#define C4  expf_v2_data.poly[3]

#define EXPF_LN2_BY_TBLSZ  expf_v2_data.ln2by_tblsz
#define EXPF_TBLSZ_BY_LN2  expf_v2_data.tblsz_byln2
#define EXPF_HUGE	       expf_v2_data.Huge
#define EXPF_TABLE         expf_v2_data.table_v3

/*
 * Implementation Notes Unlike other expf() implementation, this one assumes the
 *  arguments are within [ARG_MIN, ARG_MAX], and no denormals.
 *
 */

static float ALM_PROTO_KERN(expf)(float x);
static float
ALM_PROTO_KERN(expf)(float x)
{
    double   q, dn, r, z;
    uint64_t n, j;

    z = (double)x * EXPF_TBLSZ_BY_LN2;

    /*
     * n  = (int) scale(x)
     * dn = (double) n
     */
#undef FAST_INTEGER_CONVERSION
#define FAST_INTEGER_CONVERSION 1
#if FAST_INTEGER_CONVERSION
    dn  =  z + EXPF_HUGE;
    n   =  asuint64(dn);
    dn -=  EXPF_HUGE;
#else
    n  = z;
    dn = cast_i32_to_float(n);

#endif

    r  = (double)x - dn * EXPF_LN2_BY_TBLSZ;

    j  = n % EXPF_TABLE_SIZE;

    double qtmp  = C2 + (C3 * r);

    double r2 = r * r;

    double tbl = asdouble(asuint64(EXPF_TABLE[j]) + (n << (52 - EXPF_N)));

    q  = r  + (r2 * qtmp);

    double result = tbl + tbl* q;

    return (float)result;
}
