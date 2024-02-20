/*
 * Copyright (C) 2024 Advanced Micro Devices, Inc. All rights reserved.
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

/* Test cases to check the conformance of the logbf() routine. */
static libm_test_special_data_f32
test_logbf_conformance_data[] = {
    /* Test all the intervals in the code, including boundary values */
    /* Test Inputs of -/+0 */
    {POS_ZERO_F32,    NEG_INF_F32,       0},
    {NEG_ZERO_F32,    NEG_INF_F32,       0},

    /* Test Inputs of -/+inf */
    {POS_INF_F32,     POS_INF_F32,       0},
#ifdef WINDOWS
	{NEG_INF_F32,     NEG_INF_F32,       0},
#else
	{NEG_INF_F32,     POS_INF_F32,       0},
#endif

    /* Test Inputs that are signalling and quiet NaNs */
    {POS_SNAN_F32,    POS_SNAN_Q_F32,    FE_INVALID},
    {NEG_SNAN_F32,    NEG_SNAN_Q_F32,    FE_INVALID},
    {POS_QNAN_F32,    QNANBITPATT_SP32,  0},
    {NEG_QNAN_F32,    INDEFBITPATT_SP32, 0},

    /* Test Inputs that are standard, denormalized and extreme */
    {POS_ONE_F32,     POS_ZERO_F32,      0},
    {NEG_ONE_F32,     POS_ZERO_F32,      0},
    {POS_PI_F32,      POS_ONE_F32,       0},
    {POS_PI_BY2_F32,  POS_ZERO_F32,      0},
    {POS_HDENORM_F32, 0xc2fe0000,        0},
    {NEG_HDENORM_F32, 0xc2fe0000,        0},
    {POS_LDENORM_F32, 0xc3150000,        0},
    {NEG_LDENORM_F32, 0xc3150000,        0},
    {POS_HNORMAL_F32, 0x42fe0000,        0},
    {NEG_HNORMAL_F32, 0x42fe0000,        0},
    {POS_LNORMAL_F32, 0xc2fc0000,        0},
    {NEG_LNORMAL_F32, 0xc2fc0000,        0},
    {0xbf247208,      NEG_ONE_F32,       0}, // -0.642365
    {0xbf000000,      NEG_ONE_F32,       0}, // -0.5
    {0x3f247208,      NEG_ONE_F32,       0}, // 0.642365
    {0x3f000000,      NEG_ONE_F32,       0}, // 0.5
    {0x3f012345,      NEG_ONE_F32,       0}, // 0.504444
    {0x40000000,      POS_ONE_F32,       0}, // 2
    {0x3c000000,      0xc0e00000,        0}, // 0.0078125
    {0x3c7fffff,      0xc0e00000,        0}, // 0.0156249991
    {0x33d6bf95,      0xc1c00000,        0}, // 1.0000000e-7
    {0x40c90fdb,      0x40000000,        0}, // 2pi
    {0x41200000,      0x40400000,        0}, // 10
    {0x447a0000,      0x41100000,        0}, // 1000
    {0x42800000,      0x40c00000,        0}, // 64
    {0x42af0000,      0x40c00000,        0}, // 87.5
    {0x42b00000,      0x40c00000,        0}, // 88
    {0x42c00000,      0x40c00000,        0}, // 96
    {0xc2af0000,      0x40c00000,        0}, // -87.5
    {0xc2e00000,      0x40c00000,        0}, // -112
    {0xc3000000,      0x40e00000,        0}, // -128
    {0xc2aeac4f,      0x40c00000,        0}, // -87.3365  smallest normal result
    {0xc2aeac50,      0x40c00000,        0}, // -87.3365  largest denormal result
    {0xc2ce0000,      0x40c00000,        0}, // -103
    {0x42b17216,      0x40c00000,        0}, // 88.7228   largest value  --
    {0x42b17217,      0x40c00000,        0}, // 88.7228   largest value
    {0x42b17218,      0x40c00000,        0}, // 88.7228   overflow
    {0x50000000,      0x42040000,        0}, // large   overflow
    {0xc20a1eb8,      0x40a00000,        0}, // -34.53
    {0xc6de66b6,      0x41600000,        0}, // -28467.3555
    {0xbe99999a,      0xc0000000,        0}, // -0.3
    {0x3e99999a,      0xc0000000,        0}, // 0.3
    {0x420a1eb8,      0x40a00000,        0}, // 34.53
    {0x46de66b6,      0x41600000,        0}, // 28467.3555
    {0xc2c80000,      0x40c00000,        0}, // -100
    {0x42c80000,      0x40c00000,        0}, // 100    
};

/* Test cases to check the conformance of the logb() routine. */
static libm_test_special_data_f64
test_logb_conformance_data[] = {
    /* Test all the intervals in the code, including boundary values */
    /* Test Inputs of -/+0 */
    {POS_ZERO_F64,         NEG_INF_F64,          0},
    {NEG_ZERO_F64,         NEG_INF_F64,          0},

    /* Test Inputs of -/+inf */
    {POS_INF_F64,          POS_INF_F64,          0},
#ifdef WINDOWS
    {NEG_INF_F64,          NEG_INF_F64,          0},
#else
    {NEG_INF_F64,          POS_INF_F64,          0},
#endif

    /* Test Inputs that are signalling and quiet NaNs */
    {POS_SNAN_F64,         POS_SNAN_Q_F64,       FE_INVALID},
    {NEG_SNAN_F64,         NEG_SNAN_Q_F64,       FE_INVALID},
    {POS_QNAN_F64,         QNANBITPATT_DP64,     0},
    {NEG_QNAN_F64,         NEG_QNAN_F64,         0},

    /* Test Inputs that are standard, denormalized and extreme */
    {POS_ONE_F64,          POS_ZERO_F64,         0},
    {NEG_ONE_F64,          POS_ZERO_F64,         0},
    {POS_PI_F64,           POS_ONE_F64,          0},
    {POS_PI_BY2_F64,       POS_ZERO_F64,         0},
    {POS_HDENORM_F64,      0xc08ff80000000000LL, 0},
    {NEG_HDENORM_F64,      0xc08ff80000000000LL, 0},
    {POS_LDENORM_F64,      0xc090c80000000000LL, 0},
    {NEG_LDENORM_F64,      0xc090c80000000000LL, 0},
    {POS_HNORMAL_F64,      0x408ff80000000000LL, 0},
    {NEG_HNORMAL_F64,      0x408ff80000000000LL, 0},
    {POS_LNORMAL_F64,      0xc08ff00000000000LL, 0},
    {NEG_LNORMAL_F64,      0xc08ff00000000000LL, 0},
    {0x3FE02467BE553AC5LL, NEG_ONE_F64,          0}, // 0.504444
    {0x4000000000000000LL, POS_ONE_F64,          0}, // 2
    {0x3FE48E410B630A91LL, NEG_ONE_F64,          0}, // 0.642365
    {0x3FE0000000000000LL, NEG_ONE_F64,          0}, // 0.5
    {0xBFE48E410B630A91LL, NEG_ONE_F64,          0}, // -0.642365
    {0xBFE0000000000000LL, NEG_ONE_F64,          0}, // -0.5
    {0x3EE0624DD2F1A9FCLL, 0xc031000000000000LL, 0}, // 0.0000078125
    {0x3EF0624DC31C6CA1LL, 0xc030000000000000LL, 0}, // 0.0000156249991
    {0x3D3C25C268497682LL, 0xc046000000000000LL, 0}, // 1.0000000e-13
    {0x401921FB54442D18LL, 0x4000000000000000LL, 0}, // 2pi
    {0x4024000000000000LL, 0x4008000000000000LL, 0}, // 10
    {0x408F400000000000LL, 0x4022000000000000LL, 0}, // 1000
    {0x4060000000000000LL, 0x401c000000000000LL, 0}, // 128
    {0x4086240000000000LL, 0x4022000000000000LL, 0}, // 708.5
    {0x4086280000000000LL, 0x4022000000000000LL, 0}, // 709
    {0x408C000000000000LL, 0x4022000000000000LL, 0}, // 896
    {0xC086240000000000LL, 0x4022000000000000LL, 0}, // -708.5
    {0xC089600000000000LL, 0x4022000000000000LL, 0}, // -812
    {0xC070000000000000LL, 0x4020000000000000LL, 0}, // -256
    {0xc086232bdd7abcd2LL, 0x4022000000000000LL, 0}, // -708.3964185322641 smallest normal result
    {0xc086232bdd7abcd3LL, 0x4022000000000000LL, 0}, // -708.3964185322642 largest denormal result
    {0xC087480000000000LL, 0x4022000000000000LL, 0}, // -745
    {0x40862e42fefa39eeLL, 0x4022000000000000LL, 0}, // 7.09782712893383973096e+02   largest value  --
    {0x40862e42fefa39efLL, 0x4022000000000000LL, 0}, // 7.09782712893383973096e+02   largest value
    {0x40862e42fefa39ffLL, 0x4022000000000000LL, 0}, // 7.09782712893383973096e+02   overflow
    {0x4200000000000000LL, 0x4040800000000000LL, 0}, // large   overflow
    {0xC05021EB851EB852LL, 0x4018000000000000LL, 0}, //  -64.53
    {0xC0FF5D35B020C49CLL, 0x4030000000000000LL, 0}, //  -128467.3555
    {0xBFD3333333333333LL, 0xc000000000000000LL, 0}, //  -0.3
    {0x3FD3333333333333LL, 0xc000000000000000LL, 0}, // 0.3
    {0x405021EB851EB852LL, 0x4018000000000000LL, 0}, // 64.53
    {0x40FF5D35B020C49CLL, 0x4030000000000000LL, 0}, // 128467.3555
    {0xC08F400000000000LL, 0x4022000000000000LL, 0}, // -1000
    {0x408F400000000000LL, 0x4022000000000000LL, 0}, // 1000
};
