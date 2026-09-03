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

#ifndef __TEST_ERFC_DATA_H__
#define __TEST_ERFC_DATA_H__

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>

/*
 * Test cases to check for exceptions for the erfc() routine.
 * These test cases are not exhaustive
 */
static libm_test_special_data_f32
test_erfcf_conformance_data[] = {
    {POS_ZERO_F32, POS_ONE_F32, 0},             // erfc(0) = 1
    {NEG_ZERO_F32, POS_ONE_F32, 0},             // erfc(-0) = 1
    {POS_INF_F32, POS_ZERO_F32, 0},             // erfc(+inf) = 0
    {NEG_INF_F32, 0x40000000, 0},               // erfc(-inf) = 2
    {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID },  // erfc(+SNAN) = SNAN
    {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID },  // erfc(-SNAN) = -SNAN
    {POS_QNAN_F32, POS_QNAN_F32, 0},            // erfc(+QNAN) = QNAN
    {NEG_QNAN_F32, NEG_QNAN_F32, 0},            // erfc(-QNAN) = -QNAN

    // erfc(x) = 0, for x > 4.0 (large positive values for single precision)
    {0x40800000, 0x32846ee9, 0},              // erfc(4.0)
    {0x40A00000, 0x2bd860d4, 0},              // erfc(5.0)
    {0x40C00000, 0x23c67c0b, 0},              // erfc(6.0)
    {0x41200000, 0x00000001, 0},              // erfc(10.0)
    {0x41A00000, POS_ZERO_F32, 0},              // erfc(20.0)

    // erfc(x) = 2, for x < -4.0 (large negative values for single precision)
    {0xC0800000, 0x40000000, 0},                // erfc(-4.0)
    {0xC1200000, 0x40000000, 0},                // erfc(-10.0)
    {0xC1700000, 0x40000000, 0},                // erfc(-15.0)
    {0xC1A00000, 0x40000000, 0},                // erfc(-20.0)
    {0xC1C80000, 0x40000000, 0},                // erfc(-25.0)

    // Additional test cases from core-math worst case values
    // For tiny positive values: erfc(x) ≈ 1 - x*(2/√π)
    {0x00000001, 0x3f800000, 0},                // erfc(tiny) ≈ 1
    {0x80000001, 0x3f800000, 0},                // erfc(-tiny) ≈ 1

    // For small denormal values
    {0x007FFFFF, 0x3f800000, 0},                // erfc(max_denormal)
    {0x807FFFFF, 0x3f800000, 0},                // erfc(-max_denormal)

    // For smallest normal values
    {0x00800000, 0x3f800000, 0},                // erfc(min_normal)
    {0x80800000, 0x3f800000, 0},                // erfc(-min_normal)

    // Special boundary cases near x = 1
    {0x3F800000, 0x3e21130b, 0},                // erfc(1) ≈ 0.157299...
    {0xBF800000, 0x3febdd9f, 0},                // erfc(-1) ≈ 1.842700...

    // Test cases around the transition points between approximation intervals
    {0x3F580000, 0x3e6e5c65, 0},                // erfc(0.84375)
    {0x3FA00000, 0x3d9de68a, 0},                // erfc(1.25)
    {0x40300000, 0x38d304fc, 0},                // erfc(2.75)

    // Very small positive value that would cause underflow
    {0x41900000, 0x00000000, 0},                // erfc(18) = 0 (underflow)

    // Test symmetry around zero
    {0x3F000000, 0x3ef5810a, 0},                // erfc(0.5)
    {0xBF000000, 0x3fc29fbd, 0},                // erfc(-0.5)
};

static libm_test_special_data_f64
test_erfc_conformance_data[] = {
    {POS_ZERO_F64, POS_ONE_F64, 0},             // erfc(0) = 1
    {NEG_ZERO_F64, POS_ONE_F64, 0},             // erfc(-0) = 1
    {POS_INF_F64, POS_ZERO_F64, 0},             // erfc(+inf) = 0
    {NEG_INF_F64, 0x4000000000000000, 0},       // erfc(-inf) = 2
    {POS_SNAN_F64, POS_SNAN_F64, FE_INVALID },  // erfc(+SNAN) = SNAN
    {NEG_SNAN_F64, NEG_SNAN_F64, FE_INVALID },  // erfc(-SNAN) = -SNAN
    {POS_QNAN_F64, POS_QNAN_F64, 0},            // erfc(+QNAN) = QNAN
    {NEG_QNAN_F64, NEG_QNAN_F64, 0},            // erfc(-QNAN) = -QNAN

    // erfc(x) = 0, for x > 5.921875 (large positive values)
    {0x4017B000C9539B89, 0x3c8fe371ab089e9d, 0},
    {0x402499999999999A, 0x361adc6b03471c57, 0},
    {0x4036800000000000, 0x11f3ed2c02828af5, 0},
    {0x407B000000000000, POS_ZERO_F64, 0},
    {0x4090000000000000, POS_ZERO_F64, 0},

    // erfc(x) = 2, for x < -5.921875 (large negative values)
    {0xC017B000C9539B89, 0x4000000000000000, 0},
    {0xC090000000000000, 0x4000000000000000, 0},
    {0xC063C00000000000, 0x4000000000000000, 0},
    {0xC054800000000000, 0x4000000000000000, 0},
    {0xC037AE147AE147AE, 0x4000000000000000, 0},

    // Additional test cases from core-math worst case values
    // For tiny positive values: erfc(x) ≈ 1 - x*(2/√π)
    {0x0000000000000001, 0x3ff0000000000000, 0},  // erfc(tiny) ≈ 1
    {0x8000000000000001, 0x3ff0000000000000, 0},  // erfc(-tiny) ≈ 1

    // For small denormal values
    {0x000FFFFFFFFFFFFF, 0x3ff0000000000000, 0},  // erfc(max_denormal)
    {0x800FFFFFFFFFFFFF, 0x3ff0000000000000, 0},  // erfc(-max_denormal)

    // For smallest normal values
    {0x0010000000000000, 0x3ff0000000000000, 0},  // erfc(min_normal)
    {0x8010000000000000, 0x3ff0000000000000, 0},  // erfc(-min_normal)

    // Special boundary cases near x = 1
    {0x3FF0000000000000, 0x3fc4226162fbddd5, 0},  // erfc(1) ≈ 0.157299...
    {0xBFF0000000000000, 0x3ffd7bb3d3a08445, 0},  // erfc(-1) ≈ 1.842700...

    // Test cases around the transition points between approximation intervals
    {0x3FEB000000000000, 0x3fcdcb8cae2d747f, 0},  // erfc(0.84375)
    {0x3FF4000000000000, 0x3fb3bcd133aa0ffc, 0},  // erfc(1.25)
    {0x4006000000000000, 0x3f1a609f7584d32b, 0},  // erfc(2.75)

    // Very small positive value that would cause underflow
    {0x4062000000000000, 0x0000000000000000, 0},  // erfc(144) = 0 (underflow)

    // Test symmetry around zero
    {0x3FE0000000000000, 0x3fdeb02147ce245c, 0},  // erfc(0.5)
    {0xBFE0000000000000, 0x3ff853f7ae0c76e9, 0},  // erfc(-0.5)
};

#endif  /*__TEST_ERFC_DATA_H___*/
