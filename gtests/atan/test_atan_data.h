#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>
/*
 * Test cases to check for exceptions for the atanf() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_atanf_conformance_data[] = {
   // special accuracy tests
   {0x38800000, 0x3f800000,  0},  //min= 0.00006103515625, small enough that atan(x) = 1
   {0x387FFFFF, 0x3f800000,  0}, //min - 1 bit
   {0x38800001, 0x3f800000,  0}, //min + 1 bit
   {0xF149F2C9, 0x7f800000,  FE_INEXACT}, //lambda + x = 1, x = -9.9999994e+29
   {0xF149F2C8, 0x7f800000,  FE_INEXACT}, //lambda + x < 1
   {0xF149F2CA, 0x7f800000,  FE_OVERFLOW}, //lambda + x > 1
   {0x42B2D4FC, 0x7f7fffec,  0}, //max arg, x = 89.41598629223294,max atanf arg
   {0x42B2D4FB, 0x7f7fff6c,  0}, //max arg - 1 bit
   {0x42B2D4FD, 0x7f800000,  FE_OVERFLOW}, //max arg + 1 bit
   {0x42B2D4FF, 0x7f800000,  FE_OVERFLOW}, // > max
   {0x42B2D400, 0x7f7f820b,  0}, // < max
   {0x41A00000, 0x4d675844,  0}, //small_threshold = 20
   {0x41A80000, 0x4e1d3710,  0}, //small_threshold+1 = 21
   {0x41980000, 0x4caa36c8,  0}, //small_threshold - 1 = 19

    //atan special exception checks
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
   {0xc5812e71, 0x7f800000,  FE_INEXACT},  // -normal intermediate
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
   {0x80000000, 0x80000000,  0},
   {0x7f800001, 0x7fc00001,  0},
   {0xff800001, 0xffc00001,  0}, 
   {0x7fc00000, 0x7fc00000,  0}, 
   {0xffc00000, 0xffc00000,  0},
   {0x7f800000, 0x3fc90fdb,  0},
   {0xff800000, 0xbfc90fdb,  0},
   {0x807fffff, 0x807fffff,  0}, 
   {0xff7fffff, 0xbfc90fdb,  0}, 

   //answer from NAG test tool
   {0x35800000, 0x35800000,  0},    // 2 ^(-20) < 2.0^(-19), 9.536743164060E-07
   {0xbe4ccccd, 0xbe4a2210,  0},    // abs(-0.2)< 7./16.,   -1.973955627155E-01
   {0x3f000000, 0x3eed6338,  0},    // 0.5      < 11./16.,   4.636476090008E-01
   {0xbf7cac08, 0xbf476317,  0},    //-0.987    < 19./16.,  -7.788557245266E-01
   {0x400947ae, 0x3f913900,  0},    // 2.145    < 39./16.,   1.134552060813E+00
   {0xc2c80000, 0xbfc7c82f,  0},    // -100     < 2^26,     -1.560796660108E+00
   {0x4d000000, 0x3fc90fdb,  0}, // 2^27     > 2^26,      1.570796319344E+00
   {0xcd000000, 0xbfc90fdb,  0}, //-2^27     > 2^26,     -1.570796319344E+00

};

static libm_test_special_data_f64
test_atan_conformance_data[] = {
	// special accuracy tests
	{0x3e30000000000000LL, 0x3ff0000000000000LL, 0},  //min, small enough that atan(x) = 1 //
	{0x3E2FFFFFFFFFFFFFLL, 0x3ff0000000000000LL, 0}, //min - 1 bit
	{0x3e30000000000001LL, 0x3ff0000000000000LL, 0}, //min + 1 bit
    {0xFE37E43C8800759CLL, 0x7ff0000000000000LL, 0}, //lambda + x = 1, x = -1.0000000000000000e+300
    {0xFE37E43C8800758CLL, 0x7ff0000000000000LL, 0}, //lambda + x < 1
    {0xFE37E43C880075ACLL, 0x7ff0000000000000LL, 0}, //lambda + x > 1
    {0x408633ce8fb9f87eLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //max arg, x = 89.41598629223294,max atanf arg
    {0x408633ce8fb9f87dLL, 0x7feffffffffffd3bLL, 0}, //max arg - 1 bit
    {0x408633ce8fb9f87fLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //max arg + 1 bit
    {0x408633ce8fb9f8ffLL, 0x7ff0000000000000LL, FE_OVERFLOW}, // > max
    {0x408633ce8fb9f800LL, 0x7feffffffffe093bLL, 0}, // < max
    {0x4034000000000000LL, 0x41aceb088b68e804LL, 0}, //small_threshold = 20
    {0x4035000000000000LL, 0x41c3a6e1fd9eecfdLL, 0}, //small_threshold+1 = 21
    {0x4033000000000000LL, 0x419546d8f9ed26e2LL, 0}, //small_threshold - 1 = 19

    //atan special exception checks
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
    {0x43b3c4eafedcab02LL, 0x7ff0000000000000LL, 0}, // normal intermediate
    {0x7FEFFFFFFFFFFFFFLL, 0x7ff0000000000000LL, 0}, // normal max
    {0x8010000000000000LL, 0x3ff0000000000000LL, 0}, // -normal min
    {0xc5812e71245acfdbLL, 0x7ff0000000000000LL, 0}, // -normal intermediate
    {0xFFEFFFFFFFFFFFFFLL, 0x7ff0000000000000LL, 0}, // -normal max
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

    {0x0, 0x0, 0},
    {0x23d, 0x23d, 48},
    {0x3e38000000000000LL, 0x3e38000000000000LL, 0},
    {0xbe38000000000000LL, 0xbe38000000000000LL, 0},
    {0x3fe921fb54442d18LL, 0x3fe54e04c05d06a0LL, 0},
    {0x3f20000000000001LL, 0x3f1ffffffd555558LL, 0},
    {0x3e40000000000001LL, 0x3e40000000000001LL, 0},
    {0x7ffdf00000000000LL, 0x7ffdf00000000000LL, 0},
    {0xfffdf00000000000LL, 0xfffdf00000000000LL, 0},
    {0x7ff4000000000000LL, 0x7ffc000000000000LL, 0},
    {0xfff4000000000000LL, 0xfffc000000000000LL, 0},
    {0x3fe9e0c8f112ab1eLL, 0x3fe5c2b31b022df6LL, 0},
    {0x40306b51f0157e66LL, 0x3ff828d3654969ffLL, 0},
    {0x402ddf5adb92c01aLL, 0x3ff81028770aabe4LL, 0},
    {0x402ddb778a9ebd8aLL, 0x3ff81004ea1b89b6LL, 0},
    {0x401c462b9064a63bLL, 0x3ff6e254e96576c8LL, 0},
    {0x3fe921fb54442d19LL, 0x3fe54e04c05d06a1LL, 0},
    {0x3fe921fb54442d20LL, 0x3fe54e04c05d06a5LL, 0},
    {0x3ff10ca44655d48aLL, 0x3fea2601964ed0b3LL, 0},
    {0x400923e979f8b36aLL, 0x3ff434138906ebf1LL, 0},
    {0x4002dae59bb5c33eLL, 0x3ff2b662ea759ce2LL, 0},
    {0x4015fdca5f9a0e38LL, 0x3ff64104d68b4ed8LL, 0},
    {0x40b93bda357daddaLL, 0x3ff9215901d7452bLL, 0},
    {0x40f63525129291ffLL, 0x3ff921efcd344059LL, 0},
    {0x3ff9207824b27c17LL, 0x3ff00f79d9041e2dLL, 0},
    {0x4025fe9b31eb183dLL, 0x3ff7ae8c24fd4ef8LL, 0},
    {0x4046c6cbc45dc8deLL, 0x3ff8c814427d58baLL, 0},
    {0x3ff0000000000000LL, 0x3fe921fb54442d18LL, 0},
    {0x4000000000000000LL, 0x3ff1b6e192ebbe44LL, 0},
    {0x4008000000000000LL, 0x3ff3fc176b7a8560LL, 0},
    {0x4024000000000000LL, 0x3ff789bd2c160054LL, 0},
    {0xc000000000000000LL, 0xbff1b6e192ebbe44LL, 0},
    {0x3ff921fb54442d18LL, 0x3ff00fe987ed02ffLL, 0},
    {0x4012d97c7f3321d2LL, 0x3ff5c97d37d98aa4LL, 0},
    {0x401921fb54442d18LL, 0x3ff69b8154baf42eLL, 0},
    {0x402921fb54442d18LL, 0x3ff7dcb7c5c399ecLL, 0},
    {0x410921fb54442d18LL, 0x3ff921f63c7811a6LL, 0},
    {0x403921fb54442d18LL, 0x3ff87f17cfda0b5dLL, 0},
    {0x403921fb54442d19LL, 0x3ff87f17cfda0b5dLL, 0},
    {0x3ff921fb57442d18LL, 0x3ff00fe988ca80d4LL, 0},
    {0x400921fb52442d18LL, 0x3ff433b8a2c4a8a9LL, 0},
    {0x410921fb56442d18LL, 0x3ff921f63c78120eLL, 0},
    {0xbff921fb57442d18LL, 0xbff00fe988ca80d4LL, 0},
    {0xc00921fb54442d18LL, 0xbff433b8a322ddd2LL, 0},
    {0x400921f554442d18LL, 0x3ff433b7888324ddLL, 0},
    {0xc00921f554442d18LL, 0xbff433b7888324ddLL, 0},
    {0xbff921f557442d18LL, 0xbff00fe7cdce8b19LL, 0},
    {0xbff9217557442d18LL, 0xbff00fc2e2ee0031LL, 0},
    {0x400921fb56442d18LL, 0x3ff433b8a38112fcLL, 0},
    {0x4012d98c7f3321d2LL, 0x3ff5c97ff9d8c5f2LL, 0},
    {0x412e848abcdef000LL, 0x3ff921fa47d51180LL, 0},
    {0x439332270327f466LL, 0x3ff921fb54442d18LL, 0},
    {0x411fa317083ee0a2LL, 0x3ff921f94e648ff6LL, 0},
    {0x64ca7f352f2afdaeLL, 0x3ff921fb54442d18LL, 0},
    {0xd3d196202a791d3dLL, 0xbff921fb54442d18LL, 0},
    {0x56fdb2fb3712813bLL, 0x3ff921fb54442d18LL, 0},
    {0x54e57b4e03dbe9b3LL, 0x3ff921fb54442d18LL, 0},
    {0xea96be922b1706c5LL, 0xbff921fb54442d18LL, 0},
    {0x655e883346944823LL, 0x3ff921fb54442d18LL, 0},
};
