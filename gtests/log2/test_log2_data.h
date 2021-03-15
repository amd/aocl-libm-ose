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
    {0x35510,          0xc3034374,    0},
    {POS_SNAN_F32 ,    0x7fc00001,    FE_INVALID},
    {NEG_SNAN_F32 ,    0xffc00001,    FE_INVALID},
    {POS_QNAN_F32 ,    0x7fc00000,    0},
    {NEG_QNAN_F32 ,    0xffc00000,    0},
    {POS_INF_F32  ,    0x7f800000,    0},
    {NEG_INF_F32  ,    0x7fc00000,    FE_INVALID},
    {POS_ONE_F32  ,    0x00000000,    0},
    {NEG_ONE_F32  ,    0x7fc00000,    FE_INVALID},
    {POS_ZERO_F32 ,    0xff800000,    0},
    {NEG_ZERO_F32 ,    0xff800000,    0},
    //{POS_HDENORM_F32,  0xc2fc0000,    0},
    //{NEG_HDENORM_F32,  0x7fc00000,    FE_INVALID},
    //{POS_LDENORM_F32,  0xc3150000,    0},
    //{NEG_LDENORM_F32,  0x7fc00000,    FE_INVALID},
    //{POS_HNORMAL_F32,  0x43000000,    0},
    //{NEG_HNORMAL_F32,  0x7fc00000,    FE_INVALID},
    //{POS_LNORMAL_F32,  0xc2fc0000,    0},
    //{NEG_LNORMAL_F32,  0x7fc00000,    FE_INVALID},
    //{F32_POS_PIBY2,   0x3f26c874,    0},  //pi/2
    {0x00000000,        0xff800000,    0},  //0.0
    {0x3c000000,        0xc0e00000,    0},  //0.0078125
    {0x3c7fffff,        0xc0c00000,    0},  //0.0156249991
    {0x3f012345,        0xbf7cbb49,    0},  //0.504444
    {0x3f800000,        0x00000000,    0},  //1
    {0x40000000,        0x3f800000,    0},  //2
    {0x33d6bf95,        0xc1ba0729,    0},  //1.0000000e-7
    {POS_PI_F32,        0x3fd3643a,    0},  //pi
    {0x40c90fdb,        0x4029b21d,    0},  //2pi
    {0x41200000,        0x40549a78,    0},  //10
    {0x447a0000,        0x411f73da,    0},  //1000
    {0x42800000,        0x40c00000,    0},  //64
    {0x42af0000,        0x40ce7052,    0},  //87.5
    {0x42b00000,        0x40ceb3aa,    0},  //88
    {0x42c00000,        0x40d2b803,    0},  //96
    {0xc2af0000,        0x7fc00000,    FE_INVALID},  //-87.5
    {0xc2e00000,        0x7fc00000,    FE_INVALID},  //-112
    {0xc3000000,        0x7fc00000,    FE_INVALID},  //-128
    {0xc2aeac4f,        0x7fc00000,    FE_INVALID},  //-87.3365  smallest normal result
    {0xc2aeac50,        0x7fc00000,    FE_INVALID},  //-87.3365  largest denormal result
    {0xc2ce0000,        0x7fc00000,    FE_INVALID},  //-103
    {0x42b17216,        0x40cf1458,    0},  //88.7228   largest value  --
    {0x42b17217,        0x40cf1458,    0},  //88.7228   largest value
    {0x42b17218,        0x40cf1459,    0},  //88.7228   overflow
    {0x50000000,        0x42040000,    0},  //large   overflow
    {0xc20a1eb8,        0x7fc00000,    FE_INVALID}, // -34.53
    {0xc6de66b6,        0x7fc00000,    FE_INVALID}, // -28467.3555
    {0xbe99999a,        0x7fc00000,    FE_INVALID}, // -0.3
    {0xbf247208,        0x7fc00000,    FE_INVALID}, // -0.642365
    {0xbf000000,        0x7fc00000,    FE_INVALID}, // -0.5
    {0x3e99999a,        0xbfde54e3,    0}, // 0.3
    {0x3f247208,        0xbf237705,    0}, // 0.642365
    {0x3f000000,        0xbf800000,    0}, // 0.5
    {0x420a1eb8,        0x40a3834e,    0}, // 34.53
    {0x46de66b6,        0x416cc099,    0}, // 28467.3555
    {0xc2c80000,        0x7fc00000,    FE_INVALID}, // -100
    {0x42c80000,        0x40d49a78,    0}, // 100
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
    //{POS_HDENORM_F64,   0xc08ff00000000000LL  ,0},
    //{NEG_HDENORM_F64,   0x7ff8000000000000LL  ,FE_INVALID},
    //{POS_LDENORM_F64,   0xc090c80000000000LL  ,0},
    //{NEG_LDENORM_F64,   0x7ff8000000000000LL  ,FE_INVALID},
    //{POS_HNORMAL_F64,   0x4090000000000000LL  ,0},
    //{NEG_HNORMAL_F64,   0x7ff8000000000000LL  ,FE_INVALID},
    //{POS_LNORMAL_F64,   0xc08ff00000000000LL  ,0},
    //{NEG_LNORMAL_F64,   0x7ff8000000000000LL  ,FE_INVALID},
    //{POS_PIBY2_F64,     0x3fe4d90e7ebf727aLL  ,0},  //pi/2
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
};

