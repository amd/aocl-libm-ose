/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
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
    Signature:
    double amd_erfinv(double x);

    Computes the inverse error function erfinv(x) for a given input x.

    SPEC:
    erfinv(±0)   = ±0
    erfinv(±1)   = ±Inf, DIVBYZERO Exception
    erfinv(±Inf) =  QNaN, INVALID Exception
    erfinv(QNaN) =  QNaN
    erfinv(SNaN) =  QNaN, INVALID Exception

    erfinv(x)    =  QNaN, INVALID Exception if |x|>1
    erfinv(-x)   = -erfinv(x)


    Implementation Notes:
    Reference:
      Rational Chebyshev Approximations for the Inverse of the Error Function
      by J.M.Blair, C.A.Edwards and J.H.Johnson; Mathematics Of Computation,
      Volume 30, Number 136, October 1976, Pages 827-830.

    The function uses rational and asymptotic approximations for different ranges of x.

    1. For |x| <= 0.75
        erfinv(x) = x * Poly_P1(x^2 - offset) / Poly_Q1(x^2 - offset)

    2. For 0.75 < |x| <= 0.9375
        erfinv(x) = x * Poly_P2(x^2 - offset) / Poly_Q2(x^2 - offset)

    3. For 0.9375 < |x| < 1.0
        erfinv(x) = s^-1 * Poly_P3(s) / Poly_Q3(s)
        where s = [-ln(1-x)]^(-1/2)
*/

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/poly.h>
#include "erfinv_data.h"


static const struct
{
    const double exp_offset1; /* 0.5625 */
    const double exp_offset2; /* 0.87890625 */

    double poly_bound1[14]; /* Table 17: P7, Q7  */
    double poly_bound2[16]; /* Table 37: P8, Q8  */
    double poly_bound3[20]; /* Table 58: P11, Q9 */

} erfinv_data = {

    .exp_offset1 = ERFINV_OFFSET1,
    .exp_offset2 = ERFINV_OFFSET2,

     /* Table 17 by Blair et al */
    .poly_bound1 =
    {
       ERFINV_P100,
       ERFINV_P101,
       ERFINV_P102,
       ERFINV_P103,
       ERFINV_P104,
       ERFINV_P105,
       ERFINV_P106,

       ERFINV_Q100,
       ERFINV_Q101,
       ERFINV_Q102,
       ERFINV_Q103,
       ERFINV_Q104,
       ERFINV_Q105,
       ERFINV_Q106,
    },

     /* Table 37 by Blair et al */
    .poly_bound2 =
    {
       ERFINV_P200,
       ERFINV_P201,
       ERFINV_P202,
       ERFINV_P203,
       ERFINV_P204,
       ERFINV_P205,
       ERFINV_P206,
       ERFINV_P207,

       ERFINV_Q200,
       ERFINV_Q201,
       ERFINV_Q202,
       ERFINV_Q203,
       ERFINV_Q204,
       ERFINV_Q205,
       ERFINV_Q206,
       ERFINV_Q207,
    },

     /* Table 58 by Blair et al */
    .poly_bound3 =
    {
       ERFINV_P300,
       ERFINV_P301,
       ERFINV_P302,
       ERFINV_P303,
       ERFINV_P304,
       ERFINV_P305,
       ERFINV_P306,
       ERFINV_P307,
       ERFINV_P308,
       ERFINV_P309,
       ERFINV_P310,

       ERFINV_Q300,
       ERFINV_Q301,
       ERFINV_Q302,
       ERFINV_Q303,
       ERFINV_Q304,
       ERFINV_Q305,
       ERFINV_Q306,
       ERFINV_Q307,
       ERFINV_Q308,
    },
};

#define EXP_OFFSET1 erfinv_data.exp_offset1
#define EXP_OFFSET2 erfinv_data.exp_offset2

#define P100 erfinv_data.poly_bound1[0]
#define P101 erfinv_data.poly_bound1[1]
#define P102 erfinv_data.poly_bound1[2]
#define P103 erfinv_data.poly_bound1[3]
#define P104 erfinv_data.poly_bound1[4]
#define P105 erfinv_data.poly_bound1[5]
#define P106 erfinv_data.poly_bound1[6]

#define Q100 erfinv_data.poly_bound1[7]
#define Q101 erfinv_data.poly_bound1[8]
#define Q102 erfinv_data.poly_bound1[9]
#define Q103 erfinv_data.poly_bound1[10]
#define Q104 erfinv_data.poly_bound1[11]
#define Q105 erfinv_data.poly_bound1[12]
#define Q106 erfinv_data.poly_bound1[13]


#define P200 erfinv_data.poly_bound2[0]
#define P201 erfinv_data.poly_bound2[1]
#define P202 erfinv_data.poly_bound2[2]
#define P203 erfinv_data.poly_bound2[3]
#define P204 erfinv_data.poly_bound2[4]
#define P205 erfinv_data.poly_bound2[5]
#define P206 erfinv_data.poly_bound2[6]
#define P207 erfinv_data.poly_bound2[7]

#define Q200 erfinv_data.poly_bound2[8]
#define Q201 erfinv_data.poly_bound2[9]
#define Q202 erfinv_data.poly_bound2[10]
#define Q203 erfinv_data.poly_bound2[11]
#define Q204 erfinv_data.poly_bound2[12]
#define Q205 erfinv_data.poly_bound2[13]
#define Q206 erfinv_data.poly_bound2[14]
#define Q207 erfinv_data.poly_bound2[15]


#define P300 erfinv_data.poly_bound3[0]
#define P301 erfinv_data.poly_bound3[1]
#define P302 erfinv_data.poly_bound3[2]
#define P303 erfinv_data.poly_bound3[3]
#define P304 erfinv_data.poly_bound3[4]
#define P305 erfinv_data.poly_bound3[5]
#define P306 erfinv_data.poly_bound3[6]
#define P307 erfinv_data.poly_bound3[7]
#define P308 erfinv_data.poly_bound3[8]
#define P309 erfinv_data.poly_bound3[9]
#define P310 erfinv_data.poly_bound3[10]

#define Q300 erfinv_data.poly_bound3[11]
#define Q301 erfinv_data.poly_bound3[12]
#define Q302 erfinv_data.poly_bound3[13]
#define Q303 erfinv_data.poly_bound3[14]
#define Q304 erfinv_data.poly_bound3[15]
#define Q305 erfinv_data.poly_bound3[16]
#define Q306 erfinv_data.poly_bound3[17]
#define Q307 erfinv_data.poly_bound3[18]
#define Q308 erfinv_data.poly_bound3[19]


#define UPPER32_MASK      0x7fffffff
#define INFU              0x7ff0000000000000
#define ZERO              0x0                /* 0.0 */
#define ONEU              0x3ff0000000000000 /* 1.0 */

/* Boundary values for intervals */
#define BOUND1 0x3fe80000 /* 0.75; double(0.75)>>32; */
#define BOUND2 0x3fee0000 /* 0.9375; double(0.9375)>>32; */


double ALM_PROTO_OPT(erfinv)(double x)
{
  double P, Q, z, y, t, absx;
  uint64_t ux;
  uint32_t ix;
  uint64_t sign = 0;

  ux = asuint64(x);
  sign =  ux & SIGNBIT_DP64;
  ux = ux & ~SIGNBIT_DP64;
  ix = ( ux >> 32 ) & UPPER32_MASK;

  /* Check for NaN, Inf, Boundary */
  if (ux >= ONEU) {
    if (ux == INFU) /* Check for ±INF */
      return __alm_handle_error((POS_QNAN_F64 | sign), AMD_F_INVALID);
    else if (ux > INFU) { /* Check for ±NAN */
      /*
       * Branchless sNaN vs qNaN handling:
       * sNaN has quiet bit (bit 51) = 0, should raise FE_INVALID
       * qNaN has quiet bit (bit 51) = 1, no exception
       */
      return __alm_handle_error(
          (POS_QNAN_F64 | sign),
          (ux & QNAN_MASK_64) ? AMD_F_NONE : AMD_F_INVALID
      );
    }
    else if (ux == ONEU) /* Check for ±1 */
      return __alm_handle_error((POS_INF_F64 | sign), AMD_F_DIVBYZERO);
    else  /* Check for |x|>1 */
      return __alm_handle_error(POS_QNAN_F64, AMD_F_INVALID);
  }

  if (ux == ZERO) { /* |x| == 0 */

    return x;

  } else if (ix <= BOUND1) { /* |x| <= 0.75 */

    /* Use x^2 - offset to center the polynomial for better accuracy */
    double x2 = x * x;
    z = x2 - EXP_OFFSET1;

    /* Evaluate rational approximation P(z)/Q(z) */
    P = POLY_EVAL_HORNER_7(z, P100, P101, P102, P103, P104, P105, P106);
    Q = POLY_EVAL_HORNER_7(z, Q100, Q101, Q102, Q103, Q104, Q105, Q106);

    /* Compute x * (P/Q) with better accuracy by delaying multiplication */
    t = P / Q;
    t *= x;

    return t;

  } else if (ix <= BOUND2) { /* 0.75 < |x| <= 0.9375 */

    /* Use x^2 - offset to center the polynomial for better accuracy */
    double x2 = x * x;
    z = x2 - EXP_OFFSET2;

    /* Evaluate rational approximation P(z)/Q(z) */
    P = POLY_EVAL_HORNER_8(z, P200, P201, P202, P203, P204, P205, P206, P207);
    Q = POLY_EVAL_HORNER_8(z, Q200, Q201, Q202, Q203, Q204, Q205, Q206, Q207);

    /* Compute x * (P/Q) with better accuracy by delaying multiplication */
    t = P / Q;
    t *= x;

    return t;

  } else { /* 0.9375 < |x| < 1.0 */
           /* Special Value: (1-1e-100) */

    /* CRITICAL FIX: Use |x| for the log(1-|x|) computation, then restore sign at the end
     * Blair's algorithm: erfinv(x) = sign(x) * P(s)/Q(s) * s^-1
     * where s = sqrt(-log(1 - |x|))
     *
     * For better accuracy near |x| = 1, use log1p when possible:
     * log(1 - |x|) = log1p(-|x|) which is more accurate
     */
    absx = asdouble(ux);

    /* Use log1p for better accuracy: log(1-absx) = log1p(-absx) */
    y = ALM_PROTO(sqrt)(-1.0 * ALM_PROTO(log1p)(-absx));
    z = 1.0 / y;

    /* Evaluate rational approximation P(z)/Q(z) */
    P = POLY_EVAL_HORNER_11N(z, P300, P301, P302, P303, P304, P305,
                                P306, P307, P308, P309, P310);
    Q = POLY_EVAL_HORNER_9(z, Q300, Q301, Q302, Q303, Q304, Q305, Q306,
                              Q307, Q308);

    /* Compute y * (P/Q) */
    t = P / Q;
    t *= y;

    /* Restore the sign: erfinv(-x) = -erfinv(x) */
    if (sign)
      t = -t;

    return t;

  }

}
