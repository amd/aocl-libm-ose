#include <fenv.h>
#include <libm_tests.h>
#include <libm_util_amd.h>
/*
 * Test cases to check for exceptions for the cosf() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static struct libm_test_special_data_f32
test_cosf_conformance_data[] = {
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

};

static struct libm_test_special_data_f64
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
	{0x402921fb54442d18,0x3ff0000000000000, 0},  // 12.566370614359172 = 4pi
	{0x410921fb54442d18,0x3ff0000000000000, 0},  // 205887.41614566068 =  (2^16)pi
	{0x403921fb54442d18,0x3ff0000000000000, 0},  // 25.132741228718345 = 8pi
	{0x403921fb54442d19,0x3ff0000000000000, 0},  // 25.132741228718348 close to 8pi
	{0x3ff921fb57442d18,0xbe47fffffdcb3b39, 0},  // 1.5707963379707675 close to pi/2
	{0x400921fb52442d18,0xbfefffffffffffff, 0},  // 3.1415926386886319 close to pi
	{0x410921fb56442d18,0x3fefffff0000019c, 0},  // 205887.41712222318 close to (2^16)p
	{0xbff921fb57442d18,0xbe47fffffdcb3b39, 0},  //-1.5707963379707675 close to - pi/2
	{0x400921f554442d18,0xbfeffffffff70000, 0},  // 3.1415812094979962 close to pi
	{0xc00921f554442d18,0xbfeffffffff70000, 0},  //-3.1415812094979962 close to -pi
	{0xbff921f557442d18,0x3ed7f40000008b3a, 0},  //-1.570790615924869 close to -pi/2
	{0xbff9217557442d18,0x3f20bf9fff3c46cf, 0},  //-1.570668545612369 close to -pi/2
	{0x400921fb56442d18,0xbfefffffffffffff, 0},  // 3.1415926684909543 close to pi
	{0x4012d98c7f3321d2,0x3f0fffffffaa40c6, 0},  // 4.71245001554094 close to 3pi/2

};

static struct libm_test_special_data_f32
test_cosf_special_data[] = {
    {0x3c000000, 0x3f810101 ,},   //0.0078125
    {0x3c7fffff, 0x3f820405 ,},   //0.0156249991
    {0x3f012345, 0x3fd3f9f2 ,},   //0.504444
    {0x3f800000, 0x402df854 ,},   //1
    {0x40000000, 0x40ec7326 ,},   //2
    {0x33d6bf95, 0x3f800001 ,},   //1.0000000e-7
    {0x4048f5c3, 0x41b92025 ,},   //pi*/
    {0x40c90fdb, 0x4405df79 ,},   //2pi
    {0x41200000, 0x46ac14ee ,},   //10
    {0x447a0000, 0x7f800000 ,},   //1000
    {0x42800000, 0x6da12cc1 ,},   //64
    {0x42af0000, 0x7e96bab3 ,},   //87.5
    {0x42b00000, 0x7ef882b7 ,},   //88
    {0x42c00000, 0x7f800000 ,},   //96
    {0xc2af0000, 0x006cb2bc ,},   //-87.5
    {0xc2e00000, 0x00000000 ,},   //-112
    {0xc3000000, 0x00000000 ,},   //-128
    {0xc2aeac4f, 0x00800026 ,},   //-87.3365  smallest normal result
    {0xc2aeac50, 0x007fffe6 ,},   //-87.3365  largest denormal result
    {0xc2ce0000, 0x00000001 ,},   //-103
    {0x42b17216, 0x7f7fff04 ,},   //88.7228   largest value  --
    {0x42b17217, 0x7f7fff84 ,},   //88.7228   largest value
    {0x42b17218, 0x7f800000 ,},   //88.7228   overflow
    {0x50000000, 0x7f800000 ,},   //large   overflow
    {0xc20a1eb8, 0x269162c0 ,}, // -34.53
    {0xc6de66b6, 0x00000000 ,}, // -28467.3555
    {0xbe99999a, 0x3f3da643 ,}, // -0.3
    {0xbf247208, 0x3f06ab02 ,}, // -0.642365
    {0xbf000000, 0x3f1b4598 ,}, // -0.5
    {0x3e99999a, 0x3facc82d ,}, // 0.3
    {0x3f247208, 0x3ff35307 ,}, // 0.642365
    {0x3f000000, 0x3fd3094c ,}, // 0.5
    {0x420a1eb8, 0x586162f9 ,}, // 34.53
    {0x46de66b6, 0x7f800000 ,}, // 28467.3555
    {0xc2c80000, 0x0000001b ,}, // -100
    {0x42c80000, 0x7f800000 ,}, // 100
};

static struct libm_test_special_data_f64
test_cos_special_data[] = {
    {0xffefffffffffffff, 0x0000000000000000,},
    {0xc0862c4379671324, 0x00052288f82fe4ba,},
    {0x7ff0000000000000, 0x7ff0000000000000,}, //inf
    {0xfff0000000000000, 0x0000000000000000,}, //-inf
    {0x7ffc000000000000, 0x7ffc000000000000,}, //qnan
    {0x7ff4000000000000, 0x7ffc000000000000,}, //snan
    {0x0000000000000000, 0x3ff0000000000000,}, //0

    {0x3e45798ee2308c3a, 0x3ff0000002af31dc,}, // .00000001
    {0x400921fb54442d18, 0x403724046eb09339,}, //pi
    {0xc086be6fb2739468, 0x0000000001000000,}, // denormal result
    {0x3ff0000000000000, 0x4005bf0a8b145769,}, // 1
    {0x4000000000000000, 0x401d8e64b8d4ddae,}, // 2
    {0xbff0000000000000, 0x3fd78b56362cef38,}, // -1

    {0x4024000000000000, 0x40d5829dcf950560,}, // 10
    {0x408f400000000000, 0x7ff0000000000000,}, // 1000
    {0x4084000000000000, 0x79a40a4b9c27178a,}, // 640
    {0xc085e00000000000, 0x00d14f2b0fb9307f,}, // -700
    {0xc07f51999999999a, 0x12c0be4b336b18b7,}, // -501.1

    {0xc086d00000000000, 0x00000000001c7ea3,}, // -730
    {0xc086232bdd7abcd2, 0x001000000000007c,}, // smallest normal  result, x=-1022*ln(2)
    {0xc086232bdd7abcd3, 0x000ffffffffffe7c,}, // largest denormal result
    {0xc0874385446d71c4, 0x0000000000000001,}, // x=-1074*ln(2)
    {0xc0874910d52d3051, 0x0000000000000001,}, // smallest denormal result, x=-1075*ln(2)
    {0xc0874910d52d3052, 0x0000000000000000,}, // largest input for result zero
    {0xc08f400000000000, 0x0000000000000000,}, // -1000

    {0x40862e42fefa39ef, 0x7fefffffffffff2a,}, // largest normal result
    {0x40862e42fefa39f0, 0x7ff0000000000000,}, // smallest input for result inf
    {0x4086280000000000, 0x7fdd422d2be5dc9b,}, // 709

    {0x7fefffffffffffff, 0x7ff0000000000000,}, // largest number
    {0xffefffffffffffff, 0x0000000000000000,}, // smallest number

    // all denormal
    {0xc08625aad16d5438, 0x000bb63ae9a2ac50,},
    {0xc08627fa8b8965a4, 0x0008c5deb69c6fc8,},
    {0xc0862c4379671324, 0x00052288f82fe4ba,},
    {0xc087440b864646f5, 0x0000000000000001,},

    {0xc08743e609f06b07, 0x0000000000000001,},
    {0xc0874409d4de2a93, 0x0000000000000001,},
    {0xc08744b894a31d87, 0x0000000000000001,},
    {0xc08744ddf48a3b9c, 0x0000000000000001,},
    {0xc08745723e498e76, 0x0000000000000001,},
    {0xc0874593fa89185f, 0x0000000000000001,},
    {0xffefffffffffffff, 0x0000000000000000,},
    {0xffefffffffffffff, 0x0000000000000000,},
    {0xffefffffffffffff, 0x0000000000000000,},
    {0xffefffffffffffff, 0x0000000000000000,},
};
