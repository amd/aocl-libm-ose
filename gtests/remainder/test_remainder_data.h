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
 * Test cases to check for exceptions for the remainderf() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_remainderf_conformance_data[] = {
    {0x41200000, 0x3F800000, 0x00000, 0x40400000},
    {0xffdbc0b7, 0x7fc00311, 0, 0x8367e3b0},
    // Constant inputs from the Constants.h},
    {POS_SNAN_F32, POS_QNAN_F32, FE_INVALID, POS_SNAN_F32},
    {NEG_SNAN_F32, POS_QNAN_F32, FE_INVALID, POS_QNAN_F32},
    {POS_SNAN_Q_F32, POS_QNAN_F32, 0, NEG_QNAN_F32},
    {NEG_SNAN_Q_F32, POS_QNAN_F32, 0, POS_QNAN_F32},
    {POS_QNAN_F32, POS_QNAN_F32, 0, POS_QNAN_F32},
    {NEG_QNAN_F32, POS_QNAN_F32, FE_INVALID, POS_SNAN_F32},
    {POS_INF_F32, POS_QNAN_F32, FE_INVALID, POS_INF_F32},
    {NEG_INF_F32, POS_QNAN_F32, FE_INVALID, NEG_INF_F32},
    {NEG_INF_F32, POS_QNAN_F32, FE_INVALID, NEG_SNAN_F32},
    {NEG_INF_F32, POS_QNAN_F32, FE_INVALID, NEG_QNAN_F32},
    {POS_INF_F32, POS_QNAN_F32, FE_INVALID, POS_ONE_F32},
    {NEG_INF_F32, POS_QNAN_F32, FE_INVALID, NEG_ONE_F32},
    {POS_INF_F32, POS_QNAN_F32, FE_INVALID, POS_ZERO_F32},
    {NEG_INF_F32, POS_QNAN_F32, FE_INVALID, NEG_ZERO_F32},
    {POS_ONE_F32, POS_QNAN_F32, FE_INVALID, POS_SNAN_F32},
    {NEG_ONE_F32, NEG_QNAN_F32, 0, NEG_QNAN_F32},
    {POS_ZERO_F32, POS_QNAN_F32, FE_INVALID, POS_SNAN_F32},
    {NEG_ZERO_F32, NEG_QNAN_F32, 0, NEG_QNAN_F32},
    // Input as High denormal},
    {POS_HDENORM_F32, 0x00000000, 0, POS_HDENORM_F32},
    {POS_HDENORM_F32, 0x00000000, 0, NEG_HDENORM_F32},
    {POS_HDENORM_F32, 0x00000000, 0, POS_LDENORM_F32},
    {POS_HDENORM_F32, 0x00000000, 0, NEG_LDENORM_F32},
    {POS_LDENORM_F32, 0x00000001, 0, POS_HDENORM_F32}, //EXC_CHK_UNFL
    {POS_LDENORM_F32, 0x00000001, 0, NEG_HDENORM_F32}, //EXC_CHK_UNFL
    {POS_LDENORM_F32, 0x00000000, 0, POS_LDENORM_F32},
    {POS_LDENORM_F32, 0x00000000, 0, NEG_LDENORM_F32},
    {POS_HDENORM_F32, 0x80000001, 0, POS_LNORMAL_F32}, //EXC_CHK_UNFL
    {POS_LDENORM_F32, 0x00000001, 0, NEG_HNORMAL_F32}, //EXC_CHK_UNFL
    {NEG_HNORMAL_F32, 0x80000000, 0, POS_LDENORM_F32},
    {POS_LNORMAL_F32, 0x00000001, 0, NEG_HDENORM_F32}, //EXC_CHK_UNFL
    {POS_HDENORM_F32, 0x007fffff, 0, POS_ONE_F32}, //EXC_CHK_UNFL
    {POS_LDENORM_F32, 0x00000001, 0, NEG_ONE_F32}, //EXC_CHK_UNFL
    {POS_HDENORM_F32, 0x7fc00000, FE_INVALID, POS_ZERO_F32},
    {POS_LDENORM_F32, 0x7fc00000, FE_INVALID, NEG_ZERO_F32},
    {POS_HNORMAL_F32, 0x00000000, 0, POS_ONE_F32},
    {POS_LNORMAL_F32, 0x00800000, 0, NEG_ONE_F32},
    {POS_HNORMAL_F32, 0x7fc00000, FE_INVALID, POS_ZERO_F32},
    {POS_LNORMAL_F32, 0x7fc00000, FE_INVALID, NEG_ZERO_F32},
    {POS_PI_F32, 0x00000000, 0, NEG_PI_F32},
    {NEG_PI_BY2_F32, 0x80000000, 0, POS_PI_BY2_F32},
    // Special cases in IEEE doc},
    // remainder(x,,, inf) for finite normal x},
    {POS_ONE_F32, POS_ONE_F32, 0, POS_INF_F32},
    {NEG_ONE_F32, NEG_ONE_F32, 0, POS_INF_F32},
    {0x80000000, 0x80000000, 0, 0xa8117a2e},
};

static libm_test_special_data_f64
test_remainder_conformance_data[] = {
    {0xfff6411204576b2b, 0xc1cbe9622d66ff38, 0x7ff8000000000001, FE_INVALID},
    {POS_SNAN_F64, POS_SNAN_F64, POS_QNAN_F64, FE_INVALID}, // invalid
    {NEG_SNAN_F64, POS_QNAN_F64, POS_QNAN_F64, FE_INVALID}, // invalid
    {POS_SNAN_Q_F64, NEG_QNAN_F64, POS_QNAN_F64, 0}, //
    {NEG_SNAN_Q_F64, POS_QNAN_F64, POS_QNAN_F64, 0}, //
    {POS_QNAN_F64, POS_QNAN_F64, POS_QNAN_F64, 0}, //
    {NEG_QNAN_F64, POS_SNAN_F64, POS_QNAN_F64, FE_INVALID}, //
    {POS_INF_F64, POS_INF_F64, POS_QNAN_F64, FE_INVALID}, //
    {NEG_INF_F64, NEG_INF_F64, NEG_QNAN_F64, FE_INVALID}, //
    {NEG_INF_F64, NEG_SNAN_F64, NEG_QNAN_F64, FE_INVALID}, //
    {NEG_INF_F64, NEG_QNAN_F64, NEG_QNAN_F64, 0}, //// Refer Section 7.2 point f of document IEEE 754-2008
    {POS_INF_F64, POS_ONE_F64, POS_QNAN_F64, FE_INVALID}, //
    {NEG_INF_F64, NEG_ONE_F64, NEG_QNAN_F64, FE_INVALID}, //
    {POS_INF_F64, POS_ZERO_F64, POS_QNAN_F64, FE_INVALID}, //
    {NEG_INF_F64, NEG_ZERO_F64, NEG_QNAN_F64, FE_INVALID}, //
    {POS_ONE_F64, POS_SNAN_F64, POS_QNAN_F64, FE_INVALID}, //
    {NEG_ONE_F64, NEG_QNAN_F64, NEG_QNAN_F64, 0}, //
    {POS_ZERO_F64, POS_SNAN_F64, POS_QNAN_F64, FE_INVALID}, //
    {NEG_ZERO_F64, NEG_QNAN_F64, NEG_QNAN_F64, 0}, //
    /*Input as High denormal*/
    {POS_HDENORM_F64, POS_HDENORM_F64, 0x0000000000000000LL, 0},
    {POS_HDENORM_F64, NEG_HDENORM_F64, 0x0000000000000000LL, 0},
    {POS_HDENORM_F64,0x000000000002711ELL, 0x800000000000e907LL, 0}, // EXC_CHK_UNFL},}, //TBD
    {POS_HDENORM_F64, POS_LDENORM_F64, 0x0000000000000000LL, 0},
    {POS_HDENORM_F64, NEG_LDENORM_F64, 0x0000000000000000LL, 0},//this can raise an exception
    {POS_LDENORM_F64, POS_HDENORM_F64, 0x0000000000000001LL, 0}, // EXC_CHK_UNFL},},
    {POS_LDENORM_F64, NEG_HDENORM_F64, 0x0000000000000001LL, 0}, // EXC_CHK_UNFL},},
    {POS_LDENORM_F64, POS_LDENORM_F64, 0x0000000000000000LL, 0},
    {POS_LDENORM_F64, NEG_LDENORM_F64, 0x0000000000000000LL, 0},
    {POS_HDENORM_F64, POS_LNORMAL_F64, 0x8000000000000001LL, 0}, // EXC_CHK_UNFL},},//TBD
    {NEG_LDENORM_F64, NEG_HNORMAL_F64, 0x8000000000000001LL, 0},
    {POS_LDENORM_F64, NEG_HNORMAL_F64, 0x0000000000000001LL, 0},
    {POS_HNORMAL_F64, POS_LDENORM_F64, 0x0000000000000000LL, 0},//TBD
    {POS_LNORMAL_F64, NEG_HDENORM_F64, 0x0000000000000001LL, 0}, // EXC_CHK_UNFL},},//TBD
    {POS_HDENORM_F64, POS_ONE_F64, 0x000fffffffffffffLL, 0},
    {POS_LDENORM_F64, NEG_ONE_F64, 0x0000000000000001LL, 0},
    {POS_HDENORM_F64, POS_ZERO_F64, 0xfff8000000000000LL, FE_INVALID},
    {POS_LDENORM_F64, NEG_ZERO_F64, 0xfff8000000000000LL, FE_INVALID},
    {POS_HNORMAL_F64, POS_ONE_F64, 0x0000000000000000LL, 0},
    {POS_LNORMAL_F64, NEG_ONE_F64, 0x0010000000000000LL, 0},
    {POS_HNORMAL_F64, POS_ZERO_F64, 0xfff8000000000000LL, FE_INVALID},
    {POS_LNORMAL_F64, NEG_ZERO_F64, 0xfff8000000000000LL, FE_INVALID},
    {POS_PI_F64, NEG_PI_F64, 0x0000000000000000LL, 0}, //
    {NEG_PI_BY2_F64, POS_PI_BY2_F64, 0x8000000000000000LL, 0}, //
    //Special cases in IEEE doc
    //remainder(x, inf) for finite normal x
    {POS_ONE_F64, POS_INF_F64, POS_ONE_F64, 0},
    {NEG_ONE_F64, POS_INF_F64, NEG_ONE_F64, 0},
    {0x8000000000000000LL, 0xa8117a2e00000000LL, 0x8000000000000000LL, 0},
};
