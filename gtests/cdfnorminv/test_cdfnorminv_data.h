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

#ifndef __TEST_CDFNORMINV_DATA_H__
#define __TEST_CDFNORMINV_DATA_H__

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>

/*
 * Test cases to check for exceptions for the cdfnorminv() routine.
 * These test cases are not exhaustive
 */
 static libm_test_special_data_f32
 test_cdfnorminvf_conformance_data[] = {
 
 };

static libm_test_special_data_f64
test_cdfnorminv_conformance_data[] = {
    /* Special boundary values */
    {POS_ZERO_F64, NEG_INF_F64, 0},                        // cdfnorminv(0) = -inf
    {NEG_ZERO_F64, NEG_INF_F64, 0},                        // cdfnorminv(-0) = -inf
    {0x3FF0000000000000ULL, POS_INF_F64, 0},               // cdfnorminv(1) = +inf
    {0x3FE0000000000000ULL, POS_ZERO_F64, 0},              // cdfnorminv(0.5) = 0

    /* NaN and invalid inputs */
    {POS_QNAN_F64, POS_QNAN_F64, 0},                       // cdfnorminv(+QNAN) = QNAN
    {NEG_QNAN_F64, NEG_QNAN_F64, 0},                       // cdfnorminv(-QNAN) = -QNAN
    {POS_SNAN_F64, POS_QNAN_F64, FE_INVALID},              // cdfnorminv(+SNAN) = QNAN
    {NEG_SNAN_F64, NEG_QNAN_F64, FE_INVALID},              // cdfnorminv(-SNAN) = -QNAN
    {POS_INF_F64, NEG_QNAN_F64, FE_INVALID},               // cdfnorminv(+inf) = -QNAN
    {NEG_INF_F64, NEG_QNAN_F64, FE_INVALID},               // cdfnorminv(-inf) = -QNAN

    /* Out of domain values */
    {0x8000000000000001ULL, NEG_QNAN_F64, FE_INVALID},     // cdfnorminv(-min_denormal) = -QNAN
    {0xBFF0000000000000ULL, NEG_QNAN_F64, FE_INVALID},     // cdfnorminv(-1) = -QNAN
    {0x4000000000000000ULL, NEG_QNAN_F64, FE_INVALID},     // cdfnorminv(2) = -QNAN

    /* cdfnorminv(x) for denormals/small values */
    {0x0000000000000001ULL, 0xC0433BD3F27FCD03ULL, 0},  // cdfnorminv(min_denormal)
    {0x000FFFFFFFFFFFFFULL, 0xC042C27B05BF1A0BULL, 0},  // cdfnorminv(max_denormal)
    {0x0010000000000000ULL, 0xC042C27B05BF1A0BULL, 0},  // cdfnorminv(min_normal)

    /* cdfnorminv(x) ≈ 0.5 */
    {0x3FDFFFFFFFFFFFFFULL, 0xBCA40D931FF62706ULL, 0},     // cdfnorminv(0.5 - 1 ULP)
    {0x3FE0000000000000ULL, 0x0000000000000000ULL, 0},     // cdfnorminv(0.5) = 0
    {0x3FE0000000000001ULL, 0x3CB40D931FF62706ULL, 0},     // cdfnorminv(0.5 + 1 ULP)

    /* cdfnorminv(x) ≈ 1 */
    {0x3FEFFFFFFFFFFFFFULL, 0x40206B48528CEA52ULL, 0},  // cdfnorminv(value just before 1.0)

    /* Test cases around the transition points between approximation intervals */

    /* Central/Tail boundary: p ≈ 0.075 */
    {0x3FB3333333333332ULL, 0xBFF7085226D3E525ULL, 0},  // p just below 0.075 (tail region)
    {0x3FB3333333333333ULL, 0xBFF7085226D3E524ULL, 0},  // p = 0.075 (boundary - central region)
    {0x3FB3333333333334ULL, 0xBFF7085226D3E524ULL, 0},  // p just above 0.075 (central region)

    /* Central/Tail boundary: p ≈ 0.925 */
    {0x3FED999999999999ULL, 0x3FF7085226D3E522ULL, 0},  // p just below 0.925 (central region)
    {0x3FED99999999999AULL, 0x3FF7085226D3E525ULL, 0},  // p = 0.925 (boundary - central region)
    {0x3FED99999999999BULL, 0x3FF7085226D3E529ULL, 0},  // p just above 0.925 (tail region)

    /* Lower Tail/Extreme boundary: p = exp(-25) */
    {0x3DAE8A37A45FC32EULL, 0xC01AA1B1C13EE526ULL, 0},  // p = exp(-25) (tail/extreme boundary)

    /* Upper tail/extreme boundary: p = 1 - exp(-25) */
    {0x3FEFFFFFFFFE175DULL, 0x401AA1B1E6EAB81FULL, 0},  // p = 1-exp(-25) (tail/extreme boundary)
};

#endif  /*__TEST_CDFNORMINV_DATA_H__*/
