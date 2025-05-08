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

#ifndef __TEST_SINCOS_DATA_H__
#define __TEST_SINCOS_DATA_H__

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>
#include "sincos.h"

/*
 * Test cases to check for exceptions for the sincos() routine.
 */
// Define the test data
static libm_spec_sincosf_data
sincosf_conformance_data[] = {
    //NaN (Not a Number) Cases:
    {POS_SNAN_F32, POS_SNAN_F32, POS_SNAN_F32, FE_INVALID}, // sincosf(nan) = {nan, nan}
    {NEG_SNAN_F32, NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID}, // sincosf(-nan) = {-nan, -nan}
    {POS_QNAN_F32, POS_QNAN_F32, POS_QNAN_F32, 0},          // sincosf(qnan) = {qnan, qnan}
    {NEG_QNAN_F32, NEG_QNAN_F32, NEG_QNAN_F32, 0},          // sincosf(-qnan) = {-qnan, -qnan}

    //Zero Cases:
    {POS_ZERO_F32, POS_ZERO_F32, 0x3f800000, 0},            // sincosf(0) = {0, 1}
    {NEG_ZERO_F32, NEG_ZERO_F32, 0x3f800000, 0},            // sincosf(-0) = {-0, 1}

    //Infinity Cases:
    {POS_INF_F32, 0xffc00000, 0x7fc00000, FE_INVALID},      // sincosf(inf) = {nan, nan}
    {NEG_INF_F32, 0xffc00000, 0x7fc00000, FE_INVALID},      // sincosf(-inf) = {nan, nan}

   // Special Cases:
    {0x00000000, 0x00000000, 0x3f800000, 0},                // sincosf(0) = {sin(0), cos(0)}
    {0x3FC90FDB, 0x3f800000, 0xb33bbd2e, 0},                // sincosf(pi/2) = {1, 0}
    {0x40490FDB, 0x00000000, 0xbf800000, 0},                // sincosf(pi) = {0, -1}
    {0x40C90FDB, 0xbf800000, 0xb33bbd2e, 0},                // sincosf(3pi/2) = {-1, 0}
    {0x4116CBE4, 0x00000000, 0x3f800000, 0},                // sincosf(2pi) = {0, 1}
    {0x3F800000, 0x3f576aa4, 0x3f0a5140, FE_INEXACT},       // sincosf(1) = {sin(1), cos(1)}
    {0xBF800000, 0xbf576aa4, 0x3f0a5140, FE_INEXACT},       // sincosf(-1) = {sin(-1), cos(-1)}
    {0x41200000, 0xbf0b44f8, 0xbf56cd64, 0},                // sincosf(10) = {sin(10), cos(10)}
    {0x48F42400, 0x3e361962, 0xbf7beb6c, 0},                // sincosf(50000) = {sin(50000), cos(50000)}
    {0xC8F42400, 0xbe361962, 0xbf7beb6c, 0},                // sincosf(-50000) = {sin(-50000), cos(-50000)}

    //  Boundary Values:
    {0x3fc90de3, 0x3f800000, 0xb33bbd2e, 0},                 // 1.57074 = pi/2 - 6e-05
    {0x3fc911d2, 0x3f800000, 0xb33bbd2e, 0},                 // 1.57086 = pi/2 + 6e-05
    {0x3fc88fdb, 0x3f7fff80, 0xb33bbd2e, 0},                 // 1.56689 = pi/2 - 0.00390625
    {0x3fc98fdb, 0x3f7fff80, 0xb33bbd2e, 0},                 // 1.57470 = pi/2 + 0.00390625

};

static libm_spec_sincos_data
sincos_conformance_data[] = {
    // NaN Cases
    {POS_SNAN_F64, 0x7ff4001000000000, POS_SNAN_F64, FE_INVALID},             // sincos(snan) = {snan, snan}
    {NEG_SNAN_F64, 0xfff2000000000000, NEG_SNAN_F64, FE_INVALID},             // sincos(-snan) = {-snan, -snan}
    {POS_QNAN_F64, 0x7ff87ff7e0000000, POS_QNAN_F64, 0},                      // sincos(qnan) = {qnan, qnan}
    {NEG_QNAN_F64, 0xfffa000000000000, NEG_QNAN_F64, 0},                      // sincos(-qnan) = {-qnan, -qnan}

    // Infinity Cases
    {POS_INF_F64, 0xfff8000000000000, POS_SNAN_F64, FE_INVALID},              // sincos(inf) = {inf, snan}
    {NEG_INF_F64, 0xfff8000000000000, POS_SNAN_F64, FE_INVALID},              // sincos(-inf) = {-inf, snan}

    // Special Cases
    {POS_ZERO_F64, POS_ZERO_F64, POS_ONE_F64, 0},                             // sincos(0) = {0, 1}
    {NEG_ZERO_F64, NEG_ZERO_F64, POS_ONE_F64, 0},                             // sincos(-0) = {-0, 1}
    {POS_ONE_F64, 0x3feaed548f090cee, 0x3feaed548f090cee, FE_INEXACT},        // sincos(1) ≈ {0.84, 0.54}
    {POS_PI_F64, POS_ZERO_F64, POS_ZERO_F64, 0},                              // sincos(pi) = {0, 0}
    {0x3ff921fb544486e0, 0x0000000000000000, 0x0000000000000000, 0},          // sincos(n*Pi) = {0, 0} for any n=1,2,3..

    // Boundary Cases
    {0x0010000000000000, 0x0010000000000000, 0x0010000000000000, 0},          // sincos(smallest positive normal number)
    {0xFFEFFFFFFFFFFFFF, 0xFFEFFFFFFFFFFFFF, 0xFFEFFFFFFFFFFFFF, 0},          // sincos(largest negative normal number)

    // Corner Cases
    {0x3FF921FB54442D18, 0x0000000000000000, 0x0000000000000000, 0},          // sincos(π/2) = {1, 0}
    {0x4012D97C7F336528, 0x0000000000000000, 0x0000000000000000, 0},          // sincos(270°) = {0, 0}

    // Additional Cases
    {0x3FF0000000000000, 0x3FEF333333333333, 0x3FEF333333333333, FE_INEXACT}, // sincos(1) ≈ {0.84147, 0.54030}
    {0x4000000000000000, 0x3FE6A09E667F3BCD, 0x3FE6A09E667F3BCD, FE_INEXACT}, // sincos(2) ≈ {0.90930, -0.41615}
    {0xBFF0000000000000, 0xBFEF333333333333, 0x3FEF333333333333, FE_INEXACT}, // sincos(-1) ≈ {-0.84147, 0.54030}
    {0xC000000000000000, 0xBFE6A09E667F3BCD, 0x3FE6A09E667F3BCD, FE_INEXACT}, // sincos(-2) ≈ {-0.90930, -0.41615}
    {0x3FF921FB54442D18, 0x3FF0000000000000, 0x0000000000000000, 0},          // sincos(π/2) = {1, 0}
    {0x400921FB54442D18, 0x0000000000000000, 0xBFF0000000000000, 0},          // sincos(π) = {0, -1}
    {0x401921FB54442D18, 0xBFF0000000000000, 0x0000000000000000, 0},          // sincos(3π/2) = {-1, 0}
    {0x3FE0000000000000, 0x3FD3333333333333, 0x3FD3333333333333, FE_INEXACT}, // sincos(0.5) ≈ {0.47943, 0.87758}
    {0x3FF8000000000000, 0x3FEF5C28F5C28F5C, 0x3FEF5C28F5C28F5C, FE_INEXACT}, // sincos(1.5) ≈ {0.99749, 0.07074}
    {0x4004000000000000, 0x3FEA9B6666666666, 0x3FEA9B6666666666, FE_INEXACT}, // sincos(2.5) ≈ {0.59847, -0.80114}
    {0xBFE0000000000000, 0xBFD3333333333333, 0x3FD3333333333333, FE_INEXACT}, // sincos(-0.5) ≈ {-0.47943, 0.87758}
    {0xBFF8000000000000, 0xBFEF5C28F5C28F5C, 0x3FEF5C28F5C28F5C, FE_INEXACT}, // sincos(-1.5) ≈ {-0.99749, 0.07074}
    {0xC004000000000000, 0xBFEA9B6666666666, 0x3FEA9B6666666666, FE_INEXACT}, // sincos(-2.5) ≈ {-0.59847, -0.80114}
    {0x3FF4000000000000, 0x3FEF0A3D70A3D70A, 0x3FEF0A3D70A3D70A, FE_INEXACT}, // sincos(1.25) ≈ {0.94898, 0.31623}
    {0x3FF6000000000000, 0x3FEF2B851EB851EB, 0x3FEF2B851EB851EB, FE_INEXACT}, // sincos(1.375) ≈ {0.98079, 0.19509}
    {0x3FF7000000000000, 0x3FEF3C6EF9DB22D0, 0x3FEF3C6EF9DB22D0, FE_INEXACT}, // sincos(1.4375) ≈ {0.99371, 0.11196}
    {0x3FF9000000000000, 0x3FEF5E1EB851EB85, 0x3FEF5E1EB851EB85, FE_INEXACT}, // sincos(1.5625) ≈ {0.99969, 0.02454}
    {0x3FFA000000000000, 0x3FEF6F5C28F5C28F, 0x3FEF6F5C28F5C28F, FE_INEXACT}, // sincos(1.625) ≈ {0.99863, -0.05234}
    {0x3FFB000000000000, 0x3FEF80A3D70A3D70, 0x3FEF80A3D70A3D70, FE_INEXACT}, // sincos(1.6875) ≈ {0.99027, -0.13917}
};


#endif  /*__TEST_SINCOS_DATA_H__*/
