/*
 * Copyright (C) 2025 Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __TEST_CDFNORM_DATA_H__
#define __TEST_CDFNORM_DATA_H__

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>
/*
 * Test cases to check for exceptions for the cdfnorm() routine.
 * These test cases are not exhaustive
 */
static libm_test_special_data_f32
test_cdfnormf_conformance_data[] = {
    {POS_ZERO_F32, 0x3F000000, 0},             // cdfnorm(0) = 0.5
    {NEG_ZERO_F32, 0x3F000000, 0},             // cdfnorm(-0) = 0.5
    {POS_INF_F32, POS_ONE_F32, 0},             // cdfnorm(+inf) = 1
    {NEG_INF_F32, POS_ZERO_F32, 0},            // cdfnorm(-inf) = 0
    {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID }, // cdfnorm(+SNAN) = SNAN
    {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID }, // cdfnorm(-SNAN) = -SNAN
    {POS_QNAN_F32, POS_QNAN_F32, 0},           // cdfnorm(+QNAN) = QNAN
    {NEG_QNAN_F32, NEG_QNAN_F32, 0},           // cdfnorm(-QNAN) = -QNAN

    // cdfnorm(x) → 1, for large positive x (>8 for single precision)
    {0x40800000, 0x3F800000, 0},              // cdfnorm(4.0) ≈ 0.99997
    {0x40A00000, 0x3F800000, 0},              // cdfnorm(5.0) ≈ 1.0 (saturates)
    {0x40C00000, 0x3F800000, 0},              // cdfnorm(6.0) ≈ 1.0
    {0x41200000, 0x3F800000, 0},              // cdfnorm(10.0) = 1.0
    {0x41A00000, 0x3F800000, 0},              // cdfnorm(20.0) = 1.0

    // cdfnorm(x) → 0, for large negative x (<-8 for single precision)
    {0xC0800000, 0x00000000, 0},              // cdfnorm(-4.0) ≈ 3.17e-5
    {0xC1200000, 0x00000000, 0},              // cdfnorm(-10.0) = 0 (underflow)
    {0xC1700000, 0x00000000, 0},              // cdfnorm(-15.0) = 0
    {0xC1A00000, 0x00000000, 0},              // cdfnorm(-20.0) = 0
    {0xC1C80000, 0x00000000, 0},              // cdfnorm(-25.0) = 0

    // Additional test cases
    // For tiny positive values: cdfnorm(x) ≈ 0.5 + x/√(2π)
    {0x00000001, 0x3F000000, 0},              // cdfnorm(tiny) ≈ 0.5
    {0x80000001, 0x3F000000, 0},              // cdfnorm(-tiny) ≈ 0.5

    // For small denormal values
    {0x007FFFFF, 0x3F000000, 0},              // cdfnorm(max_denormal) ≈ 0.5
    {0x807FFFFF, 0x3F000000, 0},              // cdfnorm(-max_denormal) ≈ 0.5

    // For smallest normal values
    {0x00800000, 0x3F000000, 0},              // cdfnorm(min_normal) ≈ 0.5
    {0x80800000, 0x3F000000, 0},              // cdfnorm(-min_normal) ≈ 0.5

    // Special boundary cases near x = 1
    {0x3F800000, 0x3F578BC6, 0},              // cdfnorm(1) ≈ 0.841345
    {0xBF800000, 0x3E220439, 0},              // cdfnorm(-1) ≈ 0.158655

    // Test cases around the transition points between approximation intervals
    {0x3F580000, 0x3F445DB0, 0},              // cdfnorm(0.84375) ≈ 0.800
    {0x3FA00000, 0x3F6AE5A0, 0},              // cdfnorm(1.25) ≈ 0.894350
    {0x40300000, 0x3F7FCA00, 0},              // cdfnorm(2.75) ≈ 0.997020

    // Very small positive value near saturation
    {0x41900000, 0x3F800000, 0},              // cdfnorm(18) = 1.0 (saturates)

    // Test symmetry around zero
    {0x3F000000, 0x3F198880, 0},              // cdfnorm(0.5) ≈ 0.691462
    {0xBF000000, 0x3ECC7780, 0},              // cdfnorm(-0.5) ≈ 0.308538
};

static libm_test_special_data_f64
test_cdfnorm_conformance_data[] = {
    {POS_ZERO_F64, 0x3FE0000000000000, 0},             // cdfnorm(0) = 0.5
    {NEG_ZERO_F64, 0x3FE0000000000000, 0},             // cdfnorm(-0) = 0.5
    {POS_INF_F64, POS_ONE_F64, 0},                     // cdfnorm(+inf) = 1
    {NEG_INF_F64, POS_ZERO_F64, 0},                    // cdfnorm(-inf) = 0
    {POS_SNAN_F64, POS_SNAN_F64, FE_INVALID },         // cdfnorm(+SNAN) = SNAN
    {NEG_SNAN_F64, NEG_SNAN_F64, FE_INVALID },         // cdfnorm(-SNAN) = -SNAN
    {POS_QNAN_F64, POS_QNAN_F64, 0},                   // cdfnorm(+QNAN) = QNAN
    {NEG_QNAN_F64, NEG_QNAN_F64, 0},                   // cdfnorm(-QNAN) = -QNAN

    // cdfnorm(x) → 1, for large positive x (>37.5 saturates to 1.0)
    {0x4017B000C9539B89, 0x3FEFFFFFFFFFFFFF, 0},       // cdfnorm(5.921875) ≈ 0.999999999
    {0x402499999999999A, 0x3FF0000000000000, 0},       // cdfnorm(10.3) = 1.0
    {0x4036800000000000, 0x3FF0000000000000, 0},       // cdfnorm(22.5) = 1.0
    {0x407B000000000000, 0x3FF0000000000000, 0},       // cdfnorm(432.0) = 1.0
    {0x4090000000000000, 0x3FF0000000000000, 0},       // cdfnorm(1024.0) = 1.0

    // cdfnorm(x) → 0, for large negative x (<-39 underflows to 0.0)
    {0xC017B000C9539B89, 0x0000000000000000, 0},       // cdfnorm(-5.921875) ≈ tiny
    {0xC090000000000000, 0x0000000000000000, 0},       // cdfnorm(-1024.0) = 0
    {0xC063C00000000000, 0x0000000000000000, 0},       // cdfnorm(-159.0) = 0
    {0xC054800000000000, 0x0000000000000000, 0},       // cdfnorm(-82.0) = 0
    {0xC037AE147AE147AE, 0x0000000000000000, 0},       // cdfnorm(-23.68) = 0

    // Additional test cases
    // For tiny positive values: cdfnorm(x) ≈ 0.5 + x/√(2π)
    {0x0000000000000001, 0x3FE0000000000000, 0},       // cdfnorm(tiny) ≈ 0.5
    {0x8000000000000001, 0x3FE0000000000000, 0},       // cdfnorm(-tiny) ≈ 0.5

    // For small denormal values
    {0x000FFFFFFFFFFFFF, 0x3FE0000000000000, 0},       // cdfnorm(max_denormal) ≈ 0.5
    {0x800FFFFFFFFFFFFF, 0x3FE0000000000000, 0},       // cdfnorm(-max_denormal) ≈ 0.5

    // For smallest normal values
    {0x0010000000000000, 0x3FE0000000000000, 0},       // cdfnorm(min_normal) ≈ 0.5
    {0x8010000000000000, 0x3FE0000000000000, 0},       // cdfnorm(-min_normal) ≈ 0.5

    // Special boundary cases near x = 1
    {0x3FF0000000000000, 0x3FEAED548F090CEE, 0},       // cdfnorm(1) ≈ 0.841344746
    {0xBFF0000000000000, 0x3FC44F52E979BAE8, 0},       // cdfnorm(-1) ≈ 0.158655254

    // Test cases around the transition points between approximation intervals
    {0x3FEB000000000000, 0x3FE888B618EAA88C, 0},       // cdfnorm(0.84375) ≈ 0.8006
    {0x3FF4000000000000, 0x3FED5CB7ACFF59C0, 0},       // cdfnorm(1.25) ≈ 0.894350
    {0x4006000000000000, 0x3FEFF94FAC4B8300, 0},       // cdfnorm(2.75) ≈ 0.997020

    // Very small positive value near saturation
    {0x4062000000000000, 0x3FF0000000000000, 0},       // cdfnorm(144) = 1.0 (saturates)

    // Test symmetry around zero: cdfnorm(-x) = 1 - cdfnorm(x)
    {0x3FE0000000000000, 0x3FE6310FA4B3C6C4, 0},       // cdfnorm(0.5) ≈ 0.691462461
    {0xBFE0000000000000, 0x3FD398EF0956C64A, 0},       // cdfnorm(-0.5) ≈ 0.308537539
};

#endif  /*__TEST_CDFNORM_DATA_H___*/
