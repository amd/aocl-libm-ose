/*
 * Copyright (C) 2025-2026 Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __TEST_LDEXP_DATA_H__
#define __TEST_LDEXP_DATA_H__

#include <fenv.h>
#include <libm_tests.h>

/*
 * ldexp(x, n) = x * 2^n.
 *
 * libm_test_special_data_f64 field order: {in, out, exptdexpt, in2, ...}
 *   in        = x (double bit pattern)
 *   out       = expected result (double bit pattern)
 *   exptdexpt = expected exception flags
 *   in2       = exponent n, stored as exact integer value in double so that
 *               int(data[1]) gives the correct integer without raising FE_INEXACT
 *
 * libm_test_special_data_f32 field order: {in, out, exptdexpt, in2, ...}
 *   in        = x (float bit pattern)
 *   out       = expected result (float bit pattern)
 *   exptdexpt = expected exception flags
 *   in2       = exponent n, stored as exact integer value in float
 *
 * Expected exceptions reflect IEEE 754 SSE hardware behaviour (the "after
 * rounding" underflow model): FE_UNDERFLOW and FE_INEXACT are raised only
 * when the result is tiny AND inexact.  Exact subnormal results raise no
 * exception.  FE_OVERFLOW and FE_INEXACT are raised when the result is Inf.
 * FE_INVALID is raised for sNaN inputs.
 */

/* Double-precision conformance data: {in, out, exptdexpt, in2=exp_as_double} */
static libm_test_special_data_f64
test_ldexp_conformance_data[] = {
    /* --- Special inputs --- */
    /* sNaN: quietened, FE_INVALID raised; exp=1 */
    {0x7FF4001000000000, 0x7FFC001000000000, FE_INVALID, 0x3FF0000000000000},
    /* qNaN: passed through, no exception; exp=1 */
    {0x7FF8000000000001, 0x7FF8000000000001, 0, 0x3FF0000000000000},
    /* +Inf: result is +Inf, no exception; exp=10 */
    {0x7FF0000000000000, 0x7FF0000000000000, 0, 0x4024000000000000},
    /* -Inf: result is -Inf, no exception; exp=10 */
    {0xFFF0000000000000, 0xFFF0000000000000, 0, 0x4024000000000000},
    /* +0: result is +0, no exception; exp=10 */
    {0x0000000000000000, 0x0000000000000000, 0, 0x4024000000000000},
    /* -0: result is -0, no exception; exp=10 */
    {0x8000000000000000, 0x8000000000000000, 0, 0x4024000000000000},
    /* +0 with exp=0: result is +0 */
    {0x0000000000000000, 0x0000000000000000, 0, 0x0000000000000000},

    /* --- Normal fast-path cases: no exception --- */
    /* 1.0 * 2^0 = 1.0 */
    {0x3FF0000000000000, 0x3FF0000000000000, 0, 0x0000000000000000},
    /* 1.0 * 2^1 = 2.0 */
    {0x3FF0000000000000, 0x4000000000000000, 0, 0x3FF0000000000000},
    /* 1.0 * 2^-1 = 0.5 */
    {0x3FF0000000000000, 0x3FE0000000000000, 0, 0xBFF0000000000000},
    /* 1.0 * 2^10 = 1024.0 */
    {0x3FF0000000000000, 0x4090000000000000, 0, 0x4024000000000000},
    /* -1.0 * 2^3 = -8.0 */
    {0xBFF0000000000000, 0xC020000000000000, 0, 0x4008000000000000},

    /* --- Fast-path upper boundary --- */
    /* 1.0 * 2^1022 */
    {0x3FF0000000000000, 0x7FD0000000000000, 0, 0x408FF00000000000},
    /* 1.0 * 2^1023 */
    {0x3FF0000000000000, 0x7FE0000000000000, 0, 0x408FF80000000000},

    /* --- Fast-path lower boundary --- */
    /* 1.0 * 2^-1022 = smallest normal double */
    {0x3FF0000000000000, 0x0010000000000000, 0, 0xC08FF00000000000},

    /* --- Else-path: overflow --- */
    /* 1.0 * 2^1024 -> +Inf */
    {0x3FF0000000000000, 0x7FF0000000000000, FE_OVERFLOW|FE_INEXACT, 0x4090000000000000},
    /* 1.0 * 2^2046 -> +Inf */
    {0x3FF0000000000000, 0x7FF0000000000000, FE_OVERFLOW|FE_INEXACT, 0x409FF80000000000},
    /* -1.0 * 2^1024 -> -Inf */
    {0xBFF0000000000000, 0xFFF0000000000000, FE_OVERFLOW|FE_INEXACT, 0x4090000000000000},

    /* --- Else-path: exact subnormal results (no exception on SSE hardware) --- */
    /* 1.0 * 2^-1023 = 2^-1023 (exact subnormal) */
    {0x3FF0000000000000, 0x0008000000000000, 0, 0xC08FF80000000000},
    /* 1.0 * 2^-1074 = smallest subnormal (exact) */
    {0x3FF0000000000000, 0x0000000000000001, 0, 0xC090C80000000000},
    /* smallest normal * 2^-1 = 2^-1023 (exact subnormal) */
    {0x0010000000000000, 0x0008000000000000, 0, 0xBFF0000000000000},

    /* --- Else-path: inexact underflow to zero --- */
    /* 1.0 * 2^-1075 -> +0 (inexact) */
    {0x3FF0000000000000, 0x0000000000000000, FE_UNDERFLOW|FE_INEXACT, 0xC090CC0000000000},
    /* 1.0 * 2^-2044 -> +0 (deep underflow) */
    {0x3FF0000000000000, 0x0000000000000000, FE_UNDERFLOW|FE_INEXACT, 0xC09FF00000000000},

    /* --- Else-path boundary: extreme positive and negative exponents --- */
    /* EMAX * 2^-2099 -> +0 (underflow; -2099 is one past the else-path minimum) */
    {0x7FEFFFFFFFFFFFFF, 0x0000000000000000, FE_UNDERFLOW|FE_INEXACT, 0xC0A0660000000000},
    /* EMAX * 2^-2098 -> min subnormal (exact, no exception) */
    {0x7FEFFFFFFFFFFFFF, 0x0000000000000001, 0, 0xC0A0640000000000},
    /* min subnormal * 2^2097 -> 0x1p+1023 (exact, no exception) */
    {0x0000000000000001, 0x7FE0000000000000, 0, 0x40A0620000000000},
    /* min subnormal * 2^2098 -> +Inf (overflow; 2098 is the else-path maximum) */
    {0x0000000000000001, 0x7FF0000000000000, FE_OVERFLOW|FE_INEXACT, 0x40A0640000000000},
};

/* Single-precision conformance data: {in, out, exptdexpt, in2=exp_as_float} */
static libm_test_special_data_f32
test_ldexpf_conformance_data[] = {
    /* --- Special inputs --- */
    /* sNaN (0x7F800001): quietened, FE_INVALID; exp=1 */
    {0x7F800001, 0x7FC00001, FE_INVALID, 0x3F800000},
    /* qNaN: passed through; exp=1 */
    {0x7FC00001, 0x7FC00001, 0, 0x3F800000},
    /* +Inf; exp=10 */
    {0x7F800000, 0x7F800000, 0, 0x41200000},
    /* -Inf; exp=10 */
    {0xFF800000, 0xFF800000, 0, 0x41200000},
    /* +0; exp=10 */
    {0x00000000, 0x00000000, 0, 0x41200000},
    /* -0; exp=10 */
    {0x80000000, 0x80000000, 0, 0x41200000},

    /* --- Normal fast-path cases --- */
    /* 1.0f * 2^0 = 1.0f */
    {0x3F800000, 0x3F800000, 0, 0x00000000},
    /* 1.0f * 2^1 = 2.0f */
    {0x3F800000, 0x40000000, 0, 0x3F800000},
    /* 1.0f * 2^-1 = 0.5f */
    {0x3F800000, 0x3F000000, 0, 0xBF800000},
    /* 1.0f * 2^10 = 1024.0f */
    {0x3F800000, 0x44800000, 0, 0x41200000},
    /* -1.0f * 2^3 = -8.0f */
    {0xBF800000, 0xC1000000, 0, 0x40400000},

    /* --- Fast-path boundaries --- */
    /* 1.0f * 2^126 */
    {0x3F800000, 0x7E800000, 0, 0x42FC0000},
    /* 1.0f * 2^127 */
    {0x3F800000, 0x7F000000, 0, 0x42FE0000},
    /* 1.0f * 2^-126 = smallest normal float */
    {0x3F800000, 0x00800000, 0, 0xC2FC0000},

    /* --- Else-path: overflow --- */
    /* 1.0f * 2^128 -> +Inf */
    {0x3F800000, 0x7F800000, FE_OVERFLOW|FE_INEXACT, 0x43000000},
    /* -1.0f * 2^128 -> -Inf */
    {0xBF800000, 0xFF800000, FE_OVERFLOW|FE_INEXACT, 0x43000000},

    /* --- Else-path: exact subnormal results (no exception) --- */
    /* 1.0f * 2^-127 = 2^-127 (exact subnormal) */
    {0x3F800000, 0x00400000, 0, 0xC2FE0000},
    /* 1.0f * 2^-149 = smallest subnormal float (exact) */
    {0x3F800000, 0x00000001, 0, 0xC3150000},
    /* smallest normal float * 2^-1 = 2^-127 (exact subnormal) */
    {0x00800000, 0x00400000, 0, 0xBF800000},

    /* --- Else-path: inexact underflow to zero --- */
    /* 1.0f * 2^-150 -> +0 (inexact) */
    {0x3F800000, 0x00000000, FE_UNDERFLOW|FE_INEXACT, 0xC3160000},

    /* --- Else-path boundary: extreme positive and negative exponents --- */
    /* EMAXf * 2^-278 -> +0 (underflow; -278 is one past the else-path minimum) */
    {0x7F7FFFFF, 0x00000000, FE_UNDERFLOW|FE_INEXACT, 0xC38B0000},
    /* EMAXf * 2^-277 -> min subnormalf (exact, no exception) */
    {0x7F7FFFFF, 0x00000001, 0, 0xC38A8000},
    /* min subnormalf * 2^276 -> 0x1p+127 (exact, no exception) */
    {0x00000001, 0x7F000000, 0, 0x438A0000},
    /* min subnormalf * 2^277 -> +Inf (overflow; 277 is the else-path maximum) */
    {0x00000001, 0x7F800000, FE_OVERFLOW|FE_INEXACT, 0x438A8000},
};

#endif  /* __TEST_LDEXP_DATA_H__ */
