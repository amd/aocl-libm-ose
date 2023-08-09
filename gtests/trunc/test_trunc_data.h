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

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>
/*
 * Test cases to check for exceptions for the truncf() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_truncf_conformance_data[] = {
    // special accuracy tests
    {POS_SNAN_F32,      POS_QNAN_F32,       FE_INVALID}, // invalid
    {NEG_SNAN_F32,      POS_QNAN_F32,       FE_INVALID}, // invalid
    {POS_SNAN_Q_F32,    POS_QNAN_F32,       0}, // 
    {NEG_SNAN_Q_F32,    POS_QNAN_F32,       0}, // 
    {POS_QNAN_F32,      POS_QNAN_F32,       0}, // 
    {NEG_QNAN_F32,      POS_QNAN_F32,       0}, // 
    {POS_INF_F32,       POS_INF_F32,        0}, // 
    {NEG_INF_F32,       NEG_INF_F32,        0}, // 
    {POS_ONE_F32,       POS_ONE_F32,        0}, // 
    {NEG_ONE_F32,       NEG_ONE_F32,        0}, // 
    {NEG_ZERO_F32,      NEG_ZERO_F32,       0}, // 
    {POS_ZERO_F32,      POS_ZERO_F32,       0}, // 
    {POS_HDENORM_F32,   POS_ZERO_F32,       0}, // 
    {NEG_HDENORM_F32,   NEG_ZERO_F32,       0}, // 
    {POS_LDENORM_F32,   POS_ZERO_F32,       0}, // 
    {NEG_LDENORM_F32,   NEG_ZERO_F32,       0}, //
    {POS_HNORMAL_F32,   POS_HNORMAL_F32,    0}, // 
    {NEG_HNORMAL_F32,   NEG_HNORMAL_F32,    0}, // 
    {POS_LNORMAL_F32,   POS_ZERO_F32,       0}, // 
    {NEG_HNORMAL_F32,   NEG_HNORMAL_F32,    0}, // 
    {POS_PI_F32,        0x40400000,         0}, // 
    {NEG_PI_F32,        0xc0400000,         0}, // 
    {POS_PI_BY2_F32,    0x3f800000,         0}, // 
    {NEG_PI_BY2_F32,    0xbf800000,         0}, // 

    //Some Common Inputs
    {0x3fF12345, 0x3f800000, 0},
    {0xbfF12345, 0xbf800000, 0},
    {0x3fb12345, 0x3f800000, 0},
    {0xbfb12345, 0xbf800000, 0},
    {0x30b12345, 0x00000000, 0},
    {0xb0b12345, 0x80000000, 0},
    {0x400851EB, 0x40000000, 0}, // 
    {0x4b800000, 0x4b800000, 0}, // equal to 2^24
    {0xcb800000, 0xcb800000, 0}, // equal to 2^24
    {0x4b001234, 0x4b001234, 0}, // less than 2^24
    {0xcb001234, 0xcb001234, 0}, // less than 2^24
    {0x4ab70123, 0x4ab70122, 0}, // less than 2^24
    {0xcab70123, 0xcab70122, 0}, // less than 2^24
    {0x4c000000, 0x4c000000, 0}, // greater than 2^24
    {0xcc000000, 0xcc000000, 0}, // greater than 2^24
    {0x43ABCDEF, 0x43ab8000, 0}, // less than 2^24
    {0xC3ABCDEF, 0xc3ab8000, 0}, // less than 2^24
};

static libm_test_special_data_f64
test_trunc_conformance_data[] = {
    // special accuracy tests
    {POS_SNAN_F64,      POS_QNAN_F64,           FE_INVALID}, // invalid
    {NEG_SNAN_F64,      POS_QNAN_F64,           FE_INVALID}, // invalid
    {POS_SNAN_Q_F64,    POS_QNAN_F64,           0}, // 
    {NEG_SNAN_Q_F64,    POS_QNAN_F64,           0}, // 
    {POS_QNAN_F64,      POS_QNAN_F64,           0}, // 
    {NEG_QNAN_F64,      POS_QNAN_F64,           0}, // 
    {POS_INF_F64,       POS_INF_F64,            0}, // 
    {NEG_INF_F64,       NEG_INF_F64,            0}, // 
    {POS_ONE_F64,       POS_ONE_F64,            0}, // 
    {NEG_ONE_F64,       NEG_ONE_F64,            0}, // 
    {NEG_ZERO_F64,      NEG_ZERO_F64,           0}, // 
    {POS_ZERO_F64,      POS_ZERO_F64,           0}, // 
    {POS_HDENORM_F64,   POS_ZERO_F64,           0}, // 
    {NEG_HDENORM_F64,   NEG_ZERO_F64,           0}, // 
    {POS_LDENORM_F64,   POS_ZERO_F64,           0}, // 
    {NEG_LDENORM_F64,   NEG_ZERO_F64,           0}, //
    {POS_HNORMAL_F64,   POS_HNORMAL_F64,        0}, // 
    {NEG_HNORMAL_F64,   NEG_HNORMAL_F64,        0}, // 
    {POS_LNORMAL_F64,   POS_ZERO_F64,           0}, // 
    {NEG_LNORMAL_F64,   NEG_ZERO_F64,           0}, // 
    {POS_PI_F64,        0x4008000000000000LL,   0}, // 
    {NEG_PI_F64,        0xC008000000000000LL,   0}, // 
    {POS_PI_BY2_F64,    0x3ff0000000000000LL,   0}, // 
    {NEG_PI_BY2_F64,    0xbff0000000000000LL,   0}, // 

    // Some Common Inputs
    {0x3Fe1234ABCDEF123LL, 0x0000000000000000LL, 0},
    {0xbFe1234ABCDEF123LL, 0x8000000000000000LL, 0},
    {0x3FeA234ABCDEF123LL, 0x0000000000000000LL, 0},
    {0xbFeA234ABCDEF123LL, 0x8000000000000000LL, 0},
    {0x3FF1234ABCDEF123LL, 0x3ff0000000000000LL, 0},
    {0xbFF1234ABCDEF123LL, 0xbff0000000000000LL, 0},
    {0x3FFA234ABCDEF123LL, 0x3ff0000000000000LL, 0},
    {0xbFFA234ABCDEF123LL, 0xbff0000000000000LL, 0},
    {0x400853451234ABCDLL, 0x4008000000000000LL, 0},
    {0xc00853451234ABCDLL, 0xc008000000000000LL, 0},
    {0x4360056187178ABCLL, 0x4360056187178ABCLL, 0},
    {0xc36ABCDEF0000000LL, 0xc36ABCDEF0000000LL, 0},
    {0x4340000000000000LL, 0x4340000000000000LL, 0},
    {0xc340000000000000LL, 0xc340000000000000LL, 0},
    {0x4351234567891222LL, 0x4351234567891222LL, 0},
    {0xc351234567891222LL, 0xc351234567891222LL, 0},
    {0x433ABCDEF1234567LL, 0x433abcdef1234567LL, 0},
    {0xc33ABCDEF1234567LL, 0xc33abcdef1234567LL, 0},
    {0x431ABCDEF1234567LL, 0x431abcdef1234564LL, 0},
    {0xc31ABCDEF1234567LL, 0xc31abcdef1234564LL, 0},
    {0x411ABCDEF1234567LL, 0x411abcdc00000000LL, 0},
    {0xc11ABCDEF1234567LL, 0xc11abcdc00000000LL, 0},
    {0x400ABCDEF1234567LL, 0x4008000000000000LL, 0},
    {0xc00ABCDEF1234567LL, 0xc008000000000000LL, 0},
    {0x4006BCDEF1234567LL, 0x4000000000000000LL, 0},
    {0xc006BCDEF1234567LL, 0xc000000000000000LL, 0},
};
