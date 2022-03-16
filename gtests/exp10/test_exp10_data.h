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
 * Test cases to check for exceptions for the exp10f() routine.
 * These test cases are not exhaustive
 */
static libm_test_special_data_f32
test_exp10f_conformance_data[] = {
    {0x7Fa00000, 0x7Fe00000, FE_INVALID},  //snan
    {0xfFa00000, 0xfFe00000, FE_INVALID},  //-snan
    {0x7fe00000, 0x7fe00000, 0},          //expf(qnan) = qnan  
    {0xffe00000, 0xffe00000, 0},          //expf(-qnan) = -qnan  
    {0x7F800000, 0x7F800000, 0},          //inf  
    {0xFF800000, 0x00000000, 0},          //-inf
    {0x3f800000, 0x41200000, 0},          //1   
    {0xBF800000, 0x3dcccccd, 0},        //-1
    {0x00000000, 0x3F800000, 0},         //exp(0)=1 
    {0x80000000, 0x3F800000, 0},         //exp(-0)=1
    {0x007fffff, 0x3f800000, 0},         //F32_POS_HDENORM
    {0x807fffff, 0x3f800000, 0},         //F32_NEG_HDENORM
    {0x00000001, 0x3f800000, 0},         //F32_POS_LDENORM
    {0x80000001, 0x3f800000, 0},         //F32_NEG_LDENORM
    {0x7f7fffff, 0x7f800000, 40},        //F32_POS_HNORMAL
    {0xff7fffff, 0x00000000, 48},        //F32_NEG_HNORMAL
    {0x00800000, 0x3f800000, 0},         //F32_POS_LNORMAL
    {0x80800000, 0x3f800000, 0},         //F32_NEG_LNORMAL
    
    {0x7fbfffff, 0x0,        FE_INVALID}, //expf(nan)=nan
    {0xffffffff, 0xffffffff, 0,},         //expf(-nan)=-nan
  
    {0x3FC90FDB, 0x4214e309, 0},  //pi/2
    {0x3c000000, 0x3f8252cc, 0},  //0.0078125
    {0x3c7fffff, 0x3f84b063, 0},  //0.0156249991
    {0x3f012345, 0x404c77b1, 0},  //0.504444
    {0x40000000, 0x42c80000, 0},  //2
    {0x33d6bf95, 0x3f800002, 0},  //1.0000000e-7
    {0x40490FDB, 0x44ad2e98, 0},  //pi             
    {0x40c90fdb, 0x49ea5003, 0},  //2pi
    {0x41200000, 0x501502f9, 0},  //10
    {0x447a0000, 0x7f800000, 40},  //1000
    {0x42800000, 0x7f800000, 40},  //64
    {0x42160000, 0x7dbe529a, 0},  //37.5
    {0x42180000, 0x7e967699, 0},  //38
    {0x42c00000, 0x7f800000, 40},  //96
    {0xc2af0000, 0x00000000, 48},  //-87.5
    {0xc2e00000, 0x00000000, 48},  //-112
    {0xc3000000, 0x00000000, 48},  //-128
    {0xc2aeac4f, 0x00000000, 48},  //-87.3365  smallest normal result
    {0xc2aeac50, 0x00000000, 48},  //-87.3365  largest denormal result
    {0xc2ce0000, 0x00000000, 48},  //-103
    {0x421A209A, 0x7f7fffb3, 0},  //38.531   largest value  --
    {0x421A209B, 0x7f800000, 40},  //38.531   largest value
    {0x421A209C, 0x7f800000, 40},  //38.531   overflow
    {0x50000000, 0x7f800000, 40},  //large   overflow
    {0xc20a1eb8, 0x061ce9e4, 0}, // -34.53
    {0xc6de66b6, 0x00000000, 48}, // -28467.3555
    {0xbe99999a, 0x3f004dce, 0}, // -0.3
    {0xbf247208, 0x3e694f95, 0}, // -0.642365
    {0xbf000000, 0x3ea1e89b, 0}, // -0.5
    {0x3e99999a, 0x3fff64c2, 0}, // 0.3
    {0x3f247208, 0x408c72a3, 0}, // 0.642365
    {0x3f000000, 0x404a62c2, 0}, // 0.5
    {0x420a1eb8, 0x78d0d408, 0}, // 34.53
    {0x46de66b6, 0x7f800000, 40}, // 28467.3555
    {0xc2c80000, 0x00000000, 48}, // -100
    {0x42c80000, 0x7f800000, 40}, // 100

};

static libm_test_special_data_f64
test_exp10_conformance_data[] = {
    { 0x7FF0000000000001, 0x7FF8000000000001, FE_INVALID},  //snan
    { 0xffF0000000000001, 0xffF8000000000001, FE_INVALID},  //-snan
    { 0x7FF8000000000000, 0x7FF8000000000000, 0},           //qnan
    { 0xFFF8000000000000, 0xFFF8000000000000, 0},           //-qnan
    { 0x7fF0000000000000, 0x7fF0000000000000, 0},           //inf
    { 0xffF0000000000000, 0x0000000000000000, 0},           //-inf
    { 0x3fF0000000000000, 0x4024000000000000, 0},           //1
    { 0xbfF0000000000000, 0x3fb999999999999a, 0},           //-1
    { 0x0000000000000000, 0x3fF0000000000000, 0},           //0
    { 0x8000000000000000, 0x3fF0000000000000, 0},           //-0    
    { 0x000FFFFFFFFFFFFF, 0x3fF0000000000000, 0},          //F64_POS_HDENORM
    { 0x800fffffFFFFFFFF, 0x3fF0000000000000, 0},                    //F64_NEG_HDENORM 
    { 0x0000000000000001, 0x3fF0000000000000, 0},                    //F64_POS_LDENORM 
    { 0x8000000000000001, 0x3fF0000000000000, 0},                    //F64_NEG_LDENORM
    { 0x7FEfffffFFFFFFFF, 0x7fF0000000000000, 40},         //F64_POS_HNORMAL
    { 0xfFEfffffFFFFFFFF, 0x0000000000000000, 48},         //F64_NEG_HNORMAL
    { 0x0010000000000000, 0x3fF0000000000000, 0},                    //F64_POS_LNORMAL   
    { 0x8010000000000000, 0x3fF0000000000000, 0},                    //F64_NEG_LNORMAL 
    { 0x3FF921FB60000000, 0x40429c6121c48762, 0},  //pi/2
    { 0x3EE0624DD2F1A9FC, 0x3ff00012dcea13ea, 0},  //0.0000078125
    { 0x3EF0624DC31C6CA1, 0x3ff00025b9ea4057, 0},  //0.0000156249991
    { 0x3FE02467BE553AC5, 0x40098ef48d8d12ba, 0},  //0.504444
    { 0x3FF0000000000000, 0x4024000000000000, 0},  //1
    { 0x4000000000000000, 0x4059000000000000, 0},  //2
    { 0x3D3C25C268497682, 0x3ff000000000040d, 0},  //1.0000000e-13
    { 0x400921FB60000000, 0x4095a5d2f45a52a3, 0},  //pi
    { 0x401921FB54442D18, 0x413d49ff9565591a, 0},  //2pi
    { 0x4024000000000000, 0x4202a05f20000000, 0},  //10
    { 0x408F400000000000, 0x7ff0000000000000, 40},  //1000
    { 0x4060000000000000, 0x5a827748f9301d32, 0},  //128
    { 0x4073380000000000, 0x7fc68423588e3be1, 0},  //307.5
    { 0x4073400000000000, 0x7fe1ccf385ebc8a0, 0},  //308
    { 0x408C000000000000, 0x7ff0000000000000, 40},  //896
    { 0xC074280000000000, 0x0000000000000006, 48},  //-322.5
    { 0xC089600000000000, 0x0000000000000000, 48},  //-812
    { 0xC070000000000000, 0x0ac8062864ac6f43, 0},  //-256
    { 0xc086232bdd7abcd2, 0x0000000000000000, 48},  // -708.3964185322641 smallest normal result
    { 0xc086232bdd7abcd3, 0x0000000000000000, 48},  // -708.3964185322642 largest denormal result
    { 0xC087480000000000, 0x0000000000000000, 48},  //-745
    { 0x40734413509f79fe, 0x7feffffffffffba1, 0},  // 308.   largest value  --
    { 0x40734413509f79ff, 0x7ff0000000000000, 40},  // 308.  largest value
    { 0x40734413509f7a00, 0x7ff0000000000000, 40},  // 308.  overflow
    { 0x4200000000000000, 0x7ff0000000000000, 40},  //large   overflow
    { 0xC05021EB851EB852, 0x3288dd27ccc64b88, 0}, // -64.53
    { 0xC0FF5D35B020C49C, 0x0000000000000000, 48}, // -128467.3555
    { 0xBFD3333333333333, 0x3fe009b9cf334252, 0}, // -0.3
    { 0xBFE48E410B630A91, 0x3fcd29f2930fdf8a, 0}, // -0.642365
    { 0xBFE0000000000000, 0x3fd43d136248490f, 0}, // -0.5
    { 0x3FD3333333333333, 0x3fffec982d5bb8af, 0}, // 0.3
    { 0x3FE48E410B630A91, 0x40118e547a2a0560, 0}, // 0.642365
    { 0x3FE0000000000000, 0x40094c583ada5b53, 0}, // 0.5
    { 0x405021EB851EB852, 0x4d549794b5f069d2, 0}, // 64.53
    { 0x40FF5D35B020C49C, 0x7ff0000000000000, 40}, // 128467.3555
    { 0xC08F400000000000, 0x0000000000000000, 40}, // -1000
    { 0x0000000000000000, 0x3ff0000000000000, 0},
};

#endif	/*__TEST_EXP_DATA_H___*/
