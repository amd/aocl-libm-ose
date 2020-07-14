#include <fenv.h>
#include <libm_tests.h>
#include<libm_util_amd.h>

/*
 * Test cases to check for exceptions for the sinf() routine.
 * These test cases are not exhaustive
 * Test data added as per GLIBC output
 */
static struct libm_test_special_data_f32
test_sinf_conformance_data[] = {
    {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID,}, //sinf(nan)=nan
    {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID,},   //sinf(-nan)=-nan
    {POS_QNAN_F32, POS_QNAN_F32, 0,},  //sinf(qnan) = qnan
    {NEG_QNAN_F32, NEG_QNAN_F32, 0,},  //sinf(-qnan) = -qnan
    {POS_ZERO_F32, POS_ZERO_F32, 0,},   //sinf(0)=0
    {NEG_ZERO_F32, NEG_ZERO_F32, 0,},      //sinf(-0) = -0
    {POS_ONE_F32, 0x3f576aa4, FE_INEXACT,}, //sin(1)
    {POS_INF_F32, 0xffc00000, FE_INVALID,},      //inf
    {NEG_INF_F32, 0xffc00000, FE_INVALID,},      //-inf
    {NEG_ONE_F32, 0xbf576aa4, FE_INEXACT,}, //-1
    {POS_PI_F32, 0x0, 0,}, //sin(pi)=0
    {0x3B800000, 0x3B7FFFD5, 0},    //sini
    {0x3FC90FDB, 0x3f800000, 0},     //sin(Pi/2)=1
    /*some vals taken from old test framework*/
    {0x3fc90de3, 0x3f800000, 0},// 1.57074 = pi/2 - 6e-05
    {0x3fc911d2, 0x3f800000, 0},//// 1.57086 = pi/2 + 6e-05
    {0x3fc88fdb, 0x3f7fff80, 0}, // 1.56689 = pi/2 - 0.00390625
    {0x3fc98fdb, 0x3f7fff80, 0}, // 1.57470 = pi/2 + 0.00390625
    {0x40000000, 0x3f68c7b7, 0}, //sin(2)
    {0x41200000, 0xbf0b44f8, 0}, //10
    {0xC8C35000, 0x3e11f59f, 0}, //-4e5
    {0x48F42400, 0x3e361962, 0}, //50000
    {0x48F4CCC8, 0x3f3a0e4a, 0}, //501350.25 > 5.0e5
    {0xC93F6800, 0xbe1dbf77, 0}, //-784000
    {0x43FCE5F1, 0xb20fd1de, 0}, //505.796417, remainder is ~0, very close multiple of piby2
    {0x4831E550, 0x379a62a2, 0}, //182165.25, remainder is ~piby4
    {0x42FCE5F1, 0x3F3504F3, 0}, //126.449104
    {0x5123e87f, 0xb18a4ed8, 0}, //4.399877E+10, close to pi/2
};

static struct libm_test_special_data_f64
test_sin_conformance_data[] = {
    {POS_ZERO_F64, POS_ZERO_F64, 0},           //sin(0)=0
    {NEG_ZERO_F64, NEG_ZERO_F64, 0},           //sin(-0)=-0
    {POS_ONE_F64, 0x3feaed548f090cee, 48},          //sin(1)=0.84
    {POS_INF_F64, 0xfff8000000000000, FE_INVALID,}, //sin(inf)=inf
    {NEG_INF_F64, 0xfff8000000000000, FE_INVALID,}, //sin(-inf)=-inf
    {POS_SNAN_F64, 0x7ff4001000000000, FE_INVALID,}, //sin(snan)=snan
    {NEG_SNAN_F64, 0xfff2000000000000, FE_INVALID,}, //sin(-snan)=-snan
    {POS_QNAN_F64, 0x7ff87ff7e0000000, 0,},          //sin(qnan)=qnan
    {NEG_QNAN_F64, 0xfffa000000000000, 0,}, //sin(-qnan)
    {POS_PI_F64,         POS_ZERO_F64, 0,},  //sin(pi)=0
    {0x3ff921fb544486e0, 0x0000000000000000, 0,},  //sin(n*Pi)=sin(pi) for any n=1,2,3..
    {0x4012D97C7F336528, 0x0000000000000000, 0,}, //sin(270)

    {0xc000000000000000, 0xbfed18f6ead1b446, 0}, //-2
    {0x3f18000000000000, 0x3F17FFFFFF700000, 0}, //9.1552734375E-05	 =  2^-14 *1.5
    {0xbf18000000000000, 0xbF17FFFFFF700000, 0}, //-9.1552734375E-05	 = -2^-14 *1.5
    {0x412E848abcdef000, 0xbfee94b095ab2499, 0}, //1000005.3688883781 > 5e5
    {0x400921FB54442D18, 0x3ca1a62633145c07, 0}, //3.1415926535897931 = pi
    {0x400f6a7a2955385e, 0xbfe6a09e667f3bcc, 0},
    {0x400f6a7a2955385d, 0xbfe6a09e667f3bc9, 0},
    {0x400f6a7a2955385f, 0xbfe6a09e667f3bce, 0},
    {0x3e38000000000000, 0x3E38000000000000, 0}, // 5.5879354476928711E-09 = 2^-28*1.5
    {0xbe38000000000000, 0xBE38000000000000, 0}, // -5.5879354476928711E-09 = -2^-28*1.5
    {0x3f18000000000000, 0x3F17FFFFFF700000, 0}, //9.1552734375E-05	 =  2^-14 *1.5
    {0xbf18000000000000, 0xbF17FFFFFF700000, 0}, //-9.1552734375E-05	 = -2^-14 *1.5
    {0x3fe0000000000000, 0x3fdeaee8744b05f0, 0}, //0.5
    {0xbfe0000000000000, 0xBFDEAEE8744B05F0, 0}, //-0.5
    {0x3FEFFEE4E9DABC6B, 0x3FEAECBB972204E8, 0}, //0.999865013834904
    {0x3fe921fb54442d18, 0x3FE6A09E667F3BCC, 0}, //0.78539816339744828 = pi/4
    {0xbfe921fb54442d18, 0xBFE6A09E667F3BCC, 0}, //-0.78539816339744828 =-pi/4
    {0x412E848abcdef000, 0xbfee94b095ab2499, 0}, // 1000005.3688883781 > 5e5
    {0x439332270327F466, 0xbfefff7a431a18dc, 0}, // 3.4580273251621926E+17

    {0x3ff921fb54442d18LL, 0x3ff0000000000000LL, 0},  // 1.5707963267948966 = pi/2
    {0x400921fb54442d18LL, 0x3ca1a62633145c07LL, 0},  // 3.1415926535897931 = pi
    {0x4012D97C7F3321D2LL, 0xbff0000000000000LL, 0},  // 4.71238898038469 = 3pi/2
    {0x401921fb54442d18LL, 0xbcb1a62633145c07LL, 0},  // 6.2831853071795862 = 2pi
    {0x402921fb54442d18LL, 0xbcc1a62633145c07LL, 0},  // 12.566370614359172 = 4pi
    {0x410921fb54442d18LL, 0xbda1a62633145c07LL, 0},  // 205887.41614566068 =  (2^16)pi
    {0x403921fb54442d18LL, 0xbcd1a62633145c07LL, 0},  // 25.132741228718345 = 8pi
    {0x403921fb54442d19LL, 0x3ce72cece675d1fdLL, 0},  // 25.132741228718348 close to 8pi
    {0x3ff921fb57442d18LL, 0x3fefffffffffffffLL, 0},  // 1.5707963379707675 close to pi/2
    {0x400921fb52442d18LL, 0x3e5000000234c4c6LL, 0},  // 3.1415926386886319 close to pi
    {0x410921fb56442d18LL, 0x3f4fffffa6412185LL, 0},  // 205887.41712222318 close to (2^16)pi
    {0xbff921fb57442d18LL, 0xbfefffffffffffffLL, 0},  //-1.5707963379707675 close to - pi/2
    {0xc00921fb54442d18LL, 0xbca1a62633145c07LL, 0},  //-3.1415926535897931 = - pi
    {0x400921f554442d18LL, 0x3ee7fffffffeda62LL, 0},  // 3.1415812094979962 close to pi
    {0xc00921f554442d18LL, 0xbee7fffffffeda62LL, 0},  //-3.1415812094979962 close to -pi
    {0xbff921f557442d18LL, 0xbfeffffffffdc23fLL, 0},  //-1.570790615924869 close to -pi/2
    {0xbff9217557442d18LL, 0xbfeffffffb9df240LL, 0},  //-1.570668545612369 close to -pi/2
    {0x400921fb56442d18LL, 0xbe4ffffffb967673LL, 0},  // 3.1415926684909543 close to pi
    {0x4012D98C7F3321D2LL, 0xbfefffffff000000LL, 0},  // 4.71245001554094 close to 3pi/2
};

static struct libm_test_special_data_f32
test_sinf_special_data[] = {
    {0x3c000000, 0x3bffff55 ,},   //0.0078125
    {0x3c7fffff, 0x3c7ffd54 ,},   //0.0156249991
    {0x3f012345, 0x3ef775de ,},   //0.504444
    {0x3f800000, 0x3f576aa4 ,},   //1
    {0x40000000, 0x3f68c7b7 ,},   //2
    {0x33d6bf95, 0x33d6bf95 ,},   //1.0000000e-7
    {0x4048f5c3, 0x3ad0bd0b ,},   //pi*/
    {0x40c90fdb, 0x343bbd2e ,},   //2pi
    {0x41200000, 0xbf0b44f8 ,},   //10
    {0x447a0000, 0x3f53ae61 ,},   //1000
    {0x42800000, 0x3f6b86d4 ,},   //64
    {0x42af0000, 0xbee56824 ,},   //87.5
    {0x42b00000, 0x3d10fdd0 ,},   //88
    {0x42c00000, 0x3f7bcc68 ,},   //96
    {0xc2af0000, 0x3ee56824 ,},   //-87.5
    {0xc2e00000, 0x3f63d6c0 ,},   //-112
    {0xc3000000, 0xbf3895ed ,},   //-128
    {0xc2aeac4f, 0x3f166b12 ,},   //-87.3365  smallest normal result
    {0xc2aeac50, 0x3f166aab ,},   //-87.3365  largest denormal result
    {0xc2ce0000, 0xbf1f7c2f ,},   //-103
    {0x50000000, 0x3f51c81c ,},   //large   overflow
    {0xc20a1eb8, 0xbce16b33 ,}, // -34.53
    {0xc6de66b6, 0x3f7b913c ,}, // -28467.3555
    {0xbe99999a, 0xbe974e6d ,}, // -0.3
    {0xbf247208, 0xbf195e02 ,}, // -0.642365
    {0xbf000000, 0xbef57744 ,}, // -0.5
    {0x3e99999a, 0x3e974e6d ,}, // 0.3
    {0x3f247208, 0x3f195e02 ,}, // 0.642365
    {0x3f000000, 0x3ef57744 ,}, // 0.5
    {0x420a1eb8, 0x3ce16b33 ,}, // 34.53
    {0x46de66b6, 0xbf7b913c ,}, // 28467.3555
    {0xc2c80000, 0x3f01a12e ,}, // -100
    {0x42c80000, 0xbf01a12e ,}, // 100
};

static struct libm_test_special_data_f64
test_sin_special_data[] = {
    {0xffefffffffffffff, 0xbf7452fc98b34e97,},
    {0xc0862c4379671324, 0x3fdcd022a5e2d7dd,},
    {0x7ff0000000000000, 0xfff8000000000000,}, //inf
    {0xfff0000000000000, 0xfff8000000000000,}, //-inf
    {0x7ffc000000000000, 0x7ffc000000000000,}, //qnan
    {0x7ff4000000000000, 0x7ffc000000000000,}, //snan
    {0x0000000000000000, 0x0000000000000000,}, //0

    {0x3e45798ee2308c3a, 0x3e45798ee2308c3a,}, // .00000001
    {0x400921fb54442d18, 0x3ca1a62633145c07,}, //pi
    {0xc086be6fb2739468, 0x3febad47f50fa6ff,}, // denormal result
    {0x3ff0000000000000, 0x3feaed548f090cee,}, // 1
    {0x4000000000000000, 0x3fed18f6ead1b446,}, // 2
    {0xbff0000000000000, 0xbfeaed548f090cee,}, // -1

    {0x4024000000000000, 0xbfe1689ef5f34f52,}, // 10
    {0x408f400000000000, 0x3fea75cc150a206b,}, // 1000
    {0x4084000000000000, 0xbfe8c366507038d3,}, // 640
    {0xc085e00000000000, 0xbfe16834defaadb0,}, // -700
    {0xc07f51999999999a, 0x3feffef48468d803,}, // -501.1

    // all denormal
    {0xc08625aad16d5438, 0x3feec2a16a38af7d,},
    {0xc08627fa8b8965a4, 0x3feaf8d44ff01c5b,},
    {0xc0862c4379671324, 0x3fdcd022a5e2d7dd,},
    {0xc087440b864646f5, 0xbfaa86a4967bd9e3,},

    {0xc08743e609f06b07, 0xbfb1f09de81a321c,},
    {0xc0874409d4de2a93, 0xbfaaf2d8c46da264,},
    {0xc08744b894a31d87, 0x3fa0b92508d90625,},
    {0xc08744ddf48a3b9c, 0x3faa0eff9f8ab2c6,},
    {0xc08745723e498e76, 0x3fbf7db035104391,},
    {0xc0874593fa89185f, 0x3fc1d5f06f86575e,},
    {0xffefffffffffffff, 0xbf7452fc98b34e97,},
};
