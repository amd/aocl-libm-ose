/*
 * Copyright (C) 2008-2026 Advanced Micro Devices, Inc. All rights reserved.
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

#pragma once

#include <stdint.h>
#include <fenv.h>
#include <limits.h>

/*
 * Test data for lround/llround family (round half-way cases away from zero).
 *
 * Two separate tables are provided for each precision, mirroring the lrint layout:
 *
 *   LroundF64Cases: double inputs, expected results fit in long on all
 *     platforms (|result| <= 2^31-1), plus NaN/Inf/overflow cases where
 *     lround returns LONG_MIN with FE_INVALID.
 *
 *   LlroundF64Cases: double inputs testing llround-specific ranges where
 *     the result may exceed 32-bit long but fits in long long.
 *
 *   LroundF32Cases / LlroundF32Cases: same split for float inputs.
 *
 * excepts field: expected FE_INVALID (or 0).
 */

struct LroundF64Data {
    uint64_t  m_in;
    long      m_out;
    int       m_excepts;
};

struct LlroundF64Data {
    uint64_t  m_in;
    long long m_out;
    int       m_excepts;
};

struct LroundF32Data {
    uint32_t  m_in;
    long      m_out;
    int       m_excepts;
};

struct LlroundF32Data {
    uint32_t  m_in;
    long long m_out;
    int       m_excepts;
};

/*
 * Double bit-pattern constants.
 */
#define R_D_POS_ZERO    0x0000000000000000ULL  /* +0.0  */
#define R_D_NEG_ZERO    0x8000000000000000ULL  /* -0.0  */
#define R_D_POS_HALF    0x3FE0000000000000ULL  /* +0.5  */
#define R_D_NEG_HALF    0xBFE0000000000000ULL  /* -0.5  */
#define R_D_ONE         0x3FF0000000000000ULL  /* +1.0  */
#define R_D_NEG_ONE     0xBFF0000000000000ULL  /* -1.0  */
#define R_D_POS_1P5     0x3FF8000000000000ULL  /* +1.5  */
#define R_D_NEG_1P5     0xBFF8000000000000ULL  /* -1.5  */
#define R_D_TWO         0x4000000000000000ULL  /* +2.0  */
#define R_D_POS_2P5     0x4004000000000000ULL  /* +2.5  */
#define R_D_NEG_2P5     0xC004000000000000ULL  /* -2.5  */
#define R_D_THREE       0x4008000000000000ULL  /* +3.0  */
#define R_D_POS_4P5     0x4012000000000000ULL  /* +4.5  */
#define R_D_NEG_4P5     0xC012000000000000ULL  /* -4.5  */
#define R_D_2P52        0x4330000000000000ULL  /* 2^52  */
/* 2^52+1: at exponent 52, 1 ULP = 1, so this is exactly 2^52+1 */
#define R_D_2P52P1      0x4330000000000001ULL
/* largest double < 2^63 = 9223372036854774784 */
#define R_D_LLONG_MAX_F 0x43DFFFFFFFFFFFFFULL
#define R_D_2P63        0x43E0000000000000ULL  /* +2^63 (overflows long long) */
#define R_D_NEG_2P63    0xC3E0000000000000ULL  /* -2^63 = LLONG_MIN as double */
#define R_D_POS_INF     0x7FF0000000000000ULL  /* +Inf  */
#define R_D_NEG_INF     0xFFF0000000000000ULL  /* -Inf  */
#define R_D_QNAN        0x7FF8000000000000ULL  /* +qNaN */
#define R_D_SNAN        0x7FF0000000000001ULL  /* sNaN  */

/*
 * Float bit-pattern constants.
 */
#define R_F_POS_ZERO    0x00000000U  /* +0.0f  */
#define R_F_NEG_ZERO    0x80000000U  /* -0.0f  */
#define R_F_POS_HALF    0x3F000000U  /* +0.5f  */
#define R_F_NEG_HALF    0xBF000000U  /* -0.5f  */
#define R_F_ONE         0x3F800000U  /* +1.0f  */
#define R_F_NEG_ONE     0xBF800000U  /* -1.0f  */
#define R_F_POS_1P5     0x3FC00000U  /* +1.5f  */
#define R_F_NEG_1P5     0xBFC00000U  /* -1.5f  */
#define R_F_POS_2P5     0x40200000U  /* +2.5f  */
#define R_F_NEG_2P5     0xC0200000U  /* -2.5f  */
#define R_F_POS_4P5     0x40900000U  /* +4.5f  */
#define R_F_NEG_4P5     0xC0900000U  /* -4.5f  */
#define R_F_2P23        0x4B000000U  /* 2^23   */
/* largest float < 2^63 = 9223371487098961920 */
#define R_F_LLONG_MAX_F 0x5EFFFFFFU
#define R_F_2P63        0x5F000000U  /* +2^63 (overflows long long) */
#define R_F_NEG_2P63    0xDF000000U  /* -2^63 exactly               */
#define R_F_POS_INF     0x7F800000U  /* +Inf  */
#define R_F_NEG_INF     0xFF800000U  /* -Inf  */
#define R_F_QNAN        0x7FC00000U  /* +qNaN */
#define R_F_SNAN        0x7F800001U  /* sNaN  */

/*
 * lround(double): results fit in long on 32-bit and 64-bit platforms.
 * Out-of-range inputs return LONG_MIN with FE_INVALID.
 * Half-integers round away from zero: +0.5 -> 1, -0.5 -> -1, etc.
 */
static const struct LroundF64Data LroundF64Cases[] = {
    /* m_in              m_out        m_excepts */
    { R_D_POS_ZERO,      0L,          0 },
    { R_D_NEG_ZERO,      0L,          0 },
    { R_D_ONE,           1L,          0 },
    { R_D_NEG_ONE,      -1L,          0 },
    { R_D_TWO,           2L,          0 },
    { R_D_THREE,         3L,          0 },
    /* half-integers round away from zero */
    { R_D_POS_HALF,      1L,          0 },   /* +0.5 -> 1 */
    { R_D_NEG_HALF,     -1L,          0 },   /* -0.5 -> -1 */
    { R_D_POS_1P5,       2L,          0 },   /* +1.5 -> 2 */
    { R_D_NEG_1P5,      -2L,          0 },   /* -1.5 -> -2 */
    { R_D_POS_2P5,       3L,          0 },   /* +2.5 -> 3 */
    { R_D_NEG_2P5,      -3L,          0 },   /* -2.5 -> -3 */
    { R_D_POS_4P5,       5L,          0 },   /* +4.5 -> 5 */
    { R_D_NEG_4P5,      -5L,          0 },   /* -4.5 -> -5 */
    /* finite overflow (+2^63 > LONG_MAX on all platforms): FE_INVALID */
    { R_D_2P63,          LONG_MIN,    FE_INVALID },
    /* NaN and Inf: FE_INVALID, return LONG_MIN */
    { R_D_QNAN,          LONG_MIN,    FE_INVALID },
    { R_D_SNAN,          LONG_MIN,    FE_INVALID },
    { R_D_POS_INF,       LONG_MIN,    FE_INVALID },
    { R_D_NEG_INF,       LONG_MIN,    FE_INVALID },
};

/*
 * llround(double): test cases covering the full long long range.
 * -2^63 = LLONG_MIN is a valid input (exact integral double); no FE_INVALID.
 * +2^63 overflows long long: FE_INVALID.
 */
static const struct LlroundF64Data LlroundF64Cases[] = {
    /* m_in                m_out                     m_excepts */
    { R_D_POS_ZERO,        0LL,                      0 },
    { R_D_NEG_ZERO,        0LL,                      0 },
    { R_D_ONE,             1LL,                      0 },
    { R_D_NEG_ONE,        -1LL,                      0 },
    { R_D_POS_HALF,        1LL,                      0 },
    { R_D_NEG_HALF,       -1LL,                      0 },
    { R_D_POS_1P5,         2LL,                      0 },
    { R_D_NEG_1P5,        -2LL,                      0 },
    { R_D_POS_2P5,         3LL,                      0 },
    { R_D_NEG_2P5,        -3LL,                      0 },
    { R_D_POS_4P5,         5LL,                      0 },
    { R_D_NEG_4P5,        -5LL,                      0 },
    /* 2^52: exact integral double, fits in long long */
    { R_D_2P52,            4503599627370496LL,        0 },
    { R_D_2P52P1,          4503599627370497LL,        0 },
    /* largest representable double below 2^63 */
    { R_D_LLONG_MAX_F,     9223372036854774784LL,     0 },
    /* -2^63 exactly = LLONG_MIN: valid, no exception */
    { R_D_NEG_2P63,        LLONG_MIN,                0 },
    /* finite overflow (+2^63 > LLONG_MAX): FE_INVALID */
    { R_D_2P63,            LLONG_MIN,                FE_INVALID },
    /* NaN and Inf: FE_INVALID, return LLONG_MIN */
    { R_D_QNAN,            LLONG_MIN,                FE_INVALID },
    { R_D_SNAN,            LLONG_MIN,                FE_INVALID },
    { R_D_POS_INF,         LLONG_MIN,                FE_INVALID },
    { R_D_NEG_INF,         LLONG_MIN,                FE_INVALID },
};

/*
 * lroundf(float): results fit in long on both 32-bit and 64-bit platforms.
 */
static const struct LroundF32Data LroundF32Cases[] = {
    /* m_in              m_out        m_excepts */
    { R_F_POS_ZERO,      0L,          0 },
    { R_F_NEG_ZERO,      0L,          0 },
    { R_F_ONE,           1L,          0 },
    { R_F_NEG_ONE,      -1L,          0 },
    /* half-integers round away from zero */
    { R_F_POS_HALF,      1L,          0 },
    { R_F_NEG_HALF,     -1L,          0 },
    { R_F_POS_1P5,       2L,          0 },
    { R_F_NEG_1P5,      -2L,          0 },
    { R_F_POS_2P5,       3L,          0 },
    { R_F_NEG_2P5,      -3L,          0 },
    { R_F_POS_4P5,       5L,          0 },
    { R_F_NEG_4P5,      -5L,          0 },
    /* 2^23: exact integral float, fits in long everywhere */
    { R_F_2P23,          8388608L,    0 },
    /* finite overflow (+2^63 > LONG_MAX on all platforms): FE_INVALID */
    { R_F_2P63,          LONG_MIN,    FE_INVALID },
    /* NaN and Inf: FE_INVALID, return LONG_MIN */
    { R_F_QNAN,          LONG_MIN,    FE_INVALID },
    { R_F_SNAN,          LONG_MIN,    FE_INVALID },
    { R_F_POS_INF,       LONG_MIN,    FE_INVALID },
    { R_F_NEG_INF,       LONG_MIN,    FE_INVALID },
};

/*
 * llroundf(float): test cases covering long long range.
 */
static const struct LlroundF32Data LlroundF32Cases[] = {
    /* m_in                m_out                     m_excepts */
    { R_F_POS_ZERO,        0LL,                      0 },
    { R_F_NEG_ZERO,        0LL,                      0 },
    { R_F_ONE,             1LL,                      0 },
    { R_F_NEG_ONE,        -1LL,                      0 },
    { R_F_POS_HALF,        1LL,                      0 },
    { R_F_NEG_HALF,       -1LL,                      0 },
    { R_F_POS_1P5,         2LL,                      0 },
    { R_F_NEG_1P5,        -2LL,                      0 },
    { R_F_POS_2P5,         3LL,                      0 },
    { R_F_NEG_2P5,        -3LL,                      0 },
    { R_F_POS_4P5,         5LL,                      0 },
    { R_F_NEG_4P5,        -5LL,                      0 },
    /* 2^23: exact integral float */
    { R_F_2P23,            8388608LL,                0 },
    /* largest float < 2^63 */
    { R_F_LLONG_MAX_F,     9223371487098961920LL,    0 },
    /* -2^63 exactly = LLONG_MIN: valid */
    { R_F_NEG_2P63,        LLONG_MIN,               0 },
    /* finite overflow (+2^63 > LLONG_MAX): FE_INVALID */
    { R_F_2P63,            LLONG_MIN,               FE_INVALID },
    /* NaN and Inf: FE_INVALID, return LLONG_MIN */
    { R_F_QNAN,            LLONG_MIN,               FE_INVALID },
    { R_F_SNAN,            LLONG_MIN,               FE_INVALID },
    { R_F_POS_INF,         LLONG_MIN,               FE_INVALID },
    { R_F_NEG_INF,         LLONG_MIN,               FE_INVALID },
};
