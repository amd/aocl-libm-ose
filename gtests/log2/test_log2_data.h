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
 * Test cases to check for exceptions for the log2f() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_log2f_conformance_data[] = {
    {0x35510,           0xc3034374,    FE_DIVBYZERO},
    {0x7ff00000 ,       0x7fc00001,    0},
    {NEG_INF_F32,       0x7fc00000,    FE_INVALID},
    {POS_INF_F32,       0x7fc00000,    0},
    {POS_ONE_F32  ,     0x00000000,    0},
    {NEG_ONE_F32  ,     0x7fc00000,    FE_INVALID},
    {POS_ZERO_F32 ,     0xff800000,    0},
    {NEG_ZERO_F32 ,     0xff800000,    0},
    {0x00000000,        0xff800000,    4},  //0.0
    {0x3c000000,        0xc0e00000,    0},  //0.0078125
    {0x3c7fffff,        0xc0c00000,    32},  //0.0156249991
    {0x3f012345,        0xbf7cbb49,    32},  //0.504444
    {0x3f800000,        0x00000000,    0},  //1
    {0x40000000,        0x3f800000,    0},  //2
    {0x33d6bf95,        0xc1ba0729,    32},  //1.0000000e-7
    {POS_PI_F32,        0x3fd3643a,    32},  //pi
    {0x40c90fdb,        0x4029b21d,    32},  //2pi
    {0x41200000,        0x40549a78,    32},  //10
    {0x447a0000,        0x411f73da,    32},  //1000
    {0x42800000,        0x40c00000,    0},  //64
    {0x42af0000,        0x40ce7052,    32},  //87.5
    {0x42b00000,        0x40ceb3aa,    32},  //88
    {0x42c00000,        0x40d2b803,    32},  //96
    {0xc2af0000,        0x7fc00000,    FE_INVALID},  //-87.5
    {0xc2e00000,        0x7fc00000,    FE_INVALID},  //-112
    {0xc3000000,        0x7fc00000,    FE_INVALID},  //-128
    {0xc2aeac4f,        0x7fc00000,    FE_INVALID},  //-87.3365  smallest normal result
    {0xc2aeac50,        0x7fc00000,    FE_INVALID},  //-87.3365  largest denormal result
    {0xc2ce0000,        0x7fc00000,    FE_INVALID},  //-103
    {0x42b17216,        0x40cf1458,    FE_INEXACT},  //88.7228   largest value  --
    {0x42b17217,        0x40cf1458,    FE_INEXACT},  //88.7228   largest value
    {0x42b17218,        0x40cf1459,    FE_INEXACT},  //88.7228   overflow
    {0x50000000,	    0x42040000,    0},  //large   overflow
    {0xc20a1eb8,        0x7fc00000,    FE_INVALID}, // -34.53
    {0xc6de66b6,        0x7fc00000,    FE_INVALID}, // -28467.3555
    {0xbe99999a,        0x7fc00000,    FE_INVALID}, // -0.3
    {0xbf247208,        0x7fc00000,    FE_INVALID}, // -0.642365
    {0xbf000000,        0x7fc00000,    FE_INVALID}, // -0.5
    {0x3e99999a,        0xbfde54e3,    32}, // 0.3
    {0x3f247208,        0xbf237705,    32}, // 0.642365
    {0x3f000000,        0xbf800000,    0}, // 0.5
    {0x420a1eb8,        0x40a3834e,    32}, // 34.53
    {0x46de66b6,        0x416cc099,    32}, // 28467.3555
    {0xc2c80000,        0x7fc00000,    FE_INVALID}, // -100
    {0x42c80000,        0x40d49a78,    32}, // 100
//added from ancient libm repo
    {0x0005fde6,        0xc3026ac1,    FE_DIVBYZERO}, // denormal
    {0x00000001,        0xc3150000,    0}, // smallest denormal
    {0x007fffff,        0xc2fc0000,    FE_DIVBYZERO}, // largest denormal

    {0x00800000,        0xc2fc0000,    0}, // smallest normal
    {0x7f7fffff,        0x43000000,    32}, // largest normal

    {0x3f880000,        0x3db31fb8,    32}, // 1.0625
    {0x3f87ffff,        0x3db31fa2,    32}, // 1.06249988
    {0x3f700000,        0xbdbeb025,    32}, // 0.9375
    {0x3f6e147b,        0xbdd66b93,    32}, // 0.93
    {0x3f700001,        0xbdbeb018,    32}, // 0.93750006

    {0x3fc90fdb,        0x3f26c874,    32},  //pi/2
    {0x80000000,        0xff800000,    4},  //-0

#ifdef WIN64
    {0xbf800000,        0xffc00000,    0},  //-1
    {0xff800000,        0xffc00000,    0},  //-inf
    {0xc2800000,        0xffc00000,    0},  //-64
#else
    {0xbf800000,        0x7fc00000,    FE_INVALID},  //-1
    {0xff800000,        0x7fc00000,    FE_INVALID},  //-inf
    {0xc2800000,        0x7fc00000,    FE_INVALID},  //-64
#endif

    {0x3f7ae148,        0xbceec442,    FE_INEXACT},  //.98
    {0x3f828f5c,        0x3cea09cc,    FE_INEXACT},  //1.02

    {0x7f800000,        0x7f800000,    0},  //inf
    {0xffa00000,        0xffe00000,    FE_INVALID},  //snan

    {0x3fffffff,        0x3f7fffff,    FE_INEXACT},  //2--
    {0x40000001,        0x3f800001,    FE_INEXACT},  //2++

    {0x00000201, 0xc30bff48, FE_DIVBYZERO},

    {0x7f800001, 0x7fc00001, FE_INVALID},
    {0xff800001, 0xffc00001, FE_INVALID},
    {0x7fc00000, 0x7fc00000, 0},
    {0xffc00000, 0xffc00000, 0},
    {0x7f800000, 0x7f800000, 0},
    {0x7fc00000, 0x7fc00000, 0},
    {0x3f800000, 0x00000000, 0},
    {0xbf800000, 0x7fc00000, FE_INVALID},

    {0x007fffff, 0xc2fc0000, FE_DIVBYZERO},
    {0x807fffff, 0x7fc00000, FE_INVALID},
    {0x00000001, 0xc3150000, 0},
    {0x80000001, 0x7fc00000, FE_INVALID},
    {0x7f7fffff, 0x43000000, FE_INEXACT},
    {0xff7fffff, 0xffc00000, FE_INVALID},
    {0x00800000, 0xc2fc0000, 0},
    {0x80800000, 0xffc00000, FE_INVALID},
};

static libm_test_special_data_f64
test_log2_conformance_data[] = {
    {POS_SNAN_F64,      0x7ff8000000000001LL  ,FE_INVALID},
    {NEG_SNAN_F64,      0xfff8000000000001LL  ,FE_INVALID},
    {POS_QNAN_F64,      0x7ff8000000000000LL  ,0},
    {NEG_QNAN_F64,      0xfff8000000000000LL  ,0},
    {POS_INF_F64,       0x7ff0000000000000LL  ,0},
    {NEG_INF_F64,       0x7ff8000000000000LL  ,FE_INVALID},
    {POS_ONE_F64,       0x0000000000000000LL  ,0},
    {NEG_ONE_F64,       0x7ff8000000000000LL  ,FE_INVALID},
    {POS_ZERO_F64,      0xfff0000000000000LL  ,0},
    {NEG_ZERO_F64,      0xfff0000000000000LL  ,0},
    {0x0000000000000000LL,0xfff0000000000000LL,0},  //0.0
    {0x3EE0624DD2F1A9FCLL,0xc030f73da38d9d4bLL,0},  //0.0000078125
    {0x3EF0624DC31C6CA1LL,0xc02fee7b49e50bbdLL,0},  //0.0000156249991
    {0x3FE02467BE553AC5LL,0xbfef976bb487403aLL,0},  //0.504444
    {0x3FF0000000000000LL,0x0000000000000000LL,0},  //1
    {0x4000000000000000LL,0x3ff0000000000000LL,0},  //2
    {0x3D3C25C268497682LL,0xc04597b037b2d4ccLL,0},  //1.0000000e-13
    {POS_PI_F64,          0x3ffa6c873f5fb93dLL,0},  //pi
    {0x401921FB54442D18LL,0x400536439a4c6efcLL,0},  //2pi
    {0x4024000000000000LL,0x400a934f0979a371LL,0},  //10
    {0x408F400000000000LL,0x4023ee7b471b3a95LL,0},  //1000
    {0x4060000000000000LL,0x401c000000000000LL,0},  //128
    {0x4086240000000000LL,0x4022efef7d94e785LL,0},  //708.5
    {0x4086280000000000LL,0x4022f074e46bc166LL,0},  //709
    {0x408C000000000000LL,0x40239d5d9fd5010bLL,0},  //896
    {0xC086240000000000LL,0x7ff8000000000000LL,FE_INVALID},  //-708.5
    {0xC089600000000000LL,0x7ff8000000000000LL,FE_INVALID},  //-812
    {0xC070000000000000LL,0x7ff8000000000000LL,FE_INVALID},  //-256
    {0xc086232bdd7abcd2LL,0x7ff8000000000000LL,FE_INVALID},  // -708.3964185322641 smallest normal result
    {0xc086232bdd7abcd3LL,0x7ff8000000000000LL,FE_INVALID},  // -708.3964185322642 largest denormal result
    {0xC087480000000000LL,0x7ff8000000000000LL,FE_INVALID},  //-745
    {0x40862e42fefa39eeLL,0x4022f14588b1f44aLL,0},  //7.09782712893383973096e+02   largest value  --
    {0x40862e42fefa39efLL,0x4022f14588b1f44aLL,0},  //7.09782712893383973096e+02   largest value
    {0x40862e42fefa39ffLL,0x4022f14588b1f44cLL,0},  //7.09782712893383973096e+02   overflow
    {0x4200000000000000LL,0x4040800000000000LL,0},  //large   overflow
    {0xC05021EB851EB852LL,0x7ff8000000000000LL,FE_INVALID}, // -64.53
    {0xC0FF5D35B020C49CLL,0x7ff8000000000000LL,FE_INVALID}, // -128467.3555
    {0xBFD3333333333333LL,0x7ff8000000000000LL,FE_INVALID}, // -0.3
    {0xBFE48E410B630A91LL,0x7ff8000000000000LL,FE_INVALID}, // -0.642365
    {0xBFE0000000000000LL,0x7ff8000000000000LL,FE_INVALID}, // -0.5
    {0x3FD3333333333333LL,0xbffbca9c6f53897bLL,0}, // 0.3
    {0x3FE48E410B630A91LL,0xbfe46ee08c11db21LL,0}, // 0.642365
    {0x3FE0000000000000LL,0xbff0000000000000LL,0}, // 0.5
    {0x405021EB851EB852LL,0x40180c2f05577bbeLL,0}, // 64.53
    {0x40FF5D35B020C49CLL,0x4030f8963a17f1fbLL,0}, // 128467.3555
    {0xC08F400000000000LL,0x7ff8000000000000LL,FE_INVALID}, // -1000
    {0x408F400000000000LL,0x4023ee7b471b3a95LL,0}, // 1000
    {0x0000000000000000LL,0xfff0000000000000LL,0},
    //added from ancient libm repo
    {0X3FF0097D13350953LL,0X3F6B58B92ECC96C8LL,0},
    {0X0003EC71C47096D5LL,0XC090001C7C528F17LL,0},
    {0X000FFFFFFFFFFFFFLL,0XC08FF00000000000LL,0},

    {0X3FF1000000000000LL,0X3FB663F6FAC91316LL,0}, // 1.0625
    {0X3FF0FFFFFFFFFFFFLL,0X3FB663F6FAC91301LL,0}, // 1.0624999999999998
    {0X3FEE000000000000LL,0XBFB7D60496CFBB4CLL,0}, // 0.9375
    {0X3FEDC28F5C28F5C3LL,0XBFBACD728959B236LL,0}, // 0.93
    {0X3FEE000000000001LL,0XBFB7D60496CFBB40LL,0}, // 0.93750000000000011

    {0X3FF199999999999ALL,0X3FC199B728CB9D0CLL,0},
    {0X3FF1082B577D34ECLL,0X3FB71547652B82DELL,0},
    {0X3FF2216045B6F5CCLL,0X3FC71547652B82F4LL,0},
    {0X3C9B4626891A283BLL,0XC04A9D82796144E9LL,0},  // 9.4625784658987686e-17
    {0X3EA2345689904135LL,0XC034D053F57A5048LL,0},  // 5.4253475234754763e-07
    {0X410CA5F86198D046LL,0X4031D7230EE91CD7LL,0},  // 234687.04765475
    {0X40F422117627D4D2LL,0X403054DCEA9048C6LL,0},  // 82465.091346579866
    {0X3FF028F5C28F5C29LL,0X3F8D664ECEE35B7FLL,0},  //1.01
    {0X3FF0000000000000LL,0X0000000000000000LL,0},  //1.0
    {0X3FF4000000000000LL,0X3FD49A784BCD1B8BLL,0},  //1.25
    {0X4005BF0A8B145769LL,0X3FF71547652B82FELL,0},  //e
    {0X4024000000000000LL,0X400A934F0979A371LL,0},  //10
    {0X4000000000000000LL,0X3FF0000000000000LL,0},  //2
    {0X3FF8000000000000LL,0X3FE2B803473F7AD1LL,0},  //1.5

#ifdef WIN64
    {0XBFF0000000000000LL,0XFFF8000000000000LL,0},  //-1.0
    {0XFFF0000000000000LL,0XFFF8000000000000LL,0},  //-INF
#else
    {0xbff0000000000000LL,0x7ff8000000000000LL,0},  //-1.0
    {0xfff0000000000000LL,0x7ff8000000000000LL,0},  //-inf
#endif

    {0XFFF8000000000000LL,0XFFF8000000000000LL,0},  //qnan
    {0XFFF4000000000000LL,0XFFFC000000000000LL,0},  //snan
    {0X7FF0000000000000LL,0X7FF0000000000000LL,0},  //+inf
    {0X7FF8000000000000LL,0X7FF8000000000000LL,0},  //qnan
    {0X7FF4000000000000LL,0X7FFC000000000000LL,0},  //snan

    {0X0000000000000000LL,0XFFF0000000000000LL,0},  //+0.0
    {0X8000000000000000LL,0XFFF0000000000000LL,0},  //-0.0
    {0X3FF0F5C28F5C28F6LL,0X3FB5853C54075E4CLL,0},  //1.06
    {0X3FEE147AE147AE14LL,0XBFB6DA3969BA5BEFLL,0},  //.94
    {0X3FEFAE147AE147AELL,0XBF8DB1F34D2C386DLL,0},  //.99
    {0X3FE0000000000000LL,0XBFF0000000000000LL,0},  //.5
    {0X4310005D38118000LL,0X4049000433D857ACLL,0},  //1.126e+015
    {0X40157AE147AE147BLL,0X4003663D8D41B416LL,0},  //5.37

    {0X3FF0000000000001LL,0X3CB71547652B82FDLL,0}
};

