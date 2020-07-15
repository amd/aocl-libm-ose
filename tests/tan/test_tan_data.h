#include <fenv.h>
#include <libm_tests.h>
#include <libm_util_amd.h>

/*
 * Test cases to check for exceptions for the tanf() routine.
 * These test cases are not exhaustive
 */
static struct libm_test_special_data_f32
test_tanf_conformance_data[] = {
    /*special values*/
    {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID,},
    {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID,},
    {POS_INF_F32,  POS_QNAN_F32, FE_INVALID,},
    {NEG_INF_F32,  POS_QNAN_F32, FE_INVALID,},
    {POS_QNAN_F32, POS_QNAN_F32, 0,},
    {NEG_QNAN_F32, NEG_QNAN_F32, 0,},
    {POS_ZERO_F32, POS_ZERO_F32, 0,},
    {NEG_ZERO_F32, NEG_ZERO_F32, 0,},

    {0x00000039, 0x00000039, FE_UNDERFLOW},  // min_float*180/pi
    {0x387BA882, 0x387BA882, FE_UNDERFLOW},  // 6e-05, < 2^(-13)
    {0x3F490FDB, POS_ONE_F32, 0},            //tan(45)=1
    {0xbf490fdb, NEG_ONE_F32, 0},            //tan(-45) = -tan(45) = -1
    {0x3fc90fdb, POS_INF_F32, FE_INVALID},            //tan(90)=+INF
    {0xbfc90fdb, NEG_INF_F32, FE_INVALID},            //tan(-Pi/2) = -INF
    {0x3C000001, 0x3c0000ac, 0},
    {0x39000001, 0x39000001, 0},
    {POS_ONE_F32, 0x3fc75923, 0},  //1
    {NEG_ONE_F32, 0xbfc75923, 0}, //-1
    {0x40000000, 0xc00bd7b1, 0},  //2
    {POS_PI_F32, 0x33bbbd2e, 0},  //pi
    {NEG_PI_F32, 0xb3bbbd2e, 0},  //tan(-pi)=-tan(pi)
    {0x40C90FDB, 0x343bbd2e, 0},  //2pi
    {0x41200000, 0x3f25fafa, 0},  //10
    {0x447A0000, 0x3fbc3395, 0},  //1000
    {0xC8C35000, 0x3e137717, 0},  //-4e5
    {0x48F42400, 0xbe390c74, 0},  //500000
    {0x48F4CCC8, 0xbf876fff, 0},  //501350.25 > 5.0e5
    {0xC93F6800, 0x3e1fa759, 0},  //-784000
    {0x43FCE5F1, 0x320fd1de, 0},  //505.796417, remainder is ~0, very close multiple of piby2
    {0x4831E550, 0xb79a62a2, 0},  //182165.25, remainder is ~piby4
    {0x42FCE5F1, 0x3f800000, 0},  //126.449104
    //Special case for verified the ulps
    {0x5123e87f, 0x318a4ed8, 0},  //4.399877E+10, close to pi/2
    {0x50a3e87f, 0xcdeceba8, 0},  //2.19993846E+10, close to pi/2, ported from OpenCL
};

static struct libm_test_special_data_f64
test_tan_conformance_data[] = {
    /*special values*/
    {POS_ZERO_F64, POS_ZERO_F64, 0}, //tan(0)=0
    {NEG_ZERO_F64, NEG_ZERO_F64, 0}, //tan(-0)=-0
    {POS_SNAN_F64, POS_SNAN_F64, FE_INVALID}, //positive snan
    {NEG_SNAN_F64, NEG_SNAN_F64, FE_INVALID},
    {POS_QNAN_F64, POS_QNAN_F64, 0},
    {NEG_QNAN_F64, NEG_QNAN_F64, 0},
    {POS_INF_F64,  POS_QNAN_F64, FE_INVALID},
    {NEG_INF_F64,  NEG_QNAN_F64, FE_INVALID},

    {0x3ff921fb55206ddf, POS_INF_F64, FE_INVALID}, //tan(pi/2)= +inf
    {0xbff921fb55206ddf, NEG_INF_F64, FE_INVALID}, //tan(-90)=-tan(90)=-INF
    {0x3fe921fb544486e0, POS_ONE_F64, 0},          //tan(45)
    {0xbfe921fb544486e0, NEG_ONE_F64, 0},          //tan(-45)=-1
    {POS_PI_F64,         POS_ZERO_F64, 0},         //tan(pi)=0
    {NEG_PI_F64,         NEG_ZERO_F64, 0},         //tan(-pi)=-0

    {0x000000000000023D, 0x000000000000023D, FE_UNDERFLOW},
    {0x4f6344a970d9ac13, 0x3fe9b03b3afff843, 0},
    {0x723743730464dbf1, 0xbff69b688e1dbfa6, 0},
    {0x3FF10CA44655D48A, 0x3ffcedb5c800bd1d, 0},  // 1.0655863521072058
    {0x400f6a7a2955385e, 0x3feffffffffffffd, 0},
    {0x3FF921FB54442D18, 0x434d02967c31cdb5, 0},
    {0x3FF921FB54442D19, 0xc33617a15494767a, 0},
    {0x3FF921FB54442D17, 0x4329153d9443ed0b, 0},
    {0x3FF921FB54442D1a, 0xc3228f1859543aff, 0},

    {0x000000000000023DLL, 0x000000000000023DLL, FE_UNDERFLOW},
    {0x3e38000000000000LL, 0x3E38000000000000LL, FE_UNDERFLOW},  // 5.5879354476928711E-09 = 2^-28*1.5
    {0xbe38000000000000LL, 0xBE38000000000000LL, FE_UNDERFLOW},  // -5.5879354476928711E-09 = -2^-28*1.5
    {0x3fe921fb54442d18LL, 0x3fefffffffffffffLL, 0},
    {0x3f20000000000001LL, 0x3f20000001555556LL, 0},
    {0x3e40000000000001LL, 0x3e40000000000001LL, 0},
    {0x3FE9E0C8F112AB1ELL, 0x3ff0c363afc40f97LL, 0},  // 0.80868956646495227
    {0x40306B51F0157E66LL, 0x3feb932a53046455LL, 0},  // 16.41921902203321
    {0x402DDF5ADB92C01ALL, 0xbfef22f55ab78c87LL, 0},  // 14.93624006430041
    {0x402DDB778A9EBD8ALL, 0xbfef9cf70c111d67LL, 0},  // 14.928646404149521
    {0x401c462b9064a63bLL, 0x3fefff0d1d19082eLL, 0},  // 7.0685255586081963
    {0x3FE921FB54442D19LL, 0x3ff0000000000001LL, 0},  // 0.78539816339744839
    {0x3fe921fb54442d20LL, 0x3ff0000000000008LL, 0},  // 0.78539816339744917
    {0x3FF10CA44655D48ALL, 0x3ffcedb5c800bd1dLL, 0},  // 1.0655863521072058
    {0x400923E979F8B36ALL, 0x3f4ee25be1d30045LL, 0},  // 3.1425351647398445
    {0x4002DAE59BB5C33ELL, 0xbfeff4b918d4b96aLL, 0},  // 2.3568832554668964
    {0x4015FDCA5F9A0E38LL, 0xbfefff18a55dee76LL, 0},  // 5.4978423059251824
    {0x40b93bda357daddaLL, 0x3fed1849c8c910c6LL, 0},  // 6459.8523787068789
    {0x40F63525129291FFLL, 0x3fe7fdf796804ff9LL, 0},  // 90962.317034311578
    {0x3FF9207824B27C17LL, 0x40a52865814afd94LL, 0},  // 1.5704270776257141
    {0x4025fe9b31eb183dLL, 0xc08258311cfd7912LL, 0},  // 10.997277793834764
    {0x4046C6CBC45DC8DELL, 0xc3b66b9ebc4850c6LL, 0},  // 45.553093477052
    {0x3FF0000000000000LL, 0x3ff8eb245cbee3a6LL, 0},  // 1
    {0x4000000000000000LL, 0xc0017af62e0950f8LL, 0},  // 2
    {0x4008000000000000LL, 0xbfc23ef71254b86fLL, 0},  // 3
    {0x4024000000000000LL, 0x3fe4bf5f34be3782LL, 0},  // 10
    {0xc000000000000000LL, 0x40017af62e0950f8LL, 0},  // - 2
    {0x4012D97C7F3321D2LL, 0x4333570efd768923LL, 0},  // 4.71238898038469 = 3pi/2
    {0x402921fb54442d18LL, 0xbcc1a62633145c07LL, 0},  // 12.566370614359172 = 4pi
    {0x410921fb54442d18LL, 0xbda1a62633145c07LL, 0},  // 205887.41614566068 =  (2^16)pi
    {0x403921fb54442d18LL, 0xbcd1a62633145c07LL, 0},  // 25.132741228718345 = 8pi
    {0x403921fb54442d19LL, 0x3ce72cece675d1fdLL, 0},  // 25.132741228718348 close to 8pi
    {0x3ff921fb57442d18LL, 0xc1955555574b5994LL, 0},  // 1.5707963379707675 close to pi/2
    {0x400921fb52442d18LL, 0xbe5000000234c4c7LL, 0},  // 3.1415926386886319 close to pi
    {0x410921fb56442d18LL, 0x3f5000005320928eLL, 0},  // 205887.41712222318 close to (2^16)p
    {0xbff921fb57442d18LL, 0x41955555574b5994LL, 0},  //-1.5707963379707675 close to - pi/2
    {0x400921f554442d18LL, 0xbee8000000059a62LL, 0},  // 3.1415812094979962 close to pi
    {0xc00921f554442d18LL, 0x3ee8000000059a62LL, 0},  //-3.1415812094979962 close to -pi
    {0xbff921f557442d18LL, 0xc105600557ff5a82LL, 0},  //-1.570790615924869 close to -pi/2
    {0xbff9217557442d18LL, 0xc0be91e0ddf1c46eLL, 0},  //-1.570668545612369 close to -pi/2
    {0x400921fb56442d18LL, 0x3e4ffffffb967674LL, 0},  // 3.1415926684909543 close to pi
    {0x4012D98C7F3321D2LL, 0xc0cfffffff55bf3aLL, 0},  // 4.71245001554094 close to 3pi/2
    //reported cases
    {0x412e848abcdef000LL, 0xc009f5aedd6b6edbLL, 0}, //1000005.3688883781
    {0x439332270327f466LL, 0xc05622b8ed7db581LL, 0}, //3.4580273251621926E+17
    {0x411fa317083ee0a2LL, 0x3ff6eb80ed54d249LL, 0}, //518341.75805235852
    {0x64ca7f352f2afdaeLL, 0x3ff456dcc676e139LL, 0}, //3.3553869926485123E+177
    {0xd3d196202a791d3dLL, 0xc044a2abb21cf8bbLL, 0}, //-5.8694348256855645E+95
    {0x56fdb2fb3712813bLL, 0x4020eed0bfaadeedLL, 0}, //1.1159953350264303E+111
    {0x54e57b4e03dbe9b3LL, 0xc0009db1f2055098LL, 0}, //9.397154915883978E+100
    {0xea96be922b1706c5LL, 0xbfd9103e3f1cf2abLL, 0}, //-2.8524095750052955E+205
    {0x655e883346944823LL, 0xbfe976a695ee9826LL, 0}, //1.9795803721016825E+180

};

static struct libm_test_special_data_f32
test_tanf_special_data[] = {
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
test_tan_special_data[] = {
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
