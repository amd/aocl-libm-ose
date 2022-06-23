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
 * Test cases to check for exceptions for the floorf() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_floorf_conformance_data[] = {
    // special accuracy tests
    {POS_SNAN_F32,      POS_QNAN_F32,       FE_INVALID}, // invalid
    {NEG_SNAN_F32,      POS_QNAN_F32,       FE_INVALID}, // invalid
    {POS_SNAN_Q_F32,    POS_QNAN_F32,       0},
    {NEG_SNAN_Q_F32,    POS_QNAN_F32,       0},
    {POS_QNAN_F32,      POS_QNAN_F32,       0},
    {NEG_QNAN_F32,      POS_QNAN_F32,       0},
    {POS_INF_F32,       POS_INF_F32,        0},
    {NEG_INF_F32,       NEG_INF_F32,        0},
    {POS_ONE_F32,       POS_ONE_F32,        0},
    {NEG_ONE_F32,       NEG_ONE_F32,        0},
    {NEG_ZERO_F32,      NEG_ZERO_F32,       0},
    {POS_ZERO_F32,      POS_ZERO_F32,       0},
    {POS_HDENORM_F32,   POS_ZERO_F32,       0},
    {NEG_HDENORM_F32,   NEG_ONE_F32,        0},
    {POS_LDENORM_F32,   POS_ZERO_F32,       0},
    {NEG_LDENORM_F32,   NEG_ONE_F32,        0},
    {POS_HNORMAL_F32,   POS_HNORMAL_F32,    0},
    {NEG_HNORMAL_F32,   NEG_HNORMAL_F32,    0},
    {POS_LNORMAL_F32,   POS_ZERO_F32,       0},
    {NEG_HNORMAL_F32,   NEG_HNORMAL_F32,    0},
    {POS_PI_F32,        0x40400000,         0},
    {NEG_PI_F32,        0xc0800000,         0},
    {POS_PI_BY2_F32,    0x3f800000,         0},
    {NEG_PI_BY2_F32,    0xc0000000,         0},
    {0x400851EB,        0x40000000,         0},
    {0x4b800000,        0x4b800000,         0}, // equal to 2^24
    {0x4c000000,        0x4c000000,         0}, // greater than 2^24
};

static libm_test_special_data_f64
test_floor_conformance_data[] = {
    // special accuracy tests
    {POS_SNAN_F64,      POS_QNAN_F64,           FE_INVALID}, // invalid
    {NEG_SNAN_F64,      POS_QNAN_F64,           FE_INVALID}, // invalid
    {POS_SNAN_Q_F64,    POS_QNAN_F64,           0},
    {NEG_SNAN_Q_F64,    POS_QNAN_F64,           0},
    {POS_QNAN_F64,      POS_QNAN_F64,           0},
    {NEG_QNAN_F64,      POS_QNAN_F64,           0},
    {POS_INF_F64,       POS_INF_F64,            0},
    {NEG_INF_F64,       NEG_INF_F64,            0},
    {POS_ONE_F64,       POS_ONE_F64,            0},
    {NEG_ONE_F64,       NEG_ONE_F64,            0},
    {NEG_ZERO_F64,      NEG_ZERO_F64,           0},
    {POS_ZERO_F64,      POS_ZERO_F64,           0},
    {POS_HDENORM_F64,   POS_ZERO_F64,           0},
    {NEG_HDENORM_F64,   NEG_ONE_F64,            0},
    {POS_LDENORM_F64,   POS_ZERO_F64,           0},
    {NEG_LDENORM_F64,   NEG_ONE_F64,            0},
    {POS_HNORMAL_F64,   POS_HNORMAL_F64,        0},
    {NEG_HNORMAL_F64,   NEG_HNORMAL_F64,        0},
    {POS_LNORMAL_F64,   POS_ZERO_F64,           0},
    {NEG_LNORMAL_F64,   NEG_ZERO_F64,           0},
    {POS_PI_F64,        0x4008000000000000LL,   0},
    {NEG_PI_F64,        0xC010000000000000LL,   0},
    {POS_PI_BY2_F64,    0x3fF0000000000000LL,   0},
    {NEG_PI_BY2_F64,    0xC000000000000000LL,   0},
};

