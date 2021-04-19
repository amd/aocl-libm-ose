#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>
/*
 * Test cases to check for exceptions for the tanhf() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_tanhf_conformance_data[] = {
   {0x00000001, 0x00000001,  FE_UNDERFLOW},  // denormal min
   {0x0005fde6, 0x0005fde6,  FE_UNDERFLOW},  // denormal intermediate
   {0x007fffff, 0x007fffff,  FE_UNDERFLOW},  // denormal max
   {0x80000001, 0x80000001,  FE_UNDERFLOW},  // -denormal min
   {0x805def12, 0x805def12,  FE_UNDERFLOW},  // -denormal intermediate
   {0x807FFFFF, 0x807fffff,  FE_UNDERFLOW},  // -denormal max
   {0x00800000, 0x00800000,  FE_UNDERFLOW},  // normal min
   {0x43b3c4ea, 0x3f800000,  0},  // normal intermediate
   {0x7f7fffff, 0x3f800000,  0},  // normal max
   {0x80800000, 0x80800000,  FE_UNDERFLOW},  // -normal min
   {0xc5812e71, 0xbf800000,  0},  // -normal intermediate
   {0xFF7FFFFF, 0xbf800000,  0},  // -normal max
   {0x7F800000, 0x3f800000,  0},  // inf
   {0xfF800000, 0xbf800000,  0},  // -inf
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
   {0x3FC90FDB, 0x3f6aca7f,  0},  // pi/2
   {0x40490FDB, 0x3f7f0bb0,  0},  // pi
   {0x40C90FDB, 0x3f7fff8b,  0},  // 2pi
   {0x402DF853, 0x3f7dc7bb,  0},  // e --
   {0x402DF854, 0x3f7dc7bb,  0},  // e
   {0x402DF855, 0x3f7dc7bb,  0},  // e ++
   {0x37C0F01F, 0x37c0f01f,  FE_UNDERFLOW},  // 0.000023
   {0x3EFFFEB0, 0x3eec9996,  0},  // 0.49999
   {0x3F0000C9, 0x3eec9bdb,  0},  // 0.500012
   {0xb7C0F01F, 0xb7c0f01f,  FE_UNDERFLOW},  // -0.000023
   {0xbEFFFEB0, 0xbeec9996,  0},  // -0.49999
   {0xbF0000C9, 0xbeec9bdb,  0},  // -0.500012
   {0x3f800000, 0x3f42f7d6,  0},  // 1
   {0x3f700001, 0x3f3bec1d,  0},  // 0.93750006
   {0x3F87FFFE, 0x3f495fd8,  0},  // 1.0624998
   {0x3FBFFFAC, 0x3f67b7ad,  0},  // 1.49999
   {0x3FC00064, 0x3f67b7f0,  0},  // 1.500012
   {0xbf800000, 0xbf42f7d6,  0},  // -1
   {0xbf700001, 0xbf3bec1d,  0},  // -0.93750006
   {0xbF87FFFE, 0xbf495fd8,  0},  // -1.0624998
   {0xbFBFFFAC, 0xbf67b7ad,  0},  // -1.49999
   {0xbFC00064, 0xbf67b7f0,  0},  // -1.500012
   {0x40000000, 0x3f76ca83,  0},  // 2
   {0xc0000000, 0xbf76ca83,  0},  // -2
   {0x41200000, 0x3f800000,  0},  // 10
   {0xc1200000, 0xbf800000,  0},  // -10
   {0x447A0000, 0x3f800000,  0},  // 1000
   {0xc47A0000, 0xbf800000,  0},  // -1000
   {0x4286CCCC, 0x3f800000,  0},  // 67.4
   {0xc286CCCC, 0xbf800000,  0},  // -67.4
   {0x44F7F333, 0x3f800000,  0},  // 1983.6
   {0xc4F7F333, 0xbf800000,  0},  // -1983.6
   {0x42AF0000, 0x3f800000,  0},  // 87.5

   {0xc2AF0000, 0xbf800000,  0},  // -87.5
   {0x48015E40, 0x3f800000,  0},  // 132473
   {0xc8015E40, 0xbf800000,  0},  // -132473
   {0x4B000000, 0x3f800000,  0},  // 2^23
   {0x4B000001, 0x3f800000,  0},  // 2^23 + 1
   {0x4AFFFFFF, 0x3f800000,  0},  // 2^23 -1 + 0.5
   {0xcB000000, 0xbf800000,  0},  // -2^23
   {0xcB000001, 0xbf800000,  0},  // -(2^23 + 1)
   {0xcAFFFFFF, 0xbf800000,  0},  // -(2^23 -1 + 0.5)

   // special accuracy tests
   {0x38800000, 0x38800000,  FE_UNDERFLOW},  //min= 0.00006103515625, small enough that cosh=1 or sinh(x) = 0
   {0x387FFFFF, 0x387fffff,  FE_UNDERFLOW}, //min - 1 bit
   {0x38800001, 0x38800001,  FE_UNDERFLOW}, //min + 1 bit
   {0xF149F2C9, 0xbf800000,  0}, //lambda + x = 1, x = -9.9999994e+29
   {0xF149F2C8, 0xbf800000,  0}, //lambda + x < 1
   {0xF149F2CA, 0xbf800000,  0}, //lambda + x > 1
   {0x42B2D4FC, 0x3f800000,  0}, //max arg, x = 89.41598629223294,max coshf arg
   {0x42B2D4FB, 0x3f800000,  0}, //max arg - 1 bit
   {0x42B2D4FD, 0x3f800000,  0}, //max arg + 1 bit
   {0x42B2D4FF, 0x3f800000,  0}, // > max
   {0x42B2D400, 0x3f800000,  0}, // < max
   {0x41A00000, 0x3f800000,  0}, //small_threshold = 20
   {0x41A80000, 0x3f800000,  0}, //small_threshold+1 = 21
   {0x41980000, 0x3f800000,  0}, //small_threshold - 1 = 19
   {0x39000000, 0x39000000,  0},
   {0x39000001, 0x39000001,  0},
   {0x38ffffff, 0x38ffffff,  FE_UNDERFLOW},
   {0x4238aa3b, 0x3f800000,  0},
   {0x3cb17000, 0x3cb168e6,  0},
   {0x3585fdf4, 0x3585fdf4,  FE_UNDERFLOW},
   {0x41200000, 0x3f800000,  0},
   {0x4120000f, 0x3f800000,  0},
   {0x411fffff, 0x3f800000,  0},
   {0x3F29465E, 0x3f14453b,  0}, // 0< x < 0.9
   {0x3F7C9A56, 0x3f4186e4,  0}, //0.9 < x < 1.0

    //tanh special exception checks
   {POS_ZERO_F32, 0x00000000,0 },  //0
   {NEG_ZERO_F32, 0x80000000,0 },  //0
   {POS_INF_F32,  0x3f800000,0 },
   {NEG_INF_F32,  0xbf800000,0 },
   {POS_SNAN_F32, POS_QNAN_F32, FE_INVALID },  //
   {NEG_SNAN_F32, NEG_QNAN_F32, FE_INVALID },  //
   {POS_QNAN_F32, POS_QNAN_F32,0 },  //
   {NEG_QNAN_F32, NEG_QNAN_F32,0 },  //
   {0x42BE0000,   0x3f800000,0 },  //95
   //answer from NAG test tool  
   {0x38000000, 0x38000000,  0}, // 2^(-15), < 2 ^(-13), x
   {0xb8000000, 0xb8000000,  0}, //-2^(-15), < 2 ^(-13), x
   {0x41300000, 0x3f800000,  0}, // 11, > large_threshold, +1
   {0xc1300000, 0xbf800000,  0}, //-11, > max_sinh_arg, -1
   {0x40a00000, 0x3f7ffa0d,  0}, // 5, > 1
   {0xc0a00000, 0xbf7ffa0d,  0}, //-5, > 1
   {0x3f800000, 0x3f42f7d6,  0}, // 1, > 0.9
   {0xbf800000, 0xbf42f7d6,  0}, //-1, > 0.9
   {0x3f733333, 0x3f3d626c,  0}, // 0.95, > 0.9 
   {0xbf6c49ba, 0xbf3a3138,  0}, // -0.923, > 0.9
   {0x3dcccccd, 0x3dcc1ebc,  0}, // 0.1, < 0.9
   {0xb951b717, 0xb951b717,  0}, //-0.0002, < 0.9


};

static libm_test_special_data_f64
test_tanh_conformance_data[] = {
    {0x0000000000000001LL, 0x0000000000000001LL, FE_UNDERFLOW}, // denormal min
    {0x0005fde623545abcLL, 0x0005fde623545abcLL, FE_UNDERFLOW}, // denormal intermediate
    {0x000FFFFFFFFFFFFFLL, 0x000fffffffffffffLL, FE_UNDERFLOW}, // denormal max
    {0x8000000000000001LL, 0x8000000000000001LL, FE_UNDERFLOW}, // -denormal min
    {0x8002344ade5def12LL, 0x8002344ade5def12LL, FE_UNDERFLOW}, // -denormal intermediate
    {0x800FFFFFFFFFFFFFLL, 0x800fffffffffffffLL, FE_UNDERFLOW}, // -denormal max
    {0x0010000000000000LL, 0x0010000000000000LL, FE_UNDERFLOW}, // normal min
    {0x43b3c4eafedcab02LL, 0x3ff0000000000000LL, 0}, // normal intermediate
    {0x7FEFFFFFFFFFFFFFLL, 0x3ff0000000000000LL, 0}, // normal max
    {0x8010000000000000LL, 0x8010000000000000LL, FE_UNDERFLOW}, // -normal min
    {0xc5812e71245acfdbLL, 0xbff0000000000000LL, 0}, // -normal intermediate
    {0xFFEFFFFFFFFFFFFFLL, 0xbff0000000000000LL, 0}, // -normal max
    {0x7FF0000000000000LL, 0x3ff0000000000000LL, 0}, // inf
    {0xFFF0000000000000LL, 0xbff0000000000000LL, 0}, // -inf
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
    {0x3FF921FB54442D18LL, 0x3fed594fdae482baLL, 0}, // pi/2
    {0x400921FB54442D18LL, 0x3fefe175fa292810LL, 0}, // pi
    {0x401921FB54442D18LL, 0x3feffff15f81f9abLL, 0}, // 2pi
    {0x3FFB7E151628AED3LL, 0x3fee0146c0b8d088LL, 0}, // e --
    {0x4005BF0A8B145769LL, 0x3fefb8f76b1e2ab6LL, 0}, // e
    {0x400DBF0A8B145769LL, 0x3feff659b95529a7LL, 0}, // e ++
    {0x3C4536B8B14B676CLL, 0x3c4536b8b14b676cLL, FE_UNDERFLOW}, // 0.0000000000000000023
    {0x3FDFFFFBCE4217D3LL, 0x3fdd93508ae5560cLL, 0}, // 0.4999989999999999999
    {0x3FE000000000006CLL, 0x3fdd9353d7568b9dLL, 0}, // 0.500000000000012
    {0xBBDB2752CE74FF42LL, 0xbbdb2752ce74ff42LL, FE_UNDERFLOW}, // -0.000000000000000000023
    {0xBFDFFFFBCE4217D3LL, 0xbfdd93508ae5560cLL, 0}, // -0.4999989999999999999
    {0xBFE000000000006CLL, 0xbfdd9353d7568b9dLL, 0}, // -0.500000000000012
    {0x3FF0000000000000LL, 0x3fe85efab514f394LL, 0}, // 1
    {0x3FEFFFFFC49BD0DCLL, 0x3fe85efa9c23956fLL, 0}, // 0.9999998893750006
    {0x3FF0000000000119LL, 0x3fe85efab514f480LL, 0}, // 1.0000000000000624998
    {0x3FF7FFFEF39085F4LL, 0x3fecf6f91769ce16LL, 0}, // 1.499998999999999967
    {0x3FF8000000000001LL, 0x3fecf6f9786df578LL, 0}, // 1.50000000000000012
    {0xBFF0000000000000LL, 0xbfe85efab514f394LL, 0}, // -1
    {0xBFEFFFFFC49BD0DCLL, 0xbfe85efa9c23956fLL, 0}, // -0.9999998893750006
    {0xBFF0000000000119LL, 0xbfe85efab514f480LL, 0}, // -1.0000000000000624998
    {0xBFF7FFFEF39085F4LL, 0xbfecf6f91769ce16LL, 0}, // -1.499998999999999967
    {0xBFF8000000000001LL, 0xbfecf6f9786df578LL, 0}, // -1.50000000000000012
    {0x4000000000000000LL, 0x3feed9505e1bc3d4LL, 0}, // 2
    {0xC000000000000000LL, 0xbfeed9505e1bc3d4LL, 0}, // -2
    {0x4024000000000000LL, 0x3feffffffdc96f35LL, 0}, // 10
    {0xC024000000000000LL, 0xbfeffffffdc96f35LL, 0}, // -10
    {0x408F400000000000LL, 0x3ff0000000000000LL, 0}, // 1000
    {0xC08F400000000000LL, 0xbff0000000000000LL, 0}, // -1000
    {0x4050D9999999999ALL, 0x3ff0000000000000LL, 0}, // 67.4
    {0xC050D9999999999ALL, 0xbff0000000000000LL, 0}, // -67.4
    {0x409EFE6666666666LL, 0x3ff0000000000000LL, 0}, // 1983.6
    {0xC09EFE6666666666LL, 0xbff0000000000000LL, 0}, // -1983.6
    {0x4055E00000000000LL, 0x3ff0000000000000LL, 0}, // 87.5

    {0xC055E00000000000LL, 0xbff0000000000000LL, 0}, // -87.5
    {0x41002BC800000000LL, 0x3ff0000000000000LL, 0}, // 132473
    {0xC1002BC800000000LL, 0xbff0000000000000LL, 0}, // -132473
    {0x4330000000000000LL, 0x3ff0000000000000LL, 0}, // 2^52
    {0x4330000000000001LL, 0x3ff0000000000000LL, 0}, // 2^52 + 1
    {0x432FFFFFFFFFFFFFLL, 0x3ff0000000000000LL, 0}, // 2^52 -1 + 0.5
    {0xC330000000000000LL, 0xbff0000000000000LL, 0}, // -2^52
    {0xC330000000000001LL, 0xbff0000000000000LL, 0}, // -(2^52 + 1)
    {0xC32FFFFFFFFFFFFFLL, 0xbff0000000000000LL, 0}, // -(2^52 -1 + 0.5)

    // special accuracy tests
    {0x3E2FFFFFFFFFFFFFLL, 0x3e2fffffffffffffLL, FE_UNDERFLOW}, //min - 1 bit
    {0x3e30000000000001LL, 0x3e30000000000001LL, 0}, //min + 1 bit
    {0xFE37E43C8800759CLL, 0xbff0000000000000LL, 0}, //lambda + x = 1, x = -1.0000000000000000e+300
    {0xFE37E43C8800758CLL, 0xbff0000000000000LL, 0}, //lambda + x < 1
    {0xFE37E43C880075ACLL, 0xbff0000000000000LL, 0}, //lambda + x > 1
    {0x408633ce8fb9f87eLL, 0x3ff0000000000000LL, 0}, //max arg, x = 89.41598629223294,max coshf arg
    {0x408633ce8fb9f87dLL, 0x3ff0000000000000LL, 0}, //max arg - 1 bit
    {0x408633ce8fb9f87fLL, 0x3ff0000000000000LL, 0}, //max arg + 1 bit
    {0x408633ce8fb9f8ffLL, 0x3ff0000000000000LL, 0}, // > max
    {0x408633ce8fb9f800LL, 0x3ff0000000000000LL, 0}, // < max
    {0x4034000000000000LL, 0x3ff0000000000000LL, 0}, //small_threshold = 20
    {0x4035000000000000LL, 0x3ff0000000000000LL, 0}, //small_threshold+1 = 21
    {0x4033000000000000LL, 0x3fefffffffffffffLL, 0}, //small_threshold - 1 = 19
    {0x40471547652b82feLL, 0x3ff0000000000000LL, 0},
    {0x3f962e42fe000000LL, 0x3f962d5fb0a51defLL, 0},
    {0x3dcf473de6af278eLL, 0x3dcf473de6af278eLL, FE_UNDERFLOW},
    {0x3e30000000000000LL, 0x3e30000000000000LL, 0},
    {0x3e2fffffffffffffLL, 0x3e2fffffffffffffLL, FE_UNDERFLOW},
    {0x3FE59CE075F6FD22LL, 0x3fe2d52985592f9bLL, 0}, //0<x<0.9
    {0x3FEF934ACAFF6D33LL, 0x3fe830dc87af6b02LL, 0}, //0.9<x<1.0

    //tanh special exception checks
    {POS_ZERO_F64, 0x0000000000000000LL,0 },  //0
    {NEG_ZERO_F64, 0x8000000000000000LL,0 },  //0
    {POS_INF_F64,  0x3ff0000000000000LL,0 },
    {NEG_INF_F64,  0xbff0000000000000LL,0 },
    {POS_SNAN_F64, POS_QNAN_F64, FE_INVALID },  //
    {NEG_SNAN_F64, NEG_QNAN_F64, FE_INVALID },  //
    {POS_QNAN_F64, POS_QNAN_F64, 0 },  //
    {NEG_QNAN_F64, NEG_QNAN_F64, 0 },  //
    {0x4086340000000000LL,0x3ff0000000000000LL,0}  // 710.5

};

