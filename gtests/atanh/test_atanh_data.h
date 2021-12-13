#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>
/*
 * Test cases to check for exceptions for the atanhf() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_atanhf_conformance_data[] = {
   {0x00000001, 0x00000001,  FE_UNDERFLOW},  // denormal min
   {0x0005fde6, 0x0005fde6,  FE_UNDERFLOW},  // denormal intermediate
   {0x007fffff, 0x007fffff,  FE_UNDERFLOW},  // denormal max
   {0x80000001, 0x80000001,  FE_UNDERFLOW},  // -denormal min
   {0x805def12, 0x805def12,  FE_UNDERFLOW},  // -denormal intermediate
   {0x807FFFFF, 0x807fffff,  FE_UNDERFLOW},  // -denormal max
   {0x00800000, 0x00800000,  FE_UNDERFLOW},  // normal min
   {0x43b3c4ea, 0xffc00000,  FE_INVALID},  // normal intermediate
   {0x7f7fffff, 0xffc00000,  FE_INVALID},  // normal max
   {0x80800000, 0x80800000,  FE_UNDERFLOW},  // -normal min
   {0xc5812e71, 0xffc00000,  FE_INVALID},  // -normal intermediate
   {0xFF7FFFFF, 0xffc00000,  FE_INVALID},  // -normal max
   {0x7F800000, 0xffc00000,  FE_INVALID},  // inf
   {0xfF800000, 0xffc00000,  FE_INVALID},  // -inf
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
   {0x3FC90FDB, 0xffc00000,  FE_INVALID},  // pi/2
   {0x40490FDB, 0xffc00000,  FE_INVALID},  // pi
   {0x40C90FDB, 0xffc00000,  FE_INVALID},  // 2pi
   {0x402DF853, 0xffc00000,  FE_INVALID},  // e --
   {0x402DF854, 0xffc00000,  FE_INVALID},  // e
   {0x402DF855, 0xffc00000,  FE_INVALID},  // e ++
   {0x00000000, 0x00000000,  0},  // 0
   {0x37C0F01F, 0x37c0f01f,  FE_UNDERFLOW},  // 0.000023
   {0x3EFFFEB0, 0x3f0c9e74,  0},  // 0.49999
   {0x3F0000C9, 0x3f0ca060,  0},  // 0.500012
   {0x80000000, 0x80000000,  0},  // -0
   {0xb7C0F01F, 0xb7c0f01f,  FE_UNDERFLOW},  // -0.000023
   {0xbEFFFEB0, 0xbf0c9e74,  0},  // -0.49999
   {0xbF0000C9, 0xbf0ca060,  0},  // -0.500012
   {0x3f800000, 0x7f800000,  0},  // 1
   {0x3f700001, 0x3fdbc676,  0},  // 0.93750006
   {0x3F87FFFE, 0xffc00000,  FE_INVALID},  // 1.0624998
   {0x3FBFFFAC, 0xffc00000,  FE_INVALID},  // 1.49999
   {0x3FC00064, 0xffc00000,  FE_INVALID},  // 1.500012
   {0xbf800000, 0xff800000,  0},  // -1
   {0xbf700001, 0xbfdbc676,  0},  // -0.93750006
   {0xbF87FFFE, 0xffc00000,  FE_INVALID},  // -1.0624998
   {0xbFBFFFAC, 0xffc00000,  FE_INVALID},  // -1.49999
   {0xbFC00064, 0xffc00000,  FE_INVALID},  // -1.500012
   {0x40000000, 0xffc00000,  FE_INVALID},  // 2
   {0xc0000000, 0xffc00000,  FE_INVALID},  // -2
   {0x41200000, 0xffc00000,  FE_INVALID},  // 10
   {0xc1200000, 0xffc00000,  FE_INVALID},  // -10
   {0x447A0000, 0xffc00000,  FE_INVALID},  // 1000
   {0xc47A0000, 0xffc00000,  FE_INVALID},  // -1000
   {0x4286CCCC, 0xffc00000,  FE_INVALID},  // 67.4
   {0xc286CCCC, 0xffc00000,  FE_INVALID},  // -67.4
   {0x44F7F333, 0xffc00000,  FE_INVALID},  // 1983.6
   {0xc4F7F333, 0xffc00000,  FE_INVALID},  // -1983.6
   {0x42AF0000, 0xffc00000,  FE_INVALID},  // 87.5
   {0xc2AF0000, 0xffc00000,  FE_INVALID},  // -87.5
   {0xc2AF0000, 0xffc00000,  FE_INVALID},  // -87.5
   {0x48015E40, 0xffc00000,  FE_INVALID},  // 132473
   {0xc8015E40, 0xffc00000,  FE_INVALID},  // -132473
   {0x4B000000, 0xffc00000,  FE_INVALID},  // 2^23
   {0x4B000001, 0xffc00000,  FE_INVALID},  // 2^23 + 1
   {0x4AFFFFFF, 0xffc00000,  FE_INVALID},  // 2^23 -1 + 0.5
   {0xcB000000, 0xffc00000,  FE_INVALID},  // -2^23
   {0xcB000001, 0xffc00000,  FE_INVALID},  // -(2^23 + 1)
   {0xcAFFFFFF, 0xffc00000,  FE_INVALID},  // -(2^23 -1 + 0.5)
   // special accuracy tests
   {0x3f800000, 0x7f800000,  0},  //
   {0x3f800001, 0xffc00000,  FE_INVALID},  //
   {0x3f7fffff, 0x410aa123,  0},  //
   {0xbf800000, 0xff800000,  0},  //
   {0xbf800001, 0xffc00000,  FE_INVALID},  //
   {0xbf7fffff, 0xc10aa123,  0},  //
   {0x39000000, 0x39000000,  0},  //
   {0x39000001, 0x39000001,  0},  //
   {0x38ffffff, 0x38ffffff,  FE_UNDERFLOW},  //
   {0xb9000000, 0xb9000000,  0},  //
   {0xb9000001, 0xb9000001,  0},  //
   {0xb8ffffff, 0xb8ffffff,  FE_UNDERFLOW},  //
   {0x3f000000, 0x3f0c9f54,  0},  //
   {0x3f000001, 0x3f0c9f55,  0},  //
   {0x3effffff, 0x3f0c9f53,  0},  //
   {0xbf000000, 0xbf0c9f54,  0},  //
   {0xbf000001, 0xbf0c9f55,  0},  //
   {0xbeffffff, 0xbf0c9f53,  0},  //
   {0xbf800000, 0xff800000,  0},  //
   {0xbf7f259c, 0xc04ca857,  0},  //
   {0xbf7d70d5, 0xc029652a,  0},  //
   {0xbf7ae1ab, 0xc0130dac,  0},  //
   {0xbf77781e, 0xc0027dab,  0},  //
   {0xbf73342e, 0xbfea7cd2,  0},  //
   {0xbf6e15dc, 0xbfd44b3b,  0},  //
   {0xbf681d27, 0xbfc11aa4,  0},  //
   {0xbf614a0f, 0xbfb01e6d,  0},  //
   {0xbf599c94, 0xbfa0cefb,  0},  //
   {0xbf5114b6, 0xbf92cd66,  0},  //
   {0xbf47b275, 0xbf85d44b,  0},  //
   {0xbf3d75d1, 0xbf735e0f,  0},  //
   {0xbf325eca, 0xbf5c68c1,  0},  //
   {0xbf266d60, 0xbf46860e,  0},  //
   {0xbf19a193, 0xbf317e8e,  0},  //
   {0xbf0bfb63, 0xbf1d22c2,  0},  //
   {0xbefaf5a0, 0xbf0948a3,  0},  //
   {0xbedc3fb5, 0xbeeb937d,  0},  //
   {0xbebbd504, 0xbec5031e,  0},  //
   {0xbe99b58d, 0xbe9e9812,  0},  //
   {0xbe6bc2a0, 0xbe70104f,  0},  //
   {0xbe20b09b, 0xbe220759,  0},  //
   {0xbda46a15, 0xbda4c4db,  0},  //
   {0xb99fdd00, 0xb99fdd00,  0},  //
   {0x3da9fd72, 0x3daa61cc,  0},  //
   {0x3e2db6ec, 0x3e2f690d,  0},  //
   {0x3e84ec55, 0x3e880953,  0},  //
   {0x3eb4b1fa, 0x3ebcd02b,  0},  //
   {0x3ee62c65, 0x3ef7e1eb,  0},  //
   {0x3f0cadcb, 0x3f1e21cd,  0},  //
   {0x3f271fc6, 0x3f47bbff,  0},  //
   {0x3f426c24, 0x3f7eb4a7,  0},  //
   {0x3f5e92e5, 0x3faa5589,  0},  //
   {0x3f7b9409, 0x4017c7a0,  0},  //

   {0x40480000, 0x7fc00000,  0}, 

   //answer from NAG test tool
   {0x38d1b717, 0x38d1b717,  0}, // 0.0001, < 0x39000000
   {0x3e19999a, 0x3e1ac48f,  0}, // 0.15   < 0.5
   {0xbe19999a, 0xbe1ac48f,  0}, // -0.15, < 0.5
   {0x3f49fbe7, 0x3f88cdaf,  0}, // 0.789, > 0.5
   {0xbf49fbe7, 0xbf88cdaf,  0}, //-0.789, > 0.5
};

static libm_test_special_data_f64
test_atanh_conformance_data[] = {
    {0x0000000000000001LL, 0x0000000000000001LL, FE_UNDERFLOW}, // denormal min
    {0x0005fde623545abcLL, 0x0005fde623545abcLL, FE_UNDERFLOW}, // denormal intermediate
    {0x000FFFFFFFFFFFFFLL, 0x000fffffffffffffLL, FE_UNDERFLOW}, // denormal max
    {0x8000000000000001LL, 0x8000000000000001LL, FE_UNDERFLOW}, // -denormal min
    {0x8002344ade5def12LL, 0x8002344ade5def12LL, FE_UNDERFLOW}, // -denormal intermediate
    {0x800FFFFFFFFFFFFFLL, 0x800fffffffffffffLL, FE_UNDERFLOW}, // -denormal max
    {0x0010000000000000LL, 0x0010000000000000LL, FE_UNDERFLOW}, // normal min
    {0x43b3c4eafedcab02LL, 0xfff8000000000000LL, FE_INVALID}, // normal intermediate
    {0x7FEFFFFFFFFFFFFFLL, 0xfff8000000000000LL, FE_INVALID}, // normal max
    {0x8010000000000000LL, 0x8010000000000000LL, FE_UNDERFLOW}, // -normal min
    {0xc5812e71245acfdbLL, 0xfff8000000000000LL, FE_INVALID}, // -normal intermediate
    {0xFFEFFFFFFFFFFFFFLL, 0xfff8000000000000LL, FE_INVALID}, // -normal max
    {0x7FF0000000000000LL, 0xfff8000000000000LL, FE_INVALID}, // inf
    {0xFFF0000000000000LL, 0xfff8000000000000LL, FE_INVALID}, // -inf
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
    {0x3FF921FB54442D18LL, 0xfff8000000000000LL, FE_INVALID}, // pi/2
    {0x400921FB54442D18LL, 0xfff8000000000000LL, FE_INVALID}, // pi
    {0x401921FB54442D18LL, 0xfff8000000000000LL, FE_INVALID}, // 2pi
    {0x3FFB7E151628AED3LL, 0xfff8000000000000LL, FE_INVALID}, // e --
    {0x4005BF0A8B145769LL, 0xfff8000000000000LL, FE_INVALID}, // e
    {0x400DBF0A8B145769LL, 0xfff8000000000000LL, FE_INVALID}, // e ++
    {0x0000000000000000LL, 0x0000000000000000LL, 0}, // 0
    {0x3C4536B8B14B676CLL, 0x3c4536b8b14b676cLL, FE_UNDERFLOW}, // 0.0000000000000000023
    {0x3FDFFFFBCE4217D3LL, 0x3fe193e7aed93233LL, 0}, // 0.4999989999999999999
    {0x3FE000000000006CLL, 0x3fe193ea7aad039bLL, 0}, // 0.500000000000012
    {0x8000000000000000LL, 0x8000000000000000LL, 0}, // -0
    {0xBBDB2752CE74FF42LL, 0xbbdb2752ce74ff42LL, FE_UNDERFLOW}, // -0.000000000000000000023
    {0xBFDFFFFBCE4217D3LL, 0xbfe193e7aed93233LL, 0}, // -0.4999989999999999999
    {0xBFE000000000006CLL, 0xbfe193ea7aad039bLL, 0}, // -0.500000000000012
    {0x3FF0000000000000LL, 0x7ff0000000000000LL, 0}, // 1
    {0x3FEFFFFFC49BD0DCLL, 0x4020b5d40c9a355cLL, 0}, // 0.9999998893750006
    {0x3FF0000000000119LL, 0xfff8000000000000LL, FE_INVALID}, // 1.0000000000000624998
    {0x3FF7FFFEF39085F4LL, 0xfff8000000000000LL, FE_INVALID}, // 1.499998999999999967
    {0x3FF8000000000001LL, 0xfff8000000000000LL, FE_INVALID}, // 1.50000000000000012
    {0xBFF0000000000000LL, 0xfff0000000000000LL, 0}, // -1
    {0xBFEFFFFFC49BD0DCLL, 0xc020b5d40c9a355cLL, 0}, // -0.9999998893750006
    {0xBFF0000000000119LL, 0xfff8000000000000LL, FE_INVALID}, // -1.0000000000000624998
    {0xBFF7FFFEF39085F4LL, 0xfff8000000000000LL, FE_INVALID}, // -1.499998999999999967
    {0xBFF8000000000001LL, 0xfff8000000000000LL, FE_INVALID}, // -1.50000000000000012
    {0x4000000000000000LL, 0xfff8000000000000LL, FE_INVALID}, // 2
    {0xC000000000000000LL, 0xfff8000000000000LL, FE_INVALID}, // -2
    {0x4024000000000000LL, 0xfff8000000000000LL, FE_INVALID}, // 10
    {0xC024000000000000LL, 0xfff8000000000000LL, FE_INVALID}, // -10
    {0x408F400000000000LL, 0xfff8000000000000LL, FE_INVALID}, // 1000
    {0xC08F400000000000LL, 0xfff8000000000000LL, FE_INVALID}, // -1000
    {0x4050D9999999999ALL, 0xfff8000000000000LL, FE_INVALID}, // 67.4
    {0xC050D9999999999ALL, 0xfff8000000000000LL, FE_INVALID}, // -67.4
    {0x409EFE6666666666LL, 0xfff8000000000000LL, FE_INVALID}, // 1983.6
    {0xC09EFE6666666666LL, 0xfff8000000000000LL, FE_INVALID}, // -1983.6
    {0x4055E00000000000LL, 0xfff8000000000000LL, FE_INVALID}, // 87.5
    {0xC055E00000000000LL, 0xfff8000000000000LL, FE_INVALID}, // -87.5
    {0xC055E00000000000LL, 0xfff8000000000000LL, FE_INVALID}, // -87.5
    {0x41002BC800000000LL, 0xfff8000000000000LL, FE_INVALID}, // 132473
    {0xC1002BC800000000LL, 0xfff8000000000000LL, FE_INVALID}, // -132473
    {0x4330000000000000LL, 0xfff8000000000000LL, FE_INVALID}, // 2^52
    {0x4330000000000001LL, 0xfff8000000000000LL, FE_INVALID}, // 2^52 + 1
    {0x432FFFFFFFFFFFFFLL, 0xfff8000000000000LL, FE_INVALID}, // 2^52 -1 + 0.5
    {0xC330000000000000LL, 0xfff8000000000000LL, FE_INVALID}, // -2^52
    {0xC330000000000001LL, 0xfff8000000000000LL, FE_INVALID}, // -(2^52 + 1)
    {0xC32FFFFFFFFFFFFFLL, 0xfff8000000000000LL, FE_INVALID}, // -(2^52 -1 + 0.5)
    // special accuracy tests
    {0x3ff0000000000000LL, 0x7ff0000000000000LL, 0},  //
    {0x3ff0000000000001LL, 0xfff8000000000000LL, FE_INVALID},  //
    {0x3fefffffffffffffLL, 0x4032b708872320e2LL, 0},  //
    {0xbff0000000000000LL, 0xfff0000000000000LL, 0},  //
    {0xbff0000000000001LL, 0xfff8000000000000LL, FE_INVALID},  //
    {0xbfefffffffffffffLL, 0xc032b708872320e2LL, 0},  //
    {0x3e30000000000000LL, 0x3e30000000000000LL, 0},  //
    {0x3e30000000000001LL, 0x3e30000000000001LL, 0},  //
    {0x3e2fffffffffffffLL, 0x3e2fffffffffffffLL, FE_UNDERFLOW},  //
    {0xbe30000000000000LL, 0xbe30000000000000LL, 0},  //
    {0xbe30000000000001LL, 0xbe30000000000001LL, 0},  //
    {0xbe2fffffffffffffLL, 0xbe2fffffffffffffLL, FE_UNDERFLOW},  //
    {0x0000000000000000LL, 0x0000000000000000LL, 0},  //
    {0x8000000000000000LL, 0x8000000000000000LL, 0},  //
    {0x3fe0000000000000LL, 0x3fe193ea7aad030bLL, 0},  //
    {0x3fe0000000000001LL, 0x3fe193ea7aad030cLL, 0},  //
    {0x3fdfffffffffffffLL, 0x3fe193ea7aad030aLL, 0},  //
    {0xbff0000000000000LL, 0xfff0000000000000LL, 0},  //
    {0xbfefe4b385b43bf8LL, 0xc009950bae141397LL, 0},  //
    {0xbfefae1aaddd508cLL, 0xc0052ca5fd844a84LL, 0},  //
    {0xbfef5c35787b3dbcLL, 0xc00261b61745d642LL, 0},  //
    {0xbfeeef03e58e0387LL, 0xc0004fb5eeb96fcaLL, 0},  //
    {0xbfee6685f515a1eeLL, 0xbffd4f9b43c21dd4LL, 0},  //
    {0xbfedc2bba71218f1LL, 0xbffa8967ec350c46LL, 0},  //
    {0xbfed03a4fb836890LL, 0xbff82354d0eee2c8LL, 0},  //
    {0xbfec2941f26990caLL, 0xbff603cdd2fa6bbaLL, 0},  //
    {0xbfeb33928bc491a0LL, 0xbff419df826b4815LL, 0},  //
    {0xbfea2296c7946b12LL, 0xbff259acd2280a69LL, 0},  //
    {0xbfe8f64ea5d91d20LL, 0xbff0ba895ba76ff1LL, 0},  //
    {0xbfe7aeba2692a7c9LL, 0xbfee6bc1fc3b0087LL, 0},  //
    {0xbfe64bd949c10b0eLL, 0xbfeb8d18372f4964LL, 0},  //
    {0xbfe4cdac0f6446efLL, 0xbfe8d0c1e05308b2LL, 0},  //
    {0xbfe33432777c5b6cLL, 0xbfe62fd1e5540763LL, 0},  //
    {0xbfe17f6c82094884LL, 0xbfe3a45863232f29LL, 0},  //
    {0xbfdf5eb45e161c70LL, 0xbfe12914a9f90be6LL, 0},  //
    {0xbfdb87f6fd035910LL, 0xbfdd72700ae83fc6LL, 0},  //
    {0xbfd77aa0e0da46e7LL, 0xbfd8a0643466b05eLL, 0},  //
    {0xbfd336b2099ae5f6LL, 0xbfd3d302b89a6a0aLL, 0},  //
    {0xbfcd7854ee8a6c78LL, 0xbfce020ad3ea9ef3LL, 0},  //
    {0xbfc4161453b26f74LL, 0xbfc440ec18d83b51LL, 0},  //
    {0xbfb48d44855ba9bdLL, 0xbfb4989d4e3e2d21LL, 0},  //
    {0xbf33fd76f9397000LL, 0xbf33fd7703a0251aLL, 0},  //
    {0x3fb53fac83c271ffLL, 0x3fb54c37cac5c0a4LL, 0},  //
    {0x3fc5b6dcb56bac48LL, 0x3fc5ed20c459ce56LL, 0},  //
    {0x3fd09d8a4f915e91LL, 0x3fd1012a164150d6LL, 0},  //
    {0x3fd6963eff8335c6LL, 0x3fd79a050bcb2495LL, 0},  //
    {0x3fdcc58c6a8b5bc4LL, 0x3fdefc3d24ee8548LL, 0},  //
    {0x3fe195b94854e845LL, 0x3fe3c4397629aafdLL, 0},  //
    {0x3fe4e3f8b8ef4a0cLL, 0x3fe8f77fd9370e4aLL, 0},  //
    {0x3fe84d848714d338LL, 0x3fefd694f58ed670LL, 0},  //
    {0x3febd25cb2c583c8LL, 0x3ff54ab142b7c799LL, 0},  //
    {0x3fef72813c015bbcLL, 0x4002f8f4c3896197LL, 0},  //
};

