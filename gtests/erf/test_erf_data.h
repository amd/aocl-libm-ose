/*
 * Copyright (C) 2008-2024 Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __TEST_ERF_DATA_H__
#define __TEST_ERF_DATA_H__

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>

/*
 * Test cases to check for exceptions for the erf() routine.
 * These test cases are not exhaustive
 */
static libm_test_special_data_f32
test_erff_conformance_data[] = {
    {0x0, 0x0, 0},
    {POS_ZERO_F32, 0x0, 0,}, // erff(0) = 0
    {NEG_ZERO_F32, 0x0, 0},  // erff(-0) = 0
    {0x407AD447, POS_ONE_F32, 0,}, // cutoff at which erff(x) becomes 1
    {0xc07AD447, NEG_ONE_F32, 0,}, // cutoff at which erff(x) becomes -1
    //special numbers
    {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID },  //
    {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID },  //
    {POS_INF_F32, POS_ONE_F32, 0 },  //
    {NEG_INF_F32, NEG_ONE_F32, 0 },  //
    {POS_QNAN_F32, POS_QNAN_F32, 0},  //
    {NEG_QNAN_F32, NEG_QNAN_F32, 0},
#ifdef WIN64
    {0x7f800000, POS_ONE_F32, 0},
    {0xff800000, NEG_ONE_F32, 0},   // - INF
#else
    {0x7f800000, POS_ONE_F32, 0},  // + InF
    {0xff800000, NEG_ONE_F32, 0},   // - INF
#endif
    {0x7fc00000, 0x7fc00000, 0},  //NaN
    {0x7fc00001, 0x7fc00001, 0},  //NaN
    {0x7f800001, POS_ONE_F32, 0},   // + INF
    {0x7fef8000, 0x7fef8000, 0},   // + QNAN
    {0xffef8000, 0xffef8000, 0},   // - QNAN
    {0x80000000, 0x3f800000, 0},	// - 0
};

static libm_test_special_data_f64
test_erf_conformance_data[] = {
    {POS_ZERO_F64, POS_ZERO_F64, 0},            // erf(0) = 0
    {NEG_ZERO_F64, POS_ZERO_F64, 0},            // erf(-0) = 0
    {POS_INF_F64, POS_ONE_F64, 0},              // erf(+inf) = 1
    {NEG_INF_F64, NEG_ONE_F64, 0},              // erf(-inf) = -1
    {POS_SNAN_F64, POS_SNAN_F64, FE_INVALID },  // erf(+SNAN) = SNAN
    {NEG_SNAN_F64, NEG_SNAN_F64, FE_INVALID },  // erf(-SNAN) = -SNAN
    {POS_QNAN_F64, POS_QNAN_F64, 0},            // erf(+QNAN) = QNAN
    {NEG_QNAN_F64, NEG_QNAN_F64, 0},            // erf(-QNAN) = -QNAN
    // erf(x) = 1, for x > 5.921875
    {0x4017B000C9539B89, POS_ONE_F64, 0},
    {0x402499999999999A, POS_ONE_F64, 0},
    {0x4036800000000000, POS_ONE_F64, 0},
    {0x407B000000000000, POS_ONE_F64, 0},
    {0x4090000000000000, POS_ONE_F64, 0},
    // erf(x) = -1, for x < -5.921875
    {0xC017B000C9539B89, NEG_ONE_F64, 0},
    {0xC090000000000000, NEG_ONE_F64, 0},
    {0xC063C00000000000, NEG_ONE_F64, 0},
    {0xC054800000000000, NEG_ONE_F64, 0},
    {0xC037AE147AE147AE, NEG_ONE_F64, 0},
    // Additional test cases from core-math worst case values
    {0x0000000000000001, 0x0000000000000001, 0},
    {0x8000000000000001, 0x8000000000000001, 0},
    {0x000FFFFFFFFFFFFF, 0x00120DD750429B6C, 0},
    {0x800FFFFFFFFFFFFF, 0x80120DD750429B6C, 0},
    {0x0010000000000000, 0x00120DD750429B6D, 0},
    {0x8010000000000000, 0x80120dd750429b6d, 0},
    {0x3FF0000000000000, 0xBE343791E2362FE3, 0},
    {0xBFF0000000000000, 0xBFEAF767A741088B, 0},
};

#endif  /*__TEST_ERF_DATA_H___*/
