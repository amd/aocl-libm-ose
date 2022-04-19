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
 * Test cases to check for exceptions for the exp2f() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_exp2f_conformance_data[] = {
   // special accuracy tests
    #if defined(_WIN64) || defined(_WIN32)
        {0xc3150001, 0x1,         3},  // exp2f(-0x1.2a0002p+7)
    #else
        {0xc3150001, 0x1,         48},  // exp2f(-0x1.2a0002p+7)
    #endif
   {0x38800000, 0x3f800000,  FE_INEXACT},  //min= 0.00006103515625, small enough that exp2(x) = 1
   {0x387FFFFF, 0x3f800000,  FE_INEXACT}, //min - 1 bit
   {0x38800001, 0x3f800000,  FE_INEXACT}, //min + 1 bit
   #if defined(_WIN64) || defined(_WIN32)
       {0xF149F2C9, 0x7f800000,  3}, //lambda + x = 1, x = -9.9999994e+29
   #else
       {0xF149F2C9, 0x7f800000,  48}, //lambda + x = 1, x = -9.9999994e+29
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xF149F2C8, 0x7f800000,  3}, //lambda + x < 1
   #else
       {0xF149F2C8, 0x7f800000,  48}, //lambda + x < 1
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xF149F2CA, 0x7f800000,  3}, //lambda + x > 1
   #else
       {0xF149F2CA, 0x7f800000,  48}, //lambda + x > 1
   #endif
   {0x42B2D4FC, 0x7f7fffec,  0}, //max arg, x = 89.41598629223294,max exp2f arg
   {0x42B2D4FB, 0x7f7fff6c,  0}, //max arg - 1 bit
   {0x42B2D4FD, 0x7f800000,  FE_INEXACT}, //max arg + 1 bit
   {0x42B2D4FF, 0x7f800000,  FE_INEXACT}, // > max
   {0x42B2D400, 0x7f7f820b,  0}, // < max
   {0x41A00000, 0x4d675844,  0}, //small_threshold = 20
   {0x41A80000, 0x4e1d3710,  0}, //small_threshold+1 = 21
   {0x41980000, 0x4caa36c8,  0}, //small_threshold - 1 = 19

   /*recently found out ranges with huge ULPs*/
   #if defined(_WIN64) || defined(_WIN32)
       {0xc2fc247c, POS_ZERO_F32, 3},
   #else
       {0xc2fc247c, POS_ZERO_F32, 48},
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc2fc2ca3, POS_ZERO_F32, 3},
   #else
       {0xc2fc2ca3, POS_ZERO_F32, 48},
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc2fc7b81, POS_ZERO_F32, 3},
   #else
       {0xc2fc7b81, POS_ZERO_F32, 48},
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc2fcd3d7, POS_ZERO_F32, 3},
   #else
       {0xc2fcd3d7, POS_ZERO_F32, 48},
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc2fd3382, POS_ZERO_F32, 3},
   #else
       {0xc2fd3382, POS_ZERO_F32, 48},
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc2fd82d0, POS_ZERO_F32, 3},
   #else
       {0xc2fd82d0, POS_ZERO_F32, 48},
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc2fd9dc2, POS_ZERO_F32, 3},
   #else
       {0xc2fd9dc2, POS_ZERO_F32, 48},
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc2fdd3c4, POS_ZERO_F32, 3},
   #else
       {0xc2fdd3c4, POS_ZERO_F32, 48},
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc2fe1422, POS_ZERO_F32, 3},
   #else
       {0xc2fe1422, POS_ZERO_F32, 48},
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc2fe5bfd, POS_ZERO_F32, 3},
   #else
       {0xc2fe5bfd, POS_ZERO_F32, 48},
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc2ff9bdb, POS_ZERO_F32, 3},
   #else
       {0xc2ff9bdb, POS_ZERO_F32, 48},
   #endif
    //exp2 special exception checks
   {POS_ZERO_F32, 0x3f800000,0 },  //0
   {NEG_ZERO_F32, 0x3f800000,0 },  //0
   {POS_INF_F32,  POS_INF_F32,0 },
   {NEG_INF_F32,  NEG_INF_F32,0 },
   {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID },  //
   {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID },  //
   {POS_QNAN_F32, POS_QNAN_F32, 0 },  //
   {NEG_QNAN_F32, NEG_QNAN_F32, 0 },  //

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
   #if defined(_WIN64) || defined(_WIN32)
       {0xc5812e71, 0x7f800000,  3},  // -normal intermediate
   #else
       {0xc5812e71, 0x7f800000,  48},  // -normal intermediate
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xFF7FFFFF, 0x7f800000,  1},  // -normal max
   #else
       {0xFF7FFFFF, 0x7f800000,  48},  // -normal max
   #endif
   
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
   #if defined(_WIN64) || defined(_WIN32)
       {0xc47A0000, 0x7f800000,  3},  // -1000
   #else
       {0xc47A0000, 0x7f800000,  48},  // -1000
   #endif
   {0x4286CCCC, 0x6f96eb6f,  0},  // 67.4
   {0xc286CCCC, 0x6f96eb6f,  0},  // -67.4
   {0x44F7F333, 0x7f800000,  FE_OVERFLOW},  // 1983.6
   #if defined(_WIN64) || defined(_WIN32)
       {0xc4F7F333, 0x7f800000,  3},  // -1983.6
   #else
       {0xc4F7F333, 0x7f800000,  48},  // -1983.6
   #endif
   {0x42AF0000, 0x7e16bab3,  0},  // 87.5
   {0xc2AF0000, 0x7e16bab3,  0},  // -87.5
   {0x48015E40, 0x7f800000,  FE_OVERFLOW},  // 132473
   #if defined(_WIN64) || defined(_WIN32)
       {0xc8015E40, 0x7f800000,  3},  // -132473
   #else
       {0xc8015E40, 0x7f800000,  48},  // -132473
   #endif
   {0x4B000000, 0x7f800000,  FE_OVERFLOW},  // 2^23
   {0x4B000001, 0x7f800000,  FE_OVERFLOW},  // 2^23 + 1
   {0x4AFFFFFF, 0x7f800000,  FE_OVERFLOW},  // 2^23 -1 + 0.5
   #if defined(_WIN64) || defined(_WIN32)
       {0xcB000000, 0x7f800000,  3},  // -2^23
   #else
       {0xcB000000, 0x7f800000,  48},  // -2^23
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xcB000001, 0x7f800000,  3},  // -(2^23 + 1)
   #else
       {0xcB000001, 0x7f800000,  48},  // -(2^23 + 1)
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xcAFFFFFF, 0x7f800000,  3},  // -(2^23 -1 + 0.5)
   #else
       {0xcAFFFFFF, 0x7f800000,  48},  // -(2^23 -1 + 0.5)
   #endif
//added from ancient libm repo
   #if defined(_WIN64) || defined(_WIN32)
       {0x7f7fffff, 0x7f800000, 5}, // 3.40282346638528859812e+38
   #else
       {0x7f7fffff, 0x7f800000, 40}, // 3.40282346638528859812e+38
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xff7fffff, 0x00000000, 1}, //-3.40282346638528859812e+38
   #else
       {0xff7fffff, 0x00000000, 48}, //-3.40282346638528859812e+38
   #endif

   {0xb3000000, 0x3f800000, 0},  //-2.98023e-08        
   {0xb2e47e05, 0x3f800000, 0},  //-2.66e-08
   {0xb2c8fc0a, 0x3f800000, 0},  //-2.33977e-08
   {0xb2ad7a0f, 0x3f800000, 0},  //-2.01954e-08
   {0xb291f814, 0x3f800000, 0},  //-1.6993e-08
   {0xb26cec32, 0x3f800000, 0},  //-1.37907e-08
   {0xb235e83c, 0x3f800000, 0},  //-1.05884e-08
   {0xb1fdc88b, 0x3f800000, 0},  //-7.38607e-09
   {0xb18fc09e, 0x3f800000, 0},  //-4.18375e-09
   {0xb086e2c4, 0x3f800000, 0},  //-9.81423e-10
   {0x31189e78, 0x3f800000, 0},  //2.2209e-09
   {0x31ba5729, 0x3f800000, 0},  //5.42322e-09
   {0x32142f8b, 0x3f800000, 0},  //8.62554e-09
   {0x324b3381, 0x3f800000, 0},  //1.18279e-08
   {0x32811bbc, 0x3f800000, 0},  //1.50302e-08
   {0x329c9db7, 0x3f800000, 0},  //1.82325e-08
   {0x32b81fb2, 0x3f800000, 0},  //2.14348e-08
   {0x32d3a1ad, 0x3f800000, 0},  //2.46372e-08
   {0x32ef23a8, 0x3f800000, 0},  //2.78395e-08
   #if defined(_WIN64) || defined(_WIN32)
       {0x436c5cfa, 0x7f800000, 5},  //236.363
   #else
       {0x436c5cfa, 0x7f800000, 40},  //236.363
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x43715cfa, 0x7f800000, 5},  //241.363
   #else
       {0x43715cfa, 0x7f800000, 40},  //241.363
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x43765cfa, 0x7f800000, 5},  //246.363
   #else
       {0x43765cfa, 0x7f800000, 40},  //246.363
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x437b5cfa, 0x7f800000, 5},  //251.363
   #else
       {0x437b5cfa, 0x7f800000, 40},  //251.363
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x43802e7d, 0x7f800000, 5},  //256.363
   #else
       {0x43802e7d, 0x7f800000, 40},  //256.363
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x4382ae7d, 0x7f800000, 5},  //261.363
   #else
       {0x4382ae7d, 0x7f800000, 40},  //261.363
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x43852e7d, 0x7f800000, 5},  //266.363
   #else
       {0x43852e7d, 0x7f800000, 40},  //266.363
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x4387ae7d, 0x7f800000, 5},  //271.363
   #else
       {0x4387ae7d, 0x7f800000, 40},  //271.363
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x438a2e7d, 0x7f800000, 5},  //276.363
   #else
       {0x438a2e7d, 0x7f800000, 40},  //276.363
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x438cae7d, 0x7f800000, 5},  //281.363
   #else
       {0x438cae7d, 0x7f800000, 40},  //281.363
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x438f2e7d, 0x7f800000, 5},  //286.363
   #else
       {0x438f2e7d, 0x7f800000, 40},  //286.363
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x4391ae7d, 0x7f800000, 5},  //291.363
   #else
       {0x4391ae7d, 0x7f800000, 40},  //291.363
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x43942e7d, 0x7f800000, 5},  //296.363
   #else
       {0x43942e7d, 0x7f800000, 40},  //296.363
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x4398ab85, 0x7f800000, 5},  //305.34
   #else
       {0x4398ab85, 0x7f800000, 40},  //305.34
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x459a800b, 0x7f800000, 5},  //4944.01
   #else
       {0x459a800b, 0x7f800000, 40},  //4944.01
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x4615baaf, 0x7f800000, 5},  //9582.67
   #else
       {0x4615baaf, 0x7f800000, 40},  //9582.67
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x465e3558, 0x7f800000, 5},  //14221.3
   #else
       {0x465e3558, 0x7f800000, 40},  //14221.3
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x46935801, 0x7f800000, 5},  //18860
   #else
       {0x46935801, 0x7f800000, 40},  //18860
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x46b79556, 0x7f800000, 5},  //23498.7
   #else
       {0x46b79556, 0x7f800000, 40},  //23498.7
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x46dbd2ab, 0x7f800000, 5},  //28137.3
   #else
       {0x46dbd2ab, 0x7f800000, 40},  //28137.3
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x47000800, 0x7f800000, 5},  //32776
   #else
       {0x47000800, 0x7f800000, 40},  //32776
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x471226aa, 0x7f800000, 5},  //37414.7
   #else
       {0x471226aa, 0x7f800000, 40},  //37414.7
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x47244554, 0x7f800000, 5},  //42053.3
   #else
       {0x47244554, 0x7f800000, 40},  //42053.3
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x473663fe, 0x7f800000, 5},  //46692
   #else
       {0x473663fe, 0x7f800000, 40},  //46692
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc788b83a, 0x00000000, 3},  //-70000.5
   #else
       {0xc788b83a, 0x00000000, 48},  //-70000.5
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc7812c04, 0x00000000, 3},  //-66136
   #else
       {0xc7812c04, 0x00000000, 48},  //-66136
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc7733f9c, 0x00000000, 3},  //-62271.6
   #else
       {0xc7733f9c, 0x00000000, 48},  //-62271.6
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc7642730, 0x00000000, 3},  //-58407.2
   #else
       {0xc7642730, 0x00000000, 48},  //-58407.2
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc7550ec4, 0x00000000, 3},  //-54542.8
   #else
       {0xc7550ec4, 0x00000000, 48},  //-54542.8
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc745f658, 0x00000000, 3},  //-50678.3
   #else
       {0xc745f658, 0x00000000, 48},  //-50678.3
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc736ddec, 0x00000000, 3},  //-46813.9
   #else
       {0xc736ddec, 0x00000000, 48},  //-46813.9
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc727c580, 0x00000000, 3},  //-42949.5
   #else
       {0xc727c580, 0x00000000, 48},  //-42949.5
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc718ad14, 0x00000000, 3},  //-39085.1
   #else
       {0xc718ad14, 0x00000000, 48},  //-39085.1
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc70994a8, 0x00000000, 3},  //-35220.7
   #else
       {0xc70994a8, 0x00000000, 48},  //-35220.7
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc6f4f877, 0x00000000, 3},  //-31356.2
   #else
       {0xc6f4f877, 0x00000000, 48},  //-31356.2
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc6d6c79e, 0x00000000, 3},  //-27491.8
   #else
       {0xc6d6c79e, 0x00000000, 48},  //-27491.8
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc6b896c5, 0x00000000, 3},  //-23627.4
   #else
       {0xc6b896c5, 0x00000000, 48},  //-23627.4
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc69a65ec, 0x00000000, 3},  //-19763
   #else
       {0xc69a65ec, 0x00000000, 48},  //-19763
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc6786a26, 0x00000000, 3},  //-15898.5
   #else
       {0xc6786a26, 0x00000000, 48},  //-15898.5
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc63c0874, 0x00000000, 3},  //-12034.1
   #else
       {0xc63c0874, 0x00000000, 48},  //-12034.1
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc5ff4d85, 0x00000000, 3},  //-8169.69
   #else
       {0xc5ff4d85, 0x00000000, 48},  //-8169.69
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc5868a22, 0x00000000, 3},  //-4305.27
   #else
       {0xc5868a22, 0x00000000, 48},  //-4305.27
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0xc3dc6bee, 0x00000000, 3},  //-440.843
   #else
       {0xc3dc6bee, 0x00000000, 48},  //-440.843
   #endif
   {0xbe934b11, 0x3f51b838, 0},  //-0.287682
   {0xbe86e2ab, 0x3f5545ad, 0},  //-0.263448
   {0xbe74f48a, 0x3f58e28a, 0},  //-0.239214
   {0xbe5c23be, 0x3f5c8f11, 0},  //-0.21498
   {0xbe4352f2, 0x3f604b88, 0},  //-0.190746
   {0xbe2a8226, 0x3f641832, 0},  //-0.166512
   {0xbe11b15a, 0x3f67f556, 0},  //-0.142278
   {0xbdf1c11c, 0x3f6be33b, 0},  //-0.118044
   {0xbdc01f84, 0x3f6fe22b, 0},  //-0.0938101
   {0xbd8e7dec, 0x3f73f26e, 0},  //-0.0695761
   {0xbd39b8a7, 0x3f781451, 0},  //-0.0453421
   {0xbcaceaec, 0x3f7c481f, 0},  //-0.0211081
   {0x3b4cdbad, 0x3f804713, 0},  //0.00312589
   {0x3ce021d7, 0x3f82735b, 0},  //0.0273599
   {0x3d53541c, 0x3f84a90f, 0},  //0.0515939
   {0x3d9b4ba6, 0x3f86e859, 0},  //0.0758279
   {0x3dcced3e, 0x3f893161, 0},  //0.100062
   {0x3dfe8ed6, 0x3f8b8452, 0},  //0.124296
   {0x3e181837, 0x3f8de157, 0},  //0.14853
   {0x3e30e903, 0x3f90489c, 0},  //0.172764
   {0x3e49b9cf, 0x3f92ba4d, 0},  //0.196998
   {0x3e628a9b, 0x3f953698, 0},  //0.221232
   {0xc173b646, 0x37d9f8db, 0},  //-15.232
   {0xc160ec1f, 0x3875f8f6, 0},  //-14.0576
   {0xc14e21f8, 0x390ac8f0, 0},  //-12.8833
   {0xc13b57d1, 0x399c9cf0, 0},  //-11.7089
   {0xc1288daa, 0x3a30bb39, 0},  //-10.5346
   {0xc115c383, 0x3ac76f1b, 0},  //-9.36023
   {0xc102f95c, 0x3b610d94, 0},  //-8.18588
   {0xc0e05e69, 0x3bfdf690, 0},  //-7.01152
   {0xc0baca1a, 0x3c8f4b23, 0},  //-5.83717
   {0xc09535cb, 0x3d21b36b, 0},  //-4.66282
   {0xc05f42f8, 0x3db67909, 0},  //-3.48846
   {0xc0141a5a, 0x3e4de9bf, 0},  //-2.31411
   {0xbf91e379, 0x3ee85d4d, 0},  //-1.13975
   {0x3d0db84b, 0x3f831b5c, 0},  //0.0345996
   {0x3f9abefe, 0x4013f2e1, 0},  //1.20895
   {0x4018881d, 0x40a6f440, 0},  //2.38331
   {0x4063b0bb, 0x413c669f, 0},  //3.55766
   {0x40976cac, 0x41d49a46, 0},  //4.73202
   {0x40bd00fb, 0x426fe9d2, 0},  //5.90637
   {0x40e2954a, 0x43075dbf, 0},  //7.08072
   {0x410414cc, 0x4398c153, 0},  //8.25508
   {0x4116def3, 0x442c60be, 0},  //9.42943
   {0x4129a91a, 0x44c28576, 0},  //10.6038
   {0x413c7341, 0x455b8261, 0},  //11.7781
   {0x414f3d68, 0x45f7b508, 0},  //12.9525
   {0x4162078f, 0x468bc37e, 0},  //14.1268
   {0x4174d1b6, 0x471db7ae, 0},  //15.3012
   {0x4173b646, 0x471654c7, 0},  //15.232
   {0x41d0dde9, 0x4c89fbd7, 0},  //26.1084
   {0x4213f057, 0x51fd4cf6, 0},  //36.9847
   {0x423f71ba, 0x57687eef, 0},  //47.8611
   {0x426af31d, 0x5cd5665c, 0},  //58.7374
   {0x428b3a40, 0x6243df4f, 0},  //69.6138
   {0x42a0faf1, 0x67b3c8ad, 0},  //80.4901
   {0x42B16D62, 0x6bd1e961, 0},  //88.713638
   {0x42B19999, 0x6bdedc38, 0},  //88.799995
   {0x42b6bba2, 0x6d250456, 0},  //91.3665
   {0x42cc7c53, 0x72977681, 0},  //102.243
   {0x42e23d04, 0x780b05ad, 0},  //113.119
   {0x42f7fdb5, 0x7d7f34e0, 0},  //123.996
   #if defined(_WIN64) || defined(_WIN32)
       {0x4306df33, 0x7f800000, 5},  //134.872
   #else
       {0x4306df33, 0x7f800000, 40},  //134.872
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x4311bf8c, 0x7f800000, 5},  //145.748
   #else
       {0x4311bf8c, 0x7f800000, 40},  //145.748
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x431c9fe5, 0x7f800000, 5},  //156.625
   #else
       {0x431c9fe5, 0x7f800000, 40},  //156.625
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x4327803e, 0x7f800000, 5},  //167.501
   #else
       {0x4327803e, 0x7f800000, 40},  //167.501
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x43326097, 0x7f800000, 5},  //178.377
   #else
       {0x43326097, 0x7f800000, 40},  //178.377
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x433d40f0, 0x7f800000, 5},  //189.254
   #else
       {0x433d40f0, 0x7f800000, 40},  //189.254
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x43482149, 0x7f800000, 5},  //200.13
   #else
       {0x43482149, 0x7f800000, 40},  //200.13
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x435301a2, 0x7f800000, 5},  //211.006
   #else
       {0x435301a2, 0x7f800000, 40},  //211.006
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x435de1fb, 0x7f800000, 5},  //221.883
   #else
       {0x435de1fb, 0x7f800000, 40},  //221.883
   #endif
   #if defined(_WIN64) || defined(_WIN32)
       {0x4368c254, 0x7f800000, 5},  //232.759
   #else
       {0x4368c254, 0x7f800000, 40},  //232.759
   #endif
   {0xc18b6666, 0x36beadc7, 0},  //-17.425
   {0xc18521ed, 0x372419b2, 0},  //-16.6416
   {0xc17dbae8, 0x378d3a06, 0},  //-15.8581
   {0xc17131f6, 0x37f31522, 0},  //-15.0747
   {0xc164a904, 0x3851331a, 0},  //-14.2913
   {0xc1582012, 0x38b40a22, 0},  //-13.5078
   {0xc14b9720, 0x391af1b2, 0},  //-12.7244
   {0xc13f0e2e, 0x398558c0, 0},  //-11.941
   {0xc132853c, 0x39e584f5, 0},  //-11.1575
   {0xc125fc4a, 0x3a4586e9, 0},  //-10.3741
   {0xc1197358, 0x3aa9fe76, 0},  //-9.59066
   {0xc10cea66, 0x3b124c7c, 0},  //-8.80723
   {0xc1006174, 0x3b7bd013, 0},  //-8.02379
   {0xc0e7b104, 0x3bd8b685, 0},  //-7.24036
   {0xc0ce9f20, 0x3c3a8173, 0},  //-6.45692
   {0xc0b58d3c, 0x3ca08247, 0},  //-5.67349
   {0xc09c7b58, 0x3d0a22c3, 0},  //-4.89006
   {0xc0836974, 0x3d6dc332, 0},  //-4.10662
   {0xc0640b78, 0x3dad42b8, 0},  //-3.5632
   {0xc02617ec, 0x3e29756e, 0},  //-2.59521
   {0xbfd048bf, 0x3ea5bd7f, 0},  //-1.62722
   {0xbf28c34d, 0x3f221a74, 0},  //-0.65923
   {0x3e9e15c9, 0x3f9e8bd7, 0},  //0.30876
   {0x3fa36c8b, 0x401b1136, 0},  //1.27675
   {0x400fa9d2, 0x4097aa1f, 0},  //2.24474
   {0x404d9d5e, 0x41145626, 0},  //3.21273
   {0x4085c875, 0x419114de, 0},  //4.18072
   {0x40a4c23b, 0x420de5df, 0},  //5.14871
   {0x40c3bc01, 0x428ac8c2, 0},  //6.1167
   {0x40e2b5c7, 0x4307bd24, 0},  //7.08469
   {0x4100d7c7, 0x4384c2a3, 0},  //8.05268
   {0x411054aa, 0x4401d8db, 0},  //9.02067
   {0x411fd18d, 0x447dfee2, 0},  //9.98866
   {0x412f4e70, 0x44f86c10, 0},  //10.9566
   {0x413ecb53, 0x4572f88d, 0},  //11.9246
   {0x414e4836, 0x45eda3a9, 0},  //12.8926
   {0x415dc519, 0x46686cb9, 0},  //13.8606
   {0x416d41fc, 0x46e35313, 0},   //14.8286

   {0x3c000000, 0x3f80b1ed, 0},  //0.0078125
   {0x3c7fffff, 0x3f8164d2, 0},  //0.0156249991
   {0x3f012345, 0x3fb593ee, 0},  //0.504444
   {0x40000000, 0x40800000, 0},  //2

   {0x42800000, 0x5f800000, 0},  //64
   {0x42b00000, 0x6b800000, 0},  //88
   {0x42c00000, 0x6f800000, 0},  //96
   {0xc2e00000, 0x07800000, 0},  //-112
   {0xc3000000, 0x00200000, 0},  //-128

   {0xc2fc0000, 0x00800000, 0},  //-126  smallest normal result
   {0xc2fc0001, 0x007fffd4, 0},  //-126.000008  largest denormal result
   {0xc3150000, 0x00000001, 0},  //-149
   {0x42ffffff, 0x7f7fffa7, 0},  //127.999992   largest value
   {0x43000000, 0x7f800000, 0},  //   overflow
   {0x50000000, 0x7f800000, 0},  //   overflow

   {0xc20a1eb8, 0x2e314b4e, 0}, // -34.53
   {0xc6de66b6, 0x00000000, 0}, // -28467.3555
   {0xbe99999a, 0x3f4fefc6, 0}, // -0.3
   {0xbf247208, 0x3f24026a, 0}, // -0.642365
   {0xbf000000, 0x3f3504f3, 0}, // -0.5
   {0x3e99999a, 0x3f9d9624, 0}, // 0.3
   {0x3f247208, 0x3fc7cb1b, 0}, // 0.642365
   {0x3f000000, 0x3fb504f3, 0}, // 0.5
   {0x420a1eb8, 0x50b8d2a2, 0}, // 34.53
   {0x46de66b6, 0x7f800000, 0}, // 28467.3555
   {0xc2c80000, 0x0d800000, 0}, // -100
   {0x42c80000, 0x71800000, 0}, // 100
};

static libm_test_special_data_f64
test_exp2_conformance_data[] = {
    // special accuracy tests
    {0x3e30000000000000LL, 0x3ff0000000000000LL, 32},  //min, small enough that exp2(x) = 1 //
    {0x3E2FFFFFFFFFFFFFLL, 0x3ff0000000000000LL, 32}, //min - 1 bit
    {0x3e30000000000001LL, 0x3ff0000000000000LL, 32}, //min + 1 bit
    {0xFE37E43C8800759CLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x = 1, x = -1.0000000000000000e+300
    {0xFE37E43C8800758CLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x < 1
    {0xFE37E43C880075ACLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x > 1
    {0x408633ce8fb9f87eLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //max arg, x = 89.41598629223294,max exp2f arg
    {0x408633ce8fb9f87dLL, 0x7feffffffffffd3bLL, FE_INEXACT}, //max arg - 1 bit
    {0x408633ce8fb9f87fLL, 0x7ff0000000000000LL, FE_INEXACT}, //max arg + 1 bit
    {0x408633ce8fb9f8ffLL, 0x7ff0000000000000LL, FE_INEXACT}, // > max
    {0x408633ce8fb9f800LL, 0x7feffffffffe093bLL, FE_INEXACT}, // < max
    {0x4034000000000000LL, 0x41aceb088b68e804LL, 0}, //small_threshold = 20
    {0x4035000000000000LL, 0x41c3a6e1fd9eecfdLL, 0}, //small_threshold+1 = 21
    {0x4033000000000000LL, 0x419546d8f9ed26e2LL, 0}, //small_threshold - 1 = 19

    //exp2 special exception checks
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

    {0x43b3c4eafedcab02LL, 0x7ff0000000000000LL, 40}, // normal intermediate
    {0x7FEFFFFFFFFFFFFFLL, 0x7ff0000000000000LL, 40}, // normal max
    {0x8010000000000000LL, 0x3ff0000000000000LL, 0}, // -normal min
    {0xc5812e71245acfdbLL, 0x7ff0000000000000LL, 48}, // -normal intermediate
    {0xFFEFFFFFFFFFFFFFLL, 0x7ff0000000000000LL, 48}, // -normal max

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
    {0x3FF921FB54442D18LL, 0x400412cc2a8d4e9eLL, 32}, // pi/2
    {0x400921FB54442D18LL, 0x40272f147fee4000LL, 32}, // pi
    {0x401921FB54442D18LL, 0x4070bbf2bc2b69c6LL, 32}, // 2pi
    {0x3FFB7E151628AED3LL, 0x4007046b4f3f7e0cLL, 32}, // e --
    {0x4005BF0A8B145769LL, 0x401e70c4a4f41684LL, 32}, // e
    {0x400DBF0A8B145769LL, 0x40349be1e586228dLL, 32}, // e ++
    {0x0000000000000000LL, 0x3ff0000000000000LL, 0}, // 0
    {0x3C4536B8B14B676CLL, 0x3ff0000000000000LL, 0}, // 0.0000000000000000023
    {0x3FDFFFFBCE4217D3LL, 0x3ff20ac0fa498d2cLL, 0}, // 0.4999989999999999999
    {0x3FE000000000006CLL, 0x3ff20ac1862ae8edLL, FE_INEXACT}, // 0.500000000000012
    {0x8000000000000000LL, 0x3ff0000000000000LL, 0}, // -0
    {0xBBDB2752CE74FF42LL, 0x3ff0000000000000LL, 0}, // -0.000000000000000000023
    {0xBFDFFFFBCE4217D3LL, 0x3ff20ac0fa498d2cLL, FE_INEXACT}, // -0.4999989999999999999
    {0xBFE000000000006CLL, 0x3ff20ac1862ae8edLL, 0}, // -0.500000000000012
    {0x3FF0000000000000LL, 0x3ff8b07551d9f550LL, 0}, // 1
    {0x3FEFFFFFC49BD0DCLL, 0x3ff8b0752ef3f8a2LL, 0}, // 0.9999998893750006
    {0x3FF0000000000119LL, 0x3ff8b07551d9f69bLL, 0}, // 1.0000000000000624998
    {0x3FF7FFFEF39085F4LL, 0x4002d1bb0418ae52LL, FE_INEXACT}, // 1.499998999999999967
    {0x3FF8000000000001LL, 0x4002d1bc21e22023LL, FE_INEXACT}, // 1.50000000000000012
    {0xBFF0000000000000LL, 0x3ff8b07551d9f550LL, 0}, // -1
    {0xBFEFFFFFC49BD0DCLL, 0x3ff8b0752ef3f8a2LL, 0}, // -0.9999998893750006
    {0xBFF0000000000119LL, 0x3ff8b07551d9f69bLL, 0}, // -1.0000000000000624998
    {0xBFF7FFFEF39085F4LL, 0x4002d1bb0418ae52LL, 0}, // -1.499998999999999967
    {0xBFF8000000000001LL, 0x4002d1bc21e22023LL, 0}, // -1.50000000000000012

    {0x4000000000000000LL, 0x400e18fa0df2d9bcLL, 0}, // 2
    {0xC000000000000000LL, 0x400e18fa0df2d9bcLL, 0}, // -2
    {0x4024000000000000LL, 0x40c5829dd053712dLL, 0}, // 10
    {0xC024000000000000LL, 0x40c5829dd053712dLL, 0}, // -10
    {0x408F400000000000LL, 0x7ff0000000000000LL, 0}, // 1000
    {0xC08F400000000000LL, 0x7ff0000000000000LL, 0}, // -1000
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
    {0xbc90000000000000LL, 0x3ff0000000000000LL, 0},  //-5.55112e-17
    {0xbc8ccccccccccccdLL, 0x3ff0000000000000LL, 0},  //-4.996e-17
    {0xbc8999999999999aLL, 0x3ff0000000000000LL, 0},  //-4.44089e-17
    {0xbc86666666666667LL, 0x3ff0000000000000LL, 0},  //-3.88578e-17
    {0xbc83333333333334LL, 0x3ff0000000000000LL, 0},  //-3.33067e-17
    {0xbc80000000000001LL, 0x3ff0000000000000LL, 0},  //-2.77556e-17
    {0xbc7999999999999cLL, 0x3ff0000000000000LL, 0},  //-2.22045e-17
    {0xbc73333333333336LL, 0x3ff0000000000000LL, 0},  //-1.66533e-17
    {0xbc6999999999999fLL, 0x3ff0000000000000LL, 0},  //-1.11022e-17
    {0xbc599999999999a4LL, 0x3ff0000000000000LL, 0},  //-5.55112e-18
    {0xb944000000000000LL, 0x3ff0000000000000LL, 0},  //-7.70372e-33
    {0x3c59999999999990LL, 0x3ff0000000000000LL, 0},  //5.55112e-18
    {0x3c69999999999995LL, 0x3ff0000000000000LL, 0},  //1.11022e-17
    {0x3c73333333333331LL, 0x3ff0000000000000LL, 0},  //1.66533e-17
    {0x3c79999999999998LL, 0x3ff0000000000000LL, 0},  //2.22045e-17
    {0x3c7ffffffffffffeLL, 0x3ff0000000000000LL, 0},  //2.77556e-17
    {0x3c83333333333332LL, 0x3ff0000000000000LL, 0},  //3.33067e-17
    {0x3c86666666666665LL, 0x3ff0000000000000LL, 0},  //3.88578e-17
    {0x3c89999999999998LL, 0x3ff0000000000000LL, 0},  //4.44089e-17
    {0x3c8ccccccccccccbLL, 0x3ff0000000000000LL, 0},  //4.996e-17
    {0x3c8ffffffffffffeLL, 0x3ff0000000000000LL, 0},  //5.55112e-17
    {0x409c4474e1726455LL, 0x7ff0000000000000LL, FE_OVERFLOW},  //1809.11
    {0x409c8e99130401ccLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1827.65
    {0x409cd8bd44959f43LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1846.18
    {0x409d22e176273cbaLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1864.72
    {0x409d6d05a7b8da31LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1883.26
    {0x409db729d94a77a8LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1901.79
    {0x409e014e0adc151fLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1920.33
    {0x409e4b723c6db296LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1938.86
    {0x409e95966dff500dLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1957.4
    {0x409edfba9f90ed84LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1975.93
    {0x409f29ded1228afbLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1994.47
    {0x409f42421c044285LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //2000.56
    {0x40c4c79d6d6dc1a5LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //10639.2
    {0x40d2d3794bad7d7cLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //19277.9
    {0x40db4323e0a41a26LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //27916.6
    {0x40e1d9673acd5b68LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //36555.2
    {0x40e6113c8548a9bdLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //45193.9
    {0x40ea4911cfc3f812LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //53832.6
    {0x40ee80e71a3f4667LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //62471.2
    {0x40f15c5e325d4a5eLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //71109.9
    {0x40f37848d79af188LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //79748.6
    {0x40f594337cd898b2LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //88387.2
    {0x40f7b01e22163fdcLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //97025.9
    {0x40f9cc08c753e706LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //105665
    {0x40fbe7f36c918e30LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //114303
    {0x40fe03de11cf355aLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //122942
    {0x41000fe45b866e42LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //131581
    {0x41011dd9ae2541d7LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //140219
    {0x41022bcf00c4156cLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //148858
    {0xc104c083a6d698feLL, 0x0000000000000000LL, FE_OVERFLOW}, //-170000
    {0xc1038c4043b70ef5LL, 0x0000000000000000LL, FE_OVERFLOW}, //-160136
    {0xc10257fce09784ecLL, 0x0000000000000000LL, FE_OVERFLOW}, //-150272
    {0xc10123b97d77fae3LL, 0x0000000000000000LL, FE_OVERFLOW}, //-140407
    {0xc0ffdeec34b0e1b4LL, 0x0000000000000000LL, FE_OVERFLOW}, //-130543
    {0xc0fd76656e71cda2LL, 0x0000000000000000LL, FE_OVERFLOW}, //-120678
    {0xc0fb0ddea832b990LL, 0x0000000000000000LL, FE_OVERFLOW}, //-110814
    {0xc0f8a557e1f3a57eLL, 0x0000000000000000LL, FE_OVERFLOW}, //-100949
    {0xc0f63cd11bb4916cLL, 0x0000000000000000LL, FE_OVERFLOW}, //-91085.1
    {0xc0f3d44a55757d5aLL, 0x0000000000000000LL, FE_OVERFLOW}, //-81220.6
    {0xc0f16bc38f366948LL, 0x0000000000000000LL, FE_OVERFLOW}, //-71356.2
    {0xc0ee067991eeaa6cLL, 0x0000000000000000LL, FE_OVERFLOW}, //-61491.8
    {0xc0e9356c05708248LL, 0x0000000000000000LL, FE_OVERFLOW}, //-51627.4
    {0xc0e4645e78f25a24LL, 0x0000000000000000LL, FE_OVERFLOW}, //-41763
    {0xc0df26a1d8e86400LL, 0x0000000000000000LL, FE_OVERFLOW}, //-31898.5
    {0xc0d58486bfec13b8LL, 0x0000000000000000LL, FE_OVERFLOW}, //-22034.1
    {0xc0c7c4d74ddf86e0LL, 0x0000000000000000LL, FE_OVERFLOW}, //-12169.7
    {0xc0a202846f9b9940LL, 0x0000000000000000LL, FE_OVERFLOW}, //-2305.26
    {0xbfd269621134db92LL, 0x3fea3707024e7c8fLL, 0}, //-0.287682
    {0xbfd0dc554dd7fa10LL, 0x3feaa8b5a365e1c2LL, 0}, //-0.263448
    {0xbfce9e9114f6311bLL, 0x3feb1c51412a1b28LL, 0}, //-0.239214
    {0xbfcb84778e3c6e16LL, 0x3feb91e235769aa5LL, 0}, //-0.21498
    {0xbfc86a5e0782ab11LL, 0x3fec0970fe5db89aLL, 0}, //-0.190746
    {0xbfc5504480c8e80cLL, 0x3fec83063ec5bf7fLL, 0}, //-0.166512
    {0xbfc2362afa0f2507LL, 0x3fecfeaabf08a088LL, FE_INEXACT}, //-0.142278
    {0xbfbe3822e6aac404LL, 0x3fed7c676d965ce2LL, 0}, //-0.118044
    {0xbfb803efd9373dfaLL, 0x3fedfc455f9a2f35LL, 0}, //-0.0938101
    {0xbfb1cfbccbc3b7f0LL, 0x3fee7e4dd1a28165LL, FE_INEXACT}, //-0.0695761
    {0xbfa737137ca063cdLL, 0x3fef028a284bbab4LL, 0}, //-0.0453421
    {0xbf959d5ac372af74LL, 0x3fef8903f0edf290LL, 0}, //-0.0211081
    {0x3f699b8b92db4590LL, 0x3ff008e27126ca54LL, 0}, //0.00312593
    {0x3f9c043da82980d8LL, 0x3ff04e6b6ea78180LL, 0}, //0.0273599
    {0x3faa6a84eefbcc7fLL, 0x3ff09521f6d6a182LL, 0}, //0.0515939
    {0x3fb3697584f16c49LL, 0x3ff0dd0b2559e164LL, 0}, //0.0758279
    {0x3fb99da89264f252LL, 0x3ff1262c2bfda1efLL, 0}, //0.100062
    {0x3fbfd1db9fd8785cLL, 0x3ff1708a5314fcbbLL, 0}, //0.124296
    {0x3fc3030756a5ff33LL, 0x3ff1bc2af9db73c8LL, FE_INEXACT}, //0.14853
    {0x3fc61d20dd5fc238LL, 0x3ff2091396d848bdLL, 0}, //0.172764
    {0x3fc9373a6419853dLL, 0x3ff25749b84382ebLL, 0}, //0.196998
    {0x3fcc5153ead34842LL, 0x3ff2a6d3046cab6fLL, FE_INEXACT}, //0.221232
    {0x40421db22d0e5604LL, 0x4232ca98a2cc2221LL, 0}, //36.232
    {0x4050c6ef459d9902LL, 0x44213f7a701cccd5LL, 0}, //67.1084
    {0x40587f0574b40702LL, 0x460fa9a068629117LL, 0}, //97.9847
    {0x40601b8dd1e53a81LL, 0x47fd0fdf1e970937LL, 0}, //128.861
    {0x4063f798e9707181LL, 0x49eaaccc5b48c3deLL, 0}, //159.737
    {0x4067d3a400fba881LL, 0x4bd87bea59e74584LL, FE_INEXACT}, //190.614
    {0x406bafaf1886df81LL, 0x4dc67919c585f577LL, 0}, //221.49
    {0x406f8bba30121681LL, 0x4fb4a091f7320833LL, 0}, //252.366
    {0x4071b3e2a3cea6c1LL, 0x51a2eed9d7601168LL, 0}, //283.243
    {0x4073a1e82f944241LL, 0x539160c1550bf650LL, 0}, //314.119
    {0x40758fedbb59ddc1LL, 0x557fe6b6cc7f570bLL, 0}, //344.996
    {0x40777df3471f7941LL, 0x576d47f10d6f43bbLL, 0}, //375.872
    {0x40796bf8d2e514c1LL, 0x595ae04354924055LL, FE_INEXACT}, //406.748
    {0x407b59fe5eaab041LL, 0x5b48ab2733111910LL, FE_INEXACT}, //437.625
    {0x407d4803ea704bc1LL, 0x5d36a4755feb2a0eLL, FE_INEXACT}, //468.501
    {0x407f36097635e741LL, 0x5f24c85de7554b91LL, 0}, //499.377
    {0x4080920780fdc161LL, 0x61131360fe6b0ecaLL, 0}, //530.254
    {0x4081890a46e08f21LL, 0x630182486db32942LL, 0}, //561.13
    {0x4082800d0cc35ce1LL, 0x64f012218613c79aLL, 0}, //592.006
    {0x4083770fd2a62aa1LL, 0x66dd806f29afb931LL, 0}, //622.883
    {0x40846e129888f861LL, 0x68cb141d98ab6993LL, 0}, //653.759
    {0x408565155e6bc621LL, 0x6ab8dabf2f44ed81LL, 0}, //684.635
    {0x40862E2CD0FE8AB5LL, 0x6c4b51e851e8e42fLL, 0}, //709.771883
    {0x40862E6666666666LL, 0x6c4bdb8cdadbe029LL, 0}, //709.8
    {0x40865c18244e93e1LL, 0x6ca6d024a10f5404LL, 0}, //715.512
    {0x4087531aea3161a1LL, 0x6e94f0769f517cdeLL, 0}, //746.388
    {0x40884a1db0142f61LL, 0x7083382e9ee07ec7LL, FE_INEXACT}, //777.264
    {0x4089412075f6fd21LL, 0x7271a41035f03e00LL, FE_INEXACT}, //808.141
    {0x408a38233bd9cae1LL, 0x74603123055ef8c5LL, FE_INEXACT}, //839.017
    {0x408b2f2601bc98a1LL, 0x764db95a440dff84LL, FE_INEXACT}, //869.894
    {0x408c2628c79f6661LL, 0x783b485be7256628LL, FE_INEXACT}, //900.77
    {0x408d1d2b8d823421LL, 0x7a290ab2fe57eba6LL, FE_INEXACT}, //931.646
    {0x408e142e536501e1LL, 0x7c16fc282a56751dLL, FE_INEXACT}, //962.523
    {0x408f0b311947cfa1LL, 0x7e0518dcb349212aLL, FE_INEXACT}, //993.399
    {0x40900119ef954eb1LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1024.28
    {0x40907c9b5286b591LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1055.15
    {0x4090f81cb5781c71LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1086.03
    {0x4091739e18698351LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1116.9
    {0x4091ef1f7b5aea31LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1147.78
    {0x40926aa0de4c5111LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1178.66
    {0x4092e622413db7f1LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1209.53
    {0x409361a3a42f1ed1LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1240.41
    {0x4093dd25072085b1LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1271.29
    {0x409458a66a11ec91LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1302.16
    {0x4094d427cd035371LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1333.04
    {0x40954fa92ff4ba51LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1363.92
    {0x4095cb2a92e62131LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1394.79
    {0x409646abf5d78811LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1425.67
    {0x4096c22d58c8eef1LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1456.54
    {0x40973daebbba55d1LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1487.42
    {0x4097b9301eabbcb1LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1518.3
    {0x409834b1819d2391LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1549.17
    {0x4098b032e48e8a71LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1580.05
    {0x40992bb4477ff151LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1610.93
    {0x4099a735aa715831LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1641.8
    {0x409a22b70d62bf11LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1672.68
    {0x409a9e38705425f1LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1703.56
    {0x409b19b9d3458cd1LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1734.43
    {0x409b953b3636f3b1LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1765.31
    {0x409c10bc99285a91LL, 0x7ff0000000000000LL, FE_OVERFLOW}, //1796.18
    {0xc042b66666666666LL, 0x3d97d5b8091b3449LL, 0}, //-37.425
    {0xc04238853c148344LL, 0x3da7900e38b77550LL, 0}, //-36.4416
    {0xc041baa411c2a022LL, 0x3db74b300426b3e2LL, 0}, //-35.4581
    {0xc0413cc2e770bd00LL, 0x3dc7071b184fe6abLL, 0}, //-34.4747
    {0xc040bee1bd1ed9deLL, 0x3dd6c3cd28e5572bLL, 0}, //-33.4913
    {0xc040410092ccf6bcLL, 0x3de68143f050c61bLL, 0}, //-32.5078
    {0xc03f863ed0f62735LL, 0x3df63f7d2f9fc9caLL, 0}, //-31.5244
    {0xc03e8a7c7c5260f2LL, 0x3e05fe76ae70661cLL, FE_INEXACT}, //-30.541
    {0xc03d8eba27ae9aafLL, 0x3e15be2e3adddcdcLL, 0}, //-29.5575
    {0xc03c92f7d30ad46cLL, 0x3e257ea1a96db66eLL, FE_INEXACT}, //-28.5741
    {0xc03b97357e670e29LL, 0x3e353fced4fd01cfLL, 0}, //-27.5907
    {0xc03a9b7329c347e6LL, 0x3e4501b39eadcb72LL, 0}, //-26.6072
    {0xc0399fb0d51f81a3LL, 0x3e54c44dedd4ca3fLL, 0}, //-25.6238
    {0xc038a3ee807bbb60LL, 0x3e64879bafe74226LL, 0}, //-24.6404
    {0xc037a82c2bd7f51dLL, 0x3e744b9ad8691ba1LL, FE_INEXACT}, //-23.6569
    {0xc036ac69d7342edaLL, 0x3e84104960db2f91LL, FE_INEXACT}, //-22.6735
    {0xc035b0a782906897LL, 0x3e93d5a548a9c6d7LL, FE_INEXACT}, //-21.6901
    {0xc034b4e52deca254LL, 0x3ea39bac951b4d12LL, FE_INEXACT}, //-20.7066
    {0xc033b922d948dc11LL, 0x3eb3625d513f35f4LL, FE_INEXACT}, //-19.7232
    {0xc032bd6084a515ceLL, 0x3ec329b58ddd148fLL, FE_INEXACT}, //-18.7398
    {0xc031c19e30014f8bLL, 0x3ed2f1b36163e413LL, FE_INEXACT}, //-17.7563
    {0xc030c5dbdb5d8948LL, 0x3ee2ba54e7d98163LL, FE_INEXACT}, //-16.7729
    {0xc02f94330d738609LL, 0x3ef2839842ca54feLL, FE_INEXACT}, //-15.7895
    {0xc02d9cae642bf982LL, 0x3f024d7b99392c8fLL, FE_INEXACT}, //-14.806
    {0xc02ba529bae46cfbLL, 0x3f1217fd178f43d9LL, FE_INEXACT}, //-13.8226
    {0xc029ada5119ce074LL, 0x3f21e31aef8c7c2aLL, FE_INEXACT}, //-12.8391
    {0xc027b620685553edLL, 0x3f31aed35837c203LL, FE_INEXACT}, //-11.8557
    {0xc025be9bbf0dc766LL, 0x3f417b248dcfa050LL, FE_INEXACT}, //-10.8723
    {0xc023c71715c63adfLL, 0x3f51480cd1bb00baLL, FE_INEXACT}, //-9.88885
    {0xc021cf926c7eae58LL, 0x3f61158a6a7a188aLL, FE_INEXACT}, //-8.90541
    {0xc01fb01b866e43a3LL, 0x3f70e39ba3978197LL, FE_INEXACT}, //-7.92198
    {0xc01bc11233df2a96LL, 0x3f80b23ecd997ecfLL, FE_INEXACT}, //-6.93855
    {0xc017d208e1501189LL, 0x3f9081723df36bbfLL, FE_INEXACT}, //-5.95511
    {0xc013e2ff8ec0f87cLL, 0x3fa051344ef756b7LL, FE_INEXACT}, //-4.97168
    {0xc00fe7ec7863bedeLL, 0x3fb021835fc7c4fdLL, FE_INEXACT}, //-3.98824
    {0xc00c816f0068db8cLL, 0x3fb5a857061c7fdcLL, FE_INEXACT}, //-3.5632
    {0xc004c2fd75e2046dLL, 0x3fc52eadc525bdd6LL, FE_INEXACT}, //-2.59521
    {0xbffa0917d6b65a9cLL, 0x3fd4b7aff23877beLL, FE_INEXACT}, //-1.62722
    {0xbfe51869835158bbLL, 0x3fe4434e8e2c4c41LL, FE_INEXACT}, //-0.65923
    {0x3fd3c2b94d940784LL, 0x3ff3d17aee17448dLL, FE_INEXACT}, //0.30876
    {0x3ff46d916872b020LL, 0x40036226b974974aLL, FE_INEXACT}, //1.27675
    {0x4001f53a3ec02f2fLL, 0x4012f543e855cdf3LL, FE_INEXACT}, //2.24474
    {0x4009b3abc947064eLL, 0x40228ac4c19e1248LL, FE_INEXACT}, //3.21273
    {0x4010b90ea9e6eeb7LL, 0x4032229bd9476ac8LL, FE_INEXACT}, //4.18072
    {0x401498476f2a5a47LL, 0x4041bcbc0eb1ae54LL, FE_INEXACT}, //5.14871
    {0x40187780346dc5d7LL, 0x405159188afaf88eLL, FE_INEXACT}, //6.1167
    {0x401c56b8f9b13167LL, 0x4060f7a4bf616969LL, FE_INEXACT}, //7.08469
    {0x40201af8df7a4e7bLL, 0x4070985463adfbcbLL, FE_INEXACT}, //8.05268
    {0x40220a95421c0443LL, 0x40803b1b74a84037LL, FE_INEXACT}, //9.02067
    {0x4023fa31a4bdba0bLL, 0x408fbfdc652592d7LL, FE_INEXACT}, //9.98866
    {0x4025e9ce075f6fd3LL, 0x409f0d823f603496LL, FE_INEXACT}, //10.9567
    {0x4027d96a6a01259bLL, 0x40ae5f11fd9fca4dLL, FE_INEXACT}, //11.9246
    {0x4029c906cca2db63LL, 0x40bdb475a3c8db7dLL, FE_INEXACT}, //12.8926
    {0x402bb8a32f44912bLL, 0x40cd0d97b13fc07eLL, FE_INEXACT}, //13.8606
    {0x402da83f91e646f3LL, 0x40dc6a631e32e11bLL, FE_INEXACT}, //14.8286
    {0x402f97dbf487fcbbLL, 0x40ebcac358f42c4cLL, FE_INEXACT}, //15.7966
    {0x4030c3bc2b94d941LL, 0x40fb2ea443617395LL, FE_INEXACT}, //16.7646
    {0x4031bb8a5ce5b425LL, 0x410a95f2305b5694LL, FE_INEXACT}, //17.7326
    {0x4032b3588e368f09LL, 0x411a0099e14a6c5eLL, FE_INEXACT}, //18.7006
    {0x4033ab26bf8769edLL, 0x41296e8883b25b70LL, FE_INEXACT}, //19.6686
    {0x4034a2f4f0d844d1LL, 0x4138dfabaed29163LL, FE_INEXACT}, //20.6366
    {0x40359ac322291fb5LL, 0x414853f161544e0cLL, FE_INEXACT}, //21.6045
    {0x403692915379fa99LL, 0x4157cb47ff05b725LL, FE_INEXACT}, //22.5725
    {0x40378a5f84cad57dLL, 0x4167459e4ea1ab50LL, FE_INEXACT}, //23.5405
    {0x4038822db61bb061LL, 0x4176c2e377a40ccfLL, FE_INEXACT}, //24.5085
    {0x403979fbe76c8b45LL, 0x41864307002a3de9LL, FE_INEXACT}, //25.4765
    {0x403a71ca18bd6629LL, 0x4195c5f8cadf8a7fLL, FE_INEXACT}, //26.4445
    {0x403b69984a0e410dLL, 0x41a54ba914f53bd7LL, FE_INEXACT}, //27.4125
    {0x403c61667b5f1bf1LL, 0x41b4d40874261501LL, FE_INEXACT}, //28.3805
    {0x403d5934acaff6d5LL, 0x41c45f07d4c4f7deLL, FE_INEXACT}, //29.3485
    {0x403e5102de00d1b9LL, 0x41d3ec9877d67402LL, FE_INEXACT}, //30.3165
    {0x403f48d10f51ac9dLL, 0x41e37cabf1350227LL, FE_INEXACT}, //31.2844
    {0x4040204fa05143c0LL, 0x41f30f3425bfb03cLL, FE_INEXACT}, //32.2524
    {0x40409c36b8f9b132LL, 0x4202a423499303a7LL, FE_INEXACT}, //33.2204
    {0x4041181dd1a21ea4LL, 0x42123b6bde4bd79eLL, FE_INEXACT}, //34.1884
    {0x40419404ea4a8c16LL, 0x4221d500b154008fLL, FE_INEXACT}, //35.1564
    {0x40420fec02f2f988LL, 0x423170d4da387ce7LL, FE_INEXACT}, //36.1244
    {0x40428bd31b9b66faLL, 0x42410edbb908fddfLL, FE_INEXACT}, //37.0924
};

