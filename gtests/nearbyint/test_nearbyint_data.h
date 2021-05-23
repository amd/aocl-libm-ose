#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>
/*
 * Test cases to check for exceptions for the nearbyintf() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_nearbyintf_conformance_data[] = {
    {POS_SNAN_F32,POS_QNAN_F32,FE_INVALID},
    {NEG_SNAN_F32,NEG_QNAN_F32,FE_INVALID},
    {POS_QNAN_F32,POS_QNAN_F32,0},
    {NEG_QNAN_F32,NEG_QNAN_F32,0},
    {POS_INF_F32,POS_INF_F32,0},
    {NEG_INF_F32,NEG_INF_F32,0},
    {POS_ONE_F32,POS_ONE_F32,0},
    {NEG_ONE_F32,NEG_ONE_F32,0},
    {POS_ZERO_F32,POS_ZERO_F32,0},
    {NEG_ZERO_F32,NEG_ZERO_F32,0},
    {POS_PI_F32     ,0x40800000,0},
    {NEG_PI_F32     ,0xc0400000,0},
    {0x00000001,0x3f800000,0},  // denormal min
    {0x0005fde6,0x3f800000,0},  // denormal intermediate
    {0x007fffff,0x3f800000,0},  // denormal max
    {0x80000001,0x80000000,0},  // -denormal min
    {0x805def12,0x80000000,0},  // -denormal intermediate
    {0x807FFFFF,0x80000000,0},  // -denormal max
    {0x00800000,0x3f800000,0},  // normal min
    {0x43b3c4ea,0x43b40000,0},  // normal intermediate
    {0x7f7fffff,0x7f7fffff,0},  // normal max
    {0x80800000,0x80000000,0},  // -normal min
    {0xc5812e71,0xc5812800,0},  // -normal intermediate
    {0xFF7FFFFF,0xff7fffff,0},  // -normal max
    {0x7Fc00000,0x7fc00000,0},  // qnan min
    {0x7Fe1a570,0x7fe1a570,0},  // qnan intermediate
    {0x7FFFFFFF,0x7fffffff,0},  // qnan max
    {0xfFc00000,0xffc00000,0},  // indeterninate
    {0xFFC00001,0xffc00001,0},  // -qnan min
    {0xFFd2ba31,0xffd2ba31,0},  // -qnan intermediate
    {0xFFFFFFFF,0xffffffff,0},  // -qnan max
    {0x3FC90FDB,0x40000000,0},  // pi/2
    {0x40C90FDB,0x40e00000,0},  // 2pi
    {0x402DF853,0x40400000,0},  // e --
    {0x402DF854,0x40400000,0},  // e
    {0x402DF855,0x40400000,0},  // e ++
    {0x37C0F01F,0x3f800000,0},  // 0.000023
    {0x3EFFFEB0,0x3f800000,0},  // 0.49999
    {0x3F0000C9,0x3f800000,0},  // 0.500012
    {0xb7C0F01F,0x80000000,0},  // -0.000023
    {0x3EFFFEB0,0x3f800000,0},  // 0.49999
    {0x3F0000C9,0x3f800000,0},  // 0.500012
    {0xb7C0F01F,0x80000000,0},  // -0.000023
    {0xbEFFFEB0,0x80000000,0},  // -0.49999
    {0xbF0000C9,0x80000000,0},  // -0.500012
    {0x3f700001,0x3f800000,0},  // 0.93750006
    {0x3F87FFFE,0x40000000,0},  // 1.0624998
    {0x3FBFFFAC,0x40000000,0},  // 1.49999
    {0x3FC00064,0x40000000,0},  // 1.500012
    {0xbf700001,0x80000000,0},  // -0.93750006
    {0xbF87FFFE,0xbf800000,0},  // -1.0624998
    {0xbFBFFFAC,0xbf800000,0},  // -1.49999
    {0xbFC00064,0xbf800000,0},  // -1.500012
    {0x40000000,0x40000000,0},  // 2
    {0xc0000000,0xc0000000,0},  // -2
    {0x41200000,0x41200000,0},  // 10
    {0xc1200000,0xc1200000,0},  // -10
    {0x447A0000,0x447a0000,0},  // 1000
    {0xc47A0000,0xc47a0000,0},  // -1000
    {0x4286CCCC,0x42880000,0},  // 67.4
    {0xc286CCCC,0xc2860000,0},  // -67.4
    {0x44F7F333,0x44f80000,0},  // 1983.6
    {0xc4F7F333,0xc4f7e000,0},  // -1983.6
    {0x42AF0000,0x42b00000,0},  // 87.5
    {0xc2AF0000,0xc2ae0000,0},  // -87.5
    {0x48015E40,0x48015e40,0},  // 132473
    {0xc8015E40,0xc8015e40,0},  // -132473
    {0x4B000000,0x4b000000,0},  // 2^23
    {0x4B000001,0x4b000001,0},  // 2^23 + 1
    {0xcB000001,0xcb000001,0},  // -(2^23 + 1)
    {0xcAFFFFFF,0xcafffffe,0}   // -(2^23 -1 + 0.5)
};

static libm_test_special_data_f64
test_nearbyint_conformance_data[] = {
    {POS_SNAN_F64,POS_QNAN_F64,FE_INVALID},
    {NEG_SNAN_F64,NEG_QNAN_F64,FE_INVALID},
    {POS_QNAN_F64,POS_QNAN_F64,0},
    {NEG_QNAN_F64,NEG_QNAN_F64,0},
    {POS_INF_F64,POS_INF_F64,0},
    {NEG_INF_F64,NEG_INF_F64,0},
    {POS_ONE_F64,POS_ONE_F64,0},
    {NEG_ONE_F64,NEG_ONE_F64,0},
    {POS_ZERO_F64,POS_ZERO_F64,0},
    {NEG_ZERO_F64,NEG_ZERO_F64,0},
    {POS_PI_F64      ,0x4010000000000000LL,0},
    {NEG_PI_F64      ,0xc008000000000000LL,0},
    {0x0000000000000001LL, 0x3ff0000000000000LL,0}, // denormal min
    {0x0005fde623545abcLL, 0x3ff0000000000000LL,0}, // denormal intermediate
    {0x000FFFFFFFFFFFFFLL, 0x3ff0000000000000LL,0}, // denormal max
    {0x8000000000000001LL, 0x8000000000000000LL,0}, // -denormal min
    {0x8002344ade5def12LL, 0x8000000000000000LL,0}, // -denormal intermediate
    {0x800FFFFFFFFFFFFFLL, 0x8000000000000000LL,0}, // -denormal max
    {0x0010000000000000LL, 0x3ff0000000000000LL,0}, // normal min
    {0x43b3c4eafedcab02LL, 0x43b3c4eafedcab02LL,0}, // normal intermediate
    {0x7FEFFFFFFFFFFFFFLL, 0x7fefffffffffffffLL,0}, // normal max
    {0x8010000000000000LL, 0x8000000000000000LL,0}, // -normal min
    {0xc5812e71245acfdbLL, 0xc5812e71245acfdbLL,0}, // -normal intermediate
    {0xFFEFFFFFFFFFFFFFLL, 0xffefffffffffffffLL,0}, // -normal max
    {0x7FF8000000000000LL, 0x7ff8000000000000LL,0}, // qnan min
    {0x7FFe1a5701234dc3LL, 0x7ffe1a5701234dc3LL,0}, // qnan intermediate
    {0x7FFFFFFFFFFFFFFFLL, 0x7fffffffffffffffLL,0}, // qnan max
    {0xFFF8000000000000LL, 0xfff8000000000000LL,0}, // indeterninate
    {0xFFF8000000000001LL, 0xfff8000000000001LL,0}, // -qnan min
    {0xFFF9123425dcba31LL, 0xfff9123425dcba31LL,0}, // -qnan intermediate
    {0xFFFFFFFFFFFFFFFFLL, 0xffffffffffffffffLL,0}, // -qnan max
    {0x3FF921FB54442D18LL, 0x4000000000000000LL,0}, // pi/2
    {0x401921FB54442D18LL, 0x401c000000000000LL,0}, // 2pi
    {0x3FFB7E151628AED3LL, 0x4000000000000000LL,0}, // e --
    {0x4005BF0A8B145769LL, 0x4008000000000000LL,0}, // e
    {0x400DBF0A8B145769LL, 0x4010000000000000LL,0}, // e ++
    {0x3C4536B8B14B676CLL, 0x3ff0000000000000LL,0}, // 0.0000000000000000023
    {0x3FDFFFFBCE4217D3LL, 0x3ff0000000000000LL,0}, // 0.4999989999999999999
    {0x3FE000000000006CLL, 0x3ff0000000000000LL,0}, // 0.500000000000012
    {0xBBDB2752CE74FF42LL, 0x8000000000000000LL,0}, // -0.000000000000000000023
    {0xBFDFFFFBCE4217D3LL, 0x8000000000000000LL,0}, // -0.4999989999999999999
    {0xBFE000000000006CLL, 0x8000000000000000LL,0}, // -0.500000000000012
    {0x3FEFFFFFC49BD0DCLL, 0x3ff0000000000000LL,0}, // 0.9999998893750006
    {0x3FF0000000000119LL, 0x4000000000000000LL,0}, // 1.0000000000000624998
    {0x3FF7FFFEF39085F4LL, 0x4000000000000000LL,0}, // 1.499998999999999967
    {0x3FF8000000000001LL, 0x4000000000000000LL,0}, // 1.50000000000000012
    {0xBFEFFFFFC49BD0DCLL, 0x8000000000000000LL,0}, // -0.9999998893750006
    {0xBFF0000000000119LL, 0xbff0000000000000LL,0}, // -1.0000000000000624998
    {0xBFF7FFFEF39085F4LL, 0xbff0000000000000LL,0}, // -1.499998999999999967
    {0xBFF8000000000001LL, 0xbff0000000000000LL,0}, // -1.50000000000000012
    {0x4000000000000000LL, 0x4000000000000000LL,0}, // 2
    {0xC000000000000000LL, 0xc000000000000000LL,0}, // -2
    {0x4024000000000000LL, 0x4024000000000000LL,0}, // 10
    {0xC024000000000000LL, 0xc024000000000000LL,0}, // -10
    {0x408F400000000000LL, 0x408f400000000000LL,0}, // 1000
    {0xC08F400000000000LL, 0xc08f400000000000LL,0}, // -1000
    {0x4050D9999999999ALL, 0x4051000000000000LL,0}, // 67.4
    {0xC050D9999999999ALL, 0xc050c00000000000LL,0}, // -67.4
    {0x409EFE6666666666LL, 0x409f000000000000LL,0}, // 1983.6
    {0xC09EFE6666666666LL, 0xc09efc0000000000LL,0}, // -1983.6
    {0x4055E00000000000LL, 0x4056000000000000LL,0}, // 87.5
    {0xC055E00000000000LL, 0xc055c00000000000LL,0}, // -87.5
    {0x41002BC800000000LL, 0x41002bc800000000LL,0}, // 132473
    {0xC1002BC800000000LL, 0xc1002bc800000000LL,0}, // -132473
    {0x4330000000000000LL, 0x4330000000000000LL,0}, // 2^52
    {0x4330000000000001LL, 0x4330000000000001LL,0}, // 2^52 + 1
    {0x432FFFFFFFFFFFFFLL, 0x4330000000000000LL,0}, // 2^52 -1 + 0.5
    {0xC330000000000000LL, 0xc330000000000000LL,0}, // -2^52
    {0xC330000000000001LL, 0xc330000000000001LL,0}, // -(2^52 + 1)
    {0xC32FFFFFFFFFFFFFLL, 0xc32ffffffffffffeLL,0}  // -(2^52 -1 + 0.5)
};
