/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __TEST_ERFCINV_DATA_H__
#define __TEST_ERFCINV_DATA_H__

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>

/*
 * Test cases to check for exceptions for the erfcinv() routine.
 * These test cases are not exhaustive
 */
static libm_test_special_data_f32
test_erfcinvf_conformance_data[] = {

};

static libm_test_special_data_f64
test_erfcinv_conformance_data[] = {
    {NEG_ZERO_F64, POS_INF_F64,  0},            // erfcinv(-0) = +inf
    {POS_ZERO_F64, POS_INF_F64 , 0},            // erfcinv(0)  = +inf
    {POS_ONE_F64 , POS_ZERO_F64, 0},            // erfcinv(1)  = 0
    {0x4000000000000000, NEG_INF_F64, 0},       // erfcinv(2)  = -inf

    {NEG_ONE_F64 , NEG_QNAN_F64, FE_INVALID  }, // erfcinv(-1) = -QNAN
    {0x4008000000000000, NEG_QNAN_F64, FE_INVALID  }, // erfcinv(3) = -QNAN

    {POS_SNAN_F64, POS_QNAN_F64, FE_INVALID },  // erfcinv(+SNAN) = QNAN
    {NEG_SNAN_F64, NEG_QNAN_F64, FE_INVALID },  // erfcinv(-SNAN) = -QNAN
    {POS_QNAN_F64, POS_QNAN_F64, 0},            // erfcinv(+QNAN) = QNAN
    {NEG_QNAN_F64, NEG_QNAN_F64, 0},            // erfcinv(-QNAN) = -QNAN

    // erfcinv(x) ≈ 0
    {0x0000000000000001, 0x403B369A6244E684, 0},  // erfcinv(min_denormal)
    {0x000FFFFFFFFFFFFF, 0x403A8B12FC6E4892, 0},  // erfcinv(max_denormal)
    {0x0010000000000000, 0x403A8B12FC6E4892, 0},  // erfcinv(min_normal)

    // erfcinv(x) ≈ 1
    {0x3FEFFFFFFFFFFFFF, 0x3C9C5BF891B4EF6B, 0},
    {0x3FF0000000000000, 0x0000000000000000, 0},
    {0x3FF0000000000001, 0xBCAC5BF891B4EF6B, 0},

    // erfcinv(x) ≈ 2
    {0x3FFFFFFFFFFFFFFF, 0xC0173856D153F081, 0},

    // Test cases around the transition points between approximation intervals
    {0x2B2BFF2EE48E0530, 0x402E219301E62922, 0},  // erfcinv(1e-100)
    {0x3FB0000000000000, 0x3FF5130C3ACAE4DE, 0},  // erfcinv(0.0625)
    {0x3FE8000000000000, 0x3FCCD70681D5FF70, 0},  // erfcinv(0.75)
    {0x3FEE000000000000, 0x3FAC636951597E17, 0},  // erfcinv(0.9375)

    // Test symmetry around one
    {0x3FE0000000000000, 0x3FDE861FBB24C00A, 0},  // erfcinv(0.5)
    {0x3FF8000000000000, 0xBFDE861FBB24C00A, 0},  // erfcinv(1.5)

};

#endif  /*__TEST_ERFCINV_DATA_H___*/
