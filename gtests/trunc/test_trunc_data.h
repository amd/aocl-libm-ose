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
 * Test cases to check for exceptions for the truncf() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_truncf_conformance_data[] = {
   // special accuracy tests
   {0x38800000, 0x3f800000,  0},  //min= 0.00006103515625, small enough that trunc(x) = 1
   {0x387FFFFF, 0x3f800000,  0}, //min - 1 bit
   {0x38800001, 0x3f800000,  0}, //min + 1 bit
   {0xF149F2C9, 0x7f800000,  FE_OVERFLOW}, //lambda + x = 1, x = -9.9999994e+29
   {0xF149F2C8, 0x7f800000,  FE_OVERFLOW}, //lambda + x < 1
   {0xF149F2CA, 0x7f800000,  FE_OVERFLOW}, //lambda + x > 1
   {0x42B2D4FC, 0x7f7fffec,  0}, //max arg, x = 89.41598629223294,max truncf arg
   {0x42B2D4FB, 0x7f7fff6c,  0}, //max arg - 1 bit
   {0x42B2D4FD, 0x7f800000,  FE_OVERFLOW}, //max arg + 1 bit
   {0x42B2D4FF, 0x7f800000,  FE_OVERFLOW}, // > max
   {0x42B2D400, 0x7f7f820b,  0}, // < max
   {0x41A00000, 0x4d675844,  0}, //small_threshold = 20
   {0x41A80000, 0x4e1d3710,  0}, //small_threshold+1 = 21
   {0x41980000, 0x4caa36c8,  0}, //small_threshold - 1 = 19

    //trunc special exception checks
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

   {0xc20a1eb8, 0xc2080000,  0}, // -34.53, -34
   {0xc6de66b6, 0xc6de6600,  0}, // -28467.3555, -28467
   {0xbe99999a, 0x80000000,  0}, // -0.3, -0
   {0xbf247208, 0x80000000,  0}, // -0.642365, -0
   {0xbf000000, 0x80000000,  0}, // -0.5, -0
   {0x3e99999a, 0x00000000,  0}, // 0.3, 0
   {0x3f247208, 0x00000000,  0}, // 0.642365, 0
   {0x3f000000, 0x00000000,  0}, // 0.5, 0
   {0x420a1eb8, 0x42080000,  0}, // 34.53, 34
   {0x46de66b6, 0x46de6600,  0}, // 28467.3555, 28467
   {0xc2c80000, 0xc2c80000,  0}, // -100, -100
   {0x42c80000, 0x42c80000,  0}, // 100, 100

};

static libm_test_special_data_f64
test_trunc_conformance_data[] = {
    // special accuracy tests
    {0x3e30000000000000LL, 0x3ff0000000000000LL, 0},  //min, small enough that trunc(x) = 1 //
    {0x3E2FFFFFFFFFFFFFLL, 0x3ff0000000000000LL, 0}, //min - 1 bit
    {0x3e30000000000001LL, 0x3ff0000000000000LL, 0}, //min + 1 bit
    {0xFE37E43C8800759CLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x = 1, x = -1.0000000000000000e+300
    {0xFE37E43C8800758CLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x < 1
    {0xFE37E43C880075ACLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x > 1
    {0x408633ce8fb9f87eLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //max arg, x = 89.41598629223294,max truncf arg
    {0x408633ce8fb9f87dLL, 0x7feffffffffffd3bLL, 0}, //max arg - 1 bit
    {0x408633ce8fb9f87fLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //max arg + 1 bit
    {0x408633ce8fb9f8ffLL, 0x7ff0000000000000LL, FE_OVERFLOW}, // > max
    {0x408633ce8fb9f800LL, 0x7feffffffffe093bLL, 0}, // < max
    {0x4034000000000000LL, 0x41aceb088b68e804LL, 0}, //small_threshold = 20
    {0x4035000000000000LL, 0x41c3a6e1fd9eecfdLL, 0}, //small_threshold+1 = 21
    {0x4033000000000000LL, 0x419546d8f9ed26e2LL, 0}, //small_threshold - 1 = 19

    //trunc special exception checks
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
    {0x4002666666666666LL, 0x4000000000000000LL},  // 2.3
    {0x4007333333333333LL, 0x4000000000000000LL},  // 2.9
    {0x400F333333333333LL, 0x4008000000000000LL},  // 3.9
    {0x400A666666666666LL, 0x4008000000000000LL},  // 3.3
    {0x400C000000000000LL, 0x4008000000000000LL},  // 3.5
    {0x4004000000000000LL, 0x4000000000000000LL},  // 2.5
    {0x7ff0000000000000LL, 0x7ff0000000000000LL}, // inf
    {0xfff0000000000000LL, 0xfff0000000000000LL}, // -inf
    {0x7ffc000000000000LL, 0x7ffc000000000000LL}, // nan
    {0x7ff4000000000000LL, 0x7ffc000000000000LL}, // nan
    {0x0	         , 0x0                 }, // 0
    {0x8000000000000000LL, 0x8000000000000000LL}, // -0
    {0xffefffffffffffffLL, 0xffefffffffffffffLL}, // -1.79769e+308
    {0xc0862c4379671324LL, 0xc086280000000000LL}, // -709.533
    {0x3e45798ee2308c3aLL, 0x0                 }, // 1e-08
    {0x3fb999999999999aLL, 0x0                 }, // 0.1
    {0x3fe0000000000000LL, 0x0                 }, // 0.5
    {0x3feccccccccccccdLL, 0x0                 }, // 0.9
    {0x3ff199999999999aLL, 0x3ff0000000000000LL}, // 1.1
    {0x3ff8000000000000LL, 0x3ff0000000000000LL}, // 1.5
    {0x3ffe666666666666LL, 0x3ff0000000000000LL}, // 1.9
    {0xbe45798ee2308c3aLL, 0x8000000000000000LL}, // -1e-08
    {0xbfb999999999999aLL, 0x8000000000000000LL}, // -0.1
    {0xbfe0000000000000LL, 0x8000000000000000LL}, // -0.5
    {0xbfeccccccccccccdLL, 0x8000000000000000LL}, // -0.9
    {0xbff199999999999aLL, 0xbff0000000000000LL}, // -1.1
    {0xbff8000000000000LL, 0xbff0000000000000LL}, // -1.5
    {0xbffe666666666666LL, 0xbff0000000000000LL}, // -1.9
    {0xbff0000000000000LL, 0xbff0000000000000LL}, // -1
    {0x4024000000fff000LL, 0x4024000000000000LL}, // 10
    {0x408f4fff00000000LL, 0x408f480000000000LL}, // 1002
    {0x408f4c0000000000LL, 0x408f480000000000LL}, // 1001.5
    {0x409003999999999aLL, 0x4090000000000000LL}, // 1024.9
    {0x42cf000000000000LL, 0x42cf000000000000LL}, // 6.81697e+13
    {0x42c80000000fffffLL, 0x42c80000000fff80LL}, // 5.27766e+13
    {0x42d10237f00fffffLL, 0x42d10237f00fffc0LL}, // 7.48049e+13
    {0x42e1067adfc342b0LL, 0x42e1067adfc342a0LL}, // 1.49756e+14
    {0x42e1067adfc34208LL, 0x42e1067adfc34200LL}, // 1.49756e+14
    {0x42e800000091fabfLL, 0x42e800000091faa0LL}, // 2.11106e+14
    {0x42f8000001234fcfLL, 0x42f8000001234fc0LL}, // 4.22212e+14
    {0x42f80000000cccf8LL, 0x42f80000000cccf0LL}, // 4.22212e+14
    {0x43010237f0ccdfffLL, 0x43010237f0ccdff8LL}, // 5.98439e+14
    {0x430800056891fa02LL, 0x430800056891fa00LL}, // 8.44428e+14
    {0x43110237f00000ffLL, 0x43110237f00000fcLL}, // 1.19688e+15
    {0x43110237f00000feLL, 0x43110237f00000fcLL}, // 1.19688e+15
    {0x4321ffffffffffffLL, 0x4321fffffffffffeLL}, // 2.53327e+15
    {0x43210237f00000ffLL, 0x43210237f00000feLL}, // 2.39376e+15
    {0x43310237f00000ffLL, 0x43310237f00000ffLL}, // 4.78751e+15
    {0x433800056891fa00LL, 0x433800056891fa00LL}, // 6.75542e+15
    {0x4378000000000000LL, 0x4378000000000000LL}, // 1.08086e+17
    {0x1                 , 0x0                 }, // 4.94066e-324
    {0x5fde623545abc     , 0x0                 }, // 8.33261e-309
    {0xfffffffffffff     , 0x0                 }, // 2.22507e-308
    {0x8000000000000001LL, 0x8000000000000000LL}, // -4.94066e-324
    {0x8002344ade5def12LL, 0x8000000000000000LL}, // -3.06541e-309
    {0x800fffffffffffffLL, 0x8000000000000000LL}, // -2.22507e-308
    {0x10000000000000,     0x0                 }, // 2.22507e-308
    {0x8010000000000000LL, 0x8000000000000000LL}, // -2.22507e-308
    {0xc5812e71245acfdbLL, 0xc5812e71245acfdbLL}, // -6.64674e+26
    {0xffefffffffffffffLL, 0xffefffffffffffffLL}, // -1.79769e+308
    {0x43480677adfcfa00LL, 0x43480677adfcfa00LL}, // 1.3525e+16
    {0x435801239871fa00LL, 0x435801239871fa00LL}, // 2.70266e+16
    {0x4368111144444444LL, 0x4368111144444444LL}, // 5.41933e+16
    {0x4388ffffffffffffLL, 0x4388ffffffffffffLL}, // 2.2518e+17
    {0x4398ffffffffffffLL, 0x4398ffffffffffffLL}, // 4.5036e+17
    {0x43a8ffffffffffffLL, 0x43a8ffffffffffffLL}, // 9.0072e+17
    {0x43b8aaaa68ffffffLL, 0x43b8aaaa68ffffffLL}, // 1.77742e+18
    {0x43c8aaaa69ffffffLL, 0x43c8aaaa69ffffffLL}, // 3.55484e+18
    {0x43d8009678abcd00LL, 0x43d8009678abcd00LL}, // 6.91819e+18
    {0x7fefffffffffffffLL, 0x7fefffffffffffffLL}, // 1.79769e+308
    {0x43e8098734209870LL, 0x43e8098734209870LL}, // 1.38565e+19
    {0x43f800876abcda00LL, 0x43f800876abcda00LL}, // 2.76725e+19
    {0x44080091781cdba0LL, 0x44080091781cdba0LL}, // 5.53454e+19
    {0x4408012786abcde0LL, 0x4408012786abcde0LL}, // 5.53506e+19
    {0x43b3c4eafedcab02LL, 0x43b3c4eafedcab02LL}// 1.42452e+18


};
