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
    double amd_erfcinv(double x)

    Computes the inverse complementary error function erfcinv(x) = erfinv(1-x) for a given input x.

    SPEC:
    erfcinv(0) = +inf
    erfcinv(2) = -inf
    erfcinv(NaN) = NaN

    Implementation Notes:
    The function uses rational and asymptotic approximations for different ranges of x.
    TODO: Fill in details based on final impl

    Polynomials are based on paper: 
        "Rational Chebyshev approximations for the inverse of the error function" 
        J.M. Blair, C.A. Edwards, and J.H. Johnson. 
        Mathematics of Computation, 30(136):827–830, 1976.
 */

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/poly.h>


static struct
{
    const double one;   // 1.0
    const double zero;  // 0.0
    const double exp_offset3; // 0.5625
    const double exp_offset4; // 0.87890625

    double poly_bound_1[16]; // 8 num, 8 den
    double poly_bound_2[19]; // 11 num, 8 den
    double poly_bound_3[13]; // 7 num, 6 den
    double poly_bound_4[15]; // 8 num, 7 den

} erfcinv_data = {

    .one = 0x1.0000000000000p+0,
    .zero = 0.0,
    .exp_offset3 = 0x1.2p-1,
    .exp_offset4 = 0.87890625,

    /* Table 80 Blair et al*/
    .poly_bound_1 =
    {
        0x1.7d072dcb155bcp-32,
        0x1.0d583dbc58cafp-22,
        0x1.8d6fc37f8addep-15,
        0x1.9a6ba9d1f6bdep-9,
        0x1.3f473427b0f36p-4,
        0x1.66a2467a7c489p-1,
        0x1.defc9c5a67065p+0,
        0x1.6dd6488b73f00p-1,

        0x1.7d072bb15fd4ap-32,
        0x1.0d5845e5261bdp-22,
        0x1.8d72c4ca0c54cp-15,
        0x1.9a869cea8bbf4p-9,
        0x1.3fc9fca358d68p-4,
        0x1.6a0f9f9efbc33p-1,
        0x1.fff645502644ep+0,
        0x1.81ddc66c1b776p+0
    },

    /* Table 58 Blair et al*/
    .poly_bound_2 =
    {
        0x1.d98db2f393c93p-15,
        0x1.5ea5105db7d1bp-8,
        0x1.099dcb79c5e37p-3,
        0x1.0ae8df6736f5ap+0,
        0x1.6a463706dce53p+1,
        0x1.501296c195ce7p+1,
        0x1.0a1bd4227162ep+1,
        0x1.7451fe635fbb3p-1,
        0x1.11ae803f200b1p-4,
        -0x1.237ce1b409b07p-6,
        0x1.25db922abee60p-9,

        0x1.d98d1a3412e13p-15,
        0x1.5ea77aa937936p-8,
        0x1.09f744281bf43p-3,
        0x1.0de629224eb8ap+0,
        0x1.84dafe0d3b477p+1,
        0x1.e1add024b4c8dp+1,
        0x1.f06bab8543d1ap+1,
        0x1.04c46273c9ec0p+1
    },

    /* Table 17 Blair et al*/
    .poly_bound_3 =
    {
        0x1.007ce8f01b2e8p+4,
        -0x1.6b23cc5c6c6d7p+6,
        0x1.74e5f6ceb3548p+7,
        -0x1.5200bb15cc6bbp+7,
        0x1.05d193233a849p+6,
        -0x1.148c5474ee5e1p+3,
        0x1.689181bbafd0cp-3,
        
        0x1.d8fb0f913bd7bp+3,
        -0x1.6d7f25a3f1c24p+6,
        0x1.a450d8e7f4cbbp+7,
        -0x1.bc34804858570p+7,
        0x1.ae6b0c504ee02p+6,
        -0x1.499dfec1a7f5fp+4
    },

    /* Table 37 Blair et al*/
    .poly_bound_4 =
    {
        -0x1.f3596123109edp-7,
        0x1.60b8fe375999ep-2,
        -0x1.779bb9bef7c0fp+1,
        0x1.786ea384470a2p+3,
        -0x1.6a7c1453c85d3p+4,
        0x1.31f0fc5613142p+4,
        -0x1.5ea6c007d4dbbp+2,
        0x1.e66f265ce9e50p-3,

        -0x1.636b2dcf4edbep-7,
        0x1.0b5411e2acf29p-2,
        -0x1.3413109467a0bp+1,
        0x1.563e8136c554ap+3,
        -0x1.7b77aab1dcafbp+4,
        0x1.8a3e174e05ddcp+4,
        -0x1.4075c56404eecp+3
    }
};

#define ONE        erfcinv_data.one
#define ZERO       erfcinv_data.zero
#define EXP_OFFSET_3 erfcinv_data.exp_offset3
#define EXP_OFFSET_4 erfcinv_data.exp_offset4

// poly_bound_1: 16 elements (8 numerator, 8 denominator)
#define P10 erfcinv_data.poly_bound_1[0]
#define P11 erfcinv_data.poly_bound_1[1]
#define P12 erfcinv_data.poly_bound_1[2]
#define P13 erfcinv_data.poly_bound_1[3]
#define P14 erfcinv_data.poly_bound_1[4]
#define P15 erfcinv_data.poly_bound_1[5]
#define P16 erfcinv_data.poly_bound_1[6]
#define P17 erfcinv_data.poly_bound_1[7]
#define Q10 erfcinv_data.poly_bound_1[8]
#define Q11 erfcinv_data.poly_bound_1[9]
#define Q12 erfcinv_data.poly_bound_1[10]
#define Q13 erfcinv_data.poly_bound_1[11]
#define Q14 erfcinv_data.poly_bound_1[12]
#define Q15 erfcinv_data.poly_bound_1[13]
#define Q16 erfcinv_data.poly_bound_1[14]
#define Q17 erfcinv_data.poly_bound_1[15]

// poly_bound_2: 19 elements (11 numerator, 8 denominator)
#define P20 erfcinv_data.poly_bound_2[0]
#define P21 erfcinv_data.poly_bound_2[1]
#define P22 erfcinv_data.poly_bound_2[2]
#define P23 erfcinv_data.poly_bound_2[3]
#define P24 erfcinv_data.poly_bound_2[4]
#define P25 erfcinv_data.poly_bound_2[5]
#define P26 erfcinv_data.poly_bound_2[6]
#define P27 erfcinv_data.poly_bound_2[7]
#define P28 erfcinv_data.poly_bound_2[8]
#define P29 erfcinv_data.poly_bound_2[9]
#define P210 erfcinv_data.poly_bound_2[10]
#define Q20 erfcinv_data.poly_bound_2[11]
#define Q21 erfcinv_data.poly_bound_2[12]
#define Q22 erfcinv_data.poly_bound_2[13]
#define Q23 erfcinv_data.poly_bound_2[14]
#define Q24 erfcinv_data.poly_bound_2[15]
#define Q25 erfcinv_data.poly_bound_2[16]
#define Q26 erfcinv_data.poly_bound_2[17]
#define Q27 erfcinv_data.poly_bound_2[18]

// poly_bound_3: 13 elements (7 numerator, 6 denominator)
#define P30 erfcinv_data.poly_bound_3[0]
#define P31 erfcinv_data.poly_bound_3[1]
#define P32 erfcinv_data.poly_bound_3[2]
#define P33 erfcinv_data.poly_bound_3[3]
#define P34 erfcinv_data.poly_bound_3[4]
#define P35 erfcinv_data.poly_bound_3[5]
#define P36 erfcinv_data.poly_bound_3[6]
#define Q30 erfcinv_data.poly_bound_3[7]
#define Q31 erfcinv_data.poly_bound_3[8]
#define Q32 erfcinv_data.poly_bound_3[9]
#define Q33 erfcinv_data.poly_bound_3[10]
#define Q34 erfcinv_data.poly_bound_3[11]
#define Q35 erfcinv_data.poly_bound_3[12]

// poly_bound_4: 15 elements (8 numerator, 7 denominator)
#define P40 erfcinv_data.poly_bound_4[0]
#define P41 erfcinv_data.poly_bound_4[1]
#define P42 erfcinv_data.poly_bound_4[2]
#define P43 erfcinv_data.poly_bound_4[3]
#define P44 erfcinv_data.poly_bound_4[4]
#define P45 erfcinv_data.poly_bound_4[5]
#define P46 erfcinv_data.poly_bound_4[6]
#define P47 erfcinv_data.poly_bound_4[7]
#define Q40 erfcinv_data.poly_bound_4[8]
#define Q41 erfcinv_data.poly_bound_4[9]
#define Q42 erfcinv_data.poly_bound_4[10]
#define Q43 erfcinv_data.poly_bound_4[11]
#define Q44 erfcinv_data.poly_bound_4[12]
#define Q45 erfcinv_data.poly_bound_4[13]
#define Q46 erfcinv_data.poly_bound_4[14]

#define SIGN_MASK         0x7FFFFFFFFFFFFFFF
#define INF               0x7ff0000000000000
#define UPPER32_MASK      0x7fffffff
#define INF_NAN           0x7ff00000

/* Boundary values for intervals */

/* Main boundary values */
#define BOUND1 0x3fb00000 /* 0.0625 */
#define BOUND2 0x3fe80000 /* 0.75 */
#define BOUND3 0x3fee0000 /* 0.9375 */
#define BOUND4 0x40000000 /* 2.0 */
#define ONEU   0x3FF0000000000000 /* 1.0 */

/* Boundary values for sub-intervals */
#define B1_SUB1 0x2b2bff2ee48e0530/* 1e-100 */

double ALM_PROTO_OPT(erfcinv)(double x) {

    double P, Q;
    uint64_t ux;
    uint32_t ix;
    uint64_t sign = 0;

    ux = asuint64(x);
    sign =  ux & SIGNBIT_DP64;
    ux = ux & ~SIGNBIT_DP64;
    ix = ( ux >> 32 )& UPPER32_MASK;

    /* special cases : NaN, boundary, outside domain */
    if (unlikely(ix >= INF_NAN || sign || ix >= BOUND4 || ix == 0))
    {
        if(ix == 0)
            return asdouble(POS_INF_F64);
        else if (ix == BOUND4)
            return asdouble(NEG_INF_F64);
        else if (sign || ix > BOUND4) // out of bounds
            return alm_erfcinv_special(asdouble(NEG_QNAN_F64));
        else
            return x - x; // return NaN
    }

    if (ux == ONEU) {
        return ZERO;
    }
    else if (ix <= BOUND1) { /* erfcinv */
        double z = 1.0 / ALM_PROTO(sqrt)(-ALM_PROTO(log)(x));
        if (ux <= B1_SUB1) {
            P = POLY_EVAL_HORNER_8(z, P10, P11, P12, P13, P14, P15, P16, P17);
            Q = POLY_EVAL_HORNER_9(z, Q10, Q11, Q12, Q13, Q14, Q15, Q16, Q17, ONE);
            Q *= z;
            return P / Q;
        }
        else
        {
            P = POLY_EVAL_HORNER_11_1(z, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29, P210);
            Q = POLY_EVAL_HORNER_9(z, Q20, Q21, Q22, Q23, Q24, Q25, Q26, Q27, ONE);
            Q *= z;
            return P / Q;
        }
    }
    else { /* erfinv */
        double z = 1.0 - x;
        double a = ALM_PROTO(fabs)(z);
        uint64_t ua;
        uint32_t ia;
        ua = asuint64(a);
        ia = ( ua >> 32 )& UPPER32_MASK;
        if (ia <= BOUND2)
        {
            double z2 = z * z - EXP_OFFSET_3;
            P = POLY_EVAL_HORNER_7(z2, P30, P31, P32, P33, P34, P35, P36);
            Q = POLY_EVAL_HORNER_7(z2, Q30, Q31, Q32, Q33, Q34, Q35, ONE);
            return z * (P / Q);
        }
        else if (ia <= BOUND3)
        {
            double z2 = z * z - EXP_OFFSET_4;
            P = POLY_EVAL_HORNER_8(z2, P40, P41, P42, P43, P44, P45, P46, P47);
            Q = POLY_EVAL_HORNER_8(z2, Q40, Q41, Q42, Q43, Q44, Q45, Q46, ONE);
            return z * (P / Q);
        }
        else
        {
            double y = 1.0 / ALM_PROTO(sqrt)(-ALM_PROTO(log)(1.0 - a));
            P = POLY_EVAL_HORNER_11_1(y, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29, P210);
            Q = POLY_EVAL_HORNER_9(y, Q20, Q21, Q22, Q23, Q24, Q25, Q26, Q27, ONE);
            Q *= y;
            double result = P / Q;
            return (z < 0) ? -result : result;
        }
    }
    /* TODO: any special handling needed for underflow (denormal) / overflow (y close to inf) cases? */
}
