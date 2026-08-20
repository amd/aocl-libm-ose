/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>
/*
 * Test cases to check for exceptions for the atanf() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_atanf_conformance_data[] = {
   // special accuracy tests
   {0x38800000, 0x38800000,  0}, //min= 0.00006103515625
   {0x387FFFFF, 0x387fffff,  0}, //min - 1 bit
   {0x38800001, 0x38800001,  0}, //min + 1 bit
   {0xF149F2C9, 0xbfc90fdb,  FE_INEXACT}, //lambda + x = 1, x = -9.9999994e+29
   {0xF149F2C8, 0xbfc90fdb,  FE_INEXACT}, //lambda + x < 1
   {0xF149F2CA, 0xbfc90fdb,  FE_OVERFLOW}, //lambda + x > 1
   {0x42B2D4FC, 0x3fc7a167,  0}, //max arg, x = 89.41598629223294,max atanf arg
   {0x42B2D4FB, 0x3fc7a167,  0}, //max arg - 1 bit
   {0x42B2D4FD, 0x3fc7a167,  FE_INEXACT}, //max arg + 1 bit
   {0x42B2D4FF, 0x3fc7a167,  FE_INEXACT}, // > max
   {0x42B2D400, 0x3fc7a165,  0}, // < max
   {0x41A00000, 0x3fc2aad1,  0}, //small_threshold = 20
   {0x41A80000, 0x3fc2f8a7,  0}, //small_threshold+1 = 21
   {0x41980000, 0x3fc254d0,  0}, //small_threshold - 1 = 19

    //atan special exception checks
   {POS_ZERO_F32, 0x3f800000,0 },  //0
   {NEG_ZERO_F32, 0x3f800000,0 },  //0
   {POS_INF_F32,  0x3fc90fdb,0 },
   {NEG_INF_F32,  0xbfc90fdb,0 },
   {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID },  //
   {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID },  //
   {POS_QNAN_F32, POS_QNAN_F32, 0 },  //
   {NEG_QNAN_F32, NEG_QNAN_F32, 0 },  //
   {POS_INF_F32,  0x3fc90fdb,  FE_OVERFLOW },  //95

   {0x00000001, 0x00000001,  0},  // denormal min
   {0x0005fde6, 0x0005fde6,  0},  // denormal intermediate
   {0x007fffff, 0x007fffff,  0},  // denormal max
   {0x80000001, 0x80000001,  0},  // -denormal min
   {0x805def12, 0x805def12,  0},  // -denormal intermediate
   {0x807FFFFF, 0x807fffff,  0},  // -denormal max
   {0x00800000, 0x00800000,  0},  // normal min
   {0x43b3c4ea, 0x3fc8b4b7,  FE_INEXACT},  // normal intermediate
   {0x7f7fffff, 0x3fc90fdb,  FE_OVERFLOW},  // normal max
   {0x80800000, 0x80800000,  0},  // -normal min
   {0xc5812e71, 0xbfc907ed,  FE_INEXACT},  // -normal intermediate
   {0xFF7FFFFF, 0xbfc90fdb,  FE_OVERFLOW},  // -normal max
   {0x7F800000, 0x3fc90fdb,  0},  // inf
   {0xfF800000, 0xbfc90fdb,  0},  // -inf
   {0x7Fc00000, 0x7fc00000,  0},  // qnan min
   {0x7Fe1a570, 0x7fe1a570,  0},  // qnan intermediate
   {0x7FFFFFFF, 0x7fffffff,  0},  // qnan max
   {0xfFc00000, 0xffc00000,  0},  // indeterninate
   {0xFFC00001, 0xffc00001,  0},  // -qnan min
   {0xFFd2ba31, 0xffd2ba31,  0},  // -qnan intermediate
   {0xFFFFFFFF, 0xffffffff,  0},  // -qnan max
   {0x7F800001, 0x7fc00001,  FE_INVALID},  // snan min
   {0x7Fa0bd90, 0x7fe0bd90,  FE_INVALID},  // snan intermediate
   {0x7FBFFFFF, 0x7fffffff,  FE_INVALID},  // snan max
   {0xFF800001, 0xffc00001,  FE_INVALID},  // -snan min
   {0xfF95fffa, 0xffd5fffa,  FE_INVALID},  // -snan intermediate
   {0xFFBFFFFF, 0xffffffff,  FE_INVALID},  // -snan max
   {0x3FC90FDB, 0x3f807f4c,  0},  // pi/2
   {0x40490FDB, 0x3fa19dc5,  0},  // pi
   {0x40C90FDB, 0x3fb4dc0b,  0},  // 2pi
   {0x402DF853, 0x3f9bf0b1,  0},  // e --
   {0x402DF854, 0x3f9bf0b2,  0},  // e
   {0x402DF855, 0x3f9bf0b2,  0},  // e ++
   {0x00000000, 0x3f800000,  0},  // 0
   {0x37C0F01F, 0x37c0f01f,  0},  // 0.000023
   {0x3EFFFEB0, 0x3eed622b,  0},  // 0.49999
   {0x3F0000C9, 0x3eed647a,  0},  // 0.500012
   {0x80000000, 0x3f800000,  0},  // -0
   {0xb7C0F01F, 0xb7c0f01f,  0},  // -0.000023
   {0xbEFFFEB0, 0xbeed622b,  0},  // -0.49999
   {0xbF0000C9, 0xbeed647a,  0},  // -0.500012
   {0x3f800000, 0x3f490fdb,  0},  // 1
   {0x3f700001, 0x3f40ce86,  0},  // 0.93750006
   {0x3F87FFFE, 0x3f50d12e,  0},  // 1.0624998
   {0x3FBFFFAC, 0x3f7b982b,  0},  // 1.49999
   {0x3FC00064, 0x3f7b989c,  0},  // 1.500012
   {0xbf800000, 0xbf490fdb,  0},  // -1
   {0xbf700001, 0xbf40ce86,  0},  // -0.93750006
   {0xbF87FFFE, 0xbf50d12e,  0},  // -1.0624998
   {0xbFBFFFAC, 0xbf7b982b,  0},  // -1.49999
   {0xbFC00064, 0xbf7b989c,  0},  // -1.500012

   {0xc0000000, 0xbf8db70d,  0},  // -2
   {0x41200000, 0x3fbc4de9,  0},  // 10
   {0xc1200000, 0xbfbc4de9,  0},  // -10
   {0x447A0000, 0x3fc8ef16,  FE_INEXACT},  // 1000
   {0xc47A0000, 0xbfc8ef16,  FE_INEXACT},  // -1000
   {0x4286CCCC, 0x3fc729b8,  0},  // 67.4
   {0xc286CCCC, 0xbfc729b8,  0},  // -67.4
   {0x44F7F333, 0x3fc8ff56,  FE_INEXACT},  // 1983.6
   {0xc4F7F333, 0xbfc8ff56,  FE_INEXACT},  // -1983.6
   {0x42AF0000, 0x3fc79961,  0},  // 87.5
   {0xc2AF0000, 0xbfc79961,  0},  // -87.5
   {0x48015E40, 0x3fc90f9b,  FE_INEXACT},  // 132473
   {0xc8015E40, 0xbfc90f9b,  FE_INEXACT},  // -132473
   {0x4B000000, 0x3fc90fda,  FE_INEXACT},  // 2^23
   {0x4B000001, 0x3fc90fda,  FE_INEXACT},  // 2^23 + 1
   {0x4AFFFFFF, 0x3fc90fda,  FE_INEXACT},  // 2^23 -1 + 0.5
   {0xcB000000, 0xbfc90fda,  FE_INEXACT},  // -2^23
   {0xcB000001, 0xbfc90fda,  FE_INEXACT},  // -(2^23 + 1)
   {0xcAFFFFFF, 0xbfc90fda,  FE_INEXACT},  // -(2^23 -1 + 0.5)
   {0x80000000, 0x80000000,  0},
   {0x7f800001, 0x7fc00001,  0},
   {0xff800001, 0xffc00001,  0},
   {0x7fc00000, 0x7fc00000,  0},
   {0xffc00000, 0xffc00000,  0},
   {0x7f800000, 0x3fc90fdb,  0},
   {0xff800000, 0xbfc90fdb,  0},
   {0x807fffff, 0x807fffff,  0},
   {0xff7fffff, 0xbfc90fdb,  0},

   //answer from NAG test tool
   {0x35800000, 0x35800000,  0},    // 2 ^(-20) < 2.0^(-19), 9.536743164060E-07
   {0xbe4ccccd, 0xbe4a2210,  0},    // abs(-0.2)< 7./16.,   -1.973955627155E-01
   {0x3f000000, 0x3eed6338,  0},    // 0.5      < 11./16.,   4.636476090008E-01
   {0xbf7cac08, 0xbf476317,  0},    //-0.987    < 19./16.,  -7.788557245266E-01
   {0x400947ae, 0x3f913900,  0},    // 2.145    < 39./16.,   1.134552060813E+00
   {0xc2c80000, 0xbfc7c82f,  0},    // -100     < 2^26,     -1.560796660108E+00
   {0x4d000000, 0x3fc90fdb,  0}, // 2^27     > 2^26,      1.570796319344E+00
   {0xcd000000, 0xbfc90fdb,  0}, //-2^27     > 2^26,     -1.570796319344E+00

};

static libm_test_special_data_f64
test_atan_conformance_data[] = {
    #if defined(_WIN64) || defined(_WIN32)
        {0x23d, 0x23d, 3},
        {0xC32F'FFFF'FFFF'FFFFLL, 0xbff9'21fb'5444'2d17LL, 1}, // -(2^52 -1 + 0.5)
        {0xC330'0000'0000'0001LL, 0xbff9'21fb'5444'2d17LL, 1}, // -(2^52 + 1)
        {0xC330'0000'0000'0000LL, 0xbff9'21fb'5444'2d17LL, 1}, // -2^52
        {0x432F'FFFF'FFFF'FFFFLL, 0x3ff9'21fb'5444'2d17LL, 1}, // 2^52 -1 + 0.5
        {0x4330'0000'0000'0001LL, 0x3ff9'21fb'5444'2d17LL, 1}, // 2^52 + 1
        {0x4330'0000'0000'0000LL, 0x3ff9'21fb'5444'2d17LL, 1}, // 2^52
        {0xC100'2BC8'0000'0000LL, 0xbff9'21f3'69ec'e8e3LL, 1}, // -132473
        {0x4100'2BC8'0000'0000LL, 0x3ff9'21f3'69ec'e8e3LL, 1}, // 132473
        {0xC09E'FE66'6666'6666LL, 0xbff9'1fea'b4dd'702bLL, 1}, // -1983.6
        {0x409E'FE66'6666'6666LL, 0x3ff9'1fea'b4dd'702bLL, 1}, // 1983.6
        {0x4086'3400'0000'0000LL, 0x3ff9'1c37'8079'3b77, 1},  // 710.5
        {0x4086'33ce'8fb9'f87eLL, 0x3ff9'1c37'73a2'f9b4LL, 1}, //max arg, x = 89.41598629223294,max atanf arg
        {0x408F'4000'0000'0000LL, 0x3ff9'1de2'c0e6'58bdLL, 1}, // 1000
        {0xC08F'4000'0000'0000LL, 0xbff9'1de2'c0e6'58bdLL, 1}, // -1000
        {0x4086'33ce'8fb9'f87fLL, 0x3ff9'1c37'73a2'f9b4LL, 1}, //max arg + 1 bit
        {0x4086'33ce'8fb9'f8ffLL, 0x3ff9'1c37'73a2'f9b4LL, 1}, // > max
    #else
        {0x23d, 0x23d, 48},
        {0xC32F'FFFF'FFFF'FFFFLL, 0xbff9'21fb'5444'2d17LL, 32}, // -(2^52 -1 + 0.5)
        {0xC330'0000'0000'0001LL, 0xbff9'21fb'5444'2d17LL, 32}, // -(2^52 + 1)
        {0xC330'0000'0000'0000LL, 0xbff9'21fb'5444'2d17LL, 32}, // -2^52
        {0x432F'FFFF'FFFF'FFFFLL, 0x3ff9'21fb'5444'2d17LL, 32}, // 2^52 -1 + 0.5
        {0x4330'0000'0000'0001LL, 0x3ff9'21fb'5444'2d17LL, 32}, // 2^52 + 1
        {0x4330'0000'0000'0000LL, 0x3ff9'21fb'5444'2d17LL, 32}, // 2^52
        {0xC100'2BC8'0000'0000LL, 0xbff9'21f3'69ec'e8e3LL, 32}, // -132473
        {0x4100'2BC8'0000'0000LL, 0x3ff9'21f3'69ec'e8e3LL, 32}, // 132473
        {0xC09E'FE66'6666'6666LL, 0xbff9'1fea'b4dd'702bLL, 32}, // -1983.6
        {0x409E'FE66'6666'6666LL, 0x3ff9'1fea'b4dd'702bLL, 32}, // 1983.6
        {0x4086'3400'0000'0000LL, 0x3ff9'1c37'8079'3b77, 32},  // 710.5
        {0x4086'33ce'8fb9'f87eLL, 0x3ff9'1c37'73a2'f9b4LL, 32}, //max arg, x = 89.41598629223294,max atanf arg
        {0x408F'4000'0000'0000LL, 0x3ff9'1de2'c0e6'58bdLL, 32}, // 1000
        {0xC08F'4000'0000'0000LL, 0xbff9'1de2'c0e6'58bdLL, 32}, // -1000
        {0x4086'33ce'8fb9'f87fLL, 0x3ff9'1c37'73a2'f9b4LL, 32}, //max arg + 1 bit
        {0x4086'33ce'8fb9'f8ffLL, 0x3ff9'1c37'73a2'f9b4LL, 32}, // > max
    #endif
    // special accuracy tests
    {0x3e30'0000'0000'0000LL, 0x3e30'0000'0000'0000LL, 0}, //min
    {0x3E2F'FFFF'FFFF'FFFFLL, 0x3e2f'ffff'ffff'ffffLL, 0}, //min - 1 bit
    {0x3e30'0000'0000'0001LL, 0x3e30'0000'0000'0001LL, 0}, //min + 1 bit
    {0xFE37'E43C'8800'759CLL, 0xbff9'21fb'5444'2d18LL, 0}, //lambda + x = 1, x = -1.0000000000000000e+300
    {0xFE37'E43C'8800'758CLL, 0xbff9'21fb'5444'2d18LL, 0}, //lambda + x < 1
    {0xFE37'E43C'8800'75ACLL, 0xbff9'21fb'5444'2d18LL, 0}, //lambda + x > 1
    {0x4086'33ce'8fb9'f87dLL, 0x3ff9'1c37'73a2'f9b4LL, 0}, //max arg - 1 bit
    {0x4086'33ce'8fb9'f800LL, 0x3ff9'1c37'73a2'f9b3LL, 0}, // < max
    {0x4034'0000'0000'0000LL, 0x3ff8'555a'2787'981fLL, 0}, //small_threshold = 20
    {0x4035'0000'0000'0000LL, 0x3ff8'5f14'd43d'81beLL, 0}, //small_threshold+1 = 21
    {0x4033'0000'0000'0000LL, 0x3ff8'4a99'fe25'186bLL, 0}, //small_threshold - 1 = 19

    //atan special exception checks
    {POS_ZERO_F64, 0x3FF0'0000'0000'0000LL,0 },  //0
    {NEG_ZERO_F64, 0x3FF0'0000'0000'0000LL,0 },  //0
    {POS_INF_F64, 0x3ff9'21fb'5444'2d18,   0 },
    {NEG_INF_F64, 0xbff9'21fb'5444'2d18,   0 },
    {POS_SNAN_F64, POS_SNAN_F64, FE_INVALID },  //
    {NEG_SNAN_F64, NEG_SNAN_F64, FE_INVALID },  //
    {POS_QNAN_F64, POS_QNAN_F64, 0 },  //
    {NEG_QNAN_F64, NEG_QNAN_F64, 0 },  //
    {0x0000'0000'0000'0001LL, 0x0000'0000'0000'0001LL, 0}, // denormal min
    {0x0005'fde6'2354'5abcLL, 0x0005'fde6'2354'5abcLL, 0}, // denormal intermediate
    {0x000F'FFFF'FFFF'FFFFLL, 0x000f'ffff'ffff'ffffLL, 0}, // denormal max
    {0x8000'0000'0000'0001LL, 0x8000'0000'0000'0001LL, 0}, // -denormal min
    {0x8002'344a'de5d'ef12LL, 0x8002'344a'de5d'ef12LL, 0}, // -denormal intermediate
    {0x800F'FFFF'FFFF'FFFFLL, 0x800f'ffff'ffff'ffffLL, 0}, // -denormal max
    {0x0010'0000'0000'0000LL, 0x0010'0000'0000'0000LL, 0}, // normal min
    {0x43b3'c4ea'fedc'ab02LL, 0x3ff9'21fb'5444'2d18LL, 0}, // normal intermediate
    {0x7FEF'FFFF'FFFF'FFFFLL, 0x3ff9'21fb'5444'2d18LL, 0}, // normal max
    {0x8010'0000'0000'0000LL, 0x8010'0000'0000'0000LL, 0}, // -normal min
    {0xc581'2e71'245a'cfdbLL, 0xbff9'21fb'5444'2d18LL, 0}, // -normal intermediate
    {0xFFEF'FFFF'FFFF'FFFFLL, 0xbff9'21fb'5444'2d18LL, 0}, // -normal max
    {0x7FF0'0000'0000'0000LL, 0x3ff9'21fb'5444'2d18LL, 0}, // inf
    {0xFFF0'0000'0000'0000LL, 0xbff9'21fb'5444'2d18LL, 0}, // -inf
    {0x7FF8'0000'0000'0000LL, 0x7ff8'0000'0000'0000LL, 0}, // qnan min
    {0x7FFe'1a57'0123'4dc3LL, 0x7ffe'1a57'0123'4dc3LL, 0}, // qnan intermediate
    {0x7FFF'FFFF'FFFF'FFFFLL, 0x7fff'ffff'ffff'ffffLL, 0}, // qnan max
    {0xFFF8'0000'0000'0000LL, 0xfff8'0000'0000'0000LL, 0}, // indeterninate
    {0xFFF8'0000'0000'0001LL, 0xfff8'0000'0000'0001LL, 0}, // -qnan min
    {0xFFF9'1234'25dc'ba31LL, 0xfff9'1234'25dc'ba31LL, 0}, // -qnan intermediate
    {0xFFFF'FFFF'FFFF'FFFFLL, 0xffff'ffff'ffff'ffffLL, 0}, // -qnan max
    {0x7FF0'0000'0000'0001LL, 0x7ff8'0000'0000'0001LL, FE_INVALID}, // snan min
    {0x7FF5'3447'52a0'bd90LL, 0x7ffd'3447'52a0'bd90LL, FE_INVALID}, // snan intermediate
    {0x7FF7'FFFF'FFFF'FFFFLL, 0x7fff'ffff'ffff'ffffLL, FE_INVALID}, // snan max
    {0xFFF0'0000'0000'0001LL, 0xfff8'0000'0000'0001LL, FE_INVALID}, // -snan min
    {0xfFF4'3243'8995'fffaLL, 0xfffc'3243'8995'fffaLL, FE_INVALID}, // -snan intermediate
    {0xFFF7'FFFF'FFFF'FFFFLL, 0xffff'ffff'ffff'ffffLL, FE_INVALID}, // -snan max
    {0x3FF9'21FB'5444'2D18LL, 0x3ff0'0fe9'87ed'02ffLL, 0}, // pi/2
    {0x4009'21FB'5444'2D18LL, 0x3ff4'33b8'a322'ddd3LL, 0}, // pi
    {0x4019'21FB'5444'2D18LL, 0x3ff6'9b81'54ba'f42eLL, 0}, // 2pi
    {0x3FFB'7E15'1628'AED3LL, 0x3ff0'b323'2129'2ef0LL, 0}, // e --
    {0x4005'BF0A'8B14'5769LL, 0x3ff3'7e16'3725'3389LL, 0}, // e
    {0x400D'BF0A'8B14'5769LL, 0x3ff4'eddc'4885'c1c3LL, 0}, // e ++
    {0x0000'0000'0000'0000LL, 0x3ff0'0000'0000'0000LL, 0}, // 0
    {0x3C45'36B8'B14B'676CLL, 0x3c45'36b8'b14b'676cLL, 0}, // 0.0000000000000000023
    {0x3FDF'FFFB'CE42'17D3LL, 0x3fdd'ac63'aa63'5174LL, 0}, // 0.4999989999999999999
    {0x3FE0'0000'0000'006CLL, 0x3fdd'ac67'0561'bbfcLL, 0}, // 0.500000000000012
    {0x8000'0000'0000'0000LL, 0x3ff0'0000'0000'0000LL, 0}, // -0
    {0xBBDB'2752'CE74'FF42LL, 0xbbdb'2752'ce74'ff42LL, 0}, // -0.000000000000000000023
    {0xBFDF'FFFB'CE42'17D3LL, 0xbfdd'ac63'aa63'5174LL, 0}, // -0.4999989999999999999
    {0xBFE0'0000'0000'006CLL, 0xbfdd'ac67'0561'bbfcLL, 0}, // -0.500000000000012
    {0x3FF0'0000'0000'0000LL, 0x3fe9'21fb'5444'2d18LL, 0}, // 1
    {0x3FEF'FFFF'C49B'D0DCLL, 0x3fe9'21fb'3692'156bLL, 0}, // 0.9999998893750006
    {0x3FF0'0000'0000'0119LL, 0x3fe9'21fb'5444'2e31LL, 0}, // 1.0000000000000624998
    {0x3FF7'FFFE'F390'85F4LL, 0x3fef'730b'2d51'08f7LL, 0}, // 1.499998999999999967
    {0x3FF8'0000'0000'0001LL, 0x3fef'730b'd281'f69cLL, 0}, // 1.50000000000000012
    {0xBFF0'0000'0000'0000LL, 0xbfe9'21fb'5444'2d18LL, 0}, // -1
    {0xBFEF'FFFF'C49B'D0DCLL, 0xbfe9'21fb'3692'156bLL, 0}, // -0.9999998893750006
    {0xBFF0'0000'0000'0119LL, 0xbfe9'21fb'5444'2e31LL, 0}, // -1.0000000000000624998
    {0xBFF7'FFFE'F390'85F4LL, 0xbfef'730b'2d51'08f7LL, 0}, // -1.499998999999999967
    {0xBFF8'0000'0000'0001LL, 0xbfef'730b'd281'f69cLL, 0}, // -1.50000000000000012

    {0x4000'0000'0000'0000LL, 0x3ff1'b6e1'92eb'be44LL, 0}, // 2
    {0xC000'0000'0000'0000LL, 0xbff1'b6e1'92eb'be44LL, 0}, // -2
    {0x4024'0000'0000'0000LL, 0x3ff7'89bd'2c16'0054LL, 0}, // 10
    {0xC024'0000'0000'0000LL, 0xbff7'89bd'2c16'0054LL, 0}, // -10
    {0x4050'D999'9999'999ALL, 0x3ff8'e536'f691'7083LL, 0}, // 67.4
    {0xC050'D999'9999'999ALL, 0xbff8'e536'f691'7083LL, 0}, // -67.4
    {0x4055'E000'0000'0000LL, 0x3ff8'f32c'2009'dde7LL, 0}, // 87.5
    {0xC055'E000'0000'0000LL, 0xbff8'f32c'2009'dde7LL, 0}, // -87.5

    {0x0, 0x0, 0},
    {0x3e38'0000'0000'0000LL, 0x3e38'0000'0000'0000LL, 0},
    {0xbe38'0000'0000'0000LL, 0xbe38'0000'0000'0000LL, 0},
    {0x3fe9'21fb'5444'2d18LL, 0x3fe5'4e04'c05d'06a0LL, 0},
    {0x3f20'0000'0000'0001LL, 0x3f1f'ffff'fd55'5558LL, 0},
    {0x3e40'0000'0000'0001LL, 0x3e40'0000'0000'0001LL, 0},
    {0x7ffd'f000'0000'0000LL, 0x7ffd'f000'0000'0000LL, 0},
    {0xfffd'f000'0000'0000LL, 0xfffd'f000'0000'0000LL, 0},
    {0x7ff4'0000'0000'0000LL, 0x7ffc'0000'0000'0000LL, 0},
    {0xfff4'0000'0000'0000LL, 0xfffc'0000'0000'0000LL, 0},
    {0x3fe9'e0c8'f112'ab1eLL, 0x3fe5'c2b3'1b02'2df6LL, 0},
    {0x4030'6b51'f015'7e66LL, 0x3ff8'28d3'6549'69ffLL, 0},
    {0x402d'df5a'db92'c01aLL, 0x3ff8'1028'770a'abe4LL, 0},
    {0x402d'db77'8a9e'bd8aLL, 0x3ff8'1004'ea1b'89b6LL, 0},
    {0x401c'462b'9064'a63bLL, 0x3ff6'e254'e965'76c8LL, 0},
    {0x3fe9'21fb'5444'2d19LL, 0x3fe5'4e04'c05d'06a1LL, 0},
    {0x3fe9'21fb'5444'2d20LL, 0x3fe5'4e04'c05d'06a5LL, 0},
    {0x3ff1'0ca4'4655'd48aLL, 0x3fea'2601'964e'd0b3LL, 0},
    {0x4009'23e9'79f8'b36aLL, 0x3ff4'3413'8906'ebf1LL, 0},
    {0x4002'dae5'9bb5'c33eLL, 0x3ff2'b662'ea75'9ce2LL, 0},
    {0x4015'fdca'5f9a'0e38LL, 0x3ff6'4104'd68b'4ed8LL, 0},
    {0x40b9'3bda'357d'addaLL, 0x3ff9'2159'01d7'452bLL, 0},
    {0x40f6'3525'1292'91ffLL, 0x3ff9'21ef'cd34'4059LL, 0},
    {0x3ff9'2078'24b2'7c17LL, 0x3ff0'0f79'd904'1e2dLL, 0},
    {0x4025'fe9b'31eb'183dLL, 0x3ff7'ae8c'24fd'4ef8LL, 0},
    {0x4046'c6cb'c45d'c8deLL, 0x3ff8'c814'427d'58baLL, 0},
    {0x3ff0'0000'0000'0000LL, 0x3fe9'21fb'5444'2d18LL, 0},
    {0x4000'0000'0000'0000LL, 0x3ff1'b6e1'92eb'be44LL, 0},
    {0x4008'0000'0000'0000LL, 0x3ff3'fc17'6b7a'8560LL, 0},
    {0x4024'0000'0000'0000LL, 0x3ff7'89bd'2c16'0054LL, 0},
    {0xc000'0000'0000'0000LL, 0xbff1'b6e1'92eb'be44LL, 0},
    {0x3ff9'21fb'5444'2d18LL, 0x3ff0'0fe9'87ed'02ffLL, 0},
    {0x4012'd97c'7f33'21d2LL, 0x3ff5'c97d'37d9'8aa4LL, 0},
    {0x4019'21fb'5444'2d18LL, 0x3ff6'9b81'54ba'f42eLL, 0},
    {0x4029'21fb'5444'2d18LL, 0x3ff7'dcb7'c5c3'99ecLL, 0},
    {0x4109'21fb'5444'2d18LL, 0x3ff9'21f6'3c78'11a6LL, 0},
    {0x4039'21fb'5444'2d18LL, 0x3ff8'7f17'cfda'0b5dLL, 0},
    {0x4039'21fb'5444'2d19LL, 0x3ff8'7f17'cfda'0b5dLL, 0},
    {0x3ff9'21fb'5744'2d18LL, 0x3ff0'0fe9'88ca'80d4LL, 0},
    {0x4009'21fb'5244'2d18LL, 0x3ff4'33b8'a2c4'a8a9LL, 0},
    {0x4109'21fb'5644'2d18LL, 0x3ff9'21f6'3c78'120eLL, 0},
    {0xbff9'21fb'5744'2d18LL, 0xbff0'0fe9'88ca'80d4LL, 0},
    {0xc009'21fb'5444'2d18LL, 0xbff4'33b8'a322'ddd3LL, 0},
    {0x4009'21f5'5444'2d18LL, 0x3ff4'33b7'8883'24ddLL, 0},
    {0xc009'21f5'5444'2d18LL, 0xbff4'33b7'8883'24ddLL, 0},
    {0xbff9'21f5'5744'2d18LL, 0xbff0'0fe7'cdce'8b19LL, 0},
    {0xbff9'2175'5744'2d18LL, 0xbff0'0fc2'e2ee'0031LL, 0},
    {0x4009'21fb'5644'2d18LL, 0x3ff4'33b8'a381'12fcLL, 0},
    {0x4012'd98c'7f33'21d2LL, 0x3ff5'c97f'f9d8'c5f1LL, 0},
    {0x412e'848a'bcde'f000LL, 0x3ff9'21fa'47d5'1180LL, 0},
    {0x4393'3227'0327'f466LL, 0x3ff9'21fb'5444'2d18LL, 0},
    {0x411f'a317'083e'e0a2LL, 0x3ff9'21f9'4e64'8ff6LL, 0},
    {0x64ca'7f35'2f2a'fdaeLL, 0x3ff9'21fb'5444'2d18LL, 0},
    {0xd3d1'9620'2a79'1d3dLL, 0xbff9'21fb'5444'2d18LL, 0},
    {0x56fd'b2fb'3712'813bLL, 0x3ff9'21fb'5444'2d18LL, 0},
    {0x54e5'7b4e'03db'e9b3LL, 0x3ff9'21fb'5444'2d18LL, 0},
    {0xea96'be92'2b17'06c5LL, 0xbff9'21fb'5444'2d18LL, 0},
    {0x655e'8833'4694'4823LL, 0x3ff9'21fb'5444'2d18LL, 0},
};
