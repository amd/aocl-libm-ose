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

#ifndef __TEST_ERFINV_DATA_H__
#define __TEST_ERFINV_DATA_H__

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>

/*
 * Conformance test cases for erfinv() - double precision
 *
 * Test Strategy:
 * 1. Special IEEE 754 values (±0, ±1, ±Inf, NaN)
 * 2. Range boundaries (implementation uses 3 ranges)
 * 3. Representative values within each range
 * 4. Extreme tail values (very close to ±1)
 * 5. Out-of-range values (|x| > 1)
 *
 * Note: Reference values computed using MKL/MPFR library.
 */

static libm_test_special_data_f32
test_erfinvf_conformance_data[] = {
    /* TODO: Add float conformance data */
    /* For now, focusing on double precision tests */
};

static libm_test_special_data_f64
test_erfinv_conformance_data[] = {

    /* ===== Special IEEE 754 Values ===== */
    { POS_ZERO_F64, POS_ZERO_F64, AMD_F_NONE      },  /* erfinv(+0) = +0 */
    { NEG_ZERO_F64, NEG_ZERO_F64, AMD_F_NONE      },  /* erfinv(-0) = -0 */
    { POS_ONE_F64,  POS_INF_F64,  AMD_F_DIVBYZERO },  /* erfinv(+1) = +Inf, raises DIVBYZERO */
    { NEG_ONE_F64,  NEG_INF_F64,  AMD_F_DIVBYZERO },  /* erfinv(-1) = -Inf, raises DIVBYZERO */

    /* ===== NaN and Infinity ===== */
    { POS_INF_F64,  POS_QNAN_F64, AMD_F_INVALID   },  /* erfinv(+Inf) = QNaN, raises INVALID */
    { NEG_INF_F64,  NEG_QNAN_F64, AMD_F_INVALID   },  /* erfinv(-Inf) = QNaN, raises INVALID */
    { POS_QNAN_F64, POS_QNAN_F64, AMD_F_NONE      },  /* erfinv(+QNaN) = +QNaN */
    { NEG_QNAN_F64, NEG_QNAN_F64, AMD_F_NONE      },  /* erfinv(-QNaN) = -QNaN */
    /* TODO: Need to enquire about SNAN handling */
    //{ POS_SNAN_F64, POS_QNAN_F64, AMD_F_INVALID   }, /* erfinv(+SNaN) = +QNaN, raises INVALID */
    //{ NEG_SNAN_F64, NEG_QNAN_F64, AMD_F_INVALID   }, /* erfinv(-SNaN) = -QNaN, raises INVALID */

    /* ===== Out of Range: |x| > 1 ===== */
    { 0x3FF0000000000001,  POS_QNAN_F64, AMD_F_INVALID },  /* erfinv(1 + eps) = QNaN, |x| > 1 */
    { 0xBFF0000000000001,  POS_QNAN_F64, AMD_F_INVALID },  /* erfinv(-1 - eps) = -QNaN, |x| > 1 */
    { 0x3FF199999999999A,  POS_QNAN_F64, AMD_F_INVALID },  /* erfinv(1.1) = QNaN */
    { 0xBFF199999999999A,  POS_QNAN_F64, AMD_F_INVALID },  /* erfinv(-1.1) = QNaN */
    { 0x4000000000000000,  POS_QNAN_F64, AMD_F_INVALID },  /* erfinv(2.0) = QNaN */
    { 0xC000000000000000,  POS_QNAN_F64, AMD_F_INVALID },  /* erfinv(-2.0) = QNaN */

    /* ===== Range 1: |x| <= 0.75 ===== */
    /* Small values near zero */
    { 0x3E45798EE2308C3A,  0x3E45798EE2308C3A, AMD_F_NONE },  /* erfinv(1e-8) ≈ 8.86e-9 */
    { 0xBE45798EE2308C3A,  0xBE45798EE2308C3A, AMD_F_NONE },  /* erfinv(-1e-8) ≈ -8.86e-9 */
    { 0x3F847AE147AE147B,  0x3F822680C94CADD4, AMD_F_NONE },  /* erfinv(0.01) ≈ 0.00886 */
    { 0xBF847AE147AE147B,  0xBF822680C94CADD4, AMD_F_NONE },  /* erfinv(-0.01) ≈ -0.00886 */

    /* Mid-range values */
    { 0x3FB999999999999A,  0x3FB6BF442539FD74, AMD_F_NONE },  /* erfinv(0.1) ≈ 0.0889 */
    { 0xBFB999999999999A,  0xBFB6BF442539FD74, AMD_F_NONE },  /* erfinv(-0.1) ≈ -0.0889 */
    { 0x3FD0000000000000,  0x3FCCD70681D5FF70, AMD_F_NONE },  /* erfinv(0.25) ≈ 0.225 */
    { 0xBFD0000000000000,  0xBFCCD70681D5FF70, AMD_F_NONE },  /* erfinv(-0.25) ≈ -0.225 */
    { 0x3FE0000000000000,  0x3FDE861FBB24C00A, AMD_F_NONE },  /* erfinv(0.5) ≈ 0.477 */
    { 0xBFE0000000000000,  0xBFDE861FBB24C00A, AMD_F_NONE },  /* erfinv(-0.5) ≈ -0.477 */
    { 0x3FE6666666666666,  0x3FE773A9DA42B44F, AMD_F_NONE },  /* erfinv(0.7) ≈ 0.7329 */
    { 0xBFE6666666666666,  0xBFE773A9DA42B44F, AMD_F_NONE },  /* erfinv(-0.7) ≈ -0.7329 */

    /* ===== Range 1/2 Boundary: |x| = 0.75 ===== */
    { 0x3FE8000000000000,  0x3FEA07890F6B2BA1, AMD_F_NONE },  /* erfinv(0.75) ≈ 0.8134 */
    { 0xBFE8000000000000,  0xBFEA07890F6B2BA1, AMD_F_NONE },  /* erfinv(-0.75) ≈ -0.8134 */
    { 0x3FE7FFFFFFFFFFFE,  0x3FEA07890F6B2B9E, AMD_F_NONE },  /* erfinv(0.75 - ulp) ≈ 0.8134 */
    { 0x3FE8000000000001,  0x3FEA07890F6B2BA3, AMD_F_NONE },  /* erfinv(0.75 + ulp) ≈ 0.8134 */

    /* ===== Range 2: 0.75 < |x| <= 0.9375 ===== */
    { 0x3FE999999999999A,  0x3FECFF8A2529BAB5, AMD_F_NONE },  /* erfinv(0.8) ≈ 0.9062 */
    { 0xBFE999999999999A,  0xBFECFF8A2529BAB5, AMD_F_NONE },  /* erfinv(-0.8) ≈ -0.9062 */
    { 0x3FEB333333333333,  0x3FF04954184F8AA5, AMD_F_NONE },  /* erfinv(0.85) ≈ 1.018 */
    { 0xBFEB333333333333,  0xBFF04954184F8AA5, AMD_F_NONE },  /* erfinv(-0.85) ≈ -1.018 */
    { 0x3FECCCCCCCCCCCCD,  0x3FF29C0146770651, AMD_F_NONE },  /* erfinv(0.9) ≈ 1.163 */
    { 0xBFECCCCCCCCCCCCD,  0xBFF29C0146770651, AMD_F_NONE },  /* erfinv(-0.9) ≈ -1.163 */
    { 0x3FEDCCCCCCCCCCCD,  0x3FF497744F536BCC, AMD_F_NONE },  /* erfinv(0.93) ≈ 1.287 */
    { 0xBFEDCCCCCCCCCCCD,  0xBFF497744F536BCC, AMD_F_NONE },  /* erfinv(-0.93) ≈ -1.287 */

    /* ===== Range 2/3 Boundary: |x| = 0.9375 ===== */
    { 0x3FEE000000000000,  0x3FF5130C3ACAE4DE, AMD_F_NONE },  /* erfinv(0.9375) ≈ 1.317 */
    { 0xBFEE000000000000,  0xBFF5130C3ACAE4DE, AMD_F_NONE },  /* erfinv(-0.9375) ≈ -1.317 */
    { 0x3FEDFFFFFFFFFFFF,  0x3FF5130C3ACAE4DC, AMD_F_NONE },  /* erfinv(0.9375 - ulp) = 1.317 */
    { 0x3FEE000000000001,  0x3FF5130C3ACAE4E1, AMD_F_NONE },  /* erfinv(0.9375 + ulp) = 1.317 */

    /* ===== Range 3: 0.9375 < |x| < 1 (Tail Region) ===== */
    { 0x3FEE666666666666,  0x3FF62CA97D0FBB2A, AMD_F_NONE },  /* erfinv(0.95) ≈ 1.386 */
    { 0xBFEE666666666666,  0xBFF62CA97D0FBB2A, AMD_F_NONE },  /* erfinv(-0.95) ≈ -1.386 */
    { 0x3FEF5C28F5C28F5C,  0x3FFA51D2BA8948B8, AMD_F_NONE },  /* erfinv(0.98) ≈ 1.645 */
    { 0xBFEF5C28F5C28F5C,  0xBFFA51D2BA8948B8, AMD_F_NONE },  /* erfinv(-0.98) ≈ -1.645 */
    { 0x3FEFAE147AE147AE,  0x3FFD2466082BB20E, AMD_F_NONE },  /* erfinv(0.99) ≈ 1.8214 */
    { 0xBFEFAE147AE147AE,  0xBFFD2466082BB20E, AMD_F_NONE },  /* erfinv(-0.99) ≈ -1.8214 */

    /* ===== Extreme Tail: Very Close to ±1 ===== */
    { 0x3FEFF7CED916872B,  0x40029D311405CDB4, AMD_F_NONE },  /* erfinv(0.999) ≈ 2.327 */
    { 0xBFEFF7CED916872B,  0xC0029D311405CDB4, AMD_F_NONE },  /* erfinv(-0.999) ≈ -2.327 */
    { 0x3FEFFF2E48E8A71E,  0x4006022DCB01EA50, AMD_F_NONE },  /* erfinv(0.9999) ≈ 2.751 */
    { 0xBFEFFF2E48E8A71E,  0xC006022DCB01EA50, AMD_F_NONE },  /* erfinv(-0.9999) ≈ -2.751 */
    { 0x3FEFFFEB074A771D,  0x4008FCC0194983BC, AMD_F_NONE },  /* erfinv(0.99999) ≈ 3.123 */
    { 0xBFEFFFEB074A771D,  0xC008FCC0194983BC, AMD_F_NONE },  /* erfinv(-0.99999) ≈ -3.123 */

    /* ===== Near BOUND3 (1-1e-100) Region ===== */
    /* erfinv(0.9999998807907104) ≈ 3.7439 */
    { 0x3FEFFFFFC0000000,  0x400DF38CF0476DDE, AMD_F_NONE },
    /* erfinv(-0.9999998807907104) ≈ -3.7439 */
    { 0xBFEFFFFFC0000000,  0xC00DF38CF0476DDE, AMD_F_NONE },
    /* erfinv(0.9999998956918716) ≈ 3.761 */
    { 0x3FEFFFFFC8000000,  0x400E16D027B5D198, AMD_F_NONE },
    /* erfinv(nextafter(1.0, 0.0)) ≈ 5.8636 OR erfinv(0.9999999999999999) ≈ 5.8636 */
    { 0x3FEFFFFFFFFFFFFF,  0x4017744F8F74E94A, AMD_F_NONE },
    /* erfinv(-0.9999998956918716) ≈ -3.761 */
    { 0xBFEFFFFFC8000000,  0xC00E16D027B5D198, AMD_F_NONE },
    /* erfinv(-nextafter(1.0, 0.0)) ≈ -5.864 OR erfinv(-0.9999999999999999) ≈ -5.8636 */
    { 0xBFEFFFFFFFFFFFFF,  0xC017744F8F74E94A, AMD_F_NONE },

    /* ===== Denormal and Very Small Values ===== */
    /* erfinv(min_denorm) = min_denorm */
    { 0x0000000000000001,  0x0000000000000001, AMD_F_NONE },
    /* erfinv(-min_denorm) = -min_denorm */
    { 0x8000000000000001,  0x8000000000000001, AMD_F_NONE },
    /* erfinv(max_denorm) */
    { 0x000FFFFFFFFFFFFF,  0x000E2DFC48DA77B4, AMD_F_NONE },
    /* erfinv(-max_denorm) */
    { 0x800FFFFFFFFFFFFF,  0x800E2DFC48DA77B4, AMD_F_NONE },

    /* ===== Additional Edge Cases ===== */
    /* Very close to 1.0 but safely within BOUND3; erfinv(0.9999999998835847) ≈ 4.5566 */
    { 0x3FEFFFFFFFF00000,  0x401239E830EBA467, AMD_F_NONE },
    /* Even closer to 1.0; erfinv(0.9999999999990905) ≈ 5.0513 */
    { 0x3FEFFFFFFFFFE000,  0x4014347BF36FBAE8, AMD_F_NONE },

};

#endif  /*__TEST_ERFINV_DATA_H___*/
