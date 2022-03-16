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
 * Test cases to check for exceptions for the sqrtf() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_sqrtf_conformance_data[] = {
   // special accuracy tests
   {0x38800000, 0x3f800000,  0},  //min= 0.00006103515625, small enough that sqrt(x) = 1
   {0x387FFFFF, 0x3f800000,  0}, //min - 1 bit
   {0x38800001, 0x3f800000,  0}, //min + 1 bit
   {0xF149F2C9, 0x7f800000,  FE_OVERFLOW}, //lambda + x = 1, x = -9.9999994e+29
   {0xF149F2C8, 0x7f800000,  FE_OVERFLOW}, //lambda + x < 1
   {0xF149F2CA, 0x7f800000,  FE_OVERFLOW}, //lambda + x > 1
   {0x42B2D4FC, 0x7f7fffec,  0}, //max arg, x = 89.41598629223294,max sqrtf arg
   {0x42B2D4FB, 0x7f7fff6c,  0}, //max arg - 1 bit
   {0x42B2D4FD, 0x7f800000,  FE_OVERFLOW}, //max arg + 1 bit
   {0x42B2D4FF, 0x7f800000,  FE_OVERFLOW}, // > max
   {0x42B2D400, 0x7f7f820b,  0}, // < max
   {0x41A00000, 0x4d675844,  0}, //small_threshold = 20
   {0x41A80000, 0x4e1d3710,  0}, //small_threshold+1 = 21
   {0x41980000, 0x4caa36c8,  0}, //small_threshold - 1 = 19

    //sqrt special exception checks
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
   {0xb3000000, 0xffc00000,  33},  //-2.98023e-08
   {0xb2e47e05, 0xffc00000,  33},  //-2.66e-08
   {0xb2c8fc0a, 0xffc00000,  33},  //-2.33977e-08
   {0xb2ad7a0f, 0xffc00000,  33},  //-2.01954e-08
   {0xb291f814, 0xffc00000,  33},  //-1.6993e-08
   {0xb26cec32, 0xffc00000,  33},  //-1.37907e-08
   {0xb235e83c, 0xffc00000,  33},  //-1.05884e-08
   {0xb1fdc88b, 0xffc00000,  33},  //-7.38607e-09
   {0xb18fc09e, 0xffc00000,  33},  //-4.18375e-09
   {0xb086e2c4, 0xffc00000,  33},  //-9.81423e-10
   {0x31189e78, 0x3845a99a,  0},  //2.2209e-09
   {0x31ba5729, 0x389a7088,  0},  //5.42322e-09
   {0x32142f8b, 0x38c2c53f,  0},  //8.62554e-09
   {0x324b3381, 0x38e413ee,  0},  //1.18279e-08
   {0x32811bbc, 0x39008d90,  0},  //1.50302e-08
   {0x329c9db7, 0x390d9641,  0},  //1.82325e-08
   {0x32b81fb2, 0x391984ab,  0},  //2.14348e-08
   {0x32d3a1ad, 0x3924963d,  0},  //2.46372e-08
   {0x32ef23a8, 0x392ef4e7,  0},  //2.78395e-08
   {0x436c5cfa, 0x4175fc58,  0},  //236.363
   {0x43715cfa, 0x417892ea,  0},  //241.363
   {0x43765cfa, 0x417b22a8,  0},  //246.363
   {0x437b5cfa, 0x417dabc7,  0},  //251.363
   {0x43802e7d, 0x4180173c,  0},  //256.363
   {0x4382ae7d, 0x41815577,  0},  //261.363
   {0x43852e7d, 0x418290aa,  0},  //266.363
   {0x4387ae7d, 0x4183c8eb,  0},  //271.363
   {0x438a2e7d, 0x4184fe4f,  0},  //276.363
   {0x438cae7d, 0x418630ea,  0},  //281.363
   {0x438f2e7d, 0x418760cf,  0},  //286.363
   {0x4391ae7d, 0x41888e0f,  0},  //291.363
   {0x43942e7d, 0x4189b8bc,  0},  //296.363
   {0x4398ab85, 0x418bcab6,  0},  //305.34
   {0x459a800b, 0x428ca093,  0},  //4944.01
   {0x4615baaf, 0x42c3c841,  0},  //9582.67
   {0x465e3558, 0x42ee81a9,  0},  //14221.3
   {0x46935801, 0x430954ec,  0},  //18860
   {0x46b79556, 0x43194af2,  0},  //23498.7
   {0x46dbd2ab, 0x4327bdeb,  0},  //28137.3
   {0x47000800, 0x43350a9b,  0},  //32776
   {0x471226aa, 0x43416dc0,  0},  //37414.7
   {0x47244554, 0x434d11af,  0},  //42053.3
   {0x473663fe, 0x43581553,  0},  //46692
   {0xc788b83a, 0xffc00000,  33},  //-70000.5
   {0xc7812c04, 0xffc00000,  33},  //-66136
   {0xc7733f9c, 0xffc00000,  33},  //-62271.6
   {0xc7642730, 0xffc00000,  33},  //-58407.2
   {0xc7550ec4, 0xffc00000,  33},  //-54542.8
   {0xc745f658, 0xffc00000,  33},  //-50678.3
   {0xc736ddec, 0xffc00000,  33},  //-46813.9
   {0xc727c580, 0xffc00000,  33},  //-42949.5
   {0xc718ad14, 0xffc00000,  33},  //-39085.1
   {0xc70994a8, 0xffc00000,  33},  //-35220.7
   {0xc6f4f877, 0xffc00000,  33},  //-31356.2
   {0xc6d6c79e, 0xffc00000,  33},  //-27491.8
   {0xc6b896c5, 0xffc00000,  33},  //-23627.4
   {0xc69a65ec, 0xffc00000,  33},  //-19763
   {0xc6786a26, 0xffc00000,  33},  //-15898.5
   {0xc63c0874, 0xffc00000,  33},  //-12034.1
   {0xc5ff4d85, 0xffc00000,  33},  //-8169.69
   {0xc5868a22, 0xffc00000,  33},  //-4305.27
   {0xc3dc6bee, 0xffc00000,  33},  //-440.843
   {0xbe934b11, 0xffc00000,  33},  //-0.287682
   {0xbe86e2ab, 0xffc00000,  33},  //-0.263448
   {0xbe74f48a, 0xffc00000,  33},  //-0.239214
   {0xbe5c23be, 0xffc00000,  33},  //-0.21498
   {0xbe4352f2, 0xffc00000,  33},  //-0.190746
   {0xbe2a8226, 0xffc00000,  33},  //-0.166512
   {0xbe11b15a, 0xffc00000,  33},  //-0.142278
   {0xbdf1c11c, 0xffc00000,  33},  //-0.118044
   {0xbdc01f84, 0xffc00000,  33},  //-0.0938101
   {0xbd8e7dec, 0xffc00000,  33},  //-0.0695761
   {0xbd39b8a7, 0xffc00000,  33},  //-0.0453421
   {0xbcaceaec, 0xffc00000,  33},  //-0.0211081
   {0x3b4cdbad, 0x3d65017f,  0},  //0.00312589
   {0x3ce021d7, 0x3e2960c7,  0},  //0.0273599
   {0x3d53541c, 0x3e689824,  0},  //0.0515939
   {0x3d9b4ba6, 0x3e8cfd1e,  0},  //0.0758279
   {0x3dcced3e, 0x3ea1f56d,  0},  //0.100062
   {0x3dfe8ed6, 0x3eb4823f,  0},  //0.124296
   {0x3e181837, 0x3ec55297,  0},  //0.14853
   {0x3e30e903, 0x3ed4cfe9,  0},  //0.172764
   {0x3e49b9cf, 0x3ee33f93,  0},  //0.196998
   {0x3e628a9b, 0x3ef0d219,  0},  //0.221232
   {0xc173b646, 0xffc00000,  33},  //-15.232
   {0xc160ec1f, 0xffc00000,  33},  //-14.0576
   {0xc14e21f8, 0xffc00000,  33},  //-12.8833
   {0xc13b57d1, 0xffc00000,  33},  //-11.7089
   {0xc1288daa, 0xffc00000,  33},  //-10.5346
   {0xc115c383, 0xffc00000,  33},  //-9.36023
   {0xc102f95c, 0xffc00000,  33},  //-8.18588
   {0xc0e05e69, 0xffc00000,  33},  //-7.01152
   {0xc0baca1a, 0xffc00000,  33},  //-5.83717
   {0xc09535cb, 0xffc00000,  33},  //-4.66282
   {0xc05f42f8, 0xffc00000,  33},  //-3.48846
   {0xc0141a5a, 0xffc00000,  33},  //-2.31411
   {0xbf91e379, 0xffc00000,  33},  //-1.13975
   {0x3d0db84b, 0x3e3e794f,  0},  //0.0345996
   {0x3f9abefe, 0x3f8cbd36,  0},  //1.20895
   {0x4018881d, 0x3fc59b20,  0},  //2.38331
   {0x4063b0bb, 0x3ff16e3b,  0},  //3.55766
   {0x40976cac, 0x400b3870,  0},  //4.73202
   {0x40bd00fb, 0x401b8a13,  0},  //5.90637
   {0x40e2954a, 0x402a4d37,  0},  //7.08072
   {0x410414cc, 0x4037e1f0,  0},  //8.25508
   {0x4116def3, 0x404486f8,  0},  //9.42943
   {0x4129a91a, 0x405067f6,  0},  //10.6038
   {0x413c7341, 0x405ba4ba,  0},  //11.7781
   {0x414f3d68, 0x40665550,  0},  //12.9525
   {0x4162078f, 0x40708c67,  0},  //14.1268
   {0x4174d1b6, 0x407a58e1,  0},  //15.3012
   {0x4173b646, 0x4079c7cc,  0},  //15.232
   {0x41d0dde9, 0x40a3821e,  0},  //26.1084
   {0x4213f057, 0x40c29bb1,  0},  //36.9847
   {0x423f71ba, 0x40dd61a4,  0},  //47.8611
   {0x426af31d, 0x40f53fc4,  0},  //58.7374
   {0x428b3a40, 0x41057eec,  0},  //69.6138
   {0x42a0faf1, 0x410f8bca,  0},  //80.4901
   {0x42B16D62, 0x4116b35f,  0},  //88.713638
   {0x42B19999, 0x4116c625,  0},  //88.799995
   {0x42b6bba2, 0x4118eff3,  0},  //91.3665
   {0x42cc7c53, 0x4121c8c8,  0},  //102.243
   {0x42e23d04, 0x412a2c08,  0},  //113.119
   {0x42f7fdb5, 0x41322a4d,  0},  //123.996
   {0x4306df33, 0x4139d0a1,  0},  //134.872
   {0x4311bf8c, 0x41412977,  0},  //145.748
   {0x431c9fe5, 0x41483d56,  0},  //156.625
   {0x4327803e, 0x414f1351,  0},  //167.501
   {0x43326097, 0x4155b158,  0},  //178.377
   {0x433d40f0, 0x415c1c77,  0},  //189.254
   {0x43482149, 0x41625903,  0},  //200.13
   {0x435301a2, 0x41686abc,  0},  //211.006
   {0x435de1fb, 0x416e54e8,  0},  //221.883
   {0x4368c254, 0x41741a65,  0},  //232.759
   {0xc18b6666, 0xffc00000,  33},  //-17.425
   {0xc18521ed, 0xffc00000,  33},  //-16.6416
   {0xc17dbae8, 0xffc00000,  33},  //-15.8581
   {0xc17131f6, 0xffc00000,  33},  //-15.0747
   {0xc164a904, 0xffc00000,  33},  //-14.2913
   {0xc1582012, 0xffc00000,  33},  //-13.5078
   {0xc14b9720, 0xffc00000,  33},  //-12.7244
   {0xc13f0e2e, 0xffc00000,  33},  //-11.941
   {0xc132853c, 0xffc00000,  33},  //-11.1575
   {0xc125fc4a, 0xffc00000,  33},  //-10.3741
   {0xc1197358, 0xffc00000,  33},  //-9.59066
   {0xc10cea66, 0xffc00000,  33},  //-8.80723
   {0xc1006174, 0xffc00000,  33},  //-8.02379
   {0xc0e7b104, 0xffc00000,  33},  //-7.24036
   {0xc0ce9f20, 0xffc00000,  33},  //-6.45692
   {0xc0b58d3c, 0xffc00000,  33},  //-5.67349
   {0xc09c7b58, 0xffc00000,  33},  //-4.89006
   {0xc0836974, 0xffc00000,  33},  //-4.10662
   {0xc0640b78, 0xffc00000,  33},  //-3.5632
   {0xc02617ec, 0xffc00000,  33},  //-2.59521
   {0xbfd048bf, 0xffc00000,  33},  //-1.62722
   {0xbf28c34d, 0xffc00000,  33},  //-0.65923
   {0x3e9e15c9, 0x3f0e3fd9,  0},  //0.30876
   {0x3fa36c8b, 0x3f90a1aa,  0},  //1.27675
   {0x400fa9d2, 0x3fbfc683,  0},  //2.24474
   {0x404d9d5e, 0x3fe56da8,  0},  //3.21273
   {0x4085c875, 0x4002dc0d,  0},  //4.18072
   {0x40a4c23b, 0x4011388e,  0},  //5.14871
   {0x40c3bc01, 0x401e48d9,  0},  //6.1167
   {0x40e2b5c7, 0x402a596d,  0},  //7.08469
   {0x4100d7c7, 0x40359d47,  0},  //8.05268
   {0x411054aa, 0x40403869,  0},  //9.02067
   {0x411fd18d, 0x404a455f,  0},  //9.98866
   {0x412f4e70, 0x4053d867,  0},  //10.9566
   {0x413ecb53, 0x405d0159,  0},  //11.9246
   {0x414e4836, 0x4065cce5,  0},  //12.8926
   {0x415dc519, 0x406e4564,  0},  //13.8606
   {0x416d41fc, 0x40767366,  0},   //14.8286

   {0x49b4cc08, 0x44982000,  0}, // 1481089, 1217
   {0x363eecba, 0x3add14b0,  0}, // 2.845E-06, 0.00168671273
   {0x3d800000, 0x3e800000,  0}, // 0.0625, 0.25
   {0xc20a1eb8, 0x7fc00000,  0}, // -34.53, nan
   {0xbe99999a, 0x7fc00000,  0}, // -0.3, nan
   {0xbf247208, 0x7fc00000,  0}, // -0.642365, nan 
   {0x3e99999a, 0x3f0c378c,  0}, // 0.3, 0.5477226
   {0x3f247208, 0x3f4d2d95,  0}, // 0.642365, 0.8014768
   {0x3f000000, 0x3f3504f3,  0}, // 0.5, 0.707106769
   {0x420a1eb8, 0x40bc0a05,  0}, // 34.53, 5.876223
   {0x46de66b6, 0x4328b904,  0}, // 28467.3555, 168.722717
   {0xc2c80000, 0x7fc00000,  0}, // -100, nan
   {0x42c80000, 0x41200000,  0}, // 100, 10
   {0x718f0f0f, 0x588751f2,  0}, // 1.416786E+30, 1.19028818E+15
   {0x00080000, 0x1f000000,  0}, // 2^-130, 2^-65

   {0x7f800001, 0x7fc00001,  0},
   {0xff800001, 0xffc00001,  0},
   {0x7fc00000, 0x7fc00000,  0},
   {0xffc00000, 0xffc00000,  0},
   {0x7f800000, 0x7f800000,  0},
   {0xff800000, 0x7fc00000,  0},
   {0x3f800000, 0x3f800000,  0},
   {0x3f800000, 0x7fc00000,  0},
   {0x00000000, 0x00000000,  0},
   {0x80000000, 0x80000000,  0},

   {0x007fffff, 0x1fffffff,  0},
   {0x807fffff, 0x7fc00000,  0},
   {0x00000001, 0x1a3504f3,  0},
   {0x80000001, 0x7fc00000,  0},
   {0x7f7fffff, 0x5f7fffff,  0},
   {0xff7fffff, 0x7fc00000,  0},
   {0x00800000, 0x20000000,  0},
   {0x80800000, 0x7fc00000,  0},

};

static libm_test_special_data_f64
test_sqrt_conformance_data[] = {
    // special accuracy tests
    {0x3e30000000000000LL, 0x3ff0000000000000LL, 0},  //min, small enough that sqrt(x) = 1 //
    {0x3E2FFFFFFFFFFFFFLL, 0x3ff0000000000000LL, 0}, //min - 1 bit
    {0x3e30000000000001LL, 0x3ff0000000000000LL, 0}, //min + 1 bit
    {0xFE37E43C8800759CLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x = 1, x = -1.0000000000000000e+300
    {0xFE37E43C8800758CLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x < 1
    {0xFE37E43C880075ACLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x > 1
    {0x408633ce8fb9f87eLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //max arg, x = 89.41598629223294,max sqrtf arg
    {0x408633ce8fb9f87dLL, 0x7feffffffffffd3bLL, 0}, //max arg - 1 bit
    {0x408633ce8fb9f87fLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //max arg + 1 bit
    {0x408633ce8fb9f8ffLL, 0x7ff0000000000000LL, FE_OVERFLOW}, // > max
    {0x408633ce8fb9f800LL, 0x7feffffffffe093bLL, 0}, // < max
    {0x4034000000000000LL, 0x41aceb088b68e804LL, 0}, //small_threshold = 20
    {0x4035000000000000LL, 0x41c3a6e1fd9eecfdLL, 0}, //small_threshold+1 = 21
    {0x4033000000000000LL, 0x419546d8f9ed26e2LL, 0}, //small_threshold - 1 = 19

    //sqrt special exception checks
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
// added from ancient libm repo
    {0x0000000000000001, 0x1e60000000000000,  0}, // denormal min
    {0x0005fde623545abc, 0x1ff3951f73e71d7b,  0}, // denormal intermediate
    {0x000FFFFFFFFFFFFF, 0x1fffffffffffffff,  0}, // denormal max
    {0x8000000000000001, 0xfff8000000000000,  33}, // -denormal min
    {0x8002344ade5def12, 0xfff8000000000000,  33}, // -denormal intermediate
    {0x800FFFFFFFFFFFFF, 0xfff8000000000000,  33}, // -denormal max
    {0x0010000000000000, 0x2000000000000000,  0}, // normal min
    {0x43b3c4eafedcab02, 0x41d1c8f7df8b4aa2,  0}, // normal intermediate
    {0x7FEFFFFFFFFFFFFF, 0x5fefffffffffffff,  0}, // normal max
    {0x8010000000000000, 0xfff8000000000000,  33}, // -normal min
    {0xc5812e71245acfdb, 0xfff8000000000000,  33}, // -normal intermediate
    {0xFFEFFFFFFFFFFFFF, 0xfff8000000000000,  33}, // -normal max
    {0x7FF0000000000000, 0x7ff0000000000000,  0}, // inf
    {0xFFF0000000000000, 0xfff8000000000000,  33}, // -inf
    {0x7FF8000000000000, 0x7ff8000000000000,  0}, // qnan min
    {0x7FFe1a5701234dc3, 0x7ffe1a5701234dc3,  0}, // qnan intermediate
    {0x7FFFFFFFFFFFFFFF, 0x7fffffffffffffff,  0}, // qnan max
    {0xFFF8000000000000, 0xfff8000000000000,  0}, // indeterninate
    {0xFFF8000000000001, 0xfff8000000000001,  0}, // -qnan min
    {0xFFF9123425dcba31, 0xfff9123425dcba31,  0}, // -qnan intermediate
    {0xFFFFFFFFFFFFFFFF, 0xffffffffffffffff,  0}, // -qnan max
    {0x7FF0000000000001, 0x7ff8000000000001,  0}, // snan min
    {0x7FF5344752a0bd90, 0x7ffd344752a0bd90,  0}, // snan intermediate
    {0x7FF7FFFFFFFFFFFF, 0x7fffffffffffffff,  0}, // snan max
    {0xFFF0000000000001, 0xfff8000000000001,  0}, // -snan min
    {0xfFF432438995fffa, 0xfffc32438995fffa,  0}, // -snan intermediate
    {0xFFF7FFFFFFFFFFFF, 0xffffffffffffffff,  0}, // -snan max															
    {0x3FF921FB54442D18, 0x3ff40d931ff62705,  0}, // pi/2
    {0x400921FB54442D18, 0x3ffc5bf891b4ef6a,  0}, // pi
    {0x401921FB54442D18, 0x40040d931ff62705,  0}, // 2pi
    {0x3FFB7E151628AED3, 0x3ff4f92b7e61eb29,  0}, // e --
    {0x4005BF0A8B145769, 0x3ffa61298e1e069c,  0}, // e
    {0x400DBF0A8B145769, 0x3ffeda410adbd55c,  0}, // e ++
    {0x0000000000000000, 0x0000000000000000,  0}, // 0
    {0x3C4536B8B14B676C, 0x3e1a0df7c04f6375,  0}, // 0.0000000000000000023
    {0x3FDFFFFBCE4217D3, 0x3fe6a09ceadf2b44,  0}, // 0.4999989999999999999
    {0x3FE000000000006C, 0x3fe6a09e667f3c19,  0}, // 0.500000000000012
    {0x8000000000000000, 0x8000000000000000,  0}, // -0
    {0xBBDB2752CE74FF42, 0xfff8000000000000,  33}, // -0.000000000000000000023
    {0xBFDFFFFBCE4217D3, 0xfff8000000000000,  33}, // -0.4999989999999999999
    {0xBFE000000000006C, 0xfff8000000000000,  33}, // -0.500000000000012
    {0x3FF0000000000000, 0x3ff0000000000000,  0}, // 1
    {0x3FEFFFFFC49BD0DC, 0x3fefffffe24de860,  0}, // 0.9999998893750006
    {0x3FF0000000000119, 0x3ff000000000008c,  0}, // 1.0000000000000624998
    {0x3FF7FFFEF39085F4, 0x3ff3988da672841b,  0}, // 1.499998999999999967
    {0x3FF8000000000001, 0x3ff3988e1409212f,  0}, // 1.50000000000000012
    {0xBFF0000000000000, 0xfff8000000000000,  33}, // -1
    {0xBFEFFFFFC49BD0DC, 0xfff8000000000000,  33}, // -0.9999998893750006
    {0xBFF0000000000119, 0xfff8000000000000,  33}, // -1.0000000000000624998
    {0xBFF7FFFEF39085F4, 0xfff8000000000000,  33}, // -1.499998999999999967
    {0xBFF8000000000001, 0xfff8000000000000,  33}, // -1.50000000000000012
    {0x4000000000000000, 0x3ff6a09e667f3bcd,  0}, // 2
    {0xC000000000000000, 0xfff8000000000000,  33}, // -2
    {0x4024000000000000, 0x40094c583ada5b53,  0}, // 10
    {0xC024000000000000, 0xfff8000000000000,  33}, // -10
    {0x408F400000000000, 0x403f9f6e4990f227,  0}, // 1000
    {0xC08F400000000000, 0xfff8000000000000,  33}, // -1000
    {0x4050D9999999999A, 0x40206b646433bf41,  0}, // 67.4
    {0xC050D9999999999A, 0xfff8000000000000,  33}, // -67.4
    {0x409EFE6666666666, 0x404644d0e18c772f,  0}, // 1983.6
    {0xC09EFE6666666666, 0xfff8000000000000,  33}, // -1983.6
    {0x4055E00000000000, 0x4022b5524ae1278e,  0}, // 87.5
    {0xC055E00000000000, 0xfff8000000000000,  33}, // -87.5
    {0x41002BC800000000, 0x4076bf7e96796aca,  0}, // 132473
    {0xC1002BC800000000, 0xfff8000000000000,  33}, // -132473
    {0x4330000000000000, 0x4190000000000000,  0}, // 2^52
    {0x4330000000000001, 0x4190000000000000,  0}, // 2^52 + 1
    {0x432FFFFFFFFFFFFF, 0x418fffffffffffff,  0}, // 2^52 -1 + 0.5
    {0xC330000000000000, 0xfff8000000000000,  33}, // -2^52
    {0xC330000000000001, 0xfff8000000000000,  33}, // -(2^52 + 1)
    {0xC32FFFFFFFFFFFFF, 0xfff8000000000000,  33}, // -(2^52 -1 + 0.5)
    {0x7fefffffffffffff, 0x5fefffffffffffff,  0},  //1.79769313486231570815e+308
    {0xffefffffffffffff, 0xfff8000000000000,  33},  //-1.79769313486231570815e+308
    {0xbc90000000000000, 0xfff8000000000000,  33},  //-5.55112e-17
    {0xbc8ccccccccccccd, 0xfff8000000000000,  33},  //-4.996e-17
    {0xbc8999999999999a, 0xfff8000000000000,  33},  //-4.44089e-17
    {0xbc86666666666667, 0xfff8000000000000,  33},  //-3.88578e-17
    {0xbc83333333333334, 0xfff8000000000000,  33},  //-3.33067e-17
    {0xbc80000000000001, 0xfff8000000000000,  33},  //-2.77556e-17
    {0xbc7999999999999c, 0xfff8000000000000,  33},  //-2.22045e-17
    {0xbc73333333333336, 0xfff8000000000000,  33},  //-1.66533e-17
    {0xbc6999999999999f, 0xfff8000000000000,  33},  //-1.11022e-17
    {0xbc599999999999a4, 0xfff8000000000000,  33},  //-5.55112e-18
    {0xb944000000000000, 0xfff8000000000000,  33},  //-7.70372e-33
    {0x3c59999999999990, 0x3e243d136248490b,  0},  //5.55112e-18
    {0x3c69999999999995, 0x3e2c9f25c5bfedd7,  0},  //1.11022e-17
    {0x3c73333333333331, 0x3e3186f174f88471,  0},  //1.66533e-17
    {0x3c79999999999998, 0x3e343d136248490e,  0},  //2.22045e-17
    {0x3c7ffffffffffffe, 0x3e36a09e667f3bcc,  0},  //2.77556e-17
    {0x3c83333333333332, 0x3e38c97ef43f7247,  0},  //3.33067e-17
    {0x3c86666666666665, 0x3e3ac5eb3f7ab2f7,  0},  //3.88578e-17
    {0x3c89999999999998, 0x3e3c9f25c5bfedd8,  0},  //4.44089e-17
    {0x3c8ccccccccccccb, 0x3e3e5b9d136c6d95,  0},  //4.996e-17
    {0x3c8ffffffffffffe, 0x3e3fffffffffffff,  0},  //5.55112e-17
    {0x409c4474e1726455, 0x4045444fb4b8d593,  0},  //1809.11
    {0x409c8e99130401cc, 0x40456021565c4644,  0}, //1827.65
    {0x409cd8bd44959f43, 0x40457bcef268595b,  0}, //1846.18
    {0x409d22e176273cba, 0x4045975913650cc8,  0}, //1864.72
    {0x409d6d05a7b8da31, 0x4045b2c0406b3be0,  0}, //1883.26
    {0x409db729d94a77a8, 0x4045ce04fd42d57c,  0}, //1901.79
    {0x409e014e0adc151f, 0x4045e927ca7fbfb6,  0}, //1920.33
    {0x409e4b723c6db296, 0x40460429259d7b2c,  0}, //1938.86
    {0x409e95966dff500d, 0x40461f0989199692,  0}, //1957.4
    {0x409edfba9f90ed84, 0x404639c96c8d0267,  0}, //1975.93
    {0x409f29ded1228afb, 0x4046546944c45393,  0}, //1994.47
    {0x409f42421c044285, 0x40465d24536ba51c,  0}, //2000.56
    {0x40c4c79d6d6dc1a5, 0x4059c96298191e81,  0}, //10639.2
    {0x40d2d3794bad7d7c, 0x40615b09188f80a4,  0}, //19277.9
    {0x40db4323e0a41a26, 0x4064e2a3cf61c7f8,  0}, //27916.6
    {0x40e1d9673acd5b68, 0x4067e636f7d6729f,  0}, //36555.2
    {0x40e6113c8548a9bd, 0x406a92d56539cd38,  0}, //45193.9
    {0x40ea4911cfc3f812, 0x406d00970ffd285b,  0}, //53832.6
    {0x40ee80e71a3f4667, 0x406f3e28723a96d1,  0}, //62471.2
    {0x40f15c5e325d4a5e, 0x4070aaa14489b3dc,  0}, //71109.9
    {0x40f37848d79af188, 0x4071a65da4def67d,  0}, //79748.6
    {0x40f594337cd898b2, 0x407294cc4dcaf763,  0}, //88387.2
    {0x40f7b01e22163fdc, 0x407377d624053bda,  0}, //97025.9
    {0x40f9cc08c753e706, 0x407450f931b0c1b2,  0}, //105665
    {0x40fbe7f36c918e30, 0x40752166bdc8bf13,  0}, //114303
    {0x40fe03de11cf355a, 0x4075ea174c457728,  0}, //122942
    {0x41000fe45b866e42, 0x4076abd860eb4491,  0}, //131581
    {0x41011dd9ae2541d7, 0x407767563ea281c8,  0}, //140219
    {0x41022bcf00c4156c, 0x40781d22fb795ce6,  0}, //148858
    {0xc104c083a6d698fe, 0xfff8000000000000,  33}, //-170000
    {0xc1038c4043b70ef5, 0xfff8000000000000,  33}, //-160136
    {0xc10257fce09784ec, 0xfff8000000000000,  33}, //-150272
    {0xc10123b97d77fae3, 0xfff8000000000000,  33}, //-140407
    {0xc0ffdeec34b0e1b4, 0xfff8000000000000,  33}, //-130543
    {0xc0fd76656e71cda2, 0xfff8000000000000,  33}, //-120678
    {0xc0fb0ddea832b990, 0xfff8000000000000,  33}, //-110814
    {0xc0f8a557e1f3a57e, 0xfff8000000000000,  33}, //-100949
    {0xc0f63cd11bb4916c, 0xfff8000000000000,  33}, //-91085.1
    {0xc0f3d44a55757d5a, 0xfff8000000000000,  33}, //-81220.6
    {0xc0f16bc38f366948, 0xfff8000000000000,  33}, //-71356.2
    {0xc0ee067991eeaa6c, 0xfff8000000000000,  33}, //-61491.8
    {0xc0e9356c05708248, 0xfff8000000000000,  33}, //-51627.4
    {0xc0e4645e78f25a24, 0xfff8000000000000,  33}, //-41763
    {0xc0df26a1d8e86400, 0xfff8000000000000,  33}, //-31898.5
    {0xc0d58486bfec13b8, 0xfff8000000000000,  33}, //-22034.1
    {0xc0c7c4d74ddf86e0, 0xfff8000000000000,  33}, //-12169.7
    {0xc0a202846f9b9940, 0xfff8000000000000,  33}, //-2305.26
    {0xbfd269621134db92, 0xfff8000000000000,  33}, //-0.287682
    {0xbfd0dc554dd7fa10, 0xfff8000000000000,  33}, //-0.263448
    {0xbfce9e9114f6311b, 0xfff8000000000000,  33}, //-0.239214
    {0xbfcb84778e3c6e16, 0xfff8000000000000,  33}, //-0.21498
    {0xbfc86a5e0782ab11, 0xfff8000000000000,  33}, //-0.190746
    {0xbfc5504480c8e80c, 0xfff8000000000000,  33}, //-0.166512
    {0xbfc2362afa0f2507, 0xfff8000000000000,  33}, //-0.142278
    {0xbfbe3822e6aac404, 0xfff8000000000000,  33}, //-0.118044
    {0xbfb803efd9373dfa, 0xfff8000000000000,  33}, //-0.0938101
    {0xbfb1cfbccbc3b7f0, 0xfff8000000000000,  33}, //-0.0695761
    {0xbfa737137ca063cd, 0xfff8000000000000,  33}, //-0.0453421
    {0xbf959d5ac372af74, 0xfff8000000000000,  33}, //-0.0211081
    {0x3f699b8b92db4590, 0x3faca03c20a6bb3e,  0}, //0.00312593
    {0x3f9c043da82980d8, 0x3fc52c19f1433c87,  0}, //0.0273599
    {0x3faa6a84eefbcc7f, 0x3fcd13054fd1ba54,  0}, //0.0515939
    {0x3fb3697584f16c49, 0x3fd19fa4171843f8,  0}, //0.0758279
    {0x3fb99da89264f252, 0x3fd43eae0291d350,  0}, //0.100062
    {0x3fbfd1db9fd8785c, 0x3fd69048328d5d2b,  0}, //0.124296
    {0x3fc3030756a5ff33, 0x3fd8aa532c11134b,  0}, //0.14853
    {0x3fc61d20dd5fc238, 0x3fda99fd7005bf02,  0}, //0.172764
    {0x3fc9373a6419853d, 0x3fdc67f2b3b906ca,  0}, //0.196998
    {0x3fcc5153ead34842, 0x3fde1a4364ab9afd,  0}, //0.221232
    {0x40421db22d0e5604, 0x401813c3fa6dcfe7,  0}, //36.232
    {0x4050c6ef459d9902, 0x40206249be7646a7,  0}, //67.1084
    {0x40587f0574b40702, 0x4023cc255c7bf934,  0}, //97.9847
    {0x40601b8dd1e53a81, 0x4026b411d814a23f,  0}, //128.861
    {0x4063f798e9707181, 0x4029470734237dfe,  0}, //159.737
    {0x4067d3a400fba881, 0x402b9cd2a49ed54b,  0}, //190.614
    {0x406bafaf1886df81, 0x402dc3dcd7c0b437,  0}, //221.49
    {0x406f8bba30121681, 0x402fc5a7e7e41653,  0}, //252.366
    {0x4071b3e2a3cea6c1, 0x4030d46f10c78363,  0}, //283.243
    {0x4073a1e82f944241, 0x4031b93141d78a25,  0}, //314.119
    {0x40758fedbb59ddc1, 0x403292f54e2688bc,  0}, //344.996
    {0x40777df3471f7941, 0x4033632dafaeb753,  0}, //375.872
    {0x40796bf8d2e514c1, 0x40342b0216453df5,  0}, //406.748
    {0x407b59fe5eaab041, 0x4034eb62f9f0316f,  0}, //437.625
    {0x407d4803ea704bc1, 0x4035a5170bcd95f0,  0}, //468.501
    {0x407f36097635e741, 0x403658c4b7973394,  0}, //499.377
    {0x4080920780fdc161, 0x403706f9085f0269,  0}, //530.254
    {0x4081890a46e08f21, 0x4037b02cc44fa893,  0}, //561.13
    {0x4082800d0cc35ce1, 0x403854c84841a839,  0}, //592.006
    {0x4083770fd2a62aa1, 0x4038f5267e094a13,  0}, //622.883
    {0x40846e129888f861, 0x403991972b5fd873,  0}, //653.759
    {0x408565155e6bc621, 0x403a2a60c47841cd,  0}, //684.635
    {0x40862E2CD0FE8AB5, 0x403aa43c3f6483d3,  0}, //709.771883
    {0x40862E6666666666, 0x403aa45ed47d25ff,  0}, //709.8
    {0x40865c18244e93e1, 0x403abfc1e0cc8c7a,  0}, //715.512
    {0x4087531aea3161a1, 0x403b51f26832b89c,  0}, //746.388
    {0x40884a1db0142f61, 0x403be124886624a3,  0}, //777.264
    {0x4089412075f6fd21, 0x403c6d857f0e020d,  0}, //808.141
    {0x408a38233bd9cae1, 0x403cf73e41527056,  0}, //839.017
    {0x408b2f2601bc98a1, 0x403d7e7407eac5bb,  0}, //869.894
    {0x408c2628c79f6661, 0x403e0348c4fa202f,  0}, //900.77
    {0x408d1d2b8d823421, 0x403e85db87e168b1,  0}, //931.646
    {0x408e142e536501e1, 0x403f0648d24a8b77,  0}, //962.523
    {0x408f0b311947cfa1, 0x403f84aae1039740,  0}, //993.399
    {0x40900119ef954eb1, 0x4040008cf55dbd0b,  0}, //1024.28
    {0x40907c9b5286b591, 0x40403dd62b257a96,  0}, //1055.15
    {0x4090f81cb5781c71, 0x40407a3b74f1e0fe,  0}, //1086.03
    {0x4091739e18698351, 0x4040b5c67a202c90,  0}, //1116.9
    {0x4091ef1f7b5aea31, 0x4040f080385e2d1f,  0}, //1147.78
    {0x40926aa0de4c5111, 0x40412a71138aa08b,  0}, //1178.66
    {0x4092e622413db7f1, 0x404163a0e3b950b8,  0}, //1209.53
    {0x409361a3a42f1ed1, 0x40419c17019d6f28,  0}, //1240.41
    {0x4093dd25072085b1, 0x4041d3da5191f65d,  0}, //1271.29
    {0x409458a66a11ec91, 0x40420af14d6f16e8,  0}, //1302.16
    {0x4094d427cd035371, 0x404241620d5480ad,  0}, //1333.04
    {0x40954fa92ff4ba51, 0x404277324f8a624e,  0}, //1363.92
    {0x4095cb2a92e62131, 0x4042ac677f960417,  0}, //1394.79
    {0x409646abf5d78811, 0x4042e106bc9ac0ac,  0}, //1425.67
    {0x4096c22d58c8eef1, 0x40431514df1ca5d0,  0}, //1456.54
    {0x40973daebbba55d1, 0x404348967e371c60,  0}, //1487.42
    {0x4097b9301eabbcb1, 0x40437b8ff4577ed9,  0}, //1518.3
    {0x409834b1819d2391, 0x4043ae0563896e3e,  0}, //1549.17
    {0x4098b032e48e8a71, 0x4043dffab960ed1e,  0}, //1580.05
    {0x40992bb4477ff151, 0x40441173b28cc31b,  0}, //1610.93
    {0x4099a735aa715831, 0x40444273de1a5d90,  0}, //1641.8
    {0x409a22b70d62bf11, 0x404472fea0734333,  0}, //1672.68
    {0x409a9e38705425f1, 0x4044a317361b3933,  0}, //1703.56
    {0x409b19b9d3458cd1, 0x4044d2c0b635628a,  0}, //1734.43
    {0x409b953b3636f3b1, 0x404501fe14d5e8e2,  0}, //1765.31
    {0x409c10bc99285a91, 0x404530d225251d24,  0}, //1796.18
    {0xc042b66666666666, 0xfff8000000000000,  33}, //-37.425
    {0xc04238853c148344, 0xfff8000000000000,  33}, //-36.4416
    {0xc041baa411c2a022, 0xfff8000000000000,  33}, //-35.4581
    {0xc0413cc2e770bd00, 0xfff8000000000000,  33}, //-34.4747
    {0xc040bee1bd1ed9de, 0xfff8000000000000,  33}, //-33.4913
    {0xc040410092ccf6bc, 0xfff8000000000000,  33}, //-32.5078
    {0xc03f863ed0f62735, 0xfff8000000000000,  33}, //-31.5244
    {0xc03e8a7c7c5260f2, 0xfff8000000000000,  33}, //-30.541
    {0xc03d8eba27ae9aaf, 0xfff8000000000000,  33}, //-29.5575
    {0xc03c92f7d30ad46c, 0xfff8000000000000,  33}, //-28.5741
    {0xc03b97357e670e29, 0xfff8000000000000,  33}, //-27.5907
    {0xc03a9b7329c347e6, 0xfff8000000000000,  33}, //-26.6072
    {0xc0399fb0d51f81a3, 0xfff8000000000000,  33}, //-25.6238
    {0xc038a3ee807bbb60, 0xfff8000000000000,  33}, //-24.6404
    {0xc037a82c2bd7f51d, 0xfff8000000000000,  33}, //-23.6569
    {0xc036ac69d7342eda, 0xfff8000000000000,  33}, //-22.6735
    {0xc035b0a782906897, 0xfff8000000000000,  33}, //-21.6901
    {0xc034b4e52deca254, 0xfff8000000000000,  33}, //-20.7066
    {0xc033b922d948dc11, 0xfff8000000000000,  33}, //-19.7232
    {0xc032bd6084a515ce, 0xfff8000000000000,  33}, //-18.7398
    {0xc031c19e30014f8b, 0xfff8000000000000,  33}, //-17.7563
    {0xc030c5dbdb5d8948, 0xfff8000000000000,  33}, //-16.7729
    {0xc02f94330d738609, 0xfff8000000000000,  33}, //-15.7895
    {0xc02d9cae642bf982, 0xfff8000000000000,  33}, //-14.806
    {0xc02ba529bae46cfb, 0xfff8000000000000,  33}, //-13.8226
    {0xc029ada5119ce074, 0xfff8000000000000,  33}, //-12.8391
    {0xc027b620685553ed, 0xfff8000000000000,  33}, //-11.8557
    {0xc025be9bbf0dc766, 0xfff8000000000000,  33}, //-10.8723
    {0xc023c71715c63adf, 0xfff8000000000000,  33}, //-9.88885
    {0xc021cf926c7eae58, 0xfff8000000000000,  33}, //-8.90541
    {0xc01fb01b866e43a3, 0xfff8000000000000,  33}, //-7.92198
    {0xc01bc11233df2a96, 0xfff8000000000000,  33}, //-6.93855
    {0xc017d208e1501189, 0xfff8000000000000,  33}, //-5.95511
    {0xc013e2ff8ec0f87c, 0xfff8000000000000,  33}, //-4.97168
    {0xc00fe7ec7863bede, 0xfff8000000000000,  33}, //-3.98824
    {0xc00c816f0068db8c, 0xfff8000000000000,  33}, //-3.5632
    {0xc004c2fd75e2046d, 0xfff8000000000000,  33}, //-2.59521
    {0xbffa0917d6b65a9c, 0xfff8000000000000,  33}, //-1.62722
    {0xbfe51869835158bb, 0xfff8000000000000,  33}, //-0.65923
    {0x3fd3c2b94d940784, 0x3fe1c7fb2ff2c3ad,  0}, //0.30876
    {0x3ff46d916872b020, 0x3ff21435483e81c3,  0}, //1.27675
    {0x4001f53a3ec02f2f, 0x3ff7f8d06bc5ba65,  0}, //2.24474
    {0x4009b3abc947064e, 0x3ffcadb50d829c21,  0}, //3.21273
    {0x4010b90ea9e6eeb7, 0x40005b81a9725b49,  0}, //4.18072
    {0x401498476f2a5a47, 0x40022711cad294bc,  0}, //5.14871
    {0x40187780346dc5d7, 0x4003c91b25f510e2,  0}, //6.1167
    {0x401c56b8f9b13167, 0x40054b2d9d89fcc8,  0}, //7.08469
    {0x40201af8df7a4e7b, 0x4006b3a8df16c813,  0}, //8.05268
    {0x40220a95421c0443, 0x4008070d22e4e7be,  0}, //9.02067
    {0x4023fa31a4bdba0b, 0x400948abe8ab0216,  0}, //9.98866
    {0x4025e9ce075f6fd3, 0x400a7b0cd68f4c1d,  0}, //10.9567
    {0x4027d96a6a01259b, 0x400ba02b195afd54,  0}, //11.9246
    {0x4029c906cca2db63, 0x400cb99cac9fe249,  0}, //12.8926
    {0x402bb8a32f44912b, 0x400dc8ac8c901109,  0}, //13.8606
    {0x402da83f91e646f3, 0x400ece6cc8773030,  0}, //14.8286
    {0x402f97dbf487fcbb, 0x400fcbc35778a4b0,  0}, //15.7966
    {0x4030c3bc2b94d941, 0x401060b9b748e3f4,  0}, //16.7646
    {0x4031bb8a5ce5b425, 0x4010d812386ea485,  0}, //17.7326
    {0x4032b3588e368f09, 0x40114c3398320950,  0}, //18.7006
    {0x4033ab26bf8769ed, 0x4011bd5cfc862d5d,  0}, //19.6686
    {0x4034a2f4f0d844d1, 0x40122bc5dc595f54,  0}, //20.6366
    {0x40359ac322291fb5, 0x4012979f3f318032,  0}, //21.6045
    {0x403692915379fa99, 0x40130114bcf5acf5,  0}, //22.5725
    {0x40378a5f84cad57d, 0x4013684d4cd0560a,  0}, //23.5405
    {0x4038822db61bb061, 0x4013cd6bee20ad8f,  0}, //24.5085
    {0x403979fbe76c8b45, 0x4014309033a797b6,  0}, //25.4765
    {0x403a71ca18bd6629, 0x401491d6b71ba8ff,  0}, //26.4445
    {0x403b69984a0e410d, 0x4014f15979dacdbc,  0}, //27.4125
    {0x403c61667b5f1bf1, 0x40154f30365d5398,  0}, //28.3805
    {0x403d5934acaff6d5, 0x4015ab70a540fd81,  0}, //29.3485
    {0x403e5102de00d1b9, 0x4016062eb8281a5a,  0}, //30.3165
    {0x403f48d10f51ac9d, 0x40165f7ccc32b2aa,  0}, //31.2844
    {0x4040204fa05143c0, 0x4016b76bd57d2ad2,  0}, //32.2524
    {0x40409c36b8f9b132, 0x40170e0b84c94ade,  0}, //33.2204
    {0x4041181dd1a21ea4, 0x4017636a683f7bbc,  0}, //34.1884
    {0x40419404ea4a8c16, 0x4017b796080a8b33,  0}, //35.1564
    {0x40420fec02f2f988, 0x40180a9aff6db6ff,  0}, //36.1244
    {0x40428bd31b9b66fa, 0x40185c8512d916ab,  0}, //37.0924


};

