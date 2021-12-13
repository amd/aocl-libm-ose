#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>
/*
 * Test cases to check for exceptions for the expm1f() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_expm1f_conformance_data[] = {
   // special accuracy tests
   {0x38800000, 0x3f800000,  0},  //min= 0.00006103515625, small enough that expm1(x) = 1
   {0x387FFFFF, 0x3f800000,  0}, //min - 1 bit
   {0x38800001, 0x3f800000,  0}, //min + 1 bit
   {0xF149F2C9, 0x7f800000,  FE_OVERFLOW}, //lambda + x = 1, x = -9.9999994e+29
   {0xF149F2C8, 0x7f800000,  FE_OVERFLOW}, //lambda + x < 1
   {0xF149F2CA, 0x7f800000,  FE_OVERFLOW}, //lambda + x > 1
   {0x42B2D4FC, 0x7f7fffec,  0}, //max arg, x = 89.41598629223294,max expm1f arg
   {0x42B2D4FB, 0x7f7fff6c,  0}, //max arg - 1 bit
   {0x42B2D4FD, 0x7f800000,  FE_OVERFLOW}, //max arg + 1 bit
   {0x42B2D4FF, 0x7f800000,  FE_OVERFLOW}, // > max
   {0x42B2D400, 0x7f7f820b,  0}, // < max
   {0x41A00000, 0x4d675844,  0}, //small_threshold = 20
   {0x41A80000, 0x4e1d3710,  0}, //small_threshold+1 = 21
   {0x41980000, 0x4caa36c8,  0}, //small_threshold - 1 = 19

    //expm1 special exception checks
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
// added from ancient libm repo
   //     + (2^-25) < x < - (2^-25)        //
   {0xb3000000, 0xb3000000, 0},  //-2.98023e-08        
   {0xb2e47e05, 0xb2e47e05, 0},  //-2.66e-08
   {0xb2c8fc0a, 0xb2c8fc0a, 0},  //-2.33977e-08
   {0xb2ad7a0f, 0xb2ad7a0f, 0},  //-2.01954e-08
   {0xb291f814, 0xb291f814, 0},  //-1.6993e-08
   {0xb26cec32, 0xb26cec32, 0},  //-1.37907e-08
   {0xb235e83c, 0xb235e83c, 0},  //-1.05884e-08
   {0xb1fdc88b, 0xb1fdc88b, 0},  //-7.38607e-09
   {0xb18fc09e, 0xb18fc09e, 0},  //-4.18375e-09
   {0xb086e2c4, 0xb086e2c4, 0},  //-9.81423e-10
   {0x31189e78, 0x31189e78, 0},  //2.2209e-09
   {0x31ba5729, 0x31ba5729, 0},  //5.42322e-09
   {0x32142f8b, 0x32142f8b, 0},  //8.62554e-09
   {0x324b3381, 0x324b3381, 0},  //1.18279e-08
   {0x32811bbc, 0x32811bbc, 0},  //1.50302e-08
   {0x329c9db7, 0x329c9db7, 0},  //1.82325e-08
   {0x32b81fb2, 0x32b81fb2, 0},  //2.14348e-08
   {0x32d3a1ad, 0x32d3a1ad, 0},  //2.46372e-08
   {0x32ef23a8, 0x32ef23a8, 0},  //2.78395e-08
   //        341*ln 2 < x < +inf           //
   {0x436c5cfa, 0x7f800000, 0 },  //236.363
   {0x43715cfa, 0x7f800000, 34},  //241.363
   {0x43765cfa, 0x7f800000, 34},  //246.363
   {0x437b5cfa, 0x7f800000, 34},  //251.363
   {0x43802e7d, 0x7f800000, 34},  //256.363
   {0x4382ae7d, 0x7f800000, 34},  //261.363
   {0x43852e7d, 0x7f800000, 34},  //266.363
   {0x4387ae7d, 0x7f800000, 34},  //271.363
   {0x438a2e7d, 0x7f800000, 34},  //276.363
   {0x438cae7d, 0x7f800000, 34},  //281.363
   {0x438f2e7d, 0x7f800000, 34},  //286.363
   {0x4391ae7d, 0x7f800000, 34},  //291.363
   {0x43942e7d, 0x7f800000, 34},  //296.363
   {0x4398ab85, 0x7f800000, 34},  //305.34
   {0x459a800b, 0x7f800000, 34},  //4944.01
   {0x4615baaf, 0x7f800000, 34},  //9582.67
   {0x465e3558, 0x7f800000, 34},  //14221.3
   {0x46935801, 0x7f800000, 34},  //18860
   {0x46b79556, 0x7f800000, 34},  //23498.7
   {0x46dbd2ab, 0x7f800000, 34},  //28137.3
   {0x47000800, 0x7f800000, 34},  //32776
   {0x471226aa, 0x7f800000, 34},  //37414.7
   {0x47244554, 0x7f800000, 34},  //42053.3
   {0x473663fe, 0x7f800000, 34},  //46692
    //        -inf < x < -25*ln 2         //
   {0xc788b83a, 0xbf800000, 34},  //-70000.5
   {0xc7812c04, 0xbf800000, 34},  //-66136
   {0xc7733f9c, 0xbf800000, 34},  //-62271.6
   {0xc7642730, 0xbf800000, 34},  //-58407.2
   {0xc7550ec4, 0xbf800000, 34},  //-54542.8
   {0xc745f658, 0xbf800000, 34},  //-50678.3
   {0xc736ddec, 0xbf800000, 34},  //-46813.9
   {0xc727c580, 0xbf800000, 34},  //-42949.5
   {0xc718ad14, 0xbf800000, 34},  //-39085.1
   {0xc70994a8, 0xbf800000, 34},  //-35220.7
   {0xc6f4f877, 0xbf800000, 34},  //-31356.2
   {0xc6d6c79e, 0xbf800000, 34},  //-27491.8
   {0xc6b896c5, 0xbf800000, 34},  //-23627.4
   {0xc69a65ec, 0xbf800000, 34},  //-19763
   {0xc6786a26, 0xbf800000, 34},  //-15898.5
   {0xc63c0874, 0xbf800000, 34},  //-12034.1
   {0xc5ff4d85, 0xbf800000, 34},  //-8169.69
   {0xc5868a22, 0xbf800000, 34},  //-4305.27
   {0xc3dc6bee, 0xbf800000, 34},  //-440.843
   //      log(1-1/4) < x < log(1+1/4)    //
   {0xbe934b11, 0xbe800000, 0},  //-0.287682
   {0xbe86e2ab, 0xbe6d2934, 0},  //-0.263448
   {0xbe74f48a, 0xbe59dc1a, 0},  //-0.239214
   {0xbe5c23be, 0xbe4615cb, 0},  //-0.21498
   {0xbe4352f2, 0xbe31d34e, 0},  //-0.190746
   {0xbe2a8226, 0xbe1d1198, 0},  //-0.166512
   {0xbe11b15a, 0xbe07cd8a, 0},  //-0.142278
   {0xbdf1c11c, 0xbde407e0, 0},  //-0.118044
   {0xbdc01f84, 0xbdb7630a, 0},  //-0.0938101
   {0xbd8e7dec, 0xbd89a5da, 0},  //-0.0695761
   {0xbd39b8a7, 0xbd3592e0, 0},  //-0.0453421
   {0xbcaceaec, 0xbcab1aff, 0},  //-0.0211081
   {0x3b4cdbad, 0x3b4d2dba, 0},  //0.00312589
   {0x3ce021d7, 0x3ce339f9, 0},  //0.0273599
   {0x3d53541c, 0x3d58e00b, 0},  //0.0515939
   {0x3d9b4ba6, 0x3da155c6, 0},  //0.0758279
   {0x3dcced3e, 0x3dd787b5, 0},  //0.100062
   {0x3dfe8ed6, 0x3e0786fc, 0},  //0.124296
   {0x3e181837, 0x3e23f873, 0},  //0.14853
   {0x3e30e903, 0x3e411c89, 0},  //0.172764
   {0x3e49b9cf, 0x3e5ef79d, 0},  //0.196998
   {0x3e628a9b, 0x3e7d8e2d, 0},  //0.221232
   // -15 < x < +15(to check |n| > 2^9   //
   {0xc173b646, 0xbf7ffffc, 0},  //-15.232
   {0xc160ec1f, 0xbf7ffff3, 0},  //-14.0576
   {0xc14e21f8, 0xbf7fffd5, 0},  //-12.8833
   {0xc13b57d1, 0xbf7fff76, 0},  //-11.7089
   {0xc1288daa, 0xbf7ffe42, 0},  //-10.5346
   {0xc115c383, 0xbf7ffa5c, 0},  //-9.36023
   {0xc102f95c, 0xbf7fedbf, 0},  //-8.18588
   {0xc0e05e69, 0xbf7fc4ec, 0},  //-7.01152
   {0xc0baca1a, 0xbf7f40d3, 0},  //-5.83717
   {0xc09535cb, 0xbf7d955a, 0},  //-4.66282
   {0xc05f42f8, 0xbf782e05, 0},  //-3.48846
   {0xc0141a5a, 0xbf66b17c, 0},  //-2.31411
   {0xbf91e379, 0xbf2e1b31, 0},  //-1.13975
   {0x3d0db84b, 0x3d10333c, 0},  //0.0345996
   {0x3f9abefe, 0x40166608, 0},  //1.20895
   {0x4018881d, 0x411d7383, 0},  //2.38331
   {0x4063b0bb, 0x42085305, 0},  //3.55766
   {0x40976cac, 0x42e10c5e, 0},  //4.73202
   {0x40bd00fb, 0x43b72f5e, 0},  //5.90637
   {0x40e2954a, 0x44947a84, 0},  //7.08072
   {0x410414cc, 0x457061c0, 0},  //8.25508
   {0x4116def3, 0x464281c3, 0},  //9.42943
   {0x4129a91a, 0x471d5e05, 0},  //10.6038
   {0x413c7341, 0x47fea0e9, 0},  //11.7781
   {0x414f3d68, 0x48cdffb2, 0},  //12.9525
   {0x4162078f, 0x49a6a7ea, 0},  //14.1268
   {0x4174d1b6, 0x4a86d3a9, 0},  //15.3012
   // 15 < x < 236(to check m > 23)    //
   {0x4173b646, 0x4a7b9ffb, 0},  //15.232
   {0x41d0dde9, 0x524b261e, 0},  //26.1084
   {0x4213f057, 0x5a2402f2, 0},  //36.9847
   {0x423f71ba, 0x62046a12, 0},  //47.8611
   {0x426af31d, 0x69d5cf11, 0},  //58.7374
   {0x428b3a40, 0x71ac9e3d, 0},  //69.6138
   {0x42a0faf1, 0x798b5cc2, 0},  //80.4901
   {0x42B16D62, 0x7f7da7c8, 0},  //88.713638
   {0x42B19999, 0x7f800000, 0},  //88.799995
   {0x42b6bba2, 0x7f800000, 0},  //91.3665
   {0x42cc7c53, 0x7f800000, 0},  //102.243
   {0x42e23d04, 0x7f800000, 0},  //113.119
   {0x42f7fdb5, 0x7f800000, 0},  //123.996
   {0x4306df33, 0x7f800000, 0},  //134.872
   {0x4311bf8c, 0x7f800000, 0},  //145.748
   {0x431c9fe5, 0x7f800000, 0},  //156.625
   {0x4327803e, 0x7f800000, 0},  //167.501
   {0x43326097, 0x7f800000, 0},  //178.377
   {0x433d40f0, 0x7f800000, 0},  //189.254
   {0x43482149, 0x7f800000, 0},  //200.13
   {0x435301a2, 0x7f800000, 0},  //211.006
   {0x435de1fb, 0x7f800000, 0},  //221.883
   {0x4368c254, 0x7f800000, 0},  //232.759
   // -17 < x < -3(to check m < -7)    //
   {0xc18b6666, 0xbf800000,34},  //-17.425
   {0xc18521ed, 0xbf7fffff, 0},  //-16.6416
   {0xc17dbae8, 0xbf7ffffe, 0},  //-15.8581
   {0xc17131f6, 0xbf7ffffb, 0},  //-15.0747
   {0xc164a904, 0xbf7ffff6, 0},  //-14.2913
   {0xc1582012, 0xbf7fffe9, 0},  //-13.5078
   {0xc14b9720, 0xbf7fffce, 0},  //-12.7244
   {0xc13f0e2e, 0xbf7fff93, 0},  //-11.941
   {0xc132853c, 0xbf7fff11, 0},  //-11.1575
   {0xc125fc4a, 0xbf7ffdf4, 0},  //-10.3741
   {0xc1197358, 0xbf7ffb85, 0},  //-9.59066
   {0xc10cea66, 0xbf7ff631, 0},  //-8.80723
   {0xc1006174, 0xbf7fea88, 0},  //-8.02379
   {0xc0e7b104, 0xbf7fd102, 0},  //-7.24036
   {0xc0ce9f20, 0xbf7f9922, 0},  //-6.45692
   {0xc0b58d3c, 0xbf7f1ed4, 0},  //-5.67349
   {0xc09c7b58, 0xbf7e131b, 0},  //-4.89006
   {0xc0836974, 0xbf7bc910, 0},  //-4.10662
   // -3 < x < 15(to check -8 < m < 24   //
   {0xc0640b78, 0xbf78be30, 0},  //-3.5632
   {0xc02617ec, 0xbf6ce509, 0},  //-2.59521
   {0xbfd048bf, 0xbf4db3d0, 0},  //-1.62722
   {0xbf28c34d, 0xbef72b15, 0},  //-0.65923
   {0x3e9e15c9, 0x3eb93564, 0},  //0.30876
   {0x3fa36c8b, 0x40257024, 0},  //1.27675
   {0x400fa9d2, 0x410701e4, 0},  //2.24474
   {0x404d9d5e, 0x41bec64c, 0},  //3.21273
   {0x4085c875, 0x4280d36b, 0},  //4.18072
   {0x40a4c23b, 0x432b358c, 0},  //5.14871
   {0x40c3bc01, 0x43e22ed8, 0},  //6.1167
   {0x40e2b5c7, 0x449511b0, 0},  //7.08469
   {0x4100d7c7, 0x45445347, 0},  //8.05268
   {0x411054aa, 0x46013d45, 0},  //9.02067
   {0x411fd18d, 0x46aa2230, 0},  //9.98866
   {0x412f4e70, 0x475ff50a, 0},  //10.9566
   {0x413ecb53, 0x481366d4, 0},  //11.9246
   {0x414e4836, 0x48c20773, 0},  //12.8926
   {0x415dc519, 0x497f67ce, 0},  //13.8606
   {0x416d41fc, 0x4a281926, 0},   //14.8286

   {0x3c000000, 0x3c008056, 0},  //0.0078125
   {0x3c7fffff, 0x3c810156, 0},  //0.0156249991
   {0x3f012345, 0x3f27f3e4, 0},  //0.504444
   {0x3f800000, 0x3fdbf0a8, 0},  //1
   {0x40000000, 0x40cc7326, 0},  //2
   {0x33d6bf95, 0x33d6bf96, 0},
   {0x322bcc77, 0x322bcc77, 0},

   {0x3effffff, 0x3f261298, 0},
   {0xbeffffff, 0xbec974d0, 0},

   {0x42800000, 0x6da12cc1, 0},  //64
   {0x42b00000, 0x7ef882b7, 0},  //88
   {0x42c00000, 0x7f800000, 0},  //96
   {0xc2e00000, 0xbf800000, 0},  //-112
   {0xc3000000, 0xbf800000, 0},  //-128

   {0x42b17216, 0x7f7fff04, 0},  //88.7228   largest value  --
   {0x42b17217, 0x7f7fff84, 0},  //88.7228   largest value
   {0x42b17218, 0x7f800000, 0},  //88.7228   overflow
   {0x50000000, 0x7f800000, 0},  //large   overflow

   {0xc20a1eb8, 0xbf800000, 0}, // -34.53
   {0xc6de66b6, 0xbf800000, 0}, // -28467.3555
   {0xbe99999a, 0xbe84b37a, 0}, // -0.3
   {0xbf247208, 0xbef2a9fc, 0}, // -0.642365
   {0xbf000000, 0xbec974d0, 0}, // -0.5
   {0x3e99999a, 0x3eb320b2, 0}, // 0.3
   {0x3f247208, 0x3f66a60e, 0}, // 0.642365
   {0x3f000000, 0x3f261299, 0}, // 0.5
   {0x420a1eb8, 0x586162f9, 0}, // 34.53
   {0x46de66b6, 0x7f800000, 0}, // 28467.3555
   {0xc2c80000, 0xbf800000, 0}, // -100
   {0x42c80000, 0x7f800000, 0}, // 100


};              
                
static libm_test_special_data_f64
test_expm1_conformance_data[] = {
    // special accuracy tests
    {0x3e30000000000000LL, 0x3ff0000000000000LL, 0},  //min, small enough that expm1(x) = 1 //
    {0x3E2FFFFFFFFFFFFFLL, 0x3ff0000000000000LL, 0}, //min - 1 bit
    {0x3e30000000000001LL, 0x3ff0000000000000LL, 0}, //min + 1 bit
    {0xFE37E43C8800759CLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x = 1, x = -1.0000000000000000e+300
    {0xFE37E43C8800758CLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x < 1
    {0xFE37E43C880075ACLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x > 1
    {0x408633ce8fb9f87eLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //max arg, x = 89.41598629223294,max expm1f arg
    {0x408633ce8fb9f87dLL, 0x7feffffffffffd3bLL, 0}, //max arg - 1 bit
    {0x408633ce8fb9f87fLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //max arg + 1 bit
    {0x408633ce8fb9f8ffLL, 0x7ff0000000000000LL, FE_OVERFLOW}, // > max
    {0x408633ce8fb9f800LL, 0x7feffffffffe093bLL, 0}, // < max
    {0x4034000000000000LL, 0x41aceb088b68e804LL, 0}, //small_threshold = 20
    {0x4035000000000000LL, 0x41c3a6e1fd9eecfdLL, 0}, //small_threshold+1 = 21
    {0x4033000000000000LL, 0x419546d8f9ed26e2LL, 0}, //small_threshold - 1 = 19

    //expm1 special exception checks
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
    //     + (2^-54) < x < - (2^-54)        //
    {0xbc90000000000000LL, 0xbc90000000000000LL, 0},  //-5.55112e-17
    {0xbc8ccccccccccccdLL, 0xbc8ccccccccccccdLL, 0},  //-4.996e-17
    {0xbc8999999999999aLL, 0xbc8999999999999aLL, 0},  //-4.44089e-17
    {0xbc86666666666667LL, 0xbc86666666666667LL, 0},  //-3.88578e-17
    {0xbc83333333333334LL, 0xbc83333333333334LL, 0},  //-3.33067e-17
    {0xbc80000000000001LL, 0xbc80000000000001LL, 0},  //-2.77556e-17
    {0xbc7999999999999cLL, 0xbc7999999999999cLL, 0},  //-2.22045e-17
    {0xbc73333333333336LL, 0xbc73333333333336LL, 0},  //-1.66533e-17
    {0xbc6999999999999fLL, 0xbc6999999999999fLL, 0},  //-1.11022e-17
    {0xbc599999999999a4LL, 0xbc599999999999a4LL, 0},  //-5.55112e-18
    {0xb944000000000000LL, 0xb944000000000000LL, 0},  //-7.70372e-33
    {0x3c59999999999990LL, 0x3c59999999999990LL, 0},  //5.55112e-18
    {0x3c69999999999995LL, 0x3c69999999999995LL, 0},  //1.11022e-17
    {0x3c73333333333331LL, 0x3c73333333333331LL, 0},  //1.66533e-17
    {0x3c79999999999998LL, 0x3c79999999999998LL, 0},  //2.22045e-17
    {0x3c7ffffffffffffeLL, 0x3c7ffffffffffffeLL, 0},  //2.77556e-17
    {0x3c83333333333332LL, 0x3c83333333333332LL, 0},  //3.33067e-17
    {0x3c86666666666665LL, 0x3c86666666666665LL, 0},  //3.88578e-17
    {0x3c89999999999998LL, 0x3c89999999999998LL, 0},  //4.44089e-17
    {0x3c8ccccccccccccbLL, 0x3c8ccccccccccccbLL, 0},  //4.996e-17
    {0x3c8ffffffffffffeLL, 0x3c8ffffffffffffeLL, 0},  //5.55112e-17
    //        341*ln 2 < x < +inf           //
    {0x409c4474e1726455LL, 0x7ff0000000000000LL, 0},  //1809.11
    {0x409c8e99130401ccLL, 0x7ff0000000000000LL, 34}, //1827.65
    {0x409cd8bd44959f43LL, 0x7ff0000000000000LL, 34}, //1846.18
    {0x409d22e176273cbaLL, 0x7ff0000000000000LL, 34}, //1864.72
    {0x409d6d05a7b8da31LL, 0x7ff0000000000000LL, 34}, //1883.26
    {0x409db729d94a77a8LL, 0x7ff0000000000000LL, 34}, //1901.79
    {0x409e014e0adc151fLL, 0x7ff0000000000000LL, 34}, //1920.33
    {0x409e4b723c6db296LL, 0x7ff0000000000000LL, 34}, //1938.86
    {0x409e95966dff500dLL, 0x7ff0000000000000LL, 34}, //1957.4
    {0x409edfba9f90ed84LL, 0x7ff0000000000000LL, 34}, //1975.93
    {0x409f29ded1228afbLL, 0x7ff0000000000000LL, 34}, //1994.47
    {0x409f42421c044285LL, 0x7ff0000000000000LL, 34}, //2000.56
    {0x40c4c79d6d6dc1a5LL, 0x7ff0000000000000LL, 34}, //10639.2
    {0x40d2d3794bad7d7cLL, 0x7ff0000000000000LL, 34}, //19277.9
    {0x40db4323e0a41a26LL, 0x7ff0000000000000LL, 34}, //27916.6
    {0x40e1d9673acd5b68LL, 0x7ff0000000000000LL, 34}, //36555.2
    {0x40e6113c8548a9bdLL, 0x7ff0000000000000LL, 34}, //45193.9
    {0x40ea4911cfc3f812LL, 0x7ff0000000000000LL, 34}, //53832.6
    {0x40ee80e71a3f4667LL, 0x7ff0000000000000LL, 34}, //62471.2
    {0x40f15c5e325d4a5eLL, 0x7ff0000000000000LL, 34}, //71109.9
    {0x40f37848d79af188LL, 0x7ff0000000000000LL, 34}, //79748.6
    {0x40f594337cd898b2LL, 0x7ff0000000000000LL, 34}, //88387.2
    {0x40f7b01e22163fdcLL, 0x7ff0000000000000LL, 34}, //97025.9
    {0x40f9cc08c753e706LL, 0x7ff0000000000000LL, 34}, //105665
    {0x40fbe7f36c918e30LL, 0x7ff0000000000000LL, 34}, //114303
    {0x40fe03de11cf355aLL, 0x7ff0000000000000LL, 34}, //122942
    {0x41000fe45b866e42LL, 0x7ff0000000000000LL, 34}, //131581
    {0x41011dd9ae2541d7LL, 0x7ff0000000000000LL, 34}, //140219
    {0x41022bcf00c4156cLL, 0x7ff0000000000000LL, 34}, //148858
    //        -inf < x < -54*ln 2           //
    {0xc104c083a6d698feLL, 0xbff0000000000000LL, 34}, //-170000
    {0xc1038c4043b70ef5LL, 0xbff0000000000000LL, 34}, //-160136
    {0xc10257fce09784ecLL, 0xbff0000000000000LL, 34}, //-150272
    {0xc10123b97d77fae3LL, 0xbff0000000000000LL, 34}, //-140407
    {0xc0ffdeec34b0e1b4LL, 0xbff0000000000000LL, 34}, //-130543
    {0xc0fd76656e71cda2LL, 0xbff0000000000000LL, 34}, //-120678
    {0xc0fb0ddea832b990LL, 0xbff0000000000000LL, 34}, //-110814
    {0xc0f8a557e1f3a57eLL, 0xbff0000000000000LL, 34}, //-100949
    {0xc0f63cd11bb4916cLL, 0xbff0000000000000LL, 34}, //-91085.1
    {0xc0f3d44a55757d5aLL, 0xbff0000000000000LL, 34}, //-81220.6
    {0xc0f16bc38f366948LL, 0xbff0000000000000LL, 34}, //-71356.2
    {0xc0ee067991eeaa6cLL, 0xbff0000000000000LL, 34}, //-61491.8
    {0xc0e9356c05708248LL, 0xbff0000000000000LL, 34}, //-51627.4
    {0xc0e4645e78f25a24LL, 0xbff0000000000000LL, 34}, //-41763
    {0xc0df26a1d8e86400LL, 0xbff0000000000000LL, 34}, //-31898.5
    {0xc0d58486bfec13b8LL, 0xbff0000000000000LL, 34}, //-22034.1
    {0xc0c7c4d74ddf86e0LL, 0xbff0000000000000LL, 34}, //-12169.7
    {0xc0a202846f9b9940LL, 0xbff0000000000000LL, 34}, //-2305.26
    //      log(1-1/4) < x < log(1+1/4)     /<</
    {0xbfd269621134db92LL, 0xbfcfffffffffffffLL, 0},  //-0.287682
    {0xbfd0dc554dd7fa10LL, 0xbfcda526657a007fLL, 0},  //-0.263448
    {0xbfce9e9114f6311bLL, 0xbfcb3b83153f7239LL, 0},  //-0.239214
    {0xbfcb84778e3c6e16LL, 0xbfc8c2b9323d2350LL, 0},  //-0.21498
    {0xbfc86a5e0782ab11LL, 0xbfc63a69983865afLL, 0},  //-0.190746
    {0xbfc5504480c8e80cLL, 0xbfc3a232cd81083aLL, 0},  //-0.166512
    {0xbfc2362afa0f2507LL, 0xbfc0f9b0f4497bafLL, 0},  //-0.142278
    {0xbfbe3822e6aac404LL, 0xbfbc80fb7745ddf3LL, 0},  //-0.118044
    {0xbfb803efd9373dfaLL, 0xbfb6ec60a036424aLL, 0},  //-0.0938101
    {0xbfb1cfbccbc3b7f0LL, 0xbfb134ba97f33181LL, 0},  //-0.0695761
    {0xbfa737137ca063cdLL, 0xbfa6b25a9c678855LL, 0},  //-0.0453421
    {0xbf959d5ac372af74LL, 0xbf95635d3303353cLL, 0},  //-0.0211081
    {0x3f699b8b92db4590LL, 0x3f69a5cd4e27af84LL, 0},  //0.00312593
    {0x3f9c043da82980d8LL, 0x3f9c6741fff4eee7LL, 0},  //0.0273599
    {0x3faa6a84eefbcc7fLL, 0x3fab1c02d544e44fLL, 0},  //0.0515939
    {0x3fb3697584f16c49LL, 0x3fb42ab993b90cf5LL, 0},  //0.0758279
    {0x3fb99da89264f252LL, 0x3fbaf0f77c8b8f31LL, 0},  //0.100062
    {0x3fbfd1db9fd8785cLL, 0x3fc0f0dff24d668cLL, 0},  //0.124296
    {0x3fc3030756a5ff33LL, 0x3fc47f0ef8691901LL, 0},  //0.14853
    {0x3fc61d20dd5fc238LL, 0x3fc82391a9d91b7fLL, 0},  //0.172764
    {0x3fc9373a6419853dLL, 0x3fcbdef43b3b8a10LL, 0},  //0.196998
    {0x3fcc5153ead34842LL, 0x3fcfb1c651a0ee2cLL, 0},  //0.221232
    // 36 < x < 1810(toLL check m > 52)    // 
    {0x40421db22d0e5604LL, 0x433350e75b75c262LL, 0},  //36.232
    {0x4050c6ef459d9902LL, 0x45fc2f8781cf4cc1LL, 0},  //67.1084
    {0x40587f0574b40702LL, 0x48c490656393e2a3LL, 0},  //97.9847
    {0x40601b8dd1e53a81LL, 0x4b8e01ba4b3a160cLL, 0},  //128.861
    {0x4063f798e9707181LL, 0x4e55e487e8b783dfLL, 0},  //159.737
    {0x4067d3a400fba881LL, 0x511ff20c2073038aLL, 0},  //190.614
    {0x406bafaf1886df81LL, 0x53e74ea459bad8d4LL, 0},  //221.49
    {0x406f8bba30121681LL, 0x56b101379c1f3c8bLL, 0},  //252.366
    {0x4071b3e2a3cea6c1LL, 0x5978d02634dcd210LL, 0},  //283.243
    {0x4073a1e82f944241LL, 0x5c421a7afcfc2fb9LL, 0},  //314.119
    {0x40758fedbb59ddc1LL, 0x5f0a6a9074a2816bLL, 0},  //344.996
    {0x40777df3471f7941LL, 0x61d345ea8900dc1fLL, 0},  //375.872
    {0x40796bf8d2e514c1LL, 0x649c1f7f144bbd9cLL, 0},  //406.748
    {0x407b59fe5eaab041LL, 0x676484b2d44a5eefLL, 0},  //437.625
    {0x407d4803ea704bc1LL, 0x6a2df0a8ad60e6f3LL, 0},  //468.501
    {0x407f36097635e741LL, 0x6cf5d813de9c3ba8LL, 0},  //499.377
    {0x4080920780fdc161LL, 0x6fbfdfe02ff8fc44LL, 0},  //530.254
    {0x4081890a46e08f21LL, 0x728741625498ca1aLL, 0},  //561.13
    {0x4082800d0cc35ce1LL, 0x7550f78b5bb8f205LL, 0},  //592.006
    {0x4083770fd2a62aa1LL, 0x7818c208e5baaf11LL, 0},  //622.883
    {0x40846e129888f861LL, 0x7ae2102ebeb8c4fcLL, 0},  //653.759
    {0x408565155e6bc621LL, 0x7daa5b89b066bcfaLL, 0},  //684.635
    {0x40862E2CD0FE8AB5LL, 0x7fefa7c29bcafb83LL, 0},  //709.771883
    {0x40862E6666666666LL, 0x7ff0000000000000LL, 0},  //709.8
    {0x40865c18244e93e1LL, 0x7ff0000000000000LL, 0},  //715.512
    {0x4087531aea3161a1LL, 0x7ff0000000000000LL, 0},  //746.388
    {0x40884a1db0142f61LL, 0x7ff0000000000000LL, 0},  //777.264
    {0x4089412075f6fd21LL, 0x7ff0000000000000LL, 0},  //808.141
    {0x408a38233bd9cae1LL, 0x7ff0000000000000LL, 0},  //839.017
    {0x408b2f2601bc98a1LL, 0x7ff0000000000000LL, 0},  //869.894
    {0x408c2628c79f6661LL, 0x7ff0000000000000LL, 0},  //900.77
    {0x408d1d2b8d823421LL, 0x7ff0000000000000LL, 0},  //931.646
    {0x408e142e536501e1LL, 0x7ff0000000000000LL, 0},  //962.523
    {0x408f0b311947cfa1LL, 0x7ff0000000000000LL, 0},  //993.399
    {0x40900119ef954eb1LL, 0x7ff0000000000000LL, 0},  //1024.28
    {0x40907c9b5286b591LL, 0x7ff0000000000000LL, 0},  //1055.15
    {0x4090f81cb5781c71LL, 0x7ff0000000000000LL, 0},  //1086.03
    {0x4091739e18698351LL, 0x7ff0000000000000LL, 0},  //1116.9
    {0x4091ef1f7b5aea31LL, 0x7ff0000000000000LL, 0},  //1147.78
    {0x40926aa0de4c5111LL, 0x7ff0000000000000LL, 0},  //1178.66
    {0x4092e622413db7f1LL, 0x7ff0000000000000LL, 0},  //1209.53
    {0x409361a3a42f1ed1LL, 0x7ff0000000000000LL, 0},  //1240.41
    {0x4093dd25072085b1LL, 0x7ff0000000000000LL, 0},  //1271.29
    {0x409458a66a11ec91LL, 0x7ff0000000000000LL, 0},  //1302.16
    {0x4094d427cd035371LL, 0x7ff0000000000000LL, 0},  //1333.04
    {0x40954fa92ff4ba51LL, 0x7ff0000000000000LL, 0},  //1363.92
    {0x4095cb2a92e62131LL, 0x7ff0000000000000LL, 0},  //1394.79
    {0x409646abf5d78811LL, 0x7ff0000000000000LL, 0},  //1425.67
    {0x4096c22d58c8eef1LL, 0x7ff0000000000000LL, 0},  //1456.54
    {0x40973daebbba55d1LL, 0x7ff0000000000000LL, 0},  //1487.42
    {0x4097b9301eabbcb1LL, 0x7ff0000000000000LL, 0},  //1518.3
    {0x409834b1819d2391LL, 0x7ff0000000000000LL, 0},  //1549.17
    {0x4098b032e48e8a71LL, 0x7ff0000000000000LL, 0},  //1580.05
    {0x40992bb4477ff151LL, 0x7ff0000000000000LL, 0},  //1610.93
    {0x4099a735aa715831LL, 0x7ff0000000000000LL, 0},  //1641.8
    {0x409a22b70d62bf11LL, 0x7ff0000000000000LL, 0},  //1672.68
    {0x409a9e38705425f1LL, 0x7ff0000000000000LL, 0},  //1703.56
    {0x409b19b9d3458cd1LL, 0x7ff0000000000000LL, 0},  //1734.43
    {0x409b953b3636f3b1LL, 0x7ff0000000000000LL, 0},  //1765.31
    {0x409c10bc99285a91LL, 0x7ff0000000000000LL, 0},  //1796.18
    // -37 < x < -3(to check m < -7)    //  
    {0xc042b66666666666LL, 0xbfefffffffffffffLL, 0},  //-37.425
    {0xc04238853c148344LL, 0xbfefffffffffffffLL, 0},  //-36.4416
    {0xc041baa411c2a022LL, 0xbfeffffffffffffcLL, 0},  //-35.4581
    {0xc0413cc2e770bd00LL, 0xbfeffffffffffff6LL, 0},  //-34.4747
    {0xc040bee1bd1ed9deLL, 0xbfefffffffffffe6LL, 0},  //-33.4913
    {0xc040410092ccf6bcLL, 0xbfefffffffffffbbLL, 0},  //-32.5078
    {0xc03f863ed0f62735LL, 0xbfefffffffffff48LL, 0},  //-31.5244
    {0xc03e8a7c7c5260f2LL, 0xbfeffffffffffe15LL, 0},  //-30.541
    {0xc03d8eba27ae9aafLL, 0xbfeffffffffffae0LL, 0},  //-29.5575
    {0xc03c92f7d30ad46cLL, 0xbfeffffffffff24cLL, 0},  //-28.5741
    {0xc03b97357e670e29LL, 0xbfefffffffffdb5eLL, 0},  //-27.5907
    {0xc03a9b7329c347e6LL, 0xbfefffffffff9e0eLL, 0},  //-26.6072
    {0xc0399fb0d51f81a3LL, 0xbfeffffffffefa22LL, 0},  //-25.6238
    {0xc038a3ee807bbb60LL, 0xbfeffffffffd43dfLL, 0},  //-24.6404
    {0xc037a82c2bd7f51dLL, 0xbfeffffffff8b01eLL, 0},  //-23.6569
    {0xc036ac69d7342edaLL, 0xbfefffffffec734cLL, 0},  //-22.6735
    {0xc035b0a782906897LL, 0xbfefffffffcbbb52LL, 0},  //-21.6901
    {0xc034b4e52deca254LL, 0xbfefffffff744120LL, 0},  //-20.7066
    {0xc033b922d948dc11LL, 0xbfeffffffe8a5f84LL, 0},  //-19.7232
    {0xc032bd6084a515ceLL, 0xbfeffffffc191023LL, 0},  //-18.7398
    {0xc031c19e30014f8bLL, 0xbfeffffff591386eLL, 0},  //-17.7563
    {0xc030c5dbdb5d8948LL, 0xbfefffffe41b5884LL, 0},  //-16.7729
    {0xc02f94330d738609LL, 0xbfefffffb56c9780LL, 0},  //-15.7895
    {0xc02d9cae642bf982LL, 0xbfefffff389ccbe7LL, 0},  //-14.806
    {0xc02ba529bae46cfbLL, 0xbfeffffdeae9c866LL, 0},  //-13.8226
    {0xc029ada5119ce074LL, 0xbfeffffa6eba43a1LL, 0},  //-12.8391
    {0xc027b620685553edLL, 0xbfeffff11d5c5a1aLL, 0},  //-11.8557
    {0xc025be9bbf0dc766LL, 0xbfefffd833cacb15LL, 0},  //-10.8723
    {0xc023c71715c63adfLL, 0xbfefff959896184aLL, 0},  //-9.88885
    {0xc021cf926c7eae58LL, 0xbfeffee3841cb342LL, 0},  //-8.90541
    {0xc01fb01b866e43a3LL, 0xbfeffd0765c71399LL, 0},  //-7.92198
    {0xc01bc11233df2a96LL, 0xbfeff80e6fd3cb47LL, 0},  //-6.93855
    {0xc017d208e1501189LL, 0xbfefeac30576b02bLL, 0},  //-5.95511
    {0xc013e2ff8ec0f87cLL, 0xbfefc737936835c4LL, 0},  //-4.97168
    {0xc00fe7ec7863bedeLL, 0xbfef682f19c4a09bLL, 0},  //-3.98824
    // -3 < x < 37(to check -8 < m < 53)    //
    {0xc00c816f0068db8cLL, 0xbfef17c601adf9beLL, 0},  //-3.5632
    {0xc004c2fd75e2046dLL, 0xbfed9ca11f57db60LL, 0},  //-2.59521
    {0xbffa0917d6b65a9cLL, 0xbfe9b67a06e41bccLL, 0},  //-1.62722
    {0xbfe51869835158bbLL, 0xbfdee56281d5ea14LL, 0},  //-0.65923
    {0x3fd3c2b94d940784LL, 0x3fd726acb76c32b6LL, 0},  //0.30876
    {0x3ff46d916872b020LL, 0x4004ae048d6ee477LL, 0},  //1.27675
    {0x4001f53a3ec02f2fLL, 0x4020e03c78ec6cd3LL, 0},  //2.24474
    {0x4009b3abc947064eLL, 0x4037d8c98f33a69cLL, 0},  //3.21273
    {0x4010b90ea9e6eeb7LL, 0x40501a6d80f23f6cLL, 0},  //4.18072
    {0x401498476f2a5a47LL, 0x406566b1be202b92LL, 0},  //5.14871
    {0x40187780346dc5d7LL, 0x407c45db9b6d5ad5LL, 0},  //6.1167
    {0x401c56b8f9b13167LL, 0x4092a23673f067f6LL, 0},  //7.08469
    {0x40201af8df7a4e7bLL, 0x40a88a68dc538786LL, 0},  //8.05268
    {0x40220a95421c0443LL, 0x40c027a8a9aa860cLL, 0},  //9.02067
    {0x4023fa31a4bdba0bLL, 0x40d544462e7e23bcLL, 0},  //9.98866
    {0x4025e9ce075f6fd3LL, 0x40ebfea1a91386e6LL, 0},  //10.9567
    {0x4027d96a6a01259bLL, 0x41026cdacb5dbd3aLL, 0},  //11.9246
    {0x4029c906cca2db63LL, 0x411840eef155d74eLL, 0},  //12.8926
    {0x402bb8a32f44912bLL, 0x412fecfab0f16851LL, 0},  //13.8606
    {0x402da83f91e646f3LL, 0x41450325861b8f25LL, 0},  //14.8286
    {0x402f97dbf487fcbbLL, 0x415ba8b3355896a9LL, 0},  //15.7966
    {0x4030c3bc2b94d941LL, 0x4172343f4aeeea24LL, 0},  //16.7646
    {0x4031bb8a5ce5b425LL, 0x4187f664f486a930LL, 0},  //17.7326
    {0x4032b3588e368f09LL, 0x419f8ad960baf680LL, 0},  //18.7006
    {0x4033ab26bf8769edLL, 0x41b4c28ed2b85f60LL, 0},  //19.6686
    {0x4034a2f4f0d844d1LL, 0x41cb53adc8b46b5aLL, 0},  //20.6366
    {0x40359ac322291fb5LL, 0x41e1fc4a04d3c74cLL, 0},  //21.6045
    {0x403692915379fa99LL, 0x41f7acbc34cc89c8LL, 0},  //22.5725
    {0x40378a5f84cad57dLL, 0x420f29e3c4135490LL, 0},  //23.5405
    {0x4038822db61bb061LL, 0x422482be2d9481f4LL, 0},  //24.5085
    {0x403979fbe76c8b45LL, 0x423affad777fda4aLL, 0},  //25.4765
    {0x403a71ca18bd6629LL, 0x4251c500b23d0b82LL, 0},  //26.4445
    {0x403b69984a0e410dLL, 0x426763f5d9ddbb4dLL, 0},  //27.4125
    {0x403c61667b5f1bf1LL, 0x427eca182fc13a52LL, 0},  //28.3805
    {0x403d5934acaff6d5LL, 0x429443b1b162c807LL, 0},  //29.3485
    {0x403e5102de00d1b9LL, 0x42aaacaf5cdb4998LL, 0},  //30.3165
    {0x403f48d10f51ac9dLL, 0x42c18e6152098cf9LL, 0},  //31.2844
    {0x4040204fa05143c0LL, 0x42d71c0f338683d9LL, 0},  //32.2524
    {0x40409c36b8f9b132LL, 0x42ee6b73137e24acLL, 0},  //33.2204
    {0x4041181dd1a21ea4LL, 0x43040567041e2d4cLL, 0},  //34.1884
    {0x40419404ea4a8c16LL, 0x431a5ab05f86b6beLL, 0},  //35.1564
    {0x40420fec02f2f988LL, 0x43315869d9f002a2LL, 0},  //36.1244
    {0x40428bd31b9b66faLL, 0x4346d505922dee4aLL, 0},  //37.0924

    {0x3EE0624DD2F1A9FCLL,0x3ee0625204b0496aLL},  //0.0000078125
    {0x3EF0624DC31C6CA1LL,0x3ef06256269b09c8LL},  //0.0000156249991
    {0x3FE02467BE553AC5LL,0x3fe4fe7af67676faLL},  //0.504444
    {0x3FF0000000000000LL,0x3ffb7e151628aed3LL},  //1
    {0x3D3C25C268497682LL,0x3d3c25c26849780eLL},  //1.0000000e-13
    {0x4060000000000000LL,0x4b795e54c5dd4217LL},  //128
    {0x4086240000000000LL,0x7fd1bf058bc994adLL},  //708.5
    {0x4086280000000000LL,0x7fdd422d2be5dc9bLL},  //709
    {0x408C000000000000LL,0x7ff0000000000000LL},  //896
    {0xC086240000000000LL,0xbff0000000000000LL},  //-708.5
    {0xC089600000000000LL,0xbff0000000000000LL},  //-812
    {0xC070000000000000LL,0xbff0000000000000LL},  //-256
    {0xc086232bdd7abcd2LL,0xbff0000000000000LL},  // -708.3964185322641 smallest normal result
    {0xc086232bdd7abcd3LL,0xbff0000000000000LL},  // -708.3964185322642 largest denormal result
    {0xC087480000000000LL,0xbff0000000000000LL},  //-745
    {0x40862e42fefa39eeLL,0x7feffffffffffb2aLL},  //7.09782712893383973096e+02   largest value  --
    {0x40862e42fefa39efLL,0x7fefffffffffff2aLL},  //7.09782712893383973096e+02   largest value
    {0x40862e42fefa39ffLL,0x7ff0000000000000LL},  //7.09782712893383973096e+02   overflow
    {0x4200000000000000LL,0x7ff0000000000000LL},  //large   overflow
    {0xC05021EB851EB852LL,0xbff0000000000000LL}, // -64.53
    {0xC0FF5D35B020C49CLL,0xbff0000000000000LL}, // -128467.3555
    {0xBFD3333333333333LL,0xbfd0966f2c7907f6LL}, // -0.3
    {0xBFE48E410B630A91LL,0xbfde553f96c59dcfLL}, // -0.642365
    {0xBFE0000000000000LL,0xbfd92e9a0720d3ecLL}, // -0.5
    {0x3FD3333333333333LL,0x3fd6641632306a56LL}, // 0.3
    {0x3FE48E410B630A91LL,0x3fecd4c1e623c665LL}, // 0.642365
    {0x3FE0000000000000LL,0x3fe4c2531c3c0d38LL}, // 0.5
    {0x405021EB851EB852LL,0x45c11d33b0716b95LL}, // 64.53
    {0x40FF5D35B020C49CLL,0x7ff0000000000000LL}, // 128467.3555

};

