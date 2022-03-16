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

#ifndef __TEST_COS_DATA_H__
#define __TEST_COS_DATA_H__

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>

/*
 * Test cases to check for exceptions for the cosf() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_cosf_conformance_data[] = {
    {0x3fc8f5c3, 0x0, 0},
    {POS_ZERO_F32, POS_ONE_F32, 0,}, //cos (0)=1
    {NEG_ZERO_F32, POS_ONE_F32, 0},  //cos(-x) = cos(x)
    {0x3F000000, 0x3f60a940, 0,},//0.5
    {0x3F333333, 0x3F43CCB3, 0,},//0.7
    {0x3F4CCCCD, 0x3F325B5F, 0,}, //0.8
    {0x3fc00000, 0x3D90DEAA, 0,},   //1.5
    {0x3FC90FDB, 0xb33bbd2e, 0,},   //pi/2
    {0xC07AE148, 0xBF3647C6, 0,}, //-3.92
    {0x3F490FDB, 0x3f3504f3, 0,},
    //special numbers
    {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID },  //
    {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID },  //
    {POS_INF_F32, 0x7fc00000, FE_INVALID },  //
    {NEG_INF_F32, 0x7fc00000, FE_INVALID },  //
    {POS_QNAN_F32, POS_QNAN_F32, 0},  //
    {NEG_QNAN_F32, NEG_QNAN_F32, 0},  //

    {0x3C000001,0x3f7ffe00,0},
    {0x39000001,0x3f800000,0},
    {POS_ONE_F32, 0x3f0a5140,0},  //1
    {0x40000000, 0xbed51133,0},  //2
    {POS_PI_F32, NEG_PI_F32,0},  //pi
    {0x40C90FDB, POS_ONE_F32,0},  //2pi
    {0x41200000, 0xbf56cd64,0},  //10
    {0x447A0000, 0x3f0ff813,0},  //1000
    {0xC8C35000, 0x3f7d62d4,0},  //-4e5
    {0x48F42400, 0xbf7beb6c,0},  //500000
    {0x48F4CCC8, 0xbf2fd6b2,0},  //501350.25 > 5.0e5
    {0xC93F6800, 0xbf7cf1b1,0},  //-784000
    {0x43FCE5F1, 0xbf800000,0},  //505.796417, remainder is ~0, very close multiple of piby2
    {0x4831E550, 0xbf800000,0},  //182165.25, remainder is ~piby4
    {0x42FCE5F1, 0x3F3504F3,0},  //126.449104
    //Special case for verified the ulps
    {0xd0a3e87f, 0xb10a4ed8, 0},   //-2.19993846E+10, close to pi/2
    // newlly added 
    {0xbfc90fdb, 0xb33bbd2e, 0}, // Cos(-1.5708 = +3pi/2)=-4.371139e-008
    {0x4096cbe4, 0x324cde2e, 0}, // Cos(4.71239  = -pi/2)=1.192488e-008
    {0xc096cbe4, 0x324cde2e, 0}, // Cos(-4.71239 = -3pi/2)=1.192488e-008
    {0x41fb53d1, 0x3f800000, 0}, // Cos(31.4159 = +10pi)=1.000000e+000
    {0x4203f267, 0x35f4cbd9, 0}, // Cos(32.9867 = +21pi/2)=1.823874e-006
    {0x42c90fdb, 0x3f800000, 0}, // Cos(100.531 = +32pi) = 1.000000e+000
    {0x42cc341a, 0x34ca9ee6, 0}, // Cos(102.102 = 65pi/2) = 3.774105e-007
    {0x3ec90fdb, 0x3f6c835e, 0}, 	// 0: cos(3.926990926266e-001)=	9.238795283294e-001
    {0x3f490fdb, 0x3f3504f3, 0}, 	// 1: cos(7.853981852531e-001)=	7.071067657322e-001
    {0x3f96cbe4, 0x3ec3ef15, 0}, 	// 2: cos(1.178097248077e+000)=	3.826834296108e-001
    {0x3fc90fdb, 0xb33bbd2e, 0}, 	// 3: cos(1.570796370506e+000)=	-4.371139000186e-008
    {0x3ffb53d1, 0xbec3ef14, 0}, 	// 4: cos(1.963495373726e+000)=	-3.826834002439e-001
    {0x4016cbe4, 0xbf3504f3, 0}, 	// 5: cos(2.356194496155e+000)=	-7.071067854026e-001
    {0x402feddf, 0xbf6c835e, 0}, 	// 6: cos(2.748893499374e+000)=	-9.238795047604e-001
    {0x40490fdb, 0xbf800000, 0}, 	// 7: cos(3.141592741013e+000)=	-1.000000000000e+000
    {0x406231d6, 0xbf6c835e, 0}, 	// 8: cos(3.534291744232e+000)=	-9.238795290887e-001
    {0x407b53d1, 0xbf3504f4, 0}, 	// 9: cos(3.926990747452e+000)=	-7.071068303555e-001
    {0x408a3ae6, 0xbec3ef1a, 0}, 	// 10: cos(4.319689750671e+000)=	-3.826835691127e-001
    {0x4096cbe4, 0x324cde2e, 0}, 	// 11: cos(4.712388992310e+000)=	1.192488045481e-008
    {0x40a35ce2, 0x3ec3ef1b, 0}, 	// 12: cos(5.105088233948e+000)=	3.826835911470e-001
    {0x40afeddf, 0x3f3504f1, 0}, 	// 13: cos(5.497786998749e+000)=	7.071066786325e-001
    {0x40bc7edd, 0x3f6c835f, 0}, 	// 14: cos(5.890486240387e+000)=	9.238795382156e-001
    {0x40c90fdb, 0x3f800000, 0}, 	// 15: cos(6.283185482025e+000)=	1.000000000000e+000
    {0xbec90fdb, 0x3f6c835e, 0}, 	// 16: cos(-3.926990926266e-001)=	9.238795283294e-001
    {0xbf490fdb, 0x3f3504f3, 0}, 	// 17: cos(-7.853981852531e-001)=	7.071067657322e-001
    {0xbf96cbe4, 0x3ec3ef15, 0}, 	// 18: cos(-1.178097248077e+000)=	3.826834296108e-001
    {0xbfc90fdb, 0xb33bbd2e, 0}, 	// 19: cos(-1.570796370506e+000)=	-4.371139000186e-008
    {0xbffb53d1, 0xbec3ef14, 0}, 	// 20: cos(-1.963495373726e+000)=	-3.826834002439e-001
    {0xc016cbe4, 0xbf3504f3, 0}, 	// 21: cos(-2.356194496155e+000)=	-7.071067854026e-001
    {0xc02feddf, 0xbf6c835e, 0}, 	// 22: cos(-2.748893499374e+000)=	-9.238795047604e-001
    {0xc0490fdb, 0xbf800000, 0}, 	// 23: cos(-3.141592741013e+000)=	-1.000000000000e+000
    {0xc06231d6, 0xbf6c835e, 0}, 	// 24: cos(-3.534291744232e+000)=	-9.238795290887e-001
    {0xc07b53d1, 0xbf3504f4, 0}, 	// 25: cos(-3.926990747452e+000)=	-7.071068303555e-001
    {0xc08a3ae6, 0xbec3ef1a, 0}, 	// 26: cos(-4.319689750671e+000)=	-3.826835691127e-001
    {0xc096cbe4, 0x324cde2e, 0}, 	// 27: cos(-4.712388992310e+000)=	1.192488045481e-008
    {0xc0a35ce2, 0x3ec3ef1b, 0}, 	// 28: cos(-5.105088233948e+000)=	3.826835911470e-001
    {0xc0afeddf, 0x3f3504f1, 0}, 	// 29: cos(-5.497786998749e+000)=	7.071066786325e-001
    {0xc0bc7edd, 0x3f6c835f, 0}, 	// 30: cos(-5.890486240387e+000)=	9.238795382156e-001
    {0xc0c90fdb, 0x3f800000, 0}, 	// 31: cos(-6.283185482025e+000)=	1.000000000000e+000
    {0xbf934063, 0x3ed0f51e, 0}, 	// 32: cos(-1.150402426720e+000)=	4.081200872191e-001
    {0x00000000, 0x3f800000, 0}, 	// 33: cos(0.000000000000e+000)=	1.000000000000e+000
    {0x00000000, 0x3f800000, 0}, 	// 34: cos(0.000000000000e+000)=	1.000000000000e+000
    {0x80000000, 0x3f800000, 0}, 	// 35: cos(0.000000000000e+000)=	1.000000000000e+000
    {0x00000000, 0x3f800000, 0}, 	// 36: cos(0.000000000000e+000)=	1.000000000000e+000
    {0xa6dbe299, 0x3f800000, 0}, 	// 37: cos(-1.525759706735e-015)=	1.000000000000e+000
    {0x00000000, 0x3f800000, 0}, 	// 38: cos(0.000000000000e+000)=	1.000000000000e+000
    {0x8000001c, 0x3f800000, 0}, 	// 39: cos(-3.923635700109e-044)=	1.000000000000e+000
    {0x40205226, 0xbf4ddbbe, 0}, 	// 40: cos(2.505013942719e+000)=	-8.041342378208e-001
    {0x3ff45887, 0xbea9dacd, 0}, 	// 41: cos(1.908951640129e+000)=	-3.317474387602e-001
    {0x80000000, 0x3f800000, 0}, 	// 42: cos(0.000000000000e+000)=	1.000000000000e+000
    {0x00000000, 0x3f800000, 0}, 	// 43: cos(0.000000000000e+000)=	1.000000000000e+000
    {0xc055cebc, 0xbf7af0af, 0}, 	// 44: cos(-3.340743064880e+000)=	-9.802350113348e-001
    {0xc0b255c9, 0x3f421ae2, 0}, 	// 45: cos(-5.572971820831e+000)=	7.582227010984e-001
    {0x80000000, 0x3f800000, 0}, 	// 46: cos(0.000000000000e+000)=	1.000000000000e+000
    {0x00000000, 0x3f800000, 0}, 	// 47: cos(0.000000000000e+000)=	1.000000000000e+000
    {0x403b9936, 0xbf7a5b39, 0}, 	// 48: cos(2.931226253510e+000)=	-9.779544693520e-001
    {0x3ebc9d26, 0x3f6ed32a, 0}, 	// 49: cos(3.683864474297e-001)=	9.329096171815e-001
    {0xbd94dd55, 0x3f7f52f2, 0}, 	// 50: cos(-7.268778234720e-002)=	9.973594060904e-001
    {0x80000000, 0x3f800000, 0}, 	// 51: cos(0.000000000000e+000)=	1.000000000000e+000
//from ancient libm
    //exceptional cases
#ifdef WIN64
    {0x7f800000, 0xffc00000, 0},
    {0xff800000, 0xffc00000, 0},   // - INF
#else
    {0x7f800000, 0x7fc00000, 0},  // + InF
    {0xff800000, 0x7fc00000, 0},   // - INF
#endif
    {0x7fc00000, 0x7fc00000, 0},  //NaN
    {0x7fc00001, 0x7fc00001, 0},  //NaN
    {0x7f800001, 0x7fc00001, 0},   // + INF
    {0x7fef8000, 0x7fef8000, 0},   // + QNAN
    {0xffef8000, 0xffef8000, 0},   // - QNAN
    {0x80000000, 0x3f800000, 0},	// - 0

   //The following test cases are ported from AMD LibM
    //>=5.0e5 case
    //make case for expdiff > 15
    {0x4016cb60, 0xbf35037e, 0},
    {0x4016cd61, 0xbf350929, 0},
    {0x4016cf62, 0xbf350ed4, 0},
    {0x4016d163, 0xbf35147e, 0},

};

static libm_test_special_data_f64
test_cos_conformance_data[] = {
    /*special numbers*/
    {POS_ZERO_F64, POS_ONE_F64,  0,},    //cos(0)=1
    {NEG_ZERO_F64, POS_ONE_F64,  0,},    //cos(-x)=cos(x)
    {POS_SNAN_F64, POS_SNAN_F64, FE_INVALID},  //snan
    {NEG_SNAN_F64, NEG_SNAN_F64, FE_INVALID},  //
    {POS_INF_F64,  POS_SNAN_F64, FE_INVALID},   //
    {NEG_INF_F64,  POS_SNAN_F64, FE_INVALID},  //
    {POS_QNAN_F64, POS_QNAN_F64, 0},  //
    {NEG_QNAN_F64, NEG_QNAN_F64, 0},  //

	{0x412e848abcdef000,0x3fd2d92261ee0d59, 0},  // 1000005.3688883781 > 5e5
	{0x439332270327f466,0x3f8720ef3d9337a0, 0},  // 3.4580273251621926E+17
	{0x439332270327f466,0x3f8720ef3d9337a0, 0},  // 3.4580273251621926E+17
	{0x439332270327f466,0x3f8720ef3d9337a0, 0},  // 3.4580273251621926E+17
	{0x400f6a7a2955385e,0xbfe6a09e667f3bce,	0},
	{0x400f6a7a2955385d,0xbfe6a09e667f3bd0,	0},
	{0x400f6a7a2955385f,0xbfe6a09e667f3bcb,	0},
	{0x4002d97c7f3321d2,0xbfe6a09e667f3bcc,	0},
	{0x4002d97c7f3321d1,0xbfe6a09e667f3bc9,	0},
	{0x4002d97c7f3321d3,0xbfe6a09e667f3bcf,	0},
	{0x401c463abeccb2bb,0x3fe6a09e667f3bce,	0},
	{0x401c463abeccb2ba,0x3fe6a09e667f3bd4,	0},
	{0x401c463abeccb2bc,0x3fe6a09e667f3bc9,	0},
	{0x4015fdbbe9bba775,0x3fe6a09e667f3bcb,	0},
	{0x4015fdbbe9bba774,0x3fe6a09e667f3bc6, 0},
	{0x4015fdbbe9bba776,0x3fe6a09e667f3bd1, 0},
	{0x400f6a7a2955385e,0xbfe6a09e667f3bce, 0},
	{0x401c463abeccb2bb,0x3fe6a09e667f3bce, 0},
	{0x4015fdbbe9bba775,0x3fe6a09e667f3bcb, 0},
	{0x4002d97c7f3321d2,0xbfe6a09e667f3bcc, 0},
	{0x4046c6cbc45dc8de,0xbc26d61b58c99c43, 0},  // 45.553093477052
	{0x40306b51f0157e66,0xbfe83dca43349085, 0},  // 16.41921902203321
	{0x4012d98c7f3321d2,0x3f0fffffffaa40c6, 0},  // 4.71245001554094 close to 3pi/2
	{0x3e38000000000000,0x3ff0000000000000, 0},  // 5.5879354476928711E-09 = 2^-28*1.5
	{0xbe38000000000000,0x3ff0000000000000, 0},  // -5.5879354476928711E-09 = -2^-28*1.

	{0x3f18000000000000,0x3feffffffdc00000, 0},  //  9.1552734375E-05	 =  2^-14 *1.5
	{0xbf18000000000000,0x3feffffffdc00000, 0},  // -9.1552734375E-05	 = -2^-14 *1.5
	{0x3fe0000000000000,0x3fec1528065b7d50, 0},  // .5
	{0xbfe0000000000000,0x3fec1528065b7d50, 0},  //-.5
	{0x3feffee4e9dabc6b,0x3fe14b164296c5e0, 0},  // 0.999865013834904
	{0x3fe921fb54442d18,0x3fe6a09e667f3bcd, 0},  // 0.78539816339744828 = pi/4
	{0xbfe921fb54442d18,0x3fe6a09e667f3bcd, 0},  //-0.78539816339744828 =-pi/4

	{0x3fe921fb54442d18,0x3fe6a09e667f3bcd, 0},
	{0x3f20000000000001,0x3feffffffc000000, 0},
	{0x3e40000000000001,0x3ff0000000000000, 0},
	{0x412e848abcdef000,0x3fd2d92261ee0d59, 0},  // 1000005.3688883781 > 5e5
	{0x439332270327f466,0x3f8720ef3d9337a0, 0},  // 3.4580273251621926E+17
	{0x3fe9e0c8f112ab1e,0x3fe618243eefd4fd, 0},  // 0.80868956646495227
	{0x40306b51f0157e66,0xbfe83dca43349085, 0},  // 16.41921902203321
	{0x402ddf5adb92c01a,0xbfe6ef49dae5cece, 0},  // 14.93624006430041
	{0x402ddb778a9ebd8a,0xbfe6c3bcf257d639, 0},  // 14.928646404149521
	{0x401c462b9064a63b,0x3fe6a0f446b1f80a, 0},  // 7.0685255586081963
	{0x3fe921fb54442d19,0x3fe6a09e667f3bcc, 0},  // 0.78539816339744839
	{0x3fe921fb54442d20,0x3fe6a09e667f3bc7, 0},  // 0.78539816339744917
	{0x3ff10ca44655d48a,0x3fdef9b63399ad36, 0},  // 1.0655863521072058

	{POS_PI_F64, 0xbff0000000000000, 0},  // 3.1415926535897931 = pi
	{NEG_PI_F64, 0xbff0000000000000, 0},  // cos(-x)=cos(x)

	{0x400923e979f8b36a,0xbfefffff118a9e97, 0},  // 3.1425351647398445
	{0x4002dae59bb5c33e,0xbfe6a49b6cfd78bd, 0},  // 2.3568832554668964
	{0x4015fdca5f9a0e38,0x3fe6a0f032d9f6da, 0},  // 5.4978423059251824
	{0x40b93bda357dadda,0x3fe7ad37100a511a, 0},  // 6459.8523787068789
	{0x40f63525129291ff,0x3fe99a6170de0d7d, 0},  // 90962.317034311578
	{0x3ff9207824b27c17,0x3f3832f911d6420b, 0},  // 1.5704270776257141
	{0x4025fe9b31eb183d,0x3f5be9050b9f7107, 0},  // 10.997277793834764
	{0x439332270327f466,0x3f8720ef3d9337a0, 0},  // 3.4580273251621926E+17
	{0x4046c6cbc45dc8de,0xbc26d61b58c99c43, 0},  // 45.553093477052
	{0x411fa317083ee0a2,0xbfe2512769b8cee7, 0},  // 518341.75805235852

	{0x4000000000000000,0xbfdaa22657537205, 0},  // 2
	{0x4008000000000000,0xbfefae04be85e5d2, 0},  // 3
	{0x4024000000000000,0xbfead9ac890c6b1f, 0},  // 10
	{0xc000000000000000,0xbfdaa22657537205, 0},  // - 2
	{0x3ff921fb54442d18,0x3c91a62633145c07, 0},  // 1.5707963267948966 = pi/2

	{0x4012d97c7f3321d2,0xbcaa79394c9e8a0a, 0},  // 4.71238898038469 = 3pi/2
	{0x401921fb54442d18,0x3ff0000000000000, 0},  // 6.2831853071795862 = 2pi
	{0xc00921f554442d18,0xbfeffffffff70000, 0},  //-3.1415812094979962 close to -pi
	{0xbff921f557442d18,0x3ed7f40000008b3a, 0},  //-1.570790615924869 close to -pi/2
	{0xbff9217557442d18,0x3f20bf9fff3c46cf, 0},  //-1.570668545612369 close to -pi/2
	{0x400921fb56442d18,0xbfefffffffffffff, 0},  // 3.1415926684909543 close to pi
	{0x4012d98c7f3321d2,0x3f0fffffffaa40c6, 0},  // 4.71245001554094 close to 3pi/2
	//newly added
	{0x3cb0000000000000,0x3fefffffffffffff, 0}, // cos(2.22045e-016 = min_prec) = 1.000000e+000
	{0x0000000000000001,0x3ff0000000000000, 0}, // cos(4.94066e-324 = smallest) = 1.000000e+000
	{0xc012d97c7f3321d2,0xbcaa79394c9e8a0a, 0}, // cos(-4.71239 = -3pi/2) = -1.836970e-016
	{0x40407e4cef4cbd97,0x3cfdca8488c2ae32, 0}, // cos(32.9867 = +21pi/2) = 6.614950e-015
	{0x4059868341953dcc,0x3d04f65f65f056bb, 0}, // cos(102.102 = +65pi/2) = 9.309173e-015
	{0xc0407e4cef4cbd97,0x3cfdca8488c2ae32, 0}, // cos(-32.9867 = -21pi/2) = 6.614950e-015
	{0xc059868341953dcc,0x3d04f6f565f056bb, 0}, // cos(-102.102 = -65pi/2) = 9.309173e-015
	{0x403f6a7a2955385e,0x3fefffffffffffff, 0}, // cos(31.4159 = +10pi)=1.000000e+000
	{0x405921fb54442d18,0x3fefffffffffffff, 0}, // cos(100.531 = +32pi)=1.000000e+000
	{0xc03f6a7a2955385e,0x3fefffffffffffff, 0}, // cos(-31.4159 = -10pi)=1.000000e+000
	{0xc05921fb54442d18,0x3fefffffffffffff, 0}, // cos(-100.531 = -32pi)=1.000000e+000
	{0x7fe0000000000001,0xbfcbc6a1978019b9, 0}, // cos(8.9884657412464672e+307 = very close to max double)=-0.216999
	{0x3fd921fb54442d18,0x3fed906dcf328d46, 0}, 	// 0: cos(3.926990816987e-001)=	9.238795325113e-001
	{0x3fe921fb54442d18,0x3fe6a09e667f3bcd, 0}, 	// 1: cos(7.853981633974e-001)=	7.071067811865e-001
	{0x3ff2d97c7f3321d2,0x3fd87de2a6aea964, 0}, 	// 2: cos(1.178097245096e+000)=	3.826834323651e-001
	{0x3fff6a7a2955385e,0xbfd87de2a6aea962, 0}, 	// 4: cos(1.963495408494e+000)=	-3.826834323651e-001
	{0x4002d97c7f3321d2,0xbfe6a09e667f3bcc, 0}, 	// 5: cos(2.356194490192e+000)=	-7.071067811865e-001
	{0x4005fdbbe9bba775,0xbfed906bcf328d46, 0}, 	// 6: cos(2.748893571891e+000)=	-9.238795325113e-001
	{0x400921fb54442d18,0xbff0000000000000, 0}, 	// 7: cos(3.141592653590e+000)=	-1.000000000000e+000
	{0x400c463abeccb2bb,0xbfed906bcf328d47, 0}, 	// 8: cos(3.534291735289e+000)=	-9.238795325113e-001
	{0x400f6a7a2955385e,0xbfe6a09e667f3bce, 0}, 	// 9: cos(3.926990816987e+000)=	-7.071067811865e-001
	{0x4011475cc9eedf00,0xbfd87de2a6aea96d, 0}, 	// 10: cos(4.319689898686e+000)=	-3.826834323651e-001
	{0x40146b9c347764a4,0x3fd87de3a6aea967, 0}, 	// 12: cos(5.105088062083e+000)=	3.826834323651e-001
	{0x4015fdbbe9bba775,0x3fe6a09e667f3bcb, 0}, 	// 13: cos(5.497787143782e+000)=	7.071067811865e-001
	{0x40178fdb9effea46,0x3fed906bcf328d44, 0}, 	// 14: cos(5.890486225481e+000)=	9.238795325113e-001
	{0x401921fb54442d18,0x3ff0000000000000, 0}, 	// 15: cos(6.283185307180e+000)=	1.000000000000e+000
	{0xbfd921fb54442d18,0x3fed906bcf328d46, 0}, 	// 16: cos(-3.926990816987e-001)=	9.238795325113e-001
	{0xbfe921fb54442d18,0x3fe6a09e667f3bcd, 0}, 	// 17: cos(-7.853981633974e-001)=	7.071067811865e-001
	{0xbff2d97c7f3321d2,0x3fd87de2a6aea964, 0}, 	// 18: cos(-1.178097245096e+000)=	3.826834323651e-001
	{0xbfff6a7a2955385e,0xbfd87de2a6aea962, 0}, 	// 20: cos(-1.963495408494e+000)=	-3.826834323651e-001
	{0xc002d97c7f3321d2,0xbfe6a09e667f3bcc, 0}, 	// 21: cos(-2.356194490192e+000)=	-7.071067811865e-001
	{0xc005fdbbe9bba775,0xbfed906bcf328d46, 0}, 	// 22: cos(-2.748893571891e+000)=	-9.238795325113e-001
	{0xc00921fb54442d18,0xbff0000000000000, 0}, 	// 23: cos(-3.141592653590e+000)=	-1.000000000000e+000
	{0xc00c463abeccb2bb,0xbfed906bcf328d47, 0}, 	// 24: cos(-3.534291735289e+000)=	-9.238795325113e-001
	{0xc00f6a7a2955385e,0xbfe6a09e667f3bce, 0}, 	// 25: cos(-3.926990816987e+000)=	-7.071067811865e-001
	{0xc011475cc9eedf00,0xbfd87de2a6aea96d, 0}, 	// 26: cos(-4.319689898686e+000)=	-3.826834323651e-001
	{0xc0146b9c347764a4,0x3fd87de2a6aea967, 0}, 	// 28: cos(-5.105088062083e+000)=	3.826834323651e-001
	{0xc015fdbbe9bba775,0x3fe6a09e667f3bcb, 0}, 	// 29: cos(-5.497787143782e+000)=	7.071067811865e-001
	{0xc0178fdb9effea46,0x3fed906bcf328d44, 0}, 	// 30: cos(-5.890486225481e+000)=	9.238795325113e-001
	{0xc01921fb54442d18,0x3ff0000000000000, 0}, 	// 31: cos(-6.283185307180e+000)=	1.000000000000e+000
	{0xbff2680c64e45772,0x3fda1ea3a4ff623d, 0}, 	// 32: cos(-1.150402444944e+000)=	4.081200705819e-001
	{0x0bf1b461bf13177f,0x3ff0000000000000, 0}, 	// 33: cos(3.863760058230e-251)=	1.000000000000e+000
	{0x0d03f22c4a275d0a,0x3ff0000000000000, 0}, 	// 34: cos(5.705439589205e-246)=	1.000000000000e+000
	{0x9a10b0675b81292b,0x3ff0000000000000, 0}, 	// 35: cos(-3.927669771104e-183)=	1.000000000000e+000
	{0x21fc43e98867c4a0,0x3ff0000000000000, 0}, 	// 36: cos(5.658944362964e-145)=	1.000000000000e+000
	{0xbcdb7c5310fae10f,0x3ff0000000000000, 0}, 	// 37: cos(-1.525759657038e-015)=	1.000000000000e+000
	{0x04dc4b977f03c30a,0x3ff0000000000000, 0}, 	// 38: cos(2.973158904215e-285)=	1.000000000000e+000
	{0xb6ebbd7a73688abe,0x3ff0000000000000, 0}, 	// 39: cos(-3.887222917635e-044)=	1.000000000000e+000
	{0x40040a44b3f85dab,0xbfe9bb779d065d70, 0}, 	// 40: cos(2.505013853089e+000)=	-8.041341845409e-001
	{0x3ffe8b10d6798036,0xbfd53b59780ed5e4, 0}, 	// 41: cos(1.908951604644e+000)=	-3.317474052850e-001
	{0xa608f43d1956f811,0x3ff0000000000000, 0}, 	// 42: cos(-1.843202219565e-125)=	1.000000000000e+000
	{0x3608a9c8126d98b0,0x3ff0000000000000, 0}, 	// 43: cos(2.109406557975e-048)=	1.000000000000e+000
	{0xc00ab9d7847d5de8,0xbfef5e15ccf4ec0f, 0}, 	// 44: cos(-3.340743098331e+000)=	-9.802350047170e-001
	{0xc0164ab92935b7bf,0x3fe8435c711372d8, 0}, 	// 45: cos(-5.572971958069e+000)=	7.582227905765e-001
	{0xa791cda82cee365e,0x3ff0000000000000, 0}, 	// 46: cos(-4.412486349851e-118)=	1.000000000000e+000
	{0x24520a13503dcebe,0x3ff0000000000000, 0}, 	// 47: cos(9.927570778366e-134)=	1.000000000000e+000
	{0x40077326c7c423de,0xbfef4b673257a3da, 0}, 	// 48: cos(2.931226311372e+000)=	-9.779544814347e-001
	{0x3fd793a4b27b58c8,0x3fedda65476c8031, 0}, 	// 49: cos(3.683864348399e-001)=	9.329096217152e-001
	{0xbfb29baa97f00ffa,0x3fefea5e460342e6, 0}, 	// 50: cos(-7.268778047006e-002)=	9.973594062268e-001
	{0xa89efc4d47cd8d7d,0x3ff0000000000000, 0}, 	// 51: cos(-5.032930199885e-113)=	1.000000000000e+000
	//from ancient libm
	{0x0000000000000000, 0x3ff0000000000000, 0},  // 0
	{0x8000000000000000, 0x3ff0000000000000, 0},  // -0

	//5e5 checks
	//NAN/INF checks
#ifdef WIN64
	{0x7ff0000000000000, 0xfff8000000000000, 0},  // +inf
	{0xfff0000000000000, 0xfff8000000000000, 0},  // -inf
#else
	{0x7ff0000000000000, 0x7ff8000000000000, 0},  // +inf
	{0xfff0000000000000, 0x7ff8000000000000, 0},  // -inf
#endif

	{0x7ff8000000000000, 0x7ff8000000000000, 0},  // +Qnan
	{0x7ffdf00000000000, 0x7ffdf00000000000, 0},  // +Qnan
	{0xfffdf00000000000, 0xfffdf00000000000, 0},  // -Qnan
	{0xfff8000000000000, 0xfff8000000000000, 0},  // -Qnan
	{0x7ff4000000000000, 0x7ffc000000000000, 0},  // +Snan
	{0xfff4000000000000, 0xfffc000000000000, 0},  // -Snan

	//near piby4
	//very close to piby4
	//cos expdiff is < 15
	//cos expdiff is > 15
	//integer
#ifdef acceptable
	//ms & gcc answer for the following case 0x3fe14a280fb5068c
	{0x3ff0000000000000, 0x3fe14a280fb5068b, 0},  // 1
#else
	{0x3ff0000000000000, 0x3fe14a280fb5068c, 0},  // 1
#endif
	//pi related
	//ms & gcc answer for the following case 0xbe47fffffdcb3b39


	{0x000000000000dead, 0x000000000000beef, 0},  // end
	{0xBFE83DCA43349085, 0x3FE73FB4E2E7A5BA, 0},
	{0xBFE6EF49DAE5CECE, 0x3FE82087D3F96825, 0},

	{0x3FF0000000000000, 0x3fe14a280fb5068c, 0},  // 1
	{0x402921fb54442d18, 0x3ff0000000000000, 0},  // 12.566370614359172 = 4pi
	{0x410921fb54442d18, 0x3ff0000000000000, 0},  // 205887.41614566068 =  (2^16)pi
	{0x403921fb54442d18, 0x3ff0000000000000, 0},  // 25.132741228718345 = 8pi
	{0x403921fb54442d19, 0x3ff0000000000000, 0},  // 25.132741228718348 close to 8pi
	{0x3ff921fb57442d18, 0xbe47fffffdcb3b39, 0},  // 1.5707963379707675 close to pi/2
	{0x400921fb52442d18, 0xbfefffffffffffff, 0},  // 3.1415926386886319 close to pi
	{0x410921fb56442d18, 0x3fefffff0000019c, 0},  // 205887.41712222318 close to (2^16)p
	{0xbff921fb57442d18, 0xbe47fffffdcb3b39, 0},  //-1.5707963379707675 close to - pi/2
	{0xc00921fb54442d18, 0xbff0000000000000, 0},  //-3.1415926535897931 = - pi
	{0x400921f554442d18, 0xbfeffffffff70000, 0},  // 3.1415812094979962 close to pi

};

#endif	/*__TEST_COS_DATA_H__*/












