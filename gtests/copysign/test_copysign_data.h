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

#ifndef __TEST_COPYSIGN_DATA_H__
#define __TEST_COPYSIGN_DATA_H__

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>

/*
 * Test cases to check for exceptions for the copysignf() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_copysignf_conformance_data[] = {

    {POS_ZERO_F32,  NEG_ZERO_F32,   0,  NEG_ZERO_F32},  //0
    {NEG_ZERO_F32,  NEG_ZERO_F32,   0,  NEG_ZERO_F32},  //0
    {POS_ZERO_F32,  POS_ZERO_F32,   0,  POS_ZERO_F32},  //0
    {NEG_ZERO_F32,  POS_ZERO_F32,   0,  POS_ZERO_F32},  //0
    {0xBF800000,    POS_ZERO_F32,   0,  0x3F800000},
    {0xBF800000,    NEG_ZERO_F32,   0,  0xBF800000},
    {0x3F800000,    NEG_INF_F32,    0,  0xBF800000},
    {0xBF800000,    POS_INF_F32,    0,  0x3F800000},
    {0x3F800000,    POS_INF_F32,    0,  0x3F800000},
    {0xBF800000,    NEG_INF_F32,    0,  0xBF800000},

};

static libm_test_special_data_f64
test_copysign_conformance_data[] = {

    {POS_ZERO_F64,          NEG_ZERO_F64,   0,  NEG_ZERO_F64},  //0
    {NEG_ZERO_F64,          NEG_ZERO_F64,   0,  NEG_ZERO_F64},  //0
    {POS_ZERO_F64,          POS_ZERO_F64,   0,  POS_ZERO_F64},  //0
    {NEG_ZERO_F64,          POS_ZERO_F64,   0,  POS_ZERO_F64},  //0
    {0xBFF0000000000000,    POS_ZERO_F64,   0,  0x3FF0000000000000},
    {0xBFF0000000000000,    NEG_ZERO_F64,   0,  0xBFF0000000000000},
    {0x3FF0000000000000,    NEG_INF_F64,    0,  0xBFF0000000000000},
    {0xBFF0000000000000,    POS_INF_F64,    0,  0x3FF0000000000000},
    {0x3FF0000000000000,    POS_INF_F64,    0,  0x3FF0000000000000},
    {0xBFF0000000000000,    NEG_INF_F64,    0,  0xBFF0000000000000},
};

#endif	/*__TEST_COPYSIGN_DATA_H__*/
