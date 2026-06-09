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
   1. For x in [0, 1e-100],
       erfcinv(x) = (1/z) * P1(z)/Q1(z), where z = 1/sqrt(-ln(x))
   2. For x in (1e-100, 0.0625],
       erfcinv(x) = (1/z) * P2(z)/Q2(z), where z = 1/sqrt(-ln(x))
   3. For x in (0.0625, 0.25] and (1.75, 1.9375],
       erfcinv(x) = z * P4(s)/Q4(s), where z = 1-x and s = z^2 - 0.87890625
   4. For x in (0.25, 1.75],
       erfcinv(x) = z * P3(s)/Q3(s), where z = 1-x and s = z^2 - 0.5625
   5. For x in (1.9375, 2],
       erfcinv(x) = -(1/y) * P2(y)/Q2(y), where y = 1/sqrt(-ln(2 - x))

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
     const double exp_offset3; // 0.5625
     const double exp_offset4; // 0.87890625
 
     double poly_bound_1_H[18]; //  9 num, 9 den, Table 82 Blair et al
     double poly_bound_1_T[18];
     double poly_bound_2_H[19]; // 11 num, 8 den, Table 58 Blair et al
     double poly_bound_2_T[19];
     double poly_bound_3[18];   //  9 num, 9 den, Table 22 Blair et al
     double poly_bound_4_H[15]; //  8 num, 7 den, Table 37 Blair et al
     double poly_bound_4_T[15];
 
 } erfcinv_data = {
 
     .one = 0x1.0000000000000p+0, // 1.0
     .exp_offset3 = 0x1.2p-1,     // 0.5625
     .exp_offset4 = 0x1.c2p-1,    // 0.87890625
 
     .poly_bound_1_H =
     {
         // Numerator
         0x1.3f15bb163c58ap-38,
         0x1.3d2e814977011p-28,
         0x1.5c033d288e72bp-20,
         0x1.1f9c3f094799ep-13,
         0x1.8cbdc465b3bc5p-8,
         0x1.cdfd5f83068edp-4,
         0x1.a7fcc3824eeedp-1,
         0x1.f36523ebcb622p+0,
         0x1.6628c8aa4b50ap-1,
 
         // Denominator
         0x1.3f15ba1424e32p-38,
         0x1.3d2e865de9b05p-28,
         0x1.5c048bebb08d7p-20,
         0x1.1fa4a2687bc25p-13,
         0x1.8cfbb39e0a0d8p-8,
         0x1.cf55c1b78651ep-4,
         0x1.ae16dbdd907c0p-1,
         0x1.0fd978bf81eb7p+1,
         0x1.93c2dedc938d3p+0
     },
 
     .poly_bound_1_T =
     {
         // Numerator
         -0x1.bcef3182d6326p-92,
         0x1.167a8c85971cdp-83,
         -0x1.dc5e307e8ea0fp-74,
         -0x1.67514e3ca2fddp-67,
         -0x1.d11b974e4f72fp-68,
         -0x1.731d52be751cep-60,
         -0x1.a3d41c0339492p-55,
         -0x1.c2cb3489f98eep-55,
         0x1.0a162d4c24a89p-55,
 
         // Denominator
         0x1.6870e62ddc803p-93,
         0x1.6b4f6aee9c5e5p-82,
         -0x1.679c75f626e15p-74,
         0x1.180ea175afcd8p-67,
         -0x1.eb4ec5b427967p-62,
         0x1.a42a82c7d0ec1p-60,
         0x1.edc65619bc297p-57,
         -0x1.316d772ee0c65p-53,
         -0x1.b2db12e479d09p-58
     },
 
     .poly_bound_2_H =
     {
         // Numerator
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
 
         // Denominator
         0x1.d98d1a3412e13p-15,
         0x1.5ea77aa937936p-8,
         0x1.09f744281bf43p-3,
         0x1.0de629224eb8ap+0,
         0x1.84dafe0d3b477p+1,
         0x1.e1add024b4c8dp+1,
         0x1.f06bab8543d1ap+1,
         0x1.04c46273c9ec0p+1
     },
 
     .poly_bound_2_T =
     {
         // Numerator
         -0x1.f4222b6157b42p-70,
         -0x1.de62daca659d5p-62,
         -0x1.463aa57fea1a6p-57,
         0x1.f7e59262f7185p-55,
         -0x1.788bcf5e59e53p-53,
         -0x1.89de1d122402cp-55,
         -0x1.77ae19161f6d2p-53,
         0x1.41fdd8cf65bf7p-56,
         -0x1.ca2692d33b42ap-58,
         -0x1.381d744016ce4p-61,
         0x1.be910b13800c0p-64,
 
         // Denominator
         -0x1.2d837f1682e6ap-70,
         0x1.5fbb45bed5641p-62,
         -0x1.dbcc94756d52bp-57,
         -0x1.1ef521d8c6ac1p-54,
         0x1.8ccbec6a1df41p-56,
         0x1.32b745187d541p-55,
         -0x1.3d74c3d939c1bp-55,
         0x1.ce4b02cb50838p-56
     },
 
     .poly_bound_3 =
     {
         // Numerator
         -0x1.7360798246422p+10,
         0x1.7e030587e98dbp+13,
         -0x1.3d0fcbb7f8c46p+15,
         0x1.107dd185aef42p+16,
         -0x1.02f03af1b371ep+16,
         0x1.0d3ff4261257ap+15,
         -0x1.1c278c101bc75p+13,
         0x1.fed4e855def62p+9,
         -0x1.f8ca50679dd18p+4,
 
         // Denominator
         -0x1.566bf73b936f7p+10,
         0x1.766e6c8610465p+13,
         -0x1.4e45ce6f4050ep+15,
         0x1.39f93b45f1adfp+16,
         -0x1.4d7ca13cef74dp+16,
         0x1.90cdbc53c0b15p+15,
         -0x1.02180fb6327f8p+14,
         0x1.394139cc04100p+11,
         -0x1.0cfb9cdf1ef00p+7
     },
 
     .poly_bound_4_H =
     {
         // Numerator
         -0x1.f3596123109edp-7,
         0x1.60b8fe375999ep-2,
         -0x1.779bb9bef7c0fp+1,
         0x1.786ea384470a2p+3,
         -0x1.6a7c1453c85d3p+4,
         0x1.31f0fc5613142p+4,
         -0x1.5ea6c007d4dbbp+2,
         0x1.e66f265ce9e50p-3,
 
         // Denominator
         -0x1.636b2dcf4edbep-7,
         0x1.0b5411e2acf29p-2,
         -0x1.3413109467a0bp+1,
         0x1.563e8136c554ap+3,
         -0x1.7b77aab1dcafbp+4,
         0x1.8a3e174e05ddcp+4,
         -0x1.4075c56404eecp+3
     },
 
     .poly_bound_4_T =
     {
         // Numerator
         -0x1.353bedfc52e09p-61,
         0x1.39125c8724d70p-57,
         -0x1.cb784cbf440cfp-54,
         -0x1.32fee6f928969p-52,
         -0x1.4c96404bae5d5p-50,
         -0x1.180206d4fb9c7p-52,
         0x1.de980ce3e3850p-53,
         -0x1.4b084098b5ceap-59,
 
         // Denominator
         -0x1.8907cbccf0337p-62,
         -0x1.9070eaf055b4ep-56,
         0x1.c506ebb72cb82p-53,
         -0x1.6b06e5d0cbc9cp-51,
         -0x1.3ee22d36208fep-50,
         -0x1.8dd1d27cf044cp-52,
         -0x1.b937f050132f8p-52
     }
 };
 
 #define ONE        erfcinv_data.one
 #define EXP_OFFSET_3 erfcinv_data.exp_offset3
 #define EXP_OFFSET_4 erfcinv_data.exp_offset4
 
 // poly_bound_1: 18 elements (9 numerator, 9 denominator)
 #define P10H erfcinv_data.poly_bound_1_H[0]
 #define P11H erfcinv_data.poly_bound_1_H[1]
 #define P12H erfcinv_data.poly_bound_1_H[2]
 #define P13H erfcinv_data.poly_bound_1_H[3]
 #define P14H erfcinv_data.poly_bound_1_H[4]
 #define P15H erfcinv_data.poly_bound_1_H[5]
 #define P16H erfcinv_data.poly_bound_1_H[6]
 #define P17H erfcinv_data.poly_bound_1_H[7]
 #define P18H erfcinv_data.poly_bound_1_H[8]
 #define Q10H erfcinv_data.poly_bound_1_H[9]
 #define Q11H erfcinv_data.poly_bound_1_H[10]
 #define Q12H erfcinv_data.poly_bound_1_H[11]
 #define Q13H erfcinv_data.poly_bound_1_H[12]
 #define Q14H erfcinv_data.poly_bound_1_H[13]
 #define Q15H erfcinv_data.poly_bound_1_H[14]
 #define Q16H erfcinv_data.poly_bound_1_H[15]
 #define Q17H erfcinv_data.poly_bound_1_H[16]
 #define Q18H erfcinv_data.poly_bound_1_H[17]
 #define P10T erfcinv_data.poly_bound_1_T[0]
 #define P11T erfcinv_data.poly_bound_1_T[1]
 #define P12T erfcinv_data.poly_bound_1_T[2]
 #define P13T erfcinv_data.poly_bound_1_T[3]
 #define P14T erfcinv_data.poly_bound_1_T[4]
 #define P15T erfcinv_data.poly_bound_1_T[5]
 #define P16T erfcinv_data.poly_bound_1_T[6]
 #define P17T erfcinv_data.poly_bound_1_T[7]
 #define P18T erfcinv_data.poly_bound_1_T[8]
 #define Q10T erfcinv_data.poly_bound_1_T[9]
 #define Q11T erfcinv_data.poly_bound_1_T[10]
 #define Q12T erfcinv_data.poly_bound_1_T[11]
 #define Q13T erfcinv_data.poly_bound_1_T[12]
 #define Q14T erfcinv_data.poly_bound_1_T[13]
 #define Q15T erfcinv_data.poly_bound_1_T[14]
 #define Q16T erfcinv_data.poly_bound_1_T[15]
 #define Q17T erfcinv_data.poly_bound_1_T[16]
 #define Q18T erfcinv_data.poly_bound_1_T[17]
 
 // poly_bound_2: 19 elements (11 numerator, 8 denominator)
 #define P20H erfcinv_data.poly_bound_2_H[0]
 #define P21H erfcinv_data.poly_bound_2_H[1]
 #define P22H erfcinv_data.poly_bound_2_H[2]
 #define P23H erfcinv_data.poly_bound_2_H[3]
 #define P24H erfcinv_data.poly_bound_2_H[4]
 #define P25H erfcinv_data.poly_bound_2_H[5]
 #define P26H erfcinv_data.poly_bound_2_H[6]
 #define P27H erfcinv_data.poly_bound_2_H[7]
 #define P28H erfcinv_data.poly_bound_2_H[8]
 #define P29H erfcinv_data.poly_bound_2_H[9]
 #define P210H erfcinv_data.poly_bound_2_H[10]
 #define Q20H erfcinv_data.poly_bound_2_H[11]
 #define Q21H erfcinv_data.poly_bound_2_H[12]
 #define Q22H erfcinv_data.poly_bound_2_H[13]
 #define Q23H erfcinv_data.poly_bound_2_H[14]
 #define Q24H erfcinv_data.poly_bound_2_H[15]
 #define Q25H erfcinv_data.poly_bound_2_H[16]
 #define Q26H erfcinv_data.poly_bound_2_H[17]
 #define Q27H erfcinv_data.poly_bound_2_H[18]
 #define P20T erfcinv_data.poly_bound_2_T[0]
 #define P21T erfcinv_data.poly_bound_2_T[1]
 #define P22T erfcinv_data.poly_bound_2_T[2]
 #define P23T erfcinv_data.poly_bound_2_T[3]
 #define P24T erfcinv_data.poly_bound_2_T[4]
 #define P25T erfcinv_data.poly_bound_2_T[5]
 #define P26T erfcinv_data.poly_bound_2_T[6]
 #define P27T erfcinv_data.poly_bound_2_T[7]
 #define P28T erfcinv_data.poly_bound_2_T[8]
 #define P29T erfcinv_data.poly_bound_2_T[9]
 #define P210T erfcinv_data.poly_bound_2_T[10]
 #define Q20T erfcinv_data.poly_bound_2_T[11]
 #define Q21T erfcinv_data.poly_bound_2_T[12]
 #define Q22T erfcinv_data.poly_bound_2_T[13]
 #define Q23T erfcinv_data.poly_bound_2_T[14]
 #define Q24T erfcinv_data.poly_bound_2_T[15]
 #define Q25T erfcinv_data.poly_bound_2_T[16]
 #define Q26T erfcinv_data.poly_bound_2_T[17]
 #define Q27T erfcinv_data.poly_bound_2_T[18]
 
 // poly_bound_3: 18 elements (9 numerator, 9 denominator)
 #define P30 erfcinv_data.poly_bound_3[0]
 #define P31 erfcinv_data.poly_bound_3[1]
 #define P32 erfcinv_data.poly_bound_3[2]
 #define P33 erfcinv_data.poly_bound_3[3]
 #define P34 erfcinv_data.poly_bound_3[4]
 #define P35 erfcinv_data.poly_bound_3[5]
 #define P36 erfcinv_data.poly_bound_3[6]
 #define P37 erfcinv_data.poly_bound_3[7]
 #define P38 erfcinv_data.poly_bound_3[8]
 #define Q30 erfcinv_data.poly_bound_3[9]
 #define Q31 erfcinv_data.poly_bound_3[10]
 #define Q32 erfcinv_data.poly_bound_3[11]
 #define Q33 erfcinv_data.poly_bound_3[12]
 #define Q34 erfcinv_data.poly_bound_3[13]
 #define Q35 erfcinv_data.poly_bound_3[14]
 #define Q36 erfcinv_data.poly_bound_3[15]
 #define Q37 erfcinv_data.poly_bound_3[16]
 #define Q38 erfcinv_data.poly_bound_3[17]
 
 // poly_bound_4: 15 elements (8 numerator, 7 denominator)
 #define P40H erfcinv_data.poly_bound_4_H[0]
 #define P41H erfcinv_data.poly_bound_4_H[1]
 #define P42H erfcinv_data.poly_bound_4_H[2]
 #define P43H erfcinv_data.poly_bound_4_H[3]
 #define P44H erfcinv_data.poly_bound_4_H[4]
 #define P45H erfcinv_data.poly_bound_4_H[5]
 #define P46H erfcinv_data.poly_bound_4_H[6]
 #define P47H erfcinv_data.poly_bound_4_H[7]
 #define Q40H erfcinv_data.poly_bound_4_H[8]
 #define Q41H erfcinv_data.poly_bound_4_H[9]
 #define Q42H erfcinv_data.poly_bound_4_H[10]
 #define Q43H erfcinv_data.poly_bound_4_H[11]
 #define Q44H erfcinv_data.poly_bound_4_H[12]
 #define Q45H erfcinv_data.poly_bound_4_H[13]
 #define Q46H erfcinv_data.poly_bound_4_H[14]
 #define P40T erfcinv_data.poly_bound_4_T[0]
 #define P41T erfcinv_data.poly_bound_4_T[1]
 #define P42T erfcinv_data.poly_bound_4_T[2]
 #define P43T erfcinv_data.poly_bound_4_T[3]
 #define P44T erfcinv_data.poly_bound_4_T[4]
 #define P45T erfcinv_data.poly_bound_4_T[5]
 #define P46T erfcinv_data.poly_bound_4_T[6]
 #define P47T erfcinv_data.poly_bound_4_T[7]
 #define Q40T erfcinv_data.poly_bound_4_T[8]
 #define Q41T erfcinv_data.poly_bound_4_T[9]
 #define Q42T erfcinv_data.poly_bound_4_T[10]
 #define Q43T erfcinv_data.poly_bound_4_T[11]
 #define Q44T erfcinv_data.poly_bound_4_T[12]
 #define Q45T erfcinv_data.poly_bound_4_T[13]
 #define Q46T erfcinv_data.poly_bound_4_T[14]
 
 #define UPPER32_MASK      0x7fffffff
 
 /* Boundary values for intervals */
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

    /* special cases : NaN, outside domain [0,2], -0
     * For Nan : ix >= INF_NAN (hence ix > BOUND4)
     * For x < 0 : sign is true
     * For x > 2 : ix > BOUND4
     * For x = -0 : sign is true and ix == 0 
    */
    if (unlikely(sign || ix > BOUND4))
    {
        if(ix == 0) // -0
            return asdouble(POS_INF_F64);
        if (ux > POS_INF_F64) // propagate NaN
            return x - x;
        else 
            return alm_erfcinv_special(asdouble(NEG_QNAN_F64));
    }
    
    if (ix <= BOUND1) 
    { /* erfcinv */
        double z = 1.0 / ALM_PROTO(sqrt)(-ALM_PROTO(log)(x));
        if (ux <= B1_SUB1) 
        {
            P = POLY_EVAL_ESTRIN_9_TAIL_4(z, P10H, P10T, P11H, P11T, P12H, P12T, P13H, P13T, 
                P14H, P15H, P16H, P17H, P18H);
            Q = POLY_EVAL_ESTRIN_10_TAIL_4(z, Q10H, Q10T, Q11H, Q11T, Q12H, Q12T, Q13H, Q13T,
                Q14H, Q15H, Q16H, Q17H, Q18H, ONE);
            Q *= z;
            return P / Q;
        }
        else
        {
            P = POLY_EVAL_ESTRIN_11_TAIL_2(z, P20H, P20T, P21H, P21T,
            P22H, P23H, P24H, P25H, P26H, P27H, P28H, P29H, P210H);
            Q = POLY_EVAL_ESTRIN_9_TAIL_2(z, Q20H, Q20T, Q21H, Q21T,
                Q22H, Q23H, Q24H, Q25H, Q26H, Q27H, ONE);
            Q *= z;
            return P / Q;
        }
    }
    else 
    { /* erfinv */
        double z = 1.0 - x;
        uint64_t ua;
        uint32_t ia;
        ua = asuint64(z) & ~SIGNBIT_DP64;
        ia = (ua >> 32) & UPPER32_MASK;
        if (ia <= BOUND2)
        {
            double z2 = z * z - EXP_OFFSET_3;
            P = POLY_EVAL_HORNER_9(z2, P30, P31, P32, P33, P34, P35, P36, P37, P38);
            Q = POLY_EVAL_HORNER_10(z2, Q30, Q31, Q32, Q33, Q34, Q35, Q36, Q37, Q38, ONE);
            return z * (P / Q);
        }
        else if (ia <= BOUND3)
        {
            double z2 = ((x*x)-(2*x)) + (1 - EXP_OFFSET_4); // z2 = z * z - EXP_OFFSET_4;
            P = POLY_EVAL_ESTRIN_8_TAIL_2(z2, P40H, P40T, P41H, P41T,
                P42H, P43H, P44H, P45H, P46H, P47H);
            Q = POLY_EVAL_ESTRIN_8_TAIL_2(z2, Q40H, Q40T, Q41H, Q41T,
                Q42H, Q43H, Q44H, Q45H, Q46H, ONE);
            return z * (P / Q);
        }
        else
        {
            double y = 1.0 / ALM_PROTO(sqrt)(-ALM_PROTO(log)(2.0 - x));
            P = POLY_EVAL_ESTRIN_11_TAIL_1(y, P20H, P20T,
                P21H, P22H, P23H, P24H, P25H, P26H, P27H, P28H, P29H, P210H);
            Q = POLY_EVAL_ESTRIN_9_TAIL_1(y, Q20H, Q20T,
                Q21H, Q22H, Q23H, Q24H, Q25H, Q26H, Q27H, ONE);
            Q *= y;
            return -(P / Q);
        }
     }
 }
