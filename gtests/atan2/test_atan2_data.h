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

#ifndef __TEST_ATAN2_DATA_H__
#define __TEST_ATAN2_DATA_H__

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>

/* N.B. Domain: (-inf,inf),(-inf,inf); Range=(-Pi,Pi) */

/* Test cases to check the conformance of the atan2f() routine. */
static libm_test_special_data_f32
test_atan2f_conformance_data[] = {
    /* Test all the intervals in the code, including boundary values */
    /* Test Inputs of -/+0 */
    {POS_ZERO_F32, POS_PI_F32,     0, NEG_ZERO_F32},
    {NEG_ZERO_F32, NEG_PI_F32,     0, NEG_ZERO_F32},
    {POS_ZERO_F32, POS_ZERO_F32,   0, POS_ZERO_F32},
    {NEG_ZERO_F32, NEG_ZERO_F32,   0, POS_ZERO_F32},
    {POS_ZERO_F32, POS_PI_F32,     0, 0xBF800000},
    {NEG_ZERO_F32, NEG_PI_F32,     0, 0xBF800000},
    {POS_ZERO_F32, POS_ZERO_F32,   0, 0x3F800000},
    {NEG_ZERO_F32, NEG_ZERO_F32,   0, 0x3F800000},
    {0xBF800000,   NEG_PI_BY2_F32, 0, POS_ZERO_F32},
    {0xBF800000,   NEG_PI_BY2_F32, 0, NEG_ZERO_F32},
    {0x3F800000,   POS_PI_BY2_F32, 0, POS_ZERO_F32},
    {0x3F800000,   POS_PI_BY2_F32, 0, NEG_ZERO_F32},

    /* Test Inputs of -/+inf */
    {POS_INF_F32, POS_PI_BY2_F32, 0, 0x3F800000},
    {POS_INF_F32, POS_PI_BY2_F32, 0, 0xBF800000},
    {NEG_INF_F32, NEG_PI_BY2_F32, 0, 0x3F800000},
    {NEG_INF_F32, NEG_PI_BY2_F32, 0, 0xBF800000},
    {POS_INF_F32, 0x4016cbe4,     0, NEG_INF_F32},
    {NEG_INF_F32, 0xc016cbe4,     0, NEG_INF_F32},
    {POS_INF_F32, 0x3f490fdb,     0, POS_INF_F32},
    {NEG_INF_F32, 0xbf490fdb,     0, POS_INF_F32},
    {0x3F800000,  POS_PI_F32,     0, NEG_INF_F32},
    {0xBF800000,  NEG_PI_F32,     0, NEG_INF_F32},
    {0x3F800000,  POS_ZERO_F32,   0, POS_INF_F32},
    {0xBF800000,  NEG_ZERO_F32,   0, POS_INF_F32},

    /* Test Inputs that are signalling and quiet NaNs */
    {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID, POS_ZERO_F32},
    {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID, POS_SNAN_F32},
    {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID, POS_SNAN_F32},
    {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID, NEG_SNAN_F32},
    {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID, NEG_SNAN_F32},

    /* Test Inputs that are standard, denormalized and extreme */
    {0x96421590, NEG_ZERO_F32, FE_UNDERFLOW, 0x7EEBE575},
    {0x16421590, POS_ZERO_F32, FE_UNDERFLOW, 0x7EEBE575},
    {0x40080008, 0x3f469296,   0,            0x400aabcd}, // y<x, x>0 and |y|/|x|>0.0625
    {0x400aabcd, 0x34800000,   0,            0x4b0aabcd}, // y<x, x>0 and |y|/|x|<10^-4
    {0x3E000000, 0x3d6c09dc,   0,            0x400aabcd}, // y<x, x>0 and 10^-4<|y|/|x|<0.0625
    {0xc00aabcd, 0xbfe86b51,   0,            0xbf0aabcd}, // y<x, x<0 and |y|/|x|>0.0625
    {0x3862E96F, 0x40490e38,   0,            0xbf0aabcd}, // y<x, x<0 and |y|/|x|<10^-4
    {0x3A83126F, 0x4048f19c,   0,            0xbf0aabcd}, // y<x, x<0 and 10^-4<|y|/|x|<0.0625
    {0x423C0000, 0x3fc0e71d,   0,            0x40400000}, // y>x, x>0 and |x|/|y|>0.0625
    {0x4b0aabcd, 0x3fc90fd9,   0,            0x400aabcd}, // y>x, x>0 and |x|/|y|<10^-4
    {0x42480000, 0x3fc16421,   0,            0x40400000}, // y>x, x>0 and 10^-4<|x|/|y|<0.0625
    {0x40F00000, 0x3fd19529,   0,            0xBF000000}, // y>x, x<0 and |x|/|y|>0.0625
    {0x459C4800, 0x3fc91321,   0,            0xBF000000}, // y>x, x<0 and |x|/|y|<10^-4
    {0x400aabcd, 0x3fc92fdb,   0,            0xbb0aabcd}, // y>x, x<0 and 10^-4<|x|/|y|<0.0625
};

/* Test cases to check the conformance of the atan2() routine. */
static libm_test_special_data_f64
test_atan2_conformance_data[] = {
    /* Test all the intervals in the code, including boundary values */
    {0x3fc999999999999aLL, 0x3fe921fb54442d18, 0, 0x3fc999999999999aLL},
    {0x4380000000000000LL, 0xbff921fb54442d18, 0, 0xbff0000000000000LL},
    {0x0000000000000004LL, POS_ZERO_F64, 0, 0x4020000000000000LL},
    {0x0000100000000000LL, 0x0000000400000000, 0, 0x4090000000000000LL},
    {0x0170000000000000LL, 0x0000000400000000, 0, 0x4270000000000000LL},
    {0x3af0000000000000LL, 0x39b0000000000000, 0, 0x4130000000000000LL},
    {0x3c80000000000000LL, 0xbc60000000000000, 0, 0xc010000000000000LL},
    {0x4024000000000000LL, 0x3fb983e282e2cc4c, 0, 0x4059000000000000LL},
    {0x3e45798ee2308c3aLL, 0x3ddb7cdfd9d7bdbb, 0, 0x4059000000000000LL},
    {0x3f847ae147ae147bLL, 0x3f1a36e2e9a4f662, 0, 0x4059000000000000LL},
    {0x4024000000000000LL, 0xbfb983e282e2cc4c, 0, 0xc059000000000000LL},
    {0x3e45798ee2308c3aLL, 0xbddb7cdfd9d7bdbb, 0, 0xc059000000000000LL},
    {0x3f847ae147ae147bLL, 0xbf1a36e2e9a4f662, 0, 0xc059000000000000LL},
    {0x4059000000000000LL, 0x3ff789bd2c160054, 0, 0x4024000000000000LL},
    {0x4059000000000000LL, 0x3ff921fb543d4de0, 0, 0x3e45798ee2308c3aLL},
    {0x4059000000000000LL, 0x3ff9219278b88684, 0, 0x3f847ae147ae147bLL},
    {0x4059000000000000LL, 0xbff789bd2c160054, 0, 0xc024000000000000LL},
    {0x4059000000000000LL, 0xbff921fb543d4de0, 0, 0xbe45798ee2308c3aLL},
    {0x4059000000000000LL, 0xbff9219278b88684, 0, 0xbf847ae147ae147bLL}, 

    /* Test Inputs of -/+0 */
    {POS_ZERO_F64, 0x400921fb54442d18LL, 0, NEG_ZERO_F64},
    {NEG_ZERO_F64, 0xc00921fb54442d18LL, 0, NEG_ZERO_F64},
    {POS_ZERO_F64, POS_ZERO_F64,         0, POS_ZERO_F64},
    {NEG_ZERO_F64, NEG_ZERO_F64,         0, POS_ZERO_F64},
    {POS_ZERO_F64, 0x400921fb54442d18LL, 0, NEG_ONE_F64},
    {NEG_ZERO_F64, 0xc00921fb54442d18LL, 0, NEG_ONE_F64},
    {POS_ZERO_F64, POS_ZERO_F64,         0, POS_ONE_F64},
    {NEG_ZERO_F64, NEG_ZERO_F64,         0, POS_ONE_F64},
    {NEG_ONE_F64,  0xbff921fb54442d18LL, 0, POS_ZERO_F64},
    {NEG_ONE_F64,  0xbff921fb54442d18LL, 0, NEG_ZERO_F64},
    {POS_ONE_F64,  0x3ff921fb54442d18LL, 0, POS_ZERO_F64},
    {POS_ONE_F64,  0x3ff921fb54442d18LL, 0, NEG_ZERO_F64},

    /* Test Inputs of -/+inf */
    {POS_INF_F64,  0x3ff921fb54442d18LL, 0, POS_ONE_F64},
    {NEG_INF_F64,  0xbff921fb54442d18LL, 0, NEG_ONE_F64},
    {POS_INF_F64,  0x4002d97c7f3321d2LL, 0, NEG_INF_F64},
    {NEG_INF_F64,  0xc002d97c7f3321d2LL, 0, NEG_INF_F64},
    {POS_INF_F64,  0x3fe921fb54442d18LL, 0, POS_INF_F64},
    {NEG_INF_F64,  0xbfe921fb54442d18LL, 0, POS_INF_F64},
    {POS_ONE_F64,  POS_PI_F64,           0, NEG_INF_F64},
    {NEG_ONE_F64,  NEG_PI_F64,           0, NEG_INF_F64},
    {POS_ONE_F64,  POS_ZERO_F64,         0, POS_INF_F64},
    {NEG_ONE_F64,  NEG_ZERO_F64,         0, POS_INF_F64},
    {POS_INF_F64,  POS_PI_BY2_F64,       0, POS_ONE_F64},
    {NEG_INF_F64,  NEG_PI_BY2_F64,       0, POS_ONE_F64},

    /* Test Inputs that are signalling and quiet NaNs */
    {POS_SNAN_F64, POS_SNAN_F64, FE_INVALID, POS_SNAN_F64},
    {NEG_SNAN_F64, NEG_SNAN_F64, FE_INVALID, POS_SNAN_F64},
    {POS_SNAN_F64, POS_SNAN_F64, FE_INVALID, NEG_SNAN_F64},
    {NEG_SNAN_F64, NEG_SNAN_F64, FE_INVALID, NEG_SNAN_F64},
    {POS_SNAN_F64, POS_SNAN_F64, FE_INVALID, POS_ONE_F64},
    {POS_ONE_F64,  POS_SNAN_F64, FE_INVALID, POS_SNAN_F64},

    /* Test Inputs that are standard, denormalized and extreme */
    {0xB1DB0D2835C40259LL, NEG_ZERO_F64,       FE_UNDERFLOW, 0x7FE1CCF385EBC8A0LL},
    {0x31DB0D2835C40259LL, POS_ZERO_F64,       FE_UNDERFLOW, 0x7FE1CCF385EBC8A0LL},
    {0x405fffffffffffffLL, 0x3ff911fb59997f3a, 0,            0x3fe0000000000000LL},
    {0xbfcabcdabcdabcdfLL, 0x80040000001cb936, 0,            0x7fcabcdabc1abcdfLL},
    {0xbfcabcdabcdabcdfLL, 0xc002d97c7f16689d, 0,            0xbfcabcdabc1abcdfLL},
    {0xbfcabcdabcdabcdfLL, 0xc002d97c7f3321d2, 0,            0xbfcabcdabcdabcdfLL},
    {0x3fcabcdabcdabcdfLL, 0x4002d97c7f3321d2, 0,            0xbfcabcdabcdabcdfLL},
    {0x032000a000000000LL, 0x4c9b8ce7d92d8,    0,            0x431abcdabcdabcdfLL},
    {0x6210000000000000LL, 0x3ff921fb54442d18, 0,            0x41aabcdabcdabcdfLL},
    {0x6210000000000000LL, 0x3ff921fb54442d18, 0,            0xc1aabcdabcdabcdfLL},
    {0x3fcabcdabcdabcdfLL, 0x3fe921fb54442d18, 0,            0x3fcabcdabcdabcdfLL},
    {0x3fcabcdabcdabcdfLL, 0x3c70000000000000, 0,            0x434abcdabcdabcdfLL},
    {0x3feabcdabcdabcdfLL, 0x400921fb54442d18, 0,            0xc37abcdabcdabcdfLL},
    {0xbfeabcdabcdabcdfLL, 0xc00921fb54442d18, 0,            0xc37abcdabcdabcdfLL},
    {0x43babcdabcdabcdfLL, 0x3ff921fb54442d18, 0,            0x03cabcdabcdabcdaLL},
    {0x43babcdabcdabcdfLL, 0x3ff921fb54442d18, 0,            0x03c0000000000000LL},
    {0x41aabcdabcdabcdfLL, 0x1f8abcdabcdabcdf, 0,            0x6210000000000000LL},
    {0xc1aabcdabcdabcdfLL, 0x9f8abcdabcdabcdf, 0,            0x6210000000000000LL},
    {0x000FFFFFFFFFFFFFLL, 0x3ff921ff2e48e8a7, 0,            0x0000000000000001LL},
    {0x0000000000000001LL, 0x3cb00001dd9dc7d3, 0,            0x000FFFFFFFFFFFFFLL},

    /* The following is the error case identified by Paul Zimmermann */
    {0x80000000000039a2LL, 0xbe4d0ce6fac85de8, 0, 0x000000fdf0200000LL},
};

#endif /* __TEST_ATAN2_DATA_H__ */
