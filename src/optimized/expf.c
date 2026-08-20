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
#include <libm/alm_special.h>

#define EXPF_N 6
#define EXPF_POLY_DEGREE 4
#if EXPF_N == 5
#define EXPF_POLY_DEGREE 3
#elif EXPF_N == 4
#define EXPF_POLY_DEGREE 3
#endif

#define EXPF_TABLE_SIZE (1 << EXPF_N)
#define EXPF_MAX_POLY_DEGREE 4

#define EXP_Y_ZERO      2
#define EXP_Y_INF       3
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
    .Huge = 0x1.8000000000000p+52,
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

#define C1      expf_v2_data.poly[0]
#define C2      expf_v2_data.poly[1]
#define C3  expf_v2_data.poly[2]
#define C4  expf_v2_data.poly[3]

#define EXPF_LN2_BY_TBLSZ  expf_v2_data.ln2by_tblsz
#define EXPF_TBLSZ_BY_LN2  expf_v2_data.tblsz_byln2
#define EXPF_HUGE          expf_v2_data.Huge
#define EXPF_TABLE         expf_v2_data.table_v3

#define EXPF_FARG_MIN  -0x1.9fe368p6f   /* log(2^-150) ~= -103.97 */
#define EXPF_FARG_MAX   0x1.62e42ep6f   /* log(2^128)  ~=   88.72 */

/* Bit patterns for sign/infinity checks. */
#define EXPF_ABS_MASK       (~SIGNBIT_SP32)   /* strip sign bit */
#define EXPF_NEG_INF_BITS   NINFBITPATT_SP32  /* -infinity */


/******************************************
* Implementation Notes
* ---------------------
*
* 0. Choose 'N' as 5, EXPF_TBL_SZ = 2^N i.e 32
*
* 1. Argument Reduction
 ******************************************/
#undef EXPF_N
#define EXPF_N 6

#undef EXPF_TABLE_SIZE
#define EXPF_TABLE_SIZE (1 << EXPF_N)

float
ALM_PROTO_OPT(expf)(float x)
{
    double_t  q, dn, r, z;
    uint64_t n, j;
    float_t result;

    uint32_t ix = asuint32(x);

    if (unlikely(((ix & EXPF_ABS_MASK) > PINFBITPATT_SP32) ||
                 (x > EXPF_FARG_MAX) || (x < EXPF_FARG_MIN))) {
        if ((ix & EXPF_ABS_MASK) > PINFBITPATT_SP32) {    /* NaN */
            result = __alm_handle_errorf(ix | QNAN_MASK_32,
                                         (ix & QNAN_MASK_32) ? AMD_F_NONE
                                                             : AMD_F_INVALID);
        } else if (ix == EXPF_NEG_INF_BITS) {  /* -inf: exp(-inf) = 0 */
            result = 0.0f;
        } else if (x > EXPF_FARG_MAX) {
            if (ix == PINFBITPATT_SP32) {
                result = INFINITY;
            } else {
                /* Raise FE_OVERFLOW, FE_INEXACT */
                result = alm_expf_special(INFINITY, ALM_E_IN_X_INF);
            }
        } else {
            result = alm_expf_special(0.0, ALM_E_IN_X_ZERO);
        }
    } else {
        z = (double_t)x * EXPF_TBLSZ_BY_LN2;

        /*
         * n  = (int) scale(x)
         * dn = (double) n
         */
#undef FAST_INTEGER_CONVERSION
#define FAST_INTEGER_CONVERSION 1
#if FAST_INTEGER_CONVERSION
        dn = z + EXPF_HUGE;
        n    = asuint64(dn);
        dn  -=  EXPF_HUGE;
#else
        n = z;
        dn = cast_i32_to_float(n);
#endif

        r  = fma(dn, -EXPF_LN2_BY_TBLSZ, (double_t)x);
        j  = n % EXPF_TABLE_SIZE;
        {
            double_t qtmp  = fma(C3, r, C2);
            double_t r2 = r * r;
            double_t tbl = asdouble(asuint64(EXPF_TABLE[j]) + (n << (52 - EXPF_N)));
            q  = fma(r2, qtmp, r);
            result = (float_t)fma(tbl, q, tbl);
        }
    }

    return result;
}
