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
 * Test cases to check for exceptions for the fabsf() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_fabsf_conformance_data[] = {
   // special accuracy tests
   {0x38800000, 0x3f800000,  0},  //min= 0.00006103515625, small enough that fabs(x) = 1
   {0x387FFFFF, 0x3f800000,  0}, //min - 1 bit
   {0x38800001, 0x3f800000,  0}, //min + 1 bit
   {0xF149F2C9, 0x7f800000,  FE_OVERFLOW}, //lambda + x = 1, x = -9.9999994e+29
   {0xF149F2C8, 0x7f800000,  FE_OVERFLOW}, //lambda + x < 1
   {0xF149F2CA, 0x7f800000,  FE_OVERFLOW}, //lambda + x > 1
   {0x42B2D4FC, 0x7f7fffec,  0}, //max arg, x = 89.41598629223294,max fabsf arg
   {0x42B2D4FB, 0x7f7fff6c,  0}, //max arg - 1 bit
   {0x42B2D4FD, 0x7f800000,  FE_OVERFLOW}, //max arg + 1 bit
   {0x42B2D4FF, 0x7f800000,  FE_OVERFLOW}, // > max
   {0x42B2D400, 0x7f7f820b,  0}, // < max
   {0x41A00000, 0x4d675844,  0}, //small_threshold = 20
   {0x41A80000, 0x4e1d3710,  0}, //small_threshold+1 = 21
   {0x41980000, 0x4caa36c8,  0}, //small_threshold - 1 = 19

    //fabs special exception checks
   {POS_ZERO_F32, 0x3f800000,0 },  //0
   {NEG_ZERO_F32, 0x3f800000,0 },  //0
   {POS_INF_F32,  POS_INF_F32,0 },
   {NEG_INF_F32,  NEG_INF_F32,0 },
   {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID },  //
   {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID },  //
   {POS_QNAN_F32, POS_QNAN_F32, 0 },  //
   {NEG_QNAN_F32, NEG_QNAN_F32, 0 },  //
   {POS_INF_F32,  POS_INF_F32,  FE_OVERFLOW },  //95

   {0x00000001, 0x3f800000,  0},  // denormal min
   {0x0005fde6, 0x3f800000,  0},  // denormal intermediate
   {0x007fffff, 0x3f800000,  0},  // denormal max
   {0x80000001, 0x3f800000,  0},  // -denormal min
   {0x805def12, 0x3f800000,  0},  // -denormal intermediate
   {0x807FFFFF, 0x3f800000,  0},  // -denormal max
   {0x00800000, 0x3f800000,  0},  // normal min
   {0x43b3c4ea, 0x7f800000,  FE_OVERFLOW},  // normal intermediate
   {0x7f7fffff, 0x7f800000,  FE_OVERFLOW},  // normal max
   {0x80800000, 0x3f800000,  0},  // -normal min
   {0xc5812e71, 0x7f800000,  FE_OVERFLOW},  // -normal intermediate
   {0xFF7FFFFF, 0x7f800000,  FE_OVERFLOW},  // -normal max
   {0x7F800000, 0x7f800000,  0},  // inf
   {0xfF800000, 0x7f800000,  0},  // -inf
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
   {0x3FC90FDB, 0x40209662,  0},  // pi/2
   {0x40490FDB, 0x413978a5,  0},  // pi
   {0x40C90FDB, 0x4385df97,  0},  // 2pi
   {0x402DF853, 0x40f38620,  0},  // e --
   {0x402DF854, 0x40f38624,  0},  // e
   {0x402DF855, 0x40f38628,  0},  // e ++
   {0x00000000, 0x3f800000,  0},  // 0
   {0x37C0F01F, 0x3f800000,  0},  // 0.000023
   {0x3EFFFEB0, 0x3f9055e0,  0},  // 0.49999
   {0x3F0000C9, 0x3f905641,  0},  // 0.500012
   {0x80000000, 0x3f800000,  0},  // -0
   {0xb7C0F01F, 0x3f800000,  0},  // -0.000023
   {0xbEFFFEB0, 0x3f9055e0,  0},  // -0.49999
   {0xbF0000C9, 0x3f905641,  0},  // -0.500012
   {0x3f800000, 0x3fc583ab,  0},  // 1
   {0x3f700001, 0x3fbc7e14,  0},  // 0.93750006
   {0x3F87FFFE, 0x3fcf4ed3,  0},  // 1.0624998
   {0x3FBFFFAC, 0x40168d88,  0},  // 1.49999
   {0x3FC00064, 0x40168e4c,  0},  // 1.500012
   {0xbf800000, 0x3fc583ab,  0},  // -1
   {0xbf700001, 0x3fbc7e14,  0},  // -0.93750006
   {0xbF87FFFE, 0x3fcf4ed3,  0},  // -1.0624998
   {0xbFBFFFAC, 0x40168d88,  0},  // -1.49999
   {0xbFC00064, 0x40168e4c,  0},  // -1.500012

   {0xc0000000, 0x4070c7d0,  0},  // -2
   {0x41200000, 0x462c14ef,  0},  // 10
   {0xc1200000, 0x462c14ef,  0},  // -10
   {0x447A0000, 0x7f800000,  FE_OVERFLOW},  // 1000
   {0xc47A0000, 0x7f800000,  FE_OVERFLOW},  // -1000
   {0x4286CCCC, 0x6f96eb6f,  0},  // 67.4
   {0xc286CCCC, 0x6f96eb6f,  0},  // -67.4
   {0x44F7F333, 0x7f800000,  FE_OVERFLOW},  // 1983.6
   {0xc4F7F333, 0x7f800000,  FE_OVERFLOW},  // -1983.6
   {0x42AF0000, 0x7e16bab3,  0},  // 87.5
   {0xc2AF0000, 0x7e16bab3,  0},  // -87.5
   {0x48015E40, 0x7f800000,  FE_OVERFLOW},  // 132473
   {0xc8015E40, 0x7f800000,  FE_OVERFLOW},  // -132473
   {0x4B000000, 0x7f800000,  FE_OVERFLOW},  // 2^23
   {0x4B000001, 0x7f800000,  FE_OVERFLOW},  // 2^23 + 1
   {0x4AFFFFFF, 0x7f800000,  FE_OVERFLOW},  // 2^23 -1 + 0.5
   {0xcB000000, 0x7f800000,  FE_OVERFLOW},  // -2^23
   {0xcB000001, 0x7f800000,  FE_OVERFLOW},  // -(2^23 + 1)
   {0xcAFFFFFF, 0x7f800000,  FE_OVERFLOW},  // -(2^23 -1 + 0.5)
   //added from ancient libm repo
   {0x3e000000,0X3e000000}, // 0.125
   {0x3ec00000,0X3ec00000}, // 0.375
   {0x3f200000,0X3f200000}, // 0.625
   {0x3e800000,0X3e800000}, // 0.25
   {0x3f400000,0X3f400000}, // 0.75
   {0x3f000000,0X3f000000}, // 0.5 
   {0x3f800000,0X3f800000}, // 1
   {0x3fa00000,0X3fa00000}, // 1.25
   {0x3fc00000,0X3fc00000}, // 1.5 
   {0x40100000,0X40100000}, // 2.25
   {0x40200000,0X40200000}, // 2.5 
   {0x40800000,0X40800000}, // 4
   {0x40880000,0X40880000}, // 4.25
   {0x40900000,0X40900000}, // 4.5 
   {0x41000000,0X41000000}, // 8
   {0x41040000,0X41040000}, // 8.25
   {0x41080000,0X41080000}, // 8.5 
   {0x41800000,0X41800000}, // 16
   {0x41820000,0X41820000}, // 16.25
   {0x41840000,0X41840000}, // 16.5
   {0x42000000,0X42000000}, // 32
   {0x42010000,0X42010000}, // 32.25
   {0x42020000,0X42020000}, // 32.5
   {0x42800000,0X42800000}, // 64
   {0x42808000,0X42808000}, // 64.25
   {0x42810000,0X42810000}, // 64.5
   {0x43000000,0X43000000}, // 128 
   {0x43004000,0X43004000}, // 128.25
   {0x43008000,0X43008000}, // 128.5
   {0x43800000,0X43800000}, // 256 
   {0x43802000,0X43802000}, // 256.25
   {0x43804000,0X43804000}, // 256.5
   {0x44000000,0X44000000}, // 512 
   {0x44001000,0X44001000}, // 512.25
   {0x44002000,0X44002000}, // 512.5
   {0x44800000,0X44800000}, // 1024
   {0x44800800,0X44800800}, // 1024.25
   {0x44801000,0X44801000}, // 1024.5
   {0x45000000,0X45000000}, // 2048
   {0x45000400,0X45000400}, // 2048.25
   {0x45000800,0X45000800}, // 2048.5
   {0x45800000,0X45800000}, // 4096
   {0x45800200,0X45800200}, // 4096.25
   {0x45800400,0X45800400}, // 4096.5
   {0x46000000,0X46000000}, // 8192
   {0x46000100,0X46000100}, // 8192.25
   {0x46000200,0X46000200}, // 8192.5
   {0x46800000,0X46800000}, // 16384
   {0x46800080,0X46800080}, // 16384.2
   {0x46800100,0X46800100}, // 16384.5
   {0x47000000,0X47000000}, // 32768
   {0x47000040,0X47000040}, // 32768.2
   {0x47000080,0X47000080}, // 32768.5
   {0x47800000,0X47800000}, // 65536
   {0x47800020,0X47800020}, // 65536.2
   {0x47800040,0X47800040}, // 65536.5
   {0x48000000,0X48000000}, // 131072
   {0x48000010,0X48000010}, // 131072
   {0x48000020,0X48000020}, // 131072
   {0x48800000,0X48800000}, // 262144
   {0x48800008,0X48800008}, // 262144
   {0x48800010,0X48800010}, // 262144
   {0x49000000,0X49000000}, // 524288
   {0x49000004,0X49000004}, // 524288
   {0x49000008,0X49000008}, // 524288
   {0x49800000,0X49800000}, // 1.04858e+06
   {0x49800002,0X49800002}, // 1.04858e+06
   {0x49800004,0X49800004}, // 1.04858e+06
   {0x4a000000,0X4a000000}, // 2.09715e+06
   {0x4a000001,0X4a000001}, // 2.09715e+06
   {0x4a000002,0X4a000002}, // 2.09715e+06
   {0x4a800000,0X4a800000}, // 4.1943e+06
   {0x4b000000,0X4b000000}, // 8.38861e+06
   {0x4b800000,0X4b800000}, // 1.67772e+07
   {0x4c000000,0X4c000000}, // 3.35544e+07
   {0x4c800000,0X4c800000}, // 6.71089e+07
   {0x4d000000,0X4d000000}, // 1.34218e+08
   {0x4d800000,0X4d800000}, // 2.68435e+08
   {0x4e000000,0X4e000000}, // 5.36871e+08
   {0x4e800000,0X4e800000}, // 1.07374e+09
   {0x4f000000,0X4f000000}, // 2.14748e+09
   {0x4f800000,0X4f800000}, // 4.29497e+09
   {0x50000000,0X50000000}, // 8.58993e+09
   {0x50800000,0X50800000}, // 1.71799e+10
   {0x51000000,0X51000000}, // 3.43597e+10
   {0x1,0X1}, // 1.4013e-45
   {0x5fde6,0X5fde6}, // 5.50259e-40
   {0x7fffff,0X7fffff}, // 1.17549e-38
   {0x80000001,0X1}, // -1.4013e-45
   {0x805def12,0X5def12}, // -8.62646e-39
   {0x807fffff,0X7fffff}, // -1.17549e-38
   {0x800000,0X800000}, // 1.17549e-38
   {0x43b3c4ea,0X43b3c4ea}, // 359.538
   {0x80800000,0X800000}, // -1.17549e-38
   {0xc5812e71,0X45812e71}, // -4133.81
   {0xff7fffff,0X7f7fffff}, // -3.40282e+38
   {0xff800000,0X7f800000}, // -inf
   {0xffc00000,0X7fc00000}, // nan
   {0xffc00001,0X7fc00001}, // nan
   {0xffd2ba31,0X7fd2ba31}, // nan
   {0xffffffff,0X7fffffff}, // nan
   {0xff800001,0X7f800001}, // nan
   {0xff95fffa,0X7f95fffa}, // nan
   {0xffbfffff,0X7fbfffff}, // nan
   {0x3fc90fdb,0X3fc90fdb}, // 1.5708
   {0x40490fdb,0X40490fdb}, // 3.14159
   {0x40c90fdb,0X40c90fdb}, // 6.28319
   {0x402df853,0X402df853}, // 2.71828
   {0x402df854,0X402df854}, // 2.71828
   {0x402df855,0X402df855}, // 2.71828
   {0x0,0X0}, // 0
   {0x37c0f01f,0X37c0f01f}, // 2.3e-05
   {0x3efffeb0,0X3efffeb0}, // 0.49999
   {0x3f0000c9,0X3f0000c9}, // 0.500012
   {0x80000000,0X0}, // -0
   {0xb7c0f01f,0X37c0f01f}, // -2.3e-05
   {0xbefffeb0,0X3efffeb0}, // -0.49999
   {0xbf0000c9,0X3f0000c9}, // -0.500012
   {0x3f800000,0X3f800000}, // 1
   {0x3f700001,0X3f700001}, // 0.9375
   {0x3f87fffe,0X3f87fffe}, // 1.0625
   {0x3fbfffac,0X3fbfffac}, // 1.49999
   {0x3fc00064,0X3fc00064}, // 1.50001
   {0xbf800000,0X3f800000}, // -1
   {0xbf700001,0X3f700001}, // -0.9375
   {0xbf87fffe,0X3f87fffe}, // -1.0625
   {0xbfbfffac,0X3fbfffac}, // -1.49999
   {0xbfc00064,0X3fc00064}, // -1.50001
   {0x40000000,0X40000000}, // 2
   {0xc0000000,0X40000000}, // -2
   {0x41200000,0X41200000}, // 10
   {0xc1200000,0X41200000}, // -10
   {0x447a0000,0X447a0000}, // 1000
   {0xc47a0000,0X447a0000}, // -1000
   {0x4286cccc,0X4286cccc}, // 67.4
   {0xc286cccc,0X4286cccc}, // -67.4
   {0x44f7f333,0X44f7f333}, // 1983.6
   {0xc4f7f333,0X44f7f333}, // -1983.6
   {0x42af0000,0X42af0000}, // 87.5
   {0xc2af0000,0X42af0000}, // -87.5
   {0x48015e40,0X48015e40}, // 132473
   {0xc8015e40,0X48015e40}, // -132473
   {0x4b000001,0X4b000001}, // 8.38861e+06
   {0x4affffff,0X4affffff}, // 8.38861e+06
   {0xcb000000,0X4b000000}, // -8.38861e+06
   {0xcb000001,0X4b000001}, // -8.38861e+06
   {0xcaffffff,0X4affffff}, // -8.38861e+06
   {0x7f7fffff,0X7f7fffff}, // 3.40282e+38
   {0xff800000,0X7f800000}, // -inf
   {0x7fe00000,0X7fe00000}, // nan
   {0x7fb00000,0X7fb00000}, // nan
   {0x80000000,0X0}, // -0
   {0x7f800000,0X7f800000}, // inf
   {0x7fc00000,0X7fc00000}, // nan
   {0x7fe1a570,0X7fe1a570}, // nan
   {0x7fffffff,0X7fffffff}, // nan
   {0x7f800001,0X7f800001}, // nan
   {0x7fa0bd90,0X7fa0bd90}, // nan
   {0x7fbfffff,0X7fbfffff}, // nan

   {0x50a3e87f, 0x50a3e87f },  //2.19993846E+10, close to pi/2
   {0x49486fa2, 0x49486fa2 },  //8.20986125E+05, close to 0
   {0x3F000000, 0x3F000000 },  //0.5
   {0xC07AE148, 0x407AE148 },  //-3.92	
   {0xc7f12000, 0x47f12000 },  //-123456
   {0xC8C35000, 0x48C35000 },  //-4e5


};

static libm_test_special_data_f64
test_fabs_conformance_data[] = {
    // special accuracy tests
    {0x3e30000000000000LL, 0x3ff0000000000000LL, 0},  //min, small enough that fabs(x) = 1 //
    {0x3E2FFFFFFFFFFFFFLL, 0x3ff0000000000000LL, 0}, //min - 1 bit
    {0x3e30000000000001LL, 0x3ff0000000000000LL, 0}, //min + 1 bit
    {0xFE37E43C8800759CLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x = 1, x = -1.0000000000000000e+300
    {0xFE37E43C8800758CLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x < 1
    {0xFE37E43C880075ACLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x > 1
    {0x408633ce8fb9f87eLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //max arg, x = 89.41598629223294,max fabsf arg
    {0x408633ce8fb9f87dLL, 0x7feffffffffffd3bLL, 0}, //max arg - 1 bit
    {0x408633ce8fb9f87fLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //max arg + 1 bit
    {0x408633ce8fb9f8ffLL, 0x7ff0000000000000LL, FE_OVERFLOW}, // > max
    {0x408633ce8fb9f800LL, 0x7feffffffffe093bLL, 0}, // < max
    {0x4034000000000000LL, 0x41aceb088b68e804LL, 0}, //small_threshold = 20
    {0x4035000000000000LL, 0x41c3a6e1fd9eecfdLL, 0}, //small_threshold+1 = 21
    {0x4033000000000000LL, 0x419546d8f9ed26e2LL, 0}, //small_threshold - 1 = 19

    //fabs special exception checks
    {POS_ZERO_F64, 0x3FF0000000000000LL,0 },  //0
    {NEG_ZERO_F64, 0x3FF0000000000000LL,0 },  //0
    {POS_INF_F64, POS_INF_F64,   0 },
    {NEG_INF_F64, POS_INF_F64,   0 },
    {POS_SNAN_F64, POS_SNAN_F64, FE_INVALID },  //
    {NEG_SNAN_F64, NEG_SNAN_F64, FE_INVALID },  //
    {POS_QNAN_F64, POS_QNAN_F64, 0 },  //
    {NEG_QNAN_F64, NEG_QNAN_F64, 0 },  //
    {0x4086340000000000LL, POS_INF_F64, FE_OVERFLOW},  // 710.5
    {0x0000000000000001LL, 0x3ff0000000000000LL, 0}, // denormal min
    {0x0005fde623545abcLL, 0x3ff0000000000000LL, 0}, // denormal intermediate
    {0x000FFFFFFFFFFFFFLL, 0x3ff0000000000000LL, 0}, // denormal max
    {0x8000000000000001LL, 0x3ff0000000000000LL, 0}, // -denormal min
    {0x8002344ade5def12LL, 0x3ff0000000000000LL, 0}, // -denormal intermediate
    {0x800FFFFFFFFFFFFFLL, 0x3ff0000000000000LL, 0}, // -denormal max
    {0x0010000000000000LL, 0x3ff0000000000000LL, 0}, // normal min
    {0x43b3c4eafedcab02LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // normal intermediate
    {0x7FEFFFFFFFFFFFFFLL, 0x7ff0000000000000LL, FE_OVERFLOW}, // normal max
    {0x8010000000000000LL, 0x3ff0000000000000LL, 0}, // -normal min
    {0xc5812e71245acfdbLL, 0x7ff0000000000000LL, FE_OVERFLOW}, // -normal intermediate
    {0xFFEFFFFFFFFFFFFFLL, 0x7ff0000000000000LL, FE_OVERFLOW}, // -normal max
    {0x7FF0000000000000LL, 0x7ff0000000000000LL, 0}, // inf
    {0xFFF0000000000000LL, 0x7ff0000000000000LL, 0}, // -inf
    {0x7FF8000000000000LL, 0x7ff8000000000000LL, 0}, // qnan min
    {0x7FFe1a5701234dc3LL, 0x7ffe1a5701234dc3LL, 0}, // qnan intermediate
    {0x7FFFFFFFFFFFFFFFLL, 0x7fffffffffffffffLL, 0}, // qnan max
    {0xFFF8000000000000LL, 0xfff8000000000000LL, 0}, // indeterninate
    {0xFFF8000000000001LL, 0xfff8000000000001LL, 0}, // -qnan min
    {0xFFF9123425dcba31LL, 0xfff9123425dcba31LL, 0}, // -qnan intermediate
    {0xFFFFFFFFFFFFFFFFLL, 0xffffffffffffffffLL, 0}, // -qnan max
    {0x7FF0000000000001LL, 0x7ff8000000000001LL, FE_INVALID}, // snan min
    {0x7FF5344752a0bd90LL, 0x7ffd344752a0bd90LL, FE_INVALID}, // snan intermediate
    {0x7FF7FFFFFFFFFFFFLL, 0x7fffffffffffffffLL, FE_INVALID}, // snan max
    {0xFFF0000000000001LL, 0xfff8000000000001LL, FE_INVALID}, // -snan min
    {0xfFF432438995fffaLL, 0xfffc32438995fffaLL, FE_INVALID}, // -snan intermediate
    {0xFFF7FFFFFFFFFFFFLL, 0xffffffffffffffffLL, FE_INVALID}, // -snan max
    {0x3FF921FB54442D18LL, 0x400412cc2a8d4e9eLL, 0}, // pi/2
    {0x400921FB54442D18LL, 0x40272f147fee4000LL, 0}, // pi
    {0x401921FB54442D18LL, 0x4070bbf2bc2b69c6LL, 0}, // 2pi
    {0x3FFB7E151628AED3LL, 0x4007046b4f3f7e0cLL, 0}, // e --
    {0x4005BF0A8B145769LL, 0x401e70c4a4f41684LL, 0}, // e
    {0x400DBF0A8B145769LL, 0x40349be1e586228dLL, 0}, // e ++
    {0x0000000000000000LL, 0x3ff0000000000000LL, 0}, // 0
    {0x3C4536B8B14B676CLL, 0x3ff0000000000000LL, 0}, // 0.0000000000000000023
    {0x3FDFFFFBCE4217D3LL, 0x3ff20ac0fa498d2cLL, 0}, // 0.4999989999999999999
    {0x3FE000000000006CLL, 0x3ff20ac1862ae8edLL, 0}, // 0.500000000000012
    {0x8000000000000000LL, 0x3ff0000000000000LL, 0}, // -0
    {0xBBDB2752CE74FF42LL, 0x3ff0000000000000LL, 0}, // -0.000000000000000000023
    {0xBFDFFFFBCE4217D3LL, 0x3ff20ac0fa498d2cLL, 0}, // -0.4999989999999999999
    {0xBFE000000000006CLL, 0x3ff20ac1862ae8edLL, 0}, // -0.500000000000012
    {0x3FF0000000000000LL, 0x3ff8b07551d9f550LL, 0}, // 1
    {0x3FEFFFFFC49BD0DCLL, 0x3ff8b0752ef3f8a2LL, 0}, // 0.9999998893750006
    {0x3FF0000000000119LL, 0x3ff8b07551d9f69bLL, 0}, // 1.0000000000000624998
    {0x3FF7FFFEF39085F4LL, 0x4002d1bb0418ae52LL, 0}, // 1.499998999999999967
    {0x3FF8000000000001LL, 0x4002d1bc21e22023LL, 0}, // 1.50000000000000012
    {0xBFF0000000000000LL, 0x3ff8b07551d9f550LL, 0}, // -1
    {0xBFEFFFFFC49BD0DCLL, 0x3ff8b0752ef3f8a2LL, 0}, // -0.9999998893750006
    {0xBFF0000000000119LL, 0x3ff8b07551d9f69bLL, 0}, // -1.0000000000000624998
    {0xBFF7FFFEF39085F4LL, 0x4002d1bb0418ae52LL, 0}, // -1.499998999999999967
    {0xBFF8000000000001LL, 0x4002d1bc21e22023LL, 0}, // -1.50000000000000012

    {0x4000000000000000LL, 0x400e18fa0df2d9bcLL, 0}, // 2
    {0xC000000000000000LL, 0x400e18fa0df2d9bcLL, 0}, // -2
    {0x4024000000000000LL, 0x40c5829dd053712dLL, 0}, // 10
    {0xC024000000000000LL, 0x40c5829dd053712dLL, 0}, // -10
    {0x408F400000000000LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // 1000
    {0xC08F400000000000LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // -1000
    {0x4050D9999999999ALL, 0x45f2dd7567cd83eeLL, 0}, // 67.4
    {0xC050D9999999999ALL, 0x45f2dd7567cd83eeLL, 0}, // -67.4
    {0x409EFE6666666666LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // 1983.6
    {0xC09EFE6666666666LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // -1983.6
    {0x4055E00000000000LL, 0x47c2d7566d26536bLL, 0}, // 87.5
    {0xC055E00000000000LL, 0x47c2d7566d26536bLL, 0}, // -87.5
    {0x41002BC800000000LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // 132473
    {0xC1002BC800000000LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // -132473
    {0x4330000000000000LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // 2^52
    {0x4330000000000001LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // 2^52 + 1
    {0x432FFFFFFFFFFFFFLL, 0x7ff0000000000000LL, FE_OVERFLOW}, // 2^52 -1 + 0.5
    {0xC330000000000000LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // -2^52
    {0xC330000000000001LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // -(2^52 + 1)
    {0xC32FFFFFFFFFFFFFLL, 0x7ff0000000000000LL, FE_OVERFLOW}, // -(2^52 -1 + 0.5)
    //added from ancient libm repo
    {0x3ff921fb54442d18, 0X3ff921fb54442d18}, // 1.5708
    {0x400921fb54442d18, 0X400921fb54442d18}, // 3.14159
    {0x401921fb54442d18, 0X401921fb54442d18}, // 6.28319
    {0x3ffb7e151628aed3, 0X3ffb7e151628aed3}, // 1.71828
    {0x4005bf0a8b145769, 0X4005bf0a8b145769}, // 2.71828
    {0x400dbf0a8b145769, 0X400dbf0a8b145769}, // 3.71828
    {0x0, 0X0}, // 0
    {0x3c4536b8b14b676c, 0X3c4536b8b14b676c}, // 2.3e-18
    {0x3fdffffbce4217d3, 0X3fdffffbce4217d3}, // 0.499999
    {0x3fe000000000006c, 0X3fe000000000006c}, // 0.5
    {0x8000000000000000, 0X0}, // -0
    {0xbbdb2752ce74ff42, 0X3bdb2752ce74ff42}, // -2.3e-20
    {0xbfdffffbce4217d3, 0X3fdffffbce4217d3}, // -0.499999
    {0xbfe000000000006c, 0X3fe000000000006c}, // -0.5
    {0x3ff0000000000000, 0X3ff0000000000000}, // 1
    {0x3fefffffc49bd0dc, 0X3fefffffc49bd0dc}, // 1
    {0x3ff0000000000119, 0X3ff0000000000119}, // 1
    {0x3ff7fffef39085f4, 0X3ff7fffef39085f4}, // 1.5
    {0x3ff8000000000001, 0X3ff8000000000001}, // 1.5
    {0xbff0000000000000, 0X3ff0000000000000}, // -1
    {0xbfefffffc49bd0dc, 0X3fefffffc49bd0dc}, // -1
    {0xbff0000000000119, 0X3ff0000000000119}, // -1
    {0xbff7fffef39085f4, 0X3ff7fffef39085f4}, // -1.5
    {0xbff8000000000001, 0X3ff8000000000001}, // -1.5
    {0x4000000000000000, 0X4000000000000000}, // 2
    {0xc000000000000000, 0X4000000000000000}, // -2
    {0x4024000000000000, 0X4024000000000000}, // 10
    {0xc024000000000000, 0X4024000000000000}, // -10
    {0x408f400000000000, 0X408f400000000000}, // 1000
    {0xc08f400000000000, 0X408f400000000000}, // -1000
    {0x4050d9999999999a, 0X4050d9999999999a}, // 67.4
    {0xc050d9999999999a, 0X4050d9999999999a}, // -67.4
    {0x409efe6666666666, 0X409efe6666666666}, // 1983.6
    {0xc09efe6666666666, 0X409efe6666666666}, // -1983.6
    {0x4055e00000000000, 0X4055e00000000000}, // 87.5
    {0xc055e00000000000, 0X4055e00000000000}, // -87.5
    {0x41002bc800000000, 0X41002bc800000000}, // 132473
    {0xc1002bc800000000, 0X41002bc800000000}, // -132473
    {0x4330000000000000, 0X4330000000000000}, // 4.5036e+15
    {0x4330000000000001, 0X4330000000000001}, // 4.5036e+15
    {0x432fffffffffffff, 0X432fffffffffffff}, // 4.5036e+15
    {0xc330000000000000, 0X4330000000000000}, // -4.5036e+15
    {0xc330000000000001, 0X4330000000000001}, // -4.5036e+15
    {0xc32fffffffffffff, 0X432fffffffffffff}, // -4.5036e+15
    {0x43480677adfcfa00, 0X43480677adfcfa00}, // 1.3525e+16
    {0x435801239871fa00, 0X435801239871fa00}, // 2.70266e+16
    {0x4368111144444444, 0X4368111144444444}, // 5.41933e+16
    {0x4388ffffffffffff, 0X4388ffffffffffff}, // 2.2518e+17
    {0x4398ffffffffffff, 0X4398ffffffffffff}, // 4.5036e+17
    {0x43a8ffffffffffff, 0X43a8ffffffffffff}, // 9.0072e+17
    {0x43b8aaaa68ffffff, 0X43b8aaaa68ffffff}, // 1.77742e+18
    {0x43c8aaaa69ffffff, 0X43c8aaaa69ffffff}, // 3.55484e+18
    {0x43d8009678abcd00, 0X43d8009678abcd00}, // 6.91819e+18
    {0x7fefffffffffffff, 0X7fefffffffffffff}, // 1.79769e+308
    {0x43e8098734209870, 0X43e8098734209870}, // 1.38565e+19
    {0x43f800876abcda00, 0X43f800876abcda00}, // 2.76725e+19
    {0x44080091781cdba0, 0X44080091781cdba0}, // 5.53454e+19
    {0x4408012786abcde0, 0X4408012786abcde0}, // 5.53506e+19
    {0x43b3c4eafedcab02, 0X43b3c4eafedcab02} // 1.42452e+18


};

