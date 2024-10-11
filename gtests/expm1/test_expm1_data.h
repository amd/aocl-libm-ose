/*
 * Copyright (C) 2008-2023 Advanced Micro Devices, Inc. All rights reserved.
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
 * Test cases to check the conformance of the expm1f() routine.
 */
static libm_test_special_data_f32
test_expm1f_conformance_data[] = {
    /* Test Inputs of -/+0 and -/+1*/
    {POS_ZERO_F32, POS_ZERO_F32,       0},
    {NEG_ZERO_F32, NEG_ZERO_F32,       0},
    {POS_ONE_F32, 0x3fdbf0a9,          0},
    {NEG_ONE_F32, 0xbf21d2a7,          0},

    /* Test Inputs of -/+inf */
    {POS_INF_F32, POS_INF_F32,         0},
    {NEG_INF_F32, NEG_ONE_F32,         0},

    /* Test Inputs that are signalling and quiet NaNs */
    {POS_SNAN_F32, POS_SNAN_Q_F32,     FE_INVALID},
    {NEG_SNAN_F32, NEG_SNAN_Q_F32,     FE_INVALID},
    {POS_QNAN_F32, POS_QNAN_F32,       0},
    {NEG_QNAN_F32, NEG_QNAN_F32,       0},

    /* Test Inputs that are standard, denormalized and extreme */
    {POS_HDENORM_F32, POS_HDENORM_F32, FE_INEXACT},
    {NEG_HDENORM_F32, NEG_HDENORM_F32, FE_INEXACT},
    {POS_LDENORM_F32, POS_LDENORM_F32, FE_INEXACT},
    {NEG_LDENORM_F32, NEG_LDENORM_F32, FE_INEXACT},
    {POS_HNORMAL_F32, POS_INF_F32,     FE_OVERFLOW},
    {NEG_HNORMAL_F32, NEG_ONE_F32,     0},
    {POS_LNORMAL_F32, POS_LNORMAL_F32, FE_INEXACT},
    {NEG_LNORMAL_F32, NEG_LNORMAL_F32, FE_INEXACT},
    {POS_PI_F32,      0x41b12025,      0},            // pi
    {POS_PI_BY2_F32,  0x4073dedd,      0},            // pi/2
    {0x3c000000,      0x3c008056,      0},            // 0.0078125
    {0x3c7fffff,      0x3c810156,      0},            // 0.0156249991
    {0x3f012345,      0x3f27f3e3,      0},            // 0.5044444
    {0x3f800000,      0x3fdbf0a9,      0},            // 1
    {0x40000000,      0x40cc7326,      0},            // 2
    {0x33d6bf95,      0x33d6bf96,      0},            // 1.0000000e-7
    {0x40c90fdb,      0x44059f79,      0},            // 2pi
    {0x41200000,      0x46ac12ee,      0},            // 10
    {0x447a0000,      0x7f800000,      FE_OVERFLOW},  // 1000
    {0x42800000,      0x6da12cc1,      0},            // 64
    {0x42af0000,      0x7e96bab3,      0},            // 87.5
    {0x42b00000,      0x7ef882b7,      0},            // 88
    {0x42c00000,      0x7f800000,      FE_OVERFLOW},  // 96
    {0xc2af0000,      0xbf800000,      0},            // -87.5
    {0xc2e00000,      0xbf800000,      0},            // -112
    {0xc3000000,      0xbf800000,      0},            // -128
    {0xc2aeac4f,      0xbf800000,      0},            // -87.3365  smallest normal result
    {0xc2aeac50,      0xbf800000,      0},            // -87.3365  largest denormal result
    {0xc2ce0000,      0xbf800000,      0},            // -103
    {0x42b17216,      0x7f7fff04,      0},            // 88.7228   largest value
    {0x42b17217,      0x7f7fff84,      0},            // 88.7228   largest value
    {0x42b17218,      0x7f800000,      FE_OVERFLOW},  // 88.7228   overflow
    {0x50000000,      0x7f800000,      FE_OVERFLOW},  // large   overflow
    {0xc20a1eb8,      0xbf800000,      0},            // -34.53
    {0xc6de66b6,      0xbf800000,      0},            // -28467.3555
    {0xbe99999a,      0xbe84b37a,      0},            // -0.3
    {0xbf247208,      0xbef2a9fc,      0},            // -0.642365
    {0xbf000000,      0xbec974d0,      0},            // -0.5
    {0x3e99999a,      0x3eb320b2,      0},            // 0.3
    {0x3f247208,      0x3f66a60f,      0},            // 0.642365
    {0x3f000000,      0x3f261299,      0},            // 0.5
    {0x420a1eb8,      0x586162f9,      0},            // 34.53
    {0x46de66b6,      0x7f800000,      FE_OVERFLOW},  // 28467.3555
    {0xc2c80000,      0xbf800000,      0},            // -100
    {0x42c80000,      0x7f800000,      FE_OVERFLOW},  // 100

    /* The following is the specific example of the error, provided by Paul Zimmermann */
    {0x5e317400,      0x7f800000,      0},
};

/*
 * Test cases to check the conformance of the expm1() routine.
 */
static libm_test_special_data_f64
test_expm1_conformance_data[] = {
    /* Test Inputs of -/+0 and -/+1*/
    {POS_ZERO_F64,         POS_ZERO_F64,         0},
    {NEG_ZERO_F64,         NEG_ZERO_F64,         0},
    {POS_ONE_F64,          0x3ffb7e151628aed3LL, 0},
    {NEG_ONE_F64,          0xbfe43a54e4e98864LL, 0},

    /* Test Inputs of -/+inf */
    {POS_INF_F64,          POS_INF_F64,          0},
    {NEG_INF_F64,          NEG_ONE_F64,          0},

    /* Test Inputs that are signalling and quiet NaNs */
    {POS_SNAN_F64,         POS_SNAN_Q_F64,       FE_INVALID},
    {NEG_SNAN_F64,         NEG_SNAN_Q_F64,       FE_INVALID},
    {POS_QNAN_F64,         POS_QNAN_F64,         0},
    {NEG_QNAN_F64,         NEG_QNAN_F64,         0},

    /* Test Inputs that are standard, denormalized and extreme */
    {POS_HDENORM_F64,      POS_HDENORM_F64,      FE_UNDERFLOW},
    {NEG_HDENORM_F64,      NEG_HDENORM_F64,      FE_UNDERFLOW},
    {POS_LDENORM_F64,      POS_LDENORM_F64,      FE_UNDERFLOW},
    {NEG_LDENORM_F64,      NEG_LDENORM_F64,      FE_UNDERFLOW},
    {POS_HNORMAL_F64,      POS_INF_F64,          FE_OVERFLOW},
    {NEG_HNORMAL_F64,      NEG_ONE_F64,          0},
    {POS_LNORMAL_F64,      POS_LNORMAL_F64,      FE_UNDERFLOW},
    {NEG_LNORMAL_F64,      NEG_LNORMAL_F64,      FE_UNDERFLOW},
    {POS_PI_F64,           0x4036240490a16562LL, 0},            // pi
    {POS_PI_BY2_F64,       0x400e7bdbace4109aLL, 0},            // pi/2
    {0x3EE0624DD2F1A9FCLL, 0x3ee0625204b0496aLL, 0},            // 0.0000078125
    {0x3EF0624DC31C6CA1LL, 0x3ef06256269b09c8LL, 0},            // 0.0000156249991
    {0x3FE02467BE553AC5LL, 0x3fe4fe7af67676faLL, 0},            // 0.504444
    {0x3FF0000000000000LL, 0x3ffb7e151628aed3LL, 0},            // 1
    {0x4000000000000000LL, 0x40198e64b8d4ddaeLL, 0},            // 2
    {0x3D3C25C268497682LL, 0x3d3c25c26849780eLL, 0},            // 1.0000000e-13
    {0x401921FB54442D18LL, 0x4080b3eee9177e18LL, 0},            // 2pi
    {0x4024000000000000LL, 0x40d5825dcf950560LL, 0},            // 10
    {0x408F400000000000LL, 0x7ff0000000000000LL, FE_OVERFLOW},  // 1000
    {0x4060000000000000LL, 0x4b795e54c5dd4218LL, 0},            // 128
    {0x4086240000000000LL, 0x7fd1bf058bc994adLL, 0},            // 708.5
    {0x4086280000000000LL, 0x7fdd422d2be5dc9bLL, 0},            // 709
    {0x408C000000000000LL, 0x7ff0000000000000LL, FE_OVERFLOW},  // 896
    {0xC086240000000000LL, 0xbff0000000000000LL, 0},            // -708.5
    {0xC089600000000000LL, 0xbff0000000000000LL, 0},            // -812
    {0xC070000000000000LL, 0xbff0000000000000LL, 0},            // -256
    {0xc086232bdd7abcd2LL, 0xbff0000000000000LL, 0},            // -708.3964185322641 smallest normal result
    {0xc086232bdd7abcd3LL, 0xbff0000000000000LL, 0},            // -708.3964185322642 largest denormal result
    {0xC087480000000000LL, 0xbff0000000000000LL, 0},            // -745
    {0x40862e42fefa39eeLL, 0x7feffffffffffb2aLL, 0},            // 7.09782712893383973096e+02   largest value
    {0x40862e42fefa39efLL, 0x7fefffffffffff2aLL, 0},            // 7.09782712893383973096e+02   largest value
    {0x40862e42fefa39ffLL, 0x7ff0000000000000LL, FE_OVERFLOW},  // 7.09782712893383973096e+02   overflow
    {0x4200000000000000LL, 0x7ff0000000000000LL, FE_OVERFLOW},  // large   overflow
    {0xC05021EB851EB852LL, 0xbff0000000000000LL, 0},            // -64.53
    {0xC0FF5D35B020C49CLL, 0xbff0000000000000LL, 0},            // -128467.3555
    {0xBFD3333333333333LL, 0xbfd0966f2c7907f6LL, 0},            // -0.3
    {0xBFE48E410B630A91LL, 0xbfde553f96c59dcfLL, 0},            // -0.642365
    {0xBFE0000000000000LL, 0xbfd92e9a0720d3ecLL, 0},            // -0.5
    {0x3FD3333333333333LL, 0x3fd6641632306a56LL, 0},            // 0.3
    {0x3FE48E410B630A91LL, 0x3fecd4c1e623c665LL, 0},            // 0.642365
    {0x3FE0000000000000LL, 0x3fe4c2531c3c0d38LL, 0},            // 0.5
    {0x405021EB851EB852LL, 0x45c11d33b0716b95LL, 0},            // 64.53
    {0x40FF5D35B020C49CLL, 0x7ff0000000000000LL, FE_OVERFLOW},  // 128467.3555
    {0xC08F400000000000LL, 0xbff0000000000000LL, 0},            // -1000
    {0x408F400000000000LL, 0x7ff0000000000000LL, FE_OVERFLOW},  // 1000
    {0x0000000000000000LL, 0x0000000000000000LL, 0},

    /* The following is the specific example of the error, provided by Paul Zimmermann */
    {0x408fffbfff7cfe9eLL, 0x7ff0000000000000LL, 0}, 
};

