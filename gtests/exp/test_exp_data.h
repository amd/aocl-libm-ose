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

#ifndef __TEST_EXP_DATA_H__
#define __TEST_EXP_DATA_H__

#include <fenv.h>
#include "almstruct.h"

/*
 * Test cases to check for exceptions for the expf() routine.
 * These test cases are not exhaustive
 */
static libm_test_special_data_f32
test_expf_conformance_data[] = {
    #if defined(_WIN64) || defined(_WIN32)
        {0xc2ce8ed1, 0x1,        3},          //expf (-103.278938)=-103.278938
        {0xc435f37e, 0x00000000, 3},      //denormal
        {0x447a0000, 0x7f800000, 5},      //1000
        {0xc42f0000, 0x00000000, 3},      //-700
        {0x44317218, 0x7F800000, 5},//smallest no for result infinity
        {0x7f7fff84, 0x7f800000, 5}, // expf(8.872284e+001 = log(maximum) = 1.701415e+038
        {0xc47a0000, 0x0        , 3}, // exp(-1000.000000000000)=  0
        {0xc431195f, 0x0        , 3}, // exp(-708.396418532264190) ; first underflow
        {0x447A0000, 0x7F800000, 5},  //1000
        {0x42c00000, 0x7F800000, 5},  //96
        {0xc2AF0000, 0x006CB2BC, 3},  //-87.5
        {0xc2e00000, 0x00000000, 3},  //-112
        {0xc3000000, 0x00000000, 3},  //-128
        {0xc2Aeac50, 0x007FFFE6, 3},  //-87.3365  largest denormal result
        {0xc2ce0000, 0x1, 3},  //
        {0x42b17218, 0x7F800000, 5},  //88.7228   overflow
        {0x50000000, 0x7F800000, 5},  //large   overflow
        {0xff7fffff, 0x00000000, 3},  //smallest number
        {0x7f7fffff, 0x7F800000, 5},   //largest number
        {0xc6de66b6, 0x00000000, 3}, // -28467.3555
        {0x46de66b6, 0x7f800000, 5}, // 28467.3555
        {0xc2c80000, 0x0000001b, 3}, // -100
        {0x42c80000, 0x7f800000, 5}, // 100
    #else
        {0xc2ce8ed1, 0x1,        48},          //expf (-103.278938)=-103.278938
        {0xc435f37e, 0x00000000, 48},      //denormal
        {0x447a0000, 0x7f800000, 40},      //1000
        {0xc42f0000, 0x00000000, 48},      //-700
        {0x44317218, 0x7F800000, 40},//smallest no for result infinity
        {0x7f7fff84, 0x7f800000, 40}, // expf(8.872284e+001 = log(maximum) = 1.701415e+038
        {0xc47a0000, 0x0        , 48}, // exp(-1000.000000000000)=  0
        {0xc431195f, 0x0        , 48}, // exp(-708.396418532264190) ; first underflow
        {0x447A0000, 0x7F800000, 40},  //1000
        {0x42c00000, 0x7F800000, 40},  //96
        {0xc2AF0000, 0x006CB2BC, 48},  //-87.5
        {0xc2e00000, 0x00000000, 48},  //-112
        {0xc3000000, 0x00000000, 48},  //-128
        {0xc2Aeac50, 0x007FFFE6, 48},  //-87.3365  largest denormal result
        {0xc2ce0000, 0x1, 48},  //
        {0x42b17218, 0x7F800000, 40},  //88.7228   overflow
        {0x50000000, 0x7F800000, 40},  //large   overflow
        {0xff7fffff, 0x00000000, 48},  //smallest number
        {0x7f7fffff, 0x7F800000, 40},   //largest number
        {0xc6de66b6, 0x00000000, 48}, // -28467.3555
        {0x46de66b6, 0x7f800000, 40}, // 28467.3555
        {0xc2c80000, 0x0000001b, 48}, // -100
        {0x42c80000, 0x7f800000, 40}, // 100
    #endif
    {0x7fbfffff, 0x0,        FE_INVALID,}, //expf(nan)=nan
    {0xffffffff, 0xffffffff, 0},   //expf(-nan)=-nan
    {0x7fe00000, 0x7fe00000, 0},  //expf(qnan) = qnan
    {0xffe00000, 0xffe00000, 0},  //expf(-qnan) = -qnan
    {0x00000000, 0x3F800000, 0},   //exp(0)=1
    {0x80000000, 0x3F800000, 0},      //exp(-0)
    {0x3F800000, 0x402df854, FE_INEXACT}, //exp(1)  = E
    {0x7F800000, 0x7F800000, 0},      //inf
    {0xFF800000, 0x00000000, 0},      //-inf
    {0xBF800000, 0x3ebc5ab2, FE_INEXACT,}, //-1
    {0x40490fdb, 0x41b92025, FE_INEXACT,}, //pi
    //newly added
    {0x34000000, 0x3f800001, FE_INEXACT}, // expf(1.192093e-007 = fullprec) =1.000000e+000
    {0xbf956fe8, 0x3e9f4f28 , FE_INEXACT}, // exp(-1.167477607727)=     0.311150796620
    {0x3fb5e107, 0x408482d1 , FE_INEXACT}, // exp(1.420929789543)=      4.140968880418
    {0x3f00a169, 0x3fd38e86 , FE_INEXACT}, // exp(0.502462923527)=      1.652786949780
    {0xc0883971, 0x3c68127d , FE_INEXACT}, // exp(-4.257011890411)=     0.014164564508
    {0xc0d83898, 0x3a9869b7 , FE_INEXACT}, // exp(-6.756908416748)=     0.001162818573
    {0xbf42398f, 0x3eefc248 , FE_INEXACT}, // exp(-0.758690774441)=     0.468279108831
    {0x3f5e8630, 0x4018a53e , FE_INEXACT}, // exp(0.869235038757)=      2.385085657423
    {0x40e75758, 0x44ac6d29 , FE_INEXACT}, // exp(7.229412078857)=      1379.411280586528
    {0x40d78d43, 0x44528b86 , FE_INEXACT}, // exp(6.735993862152)=     842.180079194122
    {0x40cdd726, 0x441b6f03 , FE_INEXACT}, // exp(6.432513237000)=     621.734552387876
    {0xbf492640, 0x3ee95c4e , FE_INEXACT}, // exp(-0.785748422146)=    0.455778459412
    {0xc0aefa54, 0x3b8a430f , FE_INEXACT}, // exp(-5.468057632446)=    0.004219419868
    {0xc2a58f59, 0x03be831d , FE_INEXACT}, // exp(-82.779975891113)=   0.000000000000
    {0x3f775caa, 0x402832a4 , FE_INEXACT}, // exp(0.966257691383)=     2.628090906092
    {0x40388c72, 0x418f0630 , FE_INEXACT}, // exp(2.883572101593)=     17.878021362924
    {0xc28451ef, 0x0fbb9101 , FE_INEXACT}, // exp(-66.160026550293)=   0.000000000000
    {0x3eb45fca, 0x3fb60ec4 , FE_INEXACT}, // exp(0.352293312550)=     1.422325648504
    {0x406dfd5a, 0x4224d322 , FE_INEXACT}, // exp(3.718588352203)=     41.206184413049
    {0x3d6b24e5, 0x3f879033 , FE_INEXACT}, // exp(0.057408232242)=     1.059088076057
    {0xc287c7d1, 0x0e84fa80 , FE_INEXACT}, // exp(-67.890266418457)=   0.000000000000
    {0xbff79d43, 0x3e13f764 , FE_INEXACT}, // exp(-1.934486746788)=    0.144498414069
    {0xc0ebb4f6, 0x3a25cdf3 , FE_INEXACT}, // exp(-7.365839958191)=    0.000632493915
    {0x3f622744, 0x401ad32c , FE_INEXACT}, // exp(0.883411645889)=     2.419138889085
    {0xbfd50795, 0x3e41de3a , FE_INEXACT}, // exp(-1.664293885231)=    0.189324295476
    {0xbf0b0283, 0x3f14bc52 , FE_INEXACT}, // exp(-0.543007075787)=    0.580998516328
    {0x3e1ffc35, 0x3f95a51e , FE_INEXACT}, // exp(0.156235530972)=     1.169101530285
    {0xc28e7b66, 0x0c152efc , FE_INEXACT}, // exp(-71.241012573242)=   0.000000000000
    {0x427e010f, 0x6d43b799 , FE_INEXACT}, // exp(63.501033782959)=    3785720676697755300000000000.000000000000
    {0xc0d90e78, 0x3a90ba2f , FE_INEXACT}, // exp(-6.808650970459)=    0.001104181469
    {0x4295b816, 0x757fe8e4 , FE_INEXACT}, // exp(74.859542846680)=    324404131136078150000000000000000.000000000000
    {0xc297a0b4, 0x08c539af , FE_INEXACT}, // exp(-75.813873291016)=   0.000000000000
    {0x3f6e0f0b, 0x4022325d , FE_INEXACT}, // exp(0.929926872253)=     2.534323841450
    {0xc10274bd, 0x3996d9fc , FE_INEXACT}, // exp(-8.153500556946)=    0.000287726392
    {0xbf43b64d, 0x3eee5eba , FE_INEXACT}, // exp(-0.764500439167)=    0.465566451655
    {0x3f29a024, 0x3ff84c51 , FE_INEXACT}, // exp(0.662599802017)=     1.939828955688
    {0xbc41f1e4, 0x3f7cfccb , FE_INEXACT}, // exp(-0.011837456375)=    0.988232330673
    {0xbd971619, 0x3f6dcd45 , FE_INEXACT}, // exp(-0.073772616684)=    0.928882882300
    {0xc0b2577a, 0x3b78ee64 , FE_INEXACT}, // exp(-5.573178291321)=    0.003798388841
    {0x3e60bcf9, 0x3f9f69d8 , FE_INEXACT}, // exp(0.219470873475)=     1.245417572741
    {0xc09869b4, 0x3c0bee9e , FE_INEXACT}, // exp(-4.762903213501)=    0.008540777668
    {0x4039e7bd, 0x41921696 , FE_INEXACT}, // exp(2.904769182205)=     18.261028195468
    {0xc096980a, 0x3c141de6 , FE_INEXACT}, // exp(-4.706059455872)=    0.009040331310
    {0x3eb1ca9b, 0x3fb52441 , FE_INEXACT}, // exp(0.347248882055)=     1.415168891687
    {0xc15e27a6, 0x357a7ede , FE_INEXACT}, // exp(-13.884679794312)=   0.000000933169
    {0x411f0c14, 0x46a2221c , FE_INEXACT}, // exp(9.940448760986)=     20753.055357968085
    {0x3f5ca16b, 0x40178541 , FE_INEXACT}, // exp(0.861838042736)=     2.367508278352
    {0x3f46a95b, 0x400b0f55 , FE_INEXACT}, // exp(0.776021659374)=     2.172810866183
    {0x3e1966f3, 0x3f94afa1 , FE_INEXACT}, // exp(0.149806782603)=     1.161609777826
    {0x3b28133b, 0x3f805425 , FE_INEXACT}, // exp(0.002564622788)=     1.002567914246
    {0x4250b6dc, 0x651b2d45 , FE_INEXACT}, // exp(52.178573608398)=    45800117449823512000000.000000000000
    {0xbf44c2f8, 0x3eed6513 , FE_INEXACT}, // exp(-0.768599987030)=    0.463661746585
    {0x3e9ac39b, 0x3fad2cdb , FE_INEXACT}, // exp(0.302273601294)=     1.352931339840
    {0xbdb040ab, 0x3f6ae3cc , FE_INEXACT}, // exp(-0.086060844362)=    0.917538402483
    {0x3dbbe9bf, 0x3f8c4cdc , FE_INEXACT}, // exp(0.091754429042)=     1.096095619777
    {0x40586e09, 0x41eb5ea6 , FE_INEXACT}, // exp(3.381716012955)=     29.421215005818
    {0xbdd45806, 0x3f66c965 , FE_INEXACT}, // exp(-0.103683516383)=    0.901510565602
    {0x41036e04, 0x4566d9be , FE_INEXACT}, // exp(8.214359283447)=     3693.608909913918
    {0xbdcf5496, 0x3f675a34 , FE_INEXACT}, // exp(-0.101235553622)=    0.903720133261
    {0xbf6cf623, 0x3ecae565 , FE_INEXACT}, // exp(-0.925630748272)=    0.396281386430
    {0x3f0ff5f9, 0x3fe09cd5 , FE_INEXACT}, // exp(0.562346994877)=     1.754786145149
    {0x409d3f73, 0x43082eb3 , FE_INEXACT}, // exp(4.913995265961)=     136.182413935256
    {0xc0d53d6f, 0x3aa74b45 , FE_INEXACT}, // exp(-6.663749217987)=    0.001276352066
    {0x4150c3c7, 0x48e29921 , FE_INEXACT}, // exp(13.047797203064)=    464073.026286976060
    {0x402fe302, 0x4179d6f1 , FE_INEXACT}, // exp(2.748230457306)=     15.614976055522
    {0xc0c36754, 0x3b120e64 , FE_INEXACT}, // exp(-6.106363296509)=    0.002228641000
    {0xc109ac39, 0x39402c57 , FE_INEXACT}, // exp(-8.604546546936)=    0.000183270648
    {0x421f97c6, 0x5c3cd4f8 , FE_INEXACT}, // exp(39.898216247559)=    212605822500666240.000000000000
    {0x3f6dccb7, 0x402207f3 , FE_INEXACT}, // exp(0.928904950619)=     2.531735283964
    // from ancient libm repo
    {0xfF800000, 0x00000000, 0},  //-inf
    {0x7Fc00000, 0x7Fc00000, 0},  //qnan
    {0x7Fa00000, 0x7Fe00000, FE_INVALID},  //snan
    {0xfFc00000, 0xfFc00000, 0},  //qnan
    {0xfFa00000, 0xfFe00000, FE_INVALID},  //snan
    {0x3FC90FDB, 0x4099EF6F, FE_INEXACT},  //pi/2
    {0x3c000000, 0x3F810101, FE_INEXACT},  //0.0078125
    {0x3f012345, 0x3FD3F9F2, FE_INEXACT},  //0.504444
    {0x3f800000, 0x402DF854, FE_INEXACT},  //1
    {0x40000000, 0x40EC7326, FE_INEXACT},  //2

    {0x40490FDB, 0x41B92025, FE_INEXACT},  //pi
    {0x40C90FDB, 0x4405DF79, FE_INEXACT},  //2pi
    {0x41200000, 0x46AC14EE, FE_INEXACT},  //10

    {0x42800000, 0x6DA12CC1, FE_INEXACT},  //64
    {0x42AF0000, 0x7E96BAB3, FE_INEXACT},  //87.5
    {0x42b00000, 0x7EF882B7, FE_INEXACT},  //88

    {0xc2Aeac4f, 0x00800026, FE_INEXACT},  //-87.3365  smallest normal result
    {0x42b17216, 0x7F7FFF04, FE_INEXACT},  //88.7228   largest value  --
    {0x42b17217, 0x7F7FFF84, FE_INEXACT},  //88.7228   largest value

    {0x7fc00000, 0x7fc00000, 0},
    {0xffc00000, 0xffc00000, 0},
    {0x7f800000, 0x7f800000, 0},
    {0xff800000, 0x00000000, 0},
    {0x007fffff, 0x3f800000, 0},
    {0x807fffff, 0x3f800000, 0},
    {0x00000001, 0x3f800000, 0},
    {0x80000001, 0x3f800000, 0},
    {0x00800000, 0x3f800000, FE_INEXACT},
    {0x80800000, 0x3f800000, FE_INEXACT},

    {0xc20a1eb8, 0x269162c0, FE_INEXACT}, // -34.53
    {0xbe99999a, 0x3f3da643, FE_INEXACT}, // -0.3
    {0xbf247208, 0x3f06ab02, FE_INEXACT}, // -0.642365
    {0xbf000000, 0x3f1b4598, FE_INEXACT}, // -0.5
    {0x3e99999a, 0x3facc82d, FE_INEXACT}, // 0.3
    {0x3f247208, 0x3ff35307, FE_INEXACT}, // 0.642365
    {0x3f000000, 0x3fd3094c, FE_INEXACT}, // 0.5
    {0x420a1eb8, 0x586162f9, FE_INEXACT}, // 34.53
};

static libm_test_special_data_f64
test_exp_conformance_data[] = {
    #if defined(_WIN64) || defined(_WIN32)
        {0x4086300000000000, 0x7ff0000000000000, 5},  //exp(x>ln(2)*1024)
        {0xbff2adfcfff9be7b, 0x3fd3e9e507ee8de9, 1}, // 2: exp(-1.167477607636e+000)=   3.111507966480e-001
        {0x3ff6bc20e63ebf38, 0x3f8d024fdbbc0f01, 1}, // 3: exp(1.420929812808e+000)=    4.140968976757e+000
        {0x3fe0142d2449b180, 0x3ffa71d0be141803, 1}, // 4: exp(5.024629315135e-001)=    1.652786962980e+000
        {0xc011072e19391a74, 0x3f8d024fdbbc0f01, 1}, // 5: exp(-4.257011789427e+000)=   1.416456593825e-002
        {0xc01b0712fb2c5d84, 0x3f530d37067e3e98, 1}, // 6: exp(-6.756908344825e+000)=   1.162818656642e-003
        {0xbfe84731d7d8aa93, 0x3fddf848f81806bc, 1}, // 7: exp(-7.586907592534e-001)=   4.682791159431e-001
        {0x3febd0c5fa1436cf, 0x400314a7c67e7ec5, 1}, // 8: exp(8.692350277285e-001)=    2.385085631118e+000
        {0x401ceaeaff82efea, 0x40958da5241b1163, 1}, // 9: exp(7.229412071578e+000)=    1.379411270545e+003
        {0x401af1a8686db7a3, 0x408a517104d110e4, 1}, // 10: exp(6.735993987748e+000)=   8.421801849683e+002
        {0x4019bae4b0681054, 0x40836de011429037, 1}, // 11: exp(6.432513004638e+000)=   6.217344079209e+002
        {0xc0778db11a593980, 0x1df3dd395cc6e2b6, 1}, // 12: exp(-3.768557380185e+002)=  2.155919322765e-164
        {0xbfe924d9dda20681, 0x3fdd2b796c81c93e, 1}, // 13: exp(-7.857484177368e-001)=  4.557784614218e-001
        {0xc015df4a7fafd6f3, 0x3f714861e6cded36, 1}, // 14: exp(-5.468057627780e+000)=  4.219419887586e-003
        {0xc054b1eb1232afa5, 0x3877d068c644a143, 1}, // 15: exp(-8.277997260046e+001)=  1.119733541164e-036
        {0x3feeeb9538d6688c, 0x4005065481afef57, 1}, // 16: exp(9.662576780422e-001)=   2.628090871030e+000
        {0x4007118e4d4b8851, 0x4031e0c61fc50077, 1}, // 17: exp(2.883572200649e+000)=   1.787802313385e+001
        {0xc0508a3dd3a4af5a, 0x39f77224ad676e8a, 1}, // 18: exp(-6.616002360423e+001)=  1.849553192960e-029
        {0x3fd68bf942f8dc18, 0x3ff6c1d88b17edfa, 1}, // 19: exp(3.522933153176e-001)=   1.422325652441e+000
        {0xc07d220ca5506342, 0x15e6eeb67b8d7ca7, 1}, // 20: exp(-4.661280873432e+002)=  3.657158700400e-203
        {0x400dbfab4d44b91c, 0x40449a6462635eb6, 1}, // 21: exp(3.718588451061e+000)=   4.120618848660e+001
        {0x3fad649c97715ae2, 0x3ff0f206565b73f3, 1}, // 22: exp(5.740823124568e-002)=   1.059088075002e+000
        {0xc050f8fa19cc6a01, 0x39d09f5191af9654, 1}, // 23: exp(-6.789026493990e+001)=  3.278178228385e-030
        {0xbffef3a858503caa, 0x3fc27eec9642f79c, 1}, // 24: exp(-1.934486718153e+000)=  1.444984182071e-001
        {0xc01d769eb4992941, 0x3f44b9be92046a8f, 1}, // 25: exp(-7.365839788292e+000)=  6.324940223153e-004
        {0x3fec44e875109724, 0x40035a6576cbc99d, 1}, // 26: exp(8.834116255209e-001)=   2.419138839811e+000
        {0xbffaa0f2936e31c5, 0x3fc83bc75fbdca10, 1}, // 27: exp(-1.664293838406e+000)=  1.893243043410e-001
        {0xbfe160505c1fc7c1, 0x3fe2978a35955225, 1}, // 28: exp(-5.430070685672e-001)=  5.809985205220e-001
        {0x3fc3ff8694be6825, 0x3ff2b4a3ccbf539f, 1}, // 29: exp(1.562355257309e-001)=   1.169101524157e+000
        {0xc051cf6cce4dddd5, 0x3982a5db4de6aa36, 1}, // 30: exp(-7.124101598362e+001)=  1.149264185844e-031
        {0x404fc021ed4a6e34, 0x45a876f59c84624e, 1}, // 31: exp(6.350103536734e+001)=   3.785726674723e+027
        {0xc01b3c0f08ee254f, 0x3f521745aab8891d, 1}, // 32: exp(-6.808651103530e+000)=  1.104181322156e-003
        {0x4052b702c93599c4, 0x46affd212a33a48f, 1}, // 33: exp(7.485954504237e+001)=   3.244048434267e+032
        {0xc052f4167006690b, 0x3918a73bfade3e32, 1}, // 34: exp(-7.581386948229e+001)=  1.187011241997e-033
        {0x3fedc1f602d3c2fc, 0x4004464b95ce8ece, 1}, // 35: exp(9.299268775195e-001)=   2.534323854796e+000
        {0xc0204e97acc89eb6, 0x3f32db3ef92a4eef, 1}, // 36: exp(-8.153500937929e+000)=  2.877262824452e-004
        {0xbfe876c9a3ae3c62, 0x3fddcbd73790dbd0, 1}, // 37: exp(-7.645004460227e-001)=  4.655664484630e-001
        {0x3fe53404774bbb05, 0x3fff098a0dd6ff8e, 1}, // 38: exp(6.625997858044e-001)=   1.939828924238e+000
        {0x3e45798ee2308c3a, 0x3ff0000002af31dc, 1}, // .00000001
        {0x3ff0000000000000, 0x4005bf0a8b145769, 1}, // 1
        {0x4000000000000000, 0x401d8e64b8d4ddae, 1}, // 2
        {0x4024000000000000, 0x40d5829dcf950560, 1}, // 10
        {0x408f400000000000, 0x7ff0000000000000, 5}, // 1000
        {0x4084000000000000, 0x79a40a4b9c27178a, 1}, // 640
        {0xc085e00000000000, 0x00d14f2b0fb9307f, 1}, // -700
        {0xc07f51999999999a, 0x12c0be4b336b18b7, 1}, // -501.1
        {0xc086d00000000000, 0x00000000001c7ea3, 3}, // -730
        {0xc086232bdd7abcd2, 0x001000000000007c, 1}, // smallest normal  result, x=-1022*ln(2)
        {0xc086232bdd7abcd3, 0x000ffffffffffe7c, 3}, // largest denormal result
        {0xc0874385446d71c4, 0x0000000000000001, 3}, // x=-1074*ln(2)
        {0xc0874910d52d3051, 0x0000000000000001, 3}, // smallest input for smallest denormal result, x=-1075*ln(2)
        {0xc0874910d52d3052, 0x0000000000000000, 3}, // largest input for result zero
        {0xc08f400000000000, 0x0000000000000000, 3}, // -1000
        {0x40862e42fefa39f0, 0x7ff0000000000000, 5}, // smallest input for result inf
        {0x4086280000000000, 0x7fdd422d2be5dc9b, 1}, // 709
        {0x7fefffffffffffff, 0x7ff0000000000000, 5}, // largest number
        {0xc08625aad16d5438, 0x000bb63ae9a2ac50, 3},
        {0xc087440b864646f5, 0x0000000000000001, 3},
        {0xC0862328F5C28F5C, 0x001005D07E8F743F, 1},
        {0xC087492000000000, 0x0000000000000000, 3},
        {0x40862E6666666666, 0x7FF0000000000000, 5},
        {0xc0874593fa89185f, 0x0000000000000001, 3},
        {0xc08743e609f06b07, 0x0000000000000001, 3},
        {0xc0874409d4de2a93, 0x0000000000000001, 3},
        {0xc08744b894a31d87, 0x0000000000000001, 3},
        {0xc08744ddf48a3b9c, 0x0000000000000001, 3},
        {0xc08745723e498e76, 0x0000000000000001, 3},
        {0xc08627fa8b8965a4, 0x0008c5deb69c6fc8, 1},
    #else
        {0x4086300000000000, 0x7ff0000000000000, 40},  //exp(x>ln(2)*1024)
        {0xbff2adfcfff9be7b, 0x3fd3e9e507ee8de9, 32}, // 2: exp(-1.167477607636e+000)=  3.111507966480e-001
        {0x3ff6bc20e63ebf38, 0x3f8d024fdbbc0f01, 32}, // 3: exp(1.420929812808e+000)=   4.140968976757e+000
        {0x3fe0142d2449b180, 0x3ffa71d0be141803, 32}, // 4: exp(5.024629315135e-001)=   1.652786962980e+000
        {0xc011072e19391a74, 0x3f8d024fdbbc0f01, 32}, // 5: exp(-4.257011789427e+000)=  1.416456593825e-002
        {0xc01b0712fb2c5d84, 0x3f530d37067e3e98, 32}, // 6: exp(-6.756908344825e+000)=  1.162818656642e-003
        {0xbfe84731d7d8aa93, 0x3fddf848f81806bc, 32}, // 7: exp(-7.586907592534e-001)=  4.682791159431e-001
        {0x3febd0c5fa1436cf, 0x400314a7c67e7ec5, 32}, // 8: exp(8.692350277285e-001)=   2.385085631118e+000
        {0x401ceaeaff82efea, 0x40958da5241b1163, 32}, // 9: exp(7.229412071578e+000)=   1.379411270545e+003
        {0x401af1a8686db7a3, 0x408a517104d110e4, 32}, // 10: exp(6.735993987748e+000)=  8.421801849683e+002
        {0x4019bae4b0681054, 0x40836de011429037, 32}, // 11: exp(6.432513004638e+000)=  6.217344079209e+002
        {0xc0778db11a593980, 0x1df3dd395cc6e2b6, 32}, // 12: exp(-3.768557380185e+002)= 2.155919322765e-164
        {0xbfe924d9dda20681, 0x3fdd2b796c81c93e, 32}, // 13: exp(-7.857484177368e-001)= 4.557784614218e-001
        {0xc015df4a7fafd6f3, 0x3f714861e6cded36, 32}, // 14: exp(-5.468057627780e+000)= 4.219419887586e-003
        {0xc054b1eb1232afa5, 0x3877d068c644a143, 32}, // 15: exp(-8.277997260046e+001)= 1.119733541164e-036
        {0x3feeeb9538d6688c, 0x4005065481afef57, 32}, // 16: exp(9.662576780422e-001)=  2.628090871030e+000
        {0x4007118e4d4b8851, 0x4031e0c61fc50077, 32}, // 17: exp(2.883572200649e+000)=  1.787802313385e+001
        {0xc0508a3dd3a4af5a, 0x39f77224ad676e8a, 32}, // 18: exp(-6.616002360423e+001)= 1.849553192960e-029
        {0x3fd68bf942f8dc18, 0x3ff6c1d88b17edfa, 32}, // 19: exp(3.522933153176e-001)=  1.422325652441e+000
        {0xc07d220ca5506342, 0x15e6eeb67b8d7ca7, 32}, // 20: exp(-4.661280873432e+002)= 3.657158700400e-203
        {0x400dbfab4d44b91c, 0x40449a6462635eb6, 32}, // 21: exp(3.718588451061e+000)=  4.120618848660e+001
        {0x3fad649c97715ae2, 0x3ff0f206565b73f3, 32}, // 22: exp(5.740823124568e-002)=  1.059088075002e+000
        {0xc050f8fa19cc6a01, 0x39d09f5191af9654, 32}, // 23: exp(-6.789026493990e+001)= 3.278178228385e-030
        {0xbffef3a858503caa, 0x3fc27eec9642f79c, 32}, // 24: exp(-1.934486718153e+000)= 1.444984182071e-001
        {0xc01d769eb4992941, 0x3f44b9be92046a8f, 32}, // 25: exp(-7.365839788292e+000)= 6.324940223153e-004
        {0x3fec44e875109724, 0x40035a6576cbc99d, 32}, // 26: exp(8.834116255209e-001)=  2.419138839811e+000
        {0xbffaa0f2936e31c5, 0x3fc83bc75fbdca10, 32}, // 27: exp(-1.664293838406e+000)= 1.893243043410e-001
        {0xbfe160505c1fc7c1, 0x3fe2978a35955225, 32}, // 28: exp(-5.430070685672e-001)= 5.809985205220e-001
        {0x3fc3ff8694be6825, 0x3ff2b4a3ccbf539f, 32}, // 29: exp(1.562355257309e-001)=  1.169101524157e+000
        {0xc051cf6cce4dddd5, 0x3982a5db4de6aa36, 32}, // 30: exp(-7.124101598362e+001)= 1.149264185844e-031
        {0x404fc021ed4a6e34, 0x45a876f59c84624e, 32}, // 31: exp(6.350103536734e+001)=  3.785726674723e+027
        {0xc01b3c0f08ee254f, 0x3f521745aab8891d, 32}, // 32: exp(-6.808651103530e+000)= 1.104181322156e-003
        {0x4052b702c93599c4, 0x46affd212a33a48f, 32}, // 33: exp(7.485954504237e+001)=  3.244048434267e+032
        {0xc052f4167006690b, 0x3918a73bfade3e32, 32}, // 34: exp(-7.581386948229e+001)= 1.187011241997e-033
        {0x3fedc1f602d3c2fc, 0x4004464b95ce8ece, 32}, // 35: exp(9.299268775195e-001)=  2.534323854796e+000
        {0xc0204e97acc89eb6, 0x3f32db3ef92a4eef, 32}, // 36: exp(-8.153500937929e+000)= 2.877262824452e-004
        {0xbfe876c9a3ae3c62, 0x3fddcbd73790dbd0, 32}, // 37: exp(-7.645004460227e-001)= 4.655664484630e-001
        {0x3fe53404774bbb05, 0x3fff098a0dd6ff8e, 32}, // 38: exp(6.625997858044e-001)=  1.939828924238e+000
        {0x3e45798ee2308c3a, 0x3ff0000002af31dc, 32}, // .00000001
        {0x3ff0000000000000, 0x4005bf0a8b145769, 32}, // 1
        {0x4000000000000000, 0x401d8e64b8d4ddae, 32}, // 2
        {0x4024000000000000, 0x40d5829dcf950560, 32}, // 10
        {0x408f400000000000, 0x7ff0000000000000, 32}, // 1000
        {0x4084000000000000, 0x79a40a4b9c27178a, 32}, // 640
        {0xc085e00000000000, 0x00d14f2b0fb9307f, 32}, // -700
        {0xc07f51999999999a, 0x12c0be4b336b18b7, 32}, // -501.1
        {0xc086d00000000000, 0x00000000001c7ea3, 48}, // -730
        {0xc086232bdd7abcd2, 0x001000000000007c, 32}, // smallest normal  result, x=-1022*ln(2)
        {0xc086232bdd7abcd3, 0x000ffffffffffe7c, 48}, // largest denormal result
        {0xc0874385446d71c4, 0x0000000000000001, 48}, // x=-1074*ln(2)
        {0xc0874910d52d3051, 0x0000000000000001, 48}, // smallest input for smallest denormal result, x=-1075*ln(2)
        {0xc0874910d52d3052, 0x0000000000000000, 48}, // largest input for result zero
        {0xc08f400000000000, 0x0000000000000000, 48}, // -1000
        {0x40862e42fefa39f0, 0x7ff0000000000000, 40}, // smallest input for result inf
        {0x4086280000000000, 0x7fdd422d2be5dc9b, 32}, // 709
        {0x7fefffffffffffff, 0x7ff0000000000000, 40}, // largest number
        {0xc08625aad16d5438, 0x000bb63ae9a2ac50, 48},
        {0xc087440b864646f5, 0x0000000000000001, 48},
        {0xC0862328F5C28F5C, 0x001005D07E8F743F, 32},
        {0xC087492000000000, 0x0000000000000000, 48},
        {0x40862E6666666666, 0x7FF0000000000000, 40},
        {0xc0874593fa89185f, 0x0000000000000001, 48},
        {0xc08743e609f06b07, 0x0000000000000001, 48},
        {0xc0874409d4de2a93, 0x0000000000000001, 48},
        {0xc08744b894a31d87, 0x0000000000000001, 48},
        {0xc08744ddf48a3b9c, 0x0000000000000001, 48},
        {0xc08745723e498e76, 0x0000000000000001, 48},
        {0xc08627fa8b8965a4, 0x0008c5deb69c6fc8, 32},
    #endif
    {0x0000000000000000, 0x3FF0000000000000, 0,},
    {0x0,                0x3ff0000000000000, 0,},  //exp(0)
    {0x7ff0000000000000, 0x7ff0000000000000, 0,},  //exp(infinity)
    {0x7FF0000000000000, 0x7ff0000000000000, 0,},         //exp(inf)=inf
    {0xfff0000000000000, 0x0,                0,},  //exp(-inf)
    {0x7FF4001000000000, 0x7ffc001000000000, FE_INVALID,}, //exp(snan)
    {0xfff2000000000000, 0xfffa000000000000, FE_INVALID,}, //exp(-snan)
    {0x7ff87ff7fdedffff, 0x7ff87ff7fdedffff, FE_INVALID,},  //exp(qnan)
    {0xfff2000000000000, 0xfffa000000000000, FE_INVALID,},    //exp(-qnan)
    {0x40862e42fefa39ef, 0x7fefffffffffff2a, FE_INEXACT,}, //exp(ln(2)*1024)

    //new added
    {0xbf883e3c8972babc, 0x3fef9f99687d878e,}, // 39: exp(-1.183745665034e-002)=    9.882323304011e-001
    {0xbfb2e2c32253eee0, 0x3fedb96897e41cbe,}, // 40: exp(-7.377261722643e-002)=    9.288828817961e-001
    {0xc0164aef348b6f91, 0x3f6f1dccd452fcdd,}, // 41: exp(-5.573178120623e+000)=    3.798389489560e-003
    {0x3fcc179f264f06d5, 0x3ff3ed3afb080914,}, // 42: exp(2.194708764128e-001)= 1.245417576400e+000
    {0x40589331e63ba9ae, 0x48cc2f1f1ff509f1,}, // 43: exp(9.829992061449e+001)= 4.910357298667e+042
    {0xc0130d368bca2438, 0x3f817dd39293c01c,}, // 44: exp(-4.762903389180e+000)=    8.540776167564e-003
    {0x40073cf790aa581f, 0x403242d29b6a0e0c,}, // 45: exp(2.904769067954e+000)= 1.826102610912e+001
    {0xc012d30146d61abc, 0x3f8283bc9af10a4b,}, // 46: exp(-4.706059557741e+000)=    9.040330389444e-003
    {0x3fd6395359fd661b, 0x3ff6a48820a1737b,}, // 47: exp(3.472488764579e-001)= 1.415168883766e+000
    {0xc02bc4f4c20a4501, 0x3eaf4fdba323ecf3,}, // 48: exp(-1.388467985511e+001)=    9.331686839082e-007
    {0x4023e1827ff7d3f1, 0x40d444438aa9561c,}, // 49: exp(9.940448760035e+000)= 2.075305533822e+004
    {0x3feb942d58aecfa9, 0x4002f0a829cf8ec9,}, // 50: exp(8.618380291068e-001)= 2.367508246085e+000
    {0x4057f40861dd0d5a, 0x4892c0758f727b7a,}, // 51: exp(9.581301161372e+001)= 4.083779064291e+041
    {0x3fe8d52b53ce442d, 0x400161eaa3357260,}, // 52: exp(7.760216366606e-001)= 2.172810816830e+000
    {0x3fc32cde585f863a, 0x3ff295f4214c26b9,}, // 53: exp(1.498067790517e-001)= 1.161609773701e+000
    {0x3f65026752007e17, 0x3ff00a84a73444b7,}, // 54: exp(2.564622685909e-003)= 1.002567914144e+000
    {0x404a16db813ff11a, 0x44a365a8cffe464b,}, // 55: exp(5.217857375738e+001)= 4.580012427333e+022
    {0xbfe8985ef8658fce, 0x3fddaca2588c4f70,}, // 56: exp(-7.685999728678e-001)=    4.636617531516e-001
    {0x3fd3587364605e37, 0x3ff5a59b56a07ec2,}, // 57: exp(3.022736053694e-001)= 1.352931345355e+000
    {0xbfb60815524a73dd, 0x3fed5c7980822ef7,}, // 58: exp(-8.606084116987e-002)=    9.175384054119e-001
    {0x3fb77d37da697284, 0x3ff1899b8f21b215,}, // 59: exp(9.175442774126e-002)= 1.096095618351e+000
    {0x400b0dc121e0d885, 0x403d6bd4c60af47d,}, // 60: exp(3.381716026949e+000)= 2.942121541755e+001
    {0x406ec449c2fc432f, 0x56211ac2ead60dfa,}, // 61: exp(2.461340041091e+002)= 7.845846250823e+106
    {0xbfba8b00c02f92d6, 0x3fecd92caf82e997,}, // 62: exp(-1.036835164264e-001)=    9.015105655629e-001
    {0x40206dc08333bb05, 0x40acdb37f13d812a,}, // 63: exp(8.214359378876e+000)= 3.693609262392e+003
    {0xbfb9ea92b0669357, 0x3feceb467de62144,}, // 64: exp(-1.012355499902e-001)=    9.037201365436e-001
    {0xbfed9ec467c54a0b, 0x3fd95cac94860acb,}, // 65: exp(-9.256307627459e-001)=    3.962813806940e-001
    {0x3fe1febf12bef9d6, 0x3ffc139a976d308a,}, // 66: exp(5.623469701894e-001)= 1.754786101827e+000
    {0x4013a7ee6937ee4c, 0x406105d67cfaf125,}, // 67: exp(4.913995403327e+000)= 1.361824326421e+002
    {0xc01aa7ade3207f62, 0x3f54e96888324406,}, // 68: exp(-6.663749264582e+000)=    1.276352006958e-003
    {0x402a1878e62c9e80, 0x411c5324725c4fc9,}, // 69: exp(1.304779738707e+001)= 4.640731116803e+005
    {0x4005fc603094db97, 0x402f3adde80bf3b2,}, // 70: exp(2.748230342429e+000)= 1.561497426172e+001
    {0xc0186cea7eb88a70, 0x3f6241cc8c7c456a,}, // 71: exp(-6.106363277448e+000)=    2.228641042060e-003
    {0xc021358725413e82, 0x3f28058aa1df2f75,}, // 72: exp(-8.604546703543e+000)=    1.832706192871e-004
    {0x4043f2f8b6563432, 0x43879a9d2bcdd0da,}, // 73: exp(3.989821509561e+001)= 2.126055775893e+017
    {0x3fedb996d8444ae9, 0x400440fe68d0ffab,}, // 74: exp(9.289049362145e-001)= 2.531735247496e+000
    // from ancient libm repo
    {0xffefffffffffffff, 0x0000000000000000,},
    {0xc0862c4379671324, 0x00052288f82fe4ba,},

    {0xc086be6fb2739468, 0x0000000001000000,}, // denormal result
    {0xbff0000000000000, 0x3fd78b56362cef38,}, // -1

    // all denormal
    {0xfff8000000000000, 0xfff8000000000000, 1},
    {0xfff4000000000000, 0xfffc000000000000, 1},

};

#endif  /*__TEST_EXP_DATA_H___*/
