/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>
/*
 * Test cases to check for exceptions for the asinhf() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_asinhf_conformance_data[] = {
   {0x00000001, 0x00000001,  0},  // denormal min
   {0x0005fde6, 0x0005fde6,  0},  // denormal intermediate
   {0x007fffff, 0x007fffff,  0},  // denormal max
   {0x80000001, 0x80000001,  0},  // -denormal min
   {0x805def12, 0x805def12,  0},  // -denormal intermediate
   {0x807FFFFF, 0x807fffff,  0},  // -denormal max
   {0x00800000, 0x00800000,  0},  // normal min
   {0x43b3c4ea, 0x40d27ebb,  0},  // normal intermediate
   {0x7f7fffff, 0x42b2d4fc,  0},  // normal max
   {0x80800000, 0x80800000,  0},  // -normal min
   {0xc5812e71, 0xc1105255,  0},  // -normal intermediate
   {0xFF7FFFFF, 0xc2b2d4fc,  0},  // -normal max
   {0x7F800000, 0x7f800000,  0},  // inf
   {0xfF800000, 0xff800000,  0},  // -inf
   {0x7Fc00000, 0x7fc00000,  0},  // qnan min
   {0x7Fe1a570, 0x7fe1a570,  0},  // qnan intermediate
   {0x7FFFFFFF, 0x7fffffff,  0},  // qnan max
   {0xfFc00000, 0xffc00000,  0},  // indeterninate
   {0xFFC00001, 0xffc00000,  0},  // -qnan min
   {0xFFd2ba31, 0xffc00000,  0},  // -qnan intermediate
   {0xFFFFFFFF, 0xffc00000,  0},  // -qnan max
   {0x7F800001, 0x7fc00001,  FE_INVALID},  // snan min
   {0x7Fa0bd90, 0x7fe0bd90,  FE_INVALID},  // snan intermediate
   {0x7FBFFFFF, 0x7fffffff,  FE_INVALID},  // snan max
   {0xFF800001, 0xffc00000,  FE_INVALID},  // -snan min
   {0xFF95FFFA, 0xffc00000,  FE_INVALID},  // -snan intermediate
   {0xFFBFFFFF, 0xffc00000,  FE_INVALID},  // -snan max
   {0x3FC90FDB, 0x3f9de027,  0},  // pi/2
   {0x40490FDB, 0x3fee5fb5,  0},  // pi
   {0x40C90FDB, 0x40226315,  0},  // 2pi
   {0x402DF853, 0x3fdcd955,  0},  // e --
   {0x402DF854, 0x3fdcd956,  0},  // e
   {0x402DF855, 0x3fdcd956,  0},  // e ++
   {0x00000000, 0x00000000,  0},  // 0
   {0x37C0F01F, 0x37c0f01f,  0},  // 0.000023
   {0x3EFFFEB0, 0x3ef66039,  0},  // 0.49999
   {0x3F0000C9, 0x3ef662cd,  0},  // 0.500012
   {0x80000000, 0x80000000,  0},  // -0
   {0xb7C0F01F, 0xb7c0f01f,  0},  // -0.000023
   {0xbEFFFEB0, 0xbef66039,  0},  // -0.49999
   {0xbF0000C9, 0xbef662cd,  0},  // -0.500012
   {0x3f800000, 0x3f61a1b3,  0},  // 1
   {0x3f700001, 0x3f5623ae,  0},  // 0.93750006
   {0x3F87FFFE, 0x3f6cc53a,  0},  // 1.0624998
   {0x3FBFFFAC, 0x3f98edd2,  0},  // 1.49999
   {0x3FC00064, 0x3f98ee38,  0},  // 1.500012
   {0xbf800000, 0xbf61a1b3,  0},  // -1
   {0xbf700001, 0xbf5623ae,  0},  // -0.93750006
   {0xbF87FFFE, 0xbf6cc53a,  0},  // -1.0624998
   {0xbFBFFFAC, 0xbf98edd2,  0},  // -1.49999
   {0xbFC00064, 0xbf98ee38,  0},  // -1.500012
   {0x40000000, 0x3fb8c90c,  0},  // 2
   {0xc0000000, 0xbfb8c90c,  0},  // -2
   {0x41200000, 0x403fe2e3,  0},  // 10
   {0xc1200000, 0xc03fe2e3,  0},  // -10
   {0x447A0000, 0x40f33a98,  0},  // 1000
   {0xc47A0000, 0xc0f33a98,  0},  // -1000
   {0x4286CCCC, 0x409cec51,  0},  // 67.4
   {0xc286CCCC, 0xc09cec51,  0},  // -67.4
   {0x44F7F333, 0x410492b4,  0},  // 1983.6
   {0xc4F7F333, 0xc10492b4,  0},  // -1983.6
   {0x42AF0000, 0x40a54632,  0},  // 87.5
   {0xc2AF0000, 0xc0a54632,  0},  // -87.5
   {0x48015E40, 0x4147cbe7,  0},  // 132473
   {0xc8015E40, 0xc147cbe7,  0},  // -132473
   {0x4B000000, 0x41851592,  0},  // 2^23
   {0x4B000001, 0x41851592,  0},  // 2^23 + 1
   {0x4AFFFFFF, 0x41851592,  0},  // 2^23 -1 + 0.5
   {0xcB000000, 0xc1851592,  0},  // -2^23
   {0xcB000001, 0xc1851592,  0},  // -(2^23 + 1)
   {0xcAFFFFFF, 0xc1851592,  0},  // -(2^23 -1 + 0.5)

    // special accuracy tests
   {0x38800000, 0x38800000,  0}, //
   {0x387FFFFF, 0x387fffff,  0}, //
   {0x38800001, 0x38800001,  0}, //
   {0x7f7fffec, 0x42b2d4fc,  0}, //
   {0x7f7fff6c, 0x42b2d4fb,  0}, //
   {0x42B2D4FD, 0x40a5f7a1,  0}, //
   {0x42B2D4FF, 0x40a5f7a1,  0}, //
   {0x7f7f820b, 0x42b2d400,  0}, //
   {0x4d675844, 0x41a00000,  0}, //
   {0x4e1d3710, 0x41a80000,  0}, //
   {0x4caa36c8, 0x41980000,  0}, //
   {0x46000000, 0x411b43d5,  0}, //
   {0x46000001, 0x411b43d5,  0}, //
   {0x45ffffff, 0x411b43d5,  0}, //
   {0x3F317217, 0x3f25a4a3,  0}, //
   {0x40000000, 0x3fb8c90c,  0}, //
   {0x40000001, 0x3fb8c90d,  0}, //
   {0x3fffffff, 0x3fb8c90c,  0}, //
   {0x3f800000, 0x3f61a1b3,  0}, //
   {0x40584817, 0x3ff74d6d,  0}, //
   {0x40d84817, 0x402700c0,  0}, //
   {0x41323612, 0x4046c11e,  0}, //
   {0x41f6e585, 0x4083ef1b,  0}, //
   {0x4283dc5f, 0x409c37be,  0}, //
   {0x42e8afbb, 0x40ae6407,  0}, //
   {0x43859b33, 0x40c8ff8c,  0}, //
   {0x4401a272, 0x40de3693,  0}, //
   {0x44599481, 0x40eec8c7,  0}, //
   {0x44a551e3, 0x40fc2cd2,  0}, //
   {0x48490495, 0x414ed924,  0}, //
   {0x511bd2ce, 0x41c9332f,  0}, //
   {0x62bdb16c, 0x42466d64,  0}, //
   {0x80000000, 0x80000000,  0},
   {0xff800000, 0xff800000,  0},
   {0x7f800001, 0x7fc00001,  FE_INVALID},
   {0xff800001, 0xffc00001,  FE_INVALID},
   {0x7fc00000, 0x7fc00000,  0},
   {0xffc00000, 0xffc00000,  0},
   //answer from NAG test tool
   {0x460ca000, 0x411cc521,  0}, // 9000, > 0x46000000
   {0x3fc00000, 0x3f98ee00,  0}, // 1.5
   {0x3f9e0419, 0x3f84d90a,  0}, // 1.2345

};

static libm_test_special_data_f64
test_asinh_conformance_data[] = {
    #if defined(_WIN64) || defined(_WIN32)
        {0x0000000000000001LL, 0x0000000000000001LL, 3}, // denormal min
        {0x0005fde623545abcLL, 0x0005fde623545abcLL, 3}, // denormal intermediate
        {0x8000000000000001LL, 0x8000000000000001LL, 3}, // -denormal min
        {0x8002344ade5def12LL, 0x8002344ade5def12LL, 3}, // -denormal intermediate
        {0x800FFFFFFFFFFFFFLL, 0x800fffffffffffffLL, 3}, // -denormal max
        {0x0010000000000000LL, 0x0010000000000000LL, 3}, // normal min
        {0x8010000000000000LL, 0x8010000000000000LL, 3}, // -normal min
        {0xc5812e71245acfdbLL, 0xc04f3a2c96b94100LL, 1}, // -normal intermediate
        {0xFFEFFFFFFFFFFFFFLL, 0xc08633ce8fb9f87eLL, 1}, // -normal max
        {0x3C4536B8B14B676CLL, 0x3c4536b8b14b676cLL, 1}, // 0.0000000000000000023
        {0x3FDFFFFBCE4217D3LL, 0x3fdecc28ee62c343LL, 1}, // 0.4999989999999999999
        {0x3FE000000000006CLL, 0x3fdecc2caec516cbLL, 1}, // 0.500000000000012
        {0xBBDB2752CE74FF42LL, 0xbbdb2752ce74ff42LL, 1}, // -0.000000000000000000023
        {0xBFDFFFFBCE4217D3LL, 0xbfdecc28ee62c343LL, 1}, // -0.4999989999999999999
        {0xBFE000000000006CLL, 0xbfdecc2caec516cbLL, 1}, // -0.500000000000012
        {0x3FEFFFFFC49BD0DCLL, 0x3fec3436377ad8c2LL, 1}, // 0.9999998893750006
        {0xBFF0000000000000LL, 0xbfec34366179d427LL, 1}, // -1
        {0xBFEFFFFFC49BD0DCLL, 0xbfec3436377ad8c2LL, 1}, // -0.9999998893750006
        {0xBFF0000000000119LL, 0xbfec34366179d5b4LL, 1}, // -1.0000000000000624998
        {0xBFF7FFFEF39085F4LL, 0xbff31dbf7424ac8bLL, 1}, // -1.499998999999999967
        {0xBFF8000000000001LL, 0xbff31dc0090b63d9LL, 1}, // -1.50000000000000012
        {0xC000000000000000LL, 0xbff719218313d087LL, 1}, // -2
        {0xC024000000000000LL, 0xc007fc5c506d2bdbLL, 1}, // -10
        {0xC08F400000000000LL, 0xc01e67530a363e15LL, 1}, // -1000
        {0xC050D9999999999ALL, 0xc0139d8a209a9bcaLL, 1}, // -67.4
        {0xC09EFE6666666666LL, 0xc020925676234ba5LL, 1}, // -1983.6
        {0xC055E00000000000LL, 0xc014a8c636b735bfLL, 1}, // -87.5
        {0xC1002BC800000000LL, 0xc028f97cef9a3debLL, 1}, // -132473
        {0xC330000000000000LL, 0xc0425e4f7b2737faLL, 1}, // -2^52
        {0xC330000000000001LL, 0xc0425e4f7b2737faLL, 1}, // -(2^52 + 1)
        {0xC32FFFFFFFFFFFFFLL, 0xc0425e4f7b2737faLL, 1}, // -(2^52 -1 + 0.5)
        {0x3e30000000000000LL, 0x3e30000000000000LL, 1}, //
        {0x3E2FFFFFFFFFFFFFLL, 0x3e2fffffffffffffLL, 1}, //
        {0x3e30000000000001LL, 0x3e30000000000001LL, 1}, //
        {0xFE37E43C8800759CLL, 0xc0859bbfd8b83e44LL, 1}, //
        {0xFE37E43C8800758CLL, 0xc0859bbfd8b83e44LL, 1}, //
        {0xFE37E43C880075ACLL, 0xc0859bbfd8b83e44LL, 1}, //
        {0x3fe62e42e0000000LL, 0x3fe4b4946e555844LL, 1}, //
        {0x3e6efa39ef35793cLL, 0x3e6efa39ef357937LL, 1}, //
        {0x3FEFFFFFFFFFFFF7LL, 0x3fec34366179d420LL, 1}, //
        {0xc196a09e667f3bcdLL, 0xc0330fc1931f09caLL, 1}, //
    #else
        {0x0000000000000001LL, 0x0000000000000001LL, 48}, // denormal min
        {0x0005fde623545abcLL, 0x0005fde623545abcLL, 48}, // denormal intermediate
        {0x8000000000000001LL, 0x8000000000000001LL, 48}, // -denormal min
        {0x8002344ade5def12LL, 0x8002344ade5def12LL, 48}, // -denormal intermediate
        {0x800FFFFFFFFFFFFFLL, 0x800fffffffffffffLL, 48}, // -denormal max
        {0x0010000000000000LL, 0x0010000000000000LL, 32}, // normal min
        {0x8010000000000000LL, 0x8010000000000000LL, 32}, // -normal min
        {0xc5812e71245acfdbLL, 0xc04f3a2c96b94100LL, 32}, // -normal intermediate
        {0xFFEFFFFFFFFFFFFFLL, 0xc08633ce8fb9f87eLL, 32}, // -normal max
        {0x3C4536B8B14B676CLL, 0x3c4536b8b14b676cLL, 32}, // 0.0000000000000000023
        {0x3FDFFFFBCE4217D3LL, 0x3fdecc28ee62c343LL, 32}, // 0.4999989999999999999
        {0x3FE000000000006CLL, 0x3fdecc2caec516cbLL, 32}, // 0.500000000000012
        {0xBBDB2752CE74FF42LL, 0xbbdb2752ce74ff42LL, 32}, // -0.000000000000000000023
        {0xBFDFFFFBCE4217D3LL, 0xbfdecc28ee62c343LL, 32}, // -0.4999989999999999999
        {0xBFE000000000006CLL, 0xbfdecc2caec516cbLL, 32}, // -0.500000000000012
        {0x3FEFFFFFC49BD0DCLL, 0x3fec3436377ad8c2LL, 32}, // 0.9999998893750006
        {0xBFF0000000000000LL, 0xbfec34366179d427LL, 32}, // -1
        {0xBFEFFFFFC49BD0DCLL, 0xbfec3436377ad8c2LL, 32}, // -0.9999998893750006
        {0xBFF0000000000119LL, 0xbfec34366179d5b4LL, 32}, // -1.0000000000000624998
        {0xBFF7FFFEF39085F4LL, 0xbff31dbf7424ac8bLL, 32}, // -1.499998999999999967
        {0xBFF8000000000001LL, 0xbff31dc0090b63d9LL, 32}, // -1.50000000000000012
        {0xC000000000000000LL, 0xbff719218313d087LL, 32}, // -2
        {0xC024000000000000LL, 0xc007fc5c506d2bdbLL, 32}, // -10
        {0xC08F400000000000LL, 0xc01e67530a363e15LL, 32}, // -1000
        {0xC050D9999999999ALL, 0xc0139d8a209a9bcaLL, 32}, // -67.4
        {0xC09EFE6666666666LL, 0xc020925676234ba5LL, 32}, // -1983.6
        {0xC055E00000000000LL, 0xc014a8c636b735bfLL, 32}, // -87.5
        {0xC1002BC800000000LL, 0xc028f97cef9a3debLL, 32}, // -132473
        {0xC330000000000000LL, 0xc0425e4f7b2737faLL, 32}, // -2^52
        {0xC330000000000001LL, 0xc0425e4f7b2737faLL, 32}, // -(2^52 + 1)
        {0xC32FFFFFFFFFFFFFLL, 0xc0425e4f7b2737faLL, 32}, // -(2^52 -1 + 0.5)
        {0x3e30000000000000LL, 0x3e30000000000000LL, 32}, //
        {0x3E2FFFFFFFFFFFFFLL, 0x3e2fffffffffffffLL, 32}, //
        {0x3e30000000000001LL, 0x3e30000000000001LL, 32}, //
        {0xFE37E43C8800759CLL, 0xc0859bbfd8b83e44LL, 32}, //
        {0xFE37E43C8800758CLL, 0xc0859bbfd8b83e44LL, 32}, //
        {0xFE37E43C880075ACLL, 0xc0859bbfd8b83e44LL, 32}, //
        {0x3fe62e42e0000000LL, 0x3fe4b4946e555844LL, 32}, //
        {0x3e6efa39ef35793cLL, 0x3e6efa39ef357937LL, 32}, //
        {0x3FEFFFFFFFFFFFF7LL, 0x3fec34366179d420LL, 32}, //
        {0xc196a09e667f3bcdLL, 0xc0330fc1931f09caLL, 32}, //
    #endif

    {0x000FFFFFFFFFFFFFLL, 0x000fffffffffffffLL, 0}, // denormal max
    {0x43b3c4eafedcab02LL, 0x40453f2b7feeadccLL, 0}, // normal intermediate
    {0x7FEFFFFFFFFFFFFFLL, 0x408633ce8fb9f87eLL, 0}, // normal max
    {0x7FF0000000000000LL, 0x7ff0000000000000LL, 0}, // inf
    {0xFFF0000000000000LL, 0xfff0000000000000LL, 0}, // -inf
    {0x7FF8000000000000LL, 0x7ff8000000000000LL, 0}, // qnan min
    {0x7FFe1a5701234dc3LL, 0x7ffe1a5701234dc3LL, 0}, // qnan intermediate
    {0x7FFFFFFFFFFFFFFFLL, 0x7fffffffffffffffLL, 0}, // qnan max
    {0xFFF8000000000000LL, 0xfff8000000000000LL, 0}, // indeterninate
    {0xFFF8000000000001LL, 0xfff8000000000000LL, 0}, // -qnan min
    {0xFFF9123425dcba31LL, 0xfff8000000000000LL, 0}, // -qnan intermediate
    {0xFFFFFFFFFFFFFFFFLL, 0xfff8000000000000LL, 0}, // -qnan max
    {0x7FF0000000000001LL, 0x7ff8000000000001LL, FE_INVALID}, // snan min
    {0x7FF5344752a0bd90LL, 0x7ffd344752a0bd90LL, FE_INVALID}, // snan intermediate
    {0x7FF7FFFFFFFFFFFFLL, 0x7fffffffffffffffLL, FE_INVALID}, // snan max
    {0xFFF0000000000001LL, 0xfff8000000000000LL, FE_INVALID}, // -snan min
    {0xfFF432438995fffaLL, 0xfff8000000000000LL, FE_INVALID}, // -snan intermediate
    {0xFFF7FFFFFFFFFFFFLL, 0xfff8000000000000LL, FE_INVALID}, // -snan max
    {0x3FF921FB54442D18LL, 0x3ff3bc04e847ec05LL, 0}, // pi/2
    {0x400921FB54442D18LL, 0x3ffdcbf69f10006dLL, 0}, // pi
    {0x401921FB54442D18LL, 0x40044c62a1e4f804LL, 0}, // 2pi
    {0x3FFB7E151628AED3LL, 0x3ff4f5f9f10fb81fLL, 0}, // e --
    {0x4005BF0A8B145769LL, 0x3ffb9b2abdf5c077LL, 0}, // e
    {0x400DBF0A8B145769LL, 0x40003131303a2543LL, 0}, // e ++
    {0x0000000000000000LL, 0x0000000000000000LL, 0}, // 0
    {0x8000000000000000LL, 0x8000000000000000LL, 0}, // -0
    {0x3FF0000000000000LL, 0x3fec34366179d427LL, 0}, // 1
    {0x3FF0000000000119LL, 0x3fec34366179d5b4LL, 0}, // 1.0000000000000624998
    {0x3FF7FFFEF39085F4LL, 0x3ff31dbf7424ac8bLL, 0}, // 1.499998999999999967
    {0x3FF8000000000001LL, 0x3ff31dc0090b63d9LL, 0}, // 1.50000000000000012
    {0x4000000000000000LL, 0x3ff719218313d087LL, 0}, // 2
    {0x4024000000000000LL, 0x4007fc5c506d2bdbLL, 0}, // 10
    {0x408F400000000000LL, 0x401e67530a363e15LL, 0}, // 1000
    {0x4050D9999999999ALL, 0x40139d8a209a9bcaLL, 0}, // 67.4
    {0x409EFE6666666666LL, 0x4020925676234ba5LL, 0}, // 1983.6
    {0x4055E00000000000LL, 0x4014a8c636b735bfLL, 0}, // 87.5
    {0x41002BC800000000LL, 0x4028f97cef9a3debLL, 0}, // 132473
    {0x4330000000000000LL, 0x40425e4f7b2737faLL, 0}, // 2^52
    {0x4330000000000001LL, 0x40425e4f7b2737faLL, 0}, // 2^52 + 1
    {0x432FFFFFFFFFFFFFLL, 0x40425e4f7b2737faLL, 0}, // 2^52 -1 + 0.5
    // special accuracy tests
    {0x408633ce8fb9f87eLL, 0x401d094cf61c34cdLL, 0}, //
    {0x7feffffffffffd3bLL, 0x408633ce8fb9f87dLL, 0}, //
    {0x408633ce8fb9f87fLL, 0x401d094cf61c34ceLL, 0}, //
    {0x408633ce8fb9f8ffLL, 0x401d094cf61c34e5LL, 0}, //
    {0x7feffffffffe093bLL, 0x408633ce8fb9f800LL, 0}, //
    {0x41aceb088b68e804LL, 0x4034000000000000LL, 0}, //
    {0x41c3a6e1fd9eecfdLL, 0x4035000000000000LL, 0}, //
    {0x419546d8f9ed26e2LL, 0x4033000000000000LL, 0}, //
    {0x4196a09e667f3bcdLL, 0x40330fc1931f09caLL, 0}, //
    {0x4196a09e667f3bccLL, 0x40330fc1931f09caLL, 0}, //
    {0x4196a09e667f3bceLL, 0x40330fc1931f09caLL, 0}, //
    {0x3FF0000000000000LL, 0x3fec34366179d427LL, 0}, //
    {0x3FF0000000000001LL, 0x3fec34366179d428LL, 0}, //
    {0x4060000000000000LL, 0x40162e46fef439fdLL, 0}, //
    {0x4060000000000001LL, 0x40162e46fef439fdLL, 0}, //
    {0x405fffffffffffffLL, 0x40162e46fef439fcLL, 0}, //
    {0x3ff8000000000000LL, 0x3ff31dc0090b63d8LL, 0}, //
    {0x3ff8000000000001LL, 0x3ff31dc0090b63d9LL, 0}, //
    {0x3ff7ffffffffffffLL, 0x3ff31dc0090b63d8LL, 0}, //
    {0x4040000000000000LL, 0x4010a2f2393c80a6LL, 0}, //
    {0x4040000000000001LL, 0x4010a2f2393c80a6LL, 0}, //
    {0x403fffffffffffffLL, 0x4010a2f2393c80a6LL, 0}, //
    {0x4020000000000000LL, 0x40063637195fabd3LL, 0}, //
    {0x4020000000000001LL, 0x40063637195fabd4LL, 0}, //
    {0x401fffffffffffffLL, 0x40063637195fabd3LL, 0}, //
    {0x4010000000000000LL, 0x4000c1f8a6e80eebLL, 0}, //
    {0x4010000000000001LL, 0x4000c1f8a6e80eecLL, 0}, //
    {0x400fffffffffffffLL, 0x4000c1f8a6e80eebLL, 0}, //
    {0x4000000000000000LL, 0x3ff719218313d087LL, 0}, //
    {0x4000000000000001LL, 0x3ff719218313d088LL, 0}, //
    {0x3FFFFFFFFFFFFFFBLL, 0x3ff719218313d085LL, 0}, //
    {0x3ffc000000000000LL, 0x3ff536e093aae087LL, 0}, //
    {0x3ffc000000000001LL, 0x3ff536e093aae088LL, 0}, //
    {0x3ffbffffffffffffLL, 0x3ff536e093aae087LL, 0}, //
    {0x3FF8000000000000LL, 0x3ff31dc0090b63d8LL, 0}, //
    {0x3FFC000000000000LL, 0x3ff536e093aae087LL, 0}, //
    {0x3FFAAA88C50B0C79LL, 0x3ff48a5be181ca42LL, 0}, //
    {0x3FF1FA2163FDD65ALL, 0x3feeea59c5473687LL, 0}, //
    {0x3FF2147AE147AE14LL, 0x3fef0d54c2a99e3aLL, 0}, //
    {0x3FF0F5C28F5C28F6LL, 0x3fed8a9c025db5e0LL, 0}, //
    {0x3FF0DAD538AC18F8LL, 0x3fed6597c4c174bcLL, 0}, //
    {0x3FF17961804D9839LL, 0x3fee3dcfaa55f987LL, 0}, //
    {0x3FF3C47C30D306A3LL, 0x3ff09da15161b3bdLL, 0}, //
    {0x3ff0000000000000LL, 0x3fec34366179d427LL, 0}, //
    {0x400b0902de00d1b7LL, 0x3ffee9adaa8f28d6LL, 0}, //
    {0x401b0902de00d1b7LL, 0x4004e01808eb473fLL, 0}, //
    {0x402646c226809d4aLL, 0x4008d823bda00e39LL, 0}, //
    {0x403edcb09e98dcdbLL, 0x40107de3524efd1cLL, 0}, //
    {0x40507b8bd66277c4LL, 0x401386f7c0f90b98LL, 0}, //
    {0x405d15f75104d552LL, 0x4015cc80d907df24LL, 0}, //
    {0x4070b36657b84dbaLL, 0x40191ff18b0acee6LL, 0}, //
    {0x4080344e3ffef391LL, 0x401bc6d25b7e38ccLL, 0}, //
    {0x408b3290268900c6LL, 0x401dd918dd6477e8LL, 0}, //
    {0x4094aa3c6fbd273eLL, 0x401f859a383bbe00LL, 0}, //
    {0x41092092ba93c86bLL, 0x4029db248521f309LL, 0}, //
    {0x42237a59f4429268LL, 0x40392665e1572e41LL, 0}, //
    {0x4457b62df67fc4fdLL, 0x4048cdac82cb51b7LL, 0}, //
    {0x48c191ef3d6a018aLL, 0x4058a14ffccd501eLL, 0}, //
    {0x51934b4f70b3a04cLL, 0x40688b21b9ce55e4LL, 0}, //
    {0x6337443f25f59cbbLL, 0x4078800a984ed8c7LL, 0}, //

};
