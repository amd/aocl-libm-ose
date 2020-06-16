#include <fenv.h>
#include <libm_tests.h>

/*
 * Test cases to check for exceptions for the sinf() routine.
 * These test cases are not exhaustive
 */
static struct libm_test_special_data_f32
test_sinf_conformance_data[] = {
    {0x7fbfffff, 0x7fffffff, FE_INVALID,}, //sinf(nan)=nan
    {0xffffffff, 0xffffffff, 0,},   //sinf(-nan)=-nan
    {0x7fe00000, 0x7fe00000, 0,},  //sinf(qnan) = qnan
    {0xffe00000, 0xffe00000, 0,},  //sinf(-qnan) = -qnan
    {0x00000000, 0x00000000, 0,},   //sinf(0)=0
    {0x80000000, 0x80000000, 0,},      //sinf(-0) = -0
    {0x3F800000, 0x3f576aa4, FE_INEXACT,}, //exp(1)  = E
    {0x7F800000, 0xffc00000, 0,},      //inf
    {0xFF800000, 0xffc00000, 0,},      //-inf
    {0xBF800000, 0xbf576aa4, FE_INEXACT,}, //-1
    {0x40490fdb, 0xb3bbbd2e, FE_INEXACT,}, //pi
    {0xc435f37e, 0x3f5d6966, 48,},      //denormal
    {0x447a0000, 0x3f53ae61, 56,},      //1000
    {0xc42f0000, 0xbf0b41a7, 48,},      //-700
    {0x44317218, 0xbe5cb16b, 56,}, //smallest no for result infinity
};

static struct libm_test_special_data_f64
test_sin_conformance_data[] = {
    {0x0000000000000000, 0x0, 0,},
    {0x0,                0x0, 0,},  //exp(0)
    {0x7ff0000000000000, 0xfff8000000000000, 0,},  //exp(infinity)
    {0x7FF0000000000000, 0xfff8000000000000, 0,},         //exp(inf)=inf
    {0xfff0000000000000, 0xfff8000000000000, 0,},  //exp(-inf)
    {0x7FF4001000000000, 0x7ffc001000000000, FE_INVALID,}, //exp(snan)
    {0xfff2000000000000, 0xfffa000000000000, FE_INVALID,}, //exp(-snan)
    {0x7ff87ff7fdedffff, 0x7ff87ff7fdedffff, FE_INVALID,},  //exp(qnan)
    {0xfff2000000000000, 0xfffa000000000000, FE_INVALID,},    //exp(-qnan)
    //{0x40862e42fefa39ef, 0x7fefffffffffff2a, FE_INEXACT,}, //exp(ln(2)*1024)
    //{0x4086300000000000, 0x7ff0000000000000, 40},  //exp(x>ln(2)*1024)
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
