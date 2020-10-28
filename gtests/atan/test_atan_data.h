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
   {0xF149F2C9, 0x7f800000,  FE_OVERFLOW}, //lambda + x = 1, x = -9.9999994e+29
   {0xF149F2C8, 0x7f800000,  FE_OVERFLOW}, //lambda + x < 1
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
};

static libm_test_special_data_f64
test_atan_conformance_data[] = {
    // special accuracy tests
    {0x3e30000000000000LL, 0x3ff0000000000000LL, 0},  //min, small enough that atan(x) = 1 //
    {0x3E2FFFFFFFFFFFFFLL, 0x3ff0000000000000LL, 0}, //min - 1 bit
    {0x3e30000000000001LL, 0x3ff0000000000000LL, 0}, //min + 1 bit
    {0xFE37E43C8800759CLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x = 1, x = -1.0000000000000000e+300
    {0xFE37E43C8800758CLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x < 1
    {0xFE37E43C880075ACLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //lambda + x > 1
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

};

static libm_test_special_data_f32
test_atanf_special_data[] = {
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

static libm_test_special_data_f64
test_atan_special_data[] = {
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
