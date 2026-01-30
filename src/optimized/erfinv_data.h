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

/*
 * Shared polynomial coefficients for erfinv vector implementations
 *
 * This header file contains the polynomial coefficients for computing
 * the inverse error function (erfinv) using rational Chebyshev approximations
 * by Blair et al. (1976).
 *
 * The coefficients are organized into three ranges:
 *   Range 1: |x| <= 0.75          (Table 17 - Degree 7)
 *   Range 2: 0.75 < |x| <= 0.9375 (Table 37 - Degree 8)
 *   Range 3: 0.9375 < |x| < 1.0   (Table 58 - Degree 11/9)
 *
 * Usage:
 *   Include this file in vector erfinv implementations (vrd2, vrd4, vrd8)
 *   and use the ERFINV_* macros to access coefficients.
 */

#ifndef _ERFINV_DATA_H_
#define _ERFINV_DATA_H_

/* Scalar polynomial coefficients - shared across all vector variants */

/* Range 1: Table 17 by Blair et al (|x| <= 0.75) */
/* P(x) numerator coefficients */
#define ERFINV_P100  0x1.007ce8f01b2e8p+4   /* 16.030437513509648 */
#define ERFINV_P101 -0x1.6b23cc5c6c6d7p+6   /* -90.7844015750619 */
#define ERFINV_P102  0x1.74e5f6ceb3548p+7   /* 186.79576343368022 */
#define ERFINV_P103 -0x1.5200bb15cc6bbp+7   /* -170.00583102571826 */
#define ERFINV_P104  0x1.05d193233a849p+6   /* 65.45340895431265 */
#define ERFINV_P105 -0x1.148c5474ee5e1p+3   /* -8.64030806245068 */
#define ERFINV_P106  0x1.689181bbafd0cp-3   /* 0.17619506628002433 */

/* Q(x) denominator coefficients */
#define ERFINV_Q100  0x1.d8fb0f913bd7bp+3   /* 14.78162304227907 */
#define ERFINV_Q101 -0x1.6d7f25a3f1c24p+6   /* -91.62191602897779 */
#define ERFINV_Q102  0x1.a450d8e7f4cbbp+7   /* 210.38307951120886 */
#define ERFINV_Q103 -0x1.bc34804858570p+7   /* -221.81451430479932 */
#define ERFINV_Q104  0x1.ae6b0c504ee02p+6   /* 107.67919759957815 */
#define ERFINV_Q105 -0x1.499dfec1a7f5fp+4   /* -20.60025843015807 */
#define ERFINV_Q106  0x1.0p+0                /* 1.0 */

/* Range 2: Table 37 by Blair et al (0.75 < |x| <= 0.9375) */
/* P(x) numerator coefficients */
#define ERFINV_P200 -0x1.f3596123109edp-7   /* -0.01528434079932769 */
#define ERFINV_P201  0x1.60b8fe375999ep-2   /* 0.3446360224925675 */
#define ERFINV_P202 -0x1.779bb9bef7c0fp+1   /* -2.968688929607417 */
#define ERFINV_P203  0x1.786ea384470a2p+3   /* 11.78882578095403 */
#define ERFINV_P204 -0x1.6a7c1453c85d3p+4   /* -22.624453939640686 */
#define ERFINV_P205  0x1.31f0fc5613142p+4   /* 19.121334849082184 */
#define ERFINV_P206 -0x1.5ea6c007d4dbbp+2   /* -5.479645012587244 */
#define ERFINV_P207  0x1.e66f265ce9e50p-3   /* 0.23750830451264894 */

/* Q(x) denominator coefficients */
#define ERFINV_Q200 -0x1.636b2dcf4edbep-7   /* -0.010936177270211595 */
#define ERFINV_Q201  0x1.0b5411e2acf29p-2   /* 0.2598801583138086 */
#define ERFINV_Q202 -0x1.3413109467a0bp+1   /* -2.4073837271163446 */
#define ERFINV_Q203  0x1.563e8136c554ap+3   /* 10.77553798396105 */
#define ERFINV_Q204 -0x1.7b77aab1dcafbp+4   /* -23.71694448906913 */
#define ERFINV_Q205  0x1.8a3e174e05ddcp+4   /* 24.62198012374947 */
#define ERFINV_Q206 -0x1.4075c56404eecp+3   /* -10.03073011444318 */
#define ERFINV_Q207  0x1.0000000000000p+0   /* 1.0 */

/* Range 3: Table 58 by Blair et al (0.9375 < |x| < 1.0) */
/* P(z) numerator coefficients, where z = sqrt(-log1p(-|x|)) */
#define ERFINV_P300  0x1.d98db2f393c93p-15  /* 5.641895835477563e-05 */
#define ERFINV_P301  0x1.5ea5105db7d1bp-8   /* 0.005353579106084854 */
#define ERFINV_P302  0x1.099dcb79c5e37p-3   /* 0.12975814683063994 */
#define ERFINV_P303  0x1.0ae8df6736f5ap+0   /* 1.0423950043314906 */
#define ERFINV_P304  0x1.6a463706dce53p+1   /* 2.8314511320593495 */
#define ERFINV_P305  0x1.501296c195ce7p+1   /* 2.625318246400484 */
#define ERFINV_P306  0x1.0a1bd4227162ep+1   /* 1.0394924782066213 */
#define ERFINV_P307  0x1.7451fe635fbb3p-1   /* 0.36431926301906126 */
#define ERFINV_P308  0x1.11ae803f200b1p-4   /* 0.06717962035667986 */
#define ERFINV_P309 -0x1.237ce1b409b07p-6   /* -0.017963576301490347 */
#define ERFINV_P310  0x1.25db922abee60p-9   /* 0.002256540061445434 */

/* Q(z) denominator coefficients */
#define ERFINV_Q300  0x1.d98d1a3412e13p-15  /* 5.641892930458443e-05 */
#define ERFINV_Q301  0x1.5ea77aa937936p-8   /* 0.005354240091149166 */
#define ERFINV_Q302  0x1.09f744281bf43p-3   /* 0.13003663481604523 */
#define ERFINV_Q303  0x1.0de629224eb8ap+0   /* 1.0542609003982793 */
#define ERFINV_Q304  0x1.84dafe0d3b477p+1   /* 3.0387409753130616 */
#define ERFINV_Q305  0x1.e1add024b4c8dp+1   /* 3.7647831834882804 */
#define ERFINV_Q306  0x1.f06bab8543d1ap+1   /* 3.8820794230862474 */
#define ERFINV_Q307  0x1.04c46273c9ec0p+1   /* 1.0372664014323103 */
#define ERFINV_Q308  0x1.0000000000000p+0   /* 1.0 */

/* Boundary values and offsets */
#define ERFINV_BOUND1   0x3FE8000000000000ULL  /* 0.75 */
#define ERFINV_BOUND2   0x3FEE000000000000ULL  /* 0.9375 */
#define ERFINV_OFFSET1  0x1.2000000000000p-1   /* 0.5625 */
#define ERFINV_OFFSET2  0x1.c200000000000p-1   /* 0.87890625 */

/* Bit masks */
#define ERFINV_SIGN_MASK  0x8000000000000000ULL
#define ERFINV_ABS_MASK   0x7FFFFFFFFFFFFFFFULL
#define ERFINV_ONE        0x3FF0000000000000ULL
#define ERFINV_INF        0x7FF0000000000000ULL
#define ERFINV_ZERO       0x0000000000000000ULL

#endif /* _ERFINV_DATA_H_ */
