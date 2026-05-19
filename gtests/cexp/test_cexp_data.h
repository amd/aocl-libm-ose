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

#ifndef __TEST_CEXP_DATA_H__
#define __TEST_CEXP_DATA_H__

extern "C"
{
    #if defined(_WIN64) || defined(_WIN32)
        #include "complex.h"
    #else
        #include "/usr/include/complex.h"
    #endif
}

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>
#include <libm/types.h>
#include <external/amdlibm.h>

//Helper functions to convert hex bit patterns to float/double values at compile time
//These changes ensure that test data containing special values (infinity, NaN, π)
//works correctly in complex arithmetic expressions by converting hex bit patterns
//to actual floating/double-point values before they're used in calculations.

static inline float bits_to_float(uint32_t bits) {
    union { uint32_t u; float f; } conv;
    conv.u = bits;
    return conv.f;
}

static inline double bits_to_double(uint64_t bits) {
    union { uint64_t u; double d; } conv;
    conv.u = bits;
    return conv.d;
}

// Redefine special constants as actual float/double values for use in complex expressions
#undef POS_INF_F32
#undef NEG_INF_F32
#undef POS_QNAN_F32
#undef NEG_QNAN_F32
#undef POS_PI_F32
#undef NEG_PI_F32

#define POS_INF_F32    (bits_to_float(0x7F800000))
#define NEG_INF_F32    (bits_to_float(0xFF800000))
#define POS_QNAN_F32   (bits_to_float(0x7fc00000 ))
#define NEG_QNAN_F32   (bits_to_float(0xffc00000))
#define POS_PI_F32     (bits_to_float(0x40490fd8))
#define NEG_PI_F32     (bits_to_float(0xc0490fd8))

#undef POS_INF_F64
#undef NEG_INF_F64
#undef POS_QNAN_F64
#undef NEG_QNAN_F64
#undef POS_PI_F64
#undef NEG_PI_F64

#define POS_INF_F64    (bits_to_double(0x7ff0000000000000ULL))
#define NEG_INF_F64    (bits_to_double(0xfff0000000000000ULL))
#define POS_QNAN_F64   (bits_to_double(0x7ff87ff7fdedffffULL))
#define NEG_QNAN_F64   (bits_to_double(0xfff8000000000000ULL))
#define POS_PI_F64     (bits_to_double(0x400921FB54442D18ULL))
#define NEG_PI_F64     (bits_to_double(0xc00921fb54442d18ULL))
/* Subnormal constants for F32 */
#define POS_SUBNORM_MIN_F32  (bits_to_float(0x00000001))   /* ~1.4e-45, smallest positive subnormal */
#define POS_SUBNORM_MAX_F32  (bits_to_float(0x007FFFFF))   /* ~1.17e-38, largest positive subnormal */
#define NEG_SUBNORM_MIN_F32  (bits_to_float(0x80000001))   /* ~-1.4e-45, smallest negative subnormal */
#define NEG_SUBNORM_MAX_F32  (bits_to_float(0x807FFFFF))   /* ~-1.17e-38, largest negative subnormal */

/* Subnormal constants for F64 */
#define POS_SUBNORM_MIN_F64  (bits_to_double(0x0000000000000001ULL))  /* ~4.9e-324, smallest positive subnormal */
#define POS_SUBNORM_MAX_F64  (bits_to_double(0x000FFFFFFFFFFFFFULL))  /* ~2.2e-308, largest positive subnormal */
#define NEG_SUBNORM_MIN_F64  (bits_to_double(0x8000000000000001ULL))  /* ~-4.9e-324, smallest negative subnormal */
#define NEG_SUBNORM_MAX_F64  (bits_to_double(0x800FFFFFFFFFFFFFULL))  /* ~-2.2e-308, largest negative subnormal */



/*
 * Test cases to check the conformance for the cexpf() routine.
 * These test cases are not exhaustive.
 * These values as as per GLIBC output.
 */

static libm_test_complex_data_f32
test_cexpf_conformance_data[] = {
        /* Test inputs of -/+0 */
        {{0.0, 0.0},                   {1.0, 0.0},                   0},
        {{-0.0, 0.0},                  {1.0, 0.0},                   0},
        {{0.0, -0.0},                  {1.0, -0.0},                  0},
        {{-0.0, -0.0},                 {1.0, -0.0},                  0},

        /* Test inputs of -/+inf, Signalling NAN and Quiet NAN*/
        {{POS_INF_F32, 0.0},           {POS_INF_F32, 0.0},           0},
        {{NEG_INF_F32, 0.0},           {0.0, 0.0},                   0},
        {{NEG_INF_F32, -0.0},          {0.0, -0.0},                  0},
        {{NEG_INF_F32, POS_INF_F32},   {0.0, 0.0},                   0},
        {{POS_INF_F32, POS_INF_F32},   {POS_INF_F32, POS_QNAN_F32},  FE_INVALID},
        {{POS_INF_F32, NEG_INF_F32},   {POS_INF_F32, POS_QNAN_F32},  FE_INVALID},
        {{NEG_INF_F32, POS_QNAN_F32},  {0.0, 0.0},                   0},
        {{POS_INF_F32, POS_QNAN_F32},  {POS_INF_F32, POS_QNAN_F32},  0},
        {{8.1, POS_INF_F32},           {POS_QNAN_F32, POS_QNAN_F32}, FE_INVALID},
        {{0.0, POS_INF_F32},           {POS_QNAN_F32, POS_QNAN_F32}, FE_INVALID},
        {{-0.0, POS_INF_F32},          {POS_QNAN_F32, POS_QNAN_F32}, FE_INVALID},
        {{1.0, POS_INF_F32},           {POS_QNAN_F32, POS_QNAN_F32}, FE_INVALID},
        {{-5.0, NEG_INF_F32},          {POS_QNAN_F32, POS_QNAN_F32}, FE_INVALID},
        {{POS_QNAN_F32, 0.0},          {POS_QNAN_F32, 0.0},          0},
        {{POS_QNAN_F32, 3.1},          {POS_QNAN_F32, POS_QNAN_F32}, FE_INVALID},
        {{POS_QNAN_F32, POS_QNAN_F32}, {POS_QNAN_F32, POS_QNAN_F32}, 0},
        {{POS_QNAN_F32, POS_INF_F32},  {POS_QNAN_F32, POS_QNAN_F32}, FE_INVALID},
        {{POS_QNAN_F32, NEG_INF_F32},  {POS_QNAN_F32, POS_QNAN_F32}, FE_INVALID},
        {{8.1, POS_QNAN_F32},          {POS_QNAN_F32, POS_QNAN_F32}, FE_INVALID},

        /* -INF with finite non-zero imaginary (returns 0*cis(y) = ±0±0i) */
        {{NEG_INF_F32, POS_PI_F32},    {-0.0, 0.0},                  FE_INEXACT},
        {{NEG_INF_F32, POS_PI_F32/2},  {0.0, 0.0},                   FE_INEXACT},
        {{NEG_INF_F32, 1.0},           {0.0, 0.0},                   FE_INEXACT},
        {{NEG_INF_F32, -1.0},          {0.0, -0.0},                  FE_INEXACT},
        {{NEG_INF_F32, 3.0f*POS_PI_F32/2.0f},{-0.0, -0.0},            FE_INEXACT},

        /* +INF with finite non-zero imaginary (returns ∞*cis(y)) */
        {{POS_INF_F32, POS_PI_F32},    {NEG_INF_F32, POS_INF_F32},   FE_INEXACT},
        {{POS_INF_F32, POS_PI_F32/2},  {POS_INF_F32, POS_INF_F32},   FE_INEXACT},
        {{POS_INF_F32, 1.0},           {POS_INF_F32, POS_INF_F32},   FE_INEXACT},
        {{POS_INF_F32, -1.0},          {POS_INF_F32, NEG_INF_F32},   FE_INEXACT},
        {{POS_INF_F32, POS_PI_F32/4},  {POS_INF_F32, POS_INF_F32},   FE_INEXACT},
        {{POS_INF_F32, -POS_PI_F32/4}, {POS_INF_F32, NEG_INF_F32},   FE_INEXACT},
        {{POS_INF_F32, 3.0f*POS_PI_F32/2.0f},{NEG_INF_F32, NEG_INF_F32}, FE_INEXACT},

        /* Test inputs of some standard values */
        {{0.0, POS_PI_F32},            {-1.0, 0.0},                  FE_INEXACT}, // Euler's Formula
        {{POS_PI_F32, POS_PI_F32},     {-23.1, 0.0},                 FE_INEXACT},

        /* Real part only (imaginary = ±0) */
        {{1.0, 0.0},                   {2.7182817f, 0.0},            FE_INEXACT},
        {{-1.0, 0.0},                  {0.36787945f, 0.0},           FE_INEXACT},
        {{2.0, -0.0},                  {7.389056f, -0.0},            FE_INEXACT},
        {{0.5, 0.0},                   {1.6487213f, 0.0},            FE_INEXACT},
        {{-1.0, -0.0},                 {0.36787945f, -0.0},          FE_INEXACT},
        {{POS_INF_F32, -0.0},          {POS_INF_F32, -0.0},          0},
        {{POS_QNAN_F32, -0.0},         {POS_QNAN_F32, -0.0},         0},

        /* Imaginary part only (real = ±0) - tests cis function */
        {{0.0, 1.0},                   {0.5403023f, 0.84147096f},    FE_INEXACT},
        {{-0.0, 1.0},                  {0.5403023f, 0.84147096f},    FE_INEXACT},
        {{0.0, -1.0},                  {0.5403023f, -0.84147096f},   FE_INEXACT},
        {{0.0, POS_PI_F32/2},          {6.123234e-17f, 1.0},         FE_INEXACT},
        {{0.0, -POS_PI_F32/2},         {6.123234e-17f, -1.0},        FE_INEXACT},
        {{0.0, 2.0f*POS_PI_F32},       {1.0, -2.4492936e-16f},       FE_INEXACT},

        /* Large imaginary values - tests cos/sin with large angles */
        {{0.0, 100.0f},                {0.86231887f, -0.50636564f},  FE_INEXACT},
        {{0.0, 1000.0f},               {0.56237908f, 0.82687954f},   FE_INEXACT},
        {{0.0, 1024.0f},               {0.98735362f, -0.15853338f},  FE_INEXACT},
        {{1.0, 1024.0f},               {2.6839054f, -0.43093841f},   FE_INEXACT},
        {{0.0, 1.0e6f},                {0.93675213f, -0.34999350f},  FE_INEXACT},

        /* Large positive real (near overflow) */
        {{88.0, 0.0},                  {1.6516363e+38f, 0.0},        FE_INEXACT},
        {{88.5, 0.0},                  {POS_INF_F32, 0.0},           FE_INEXACT},
        {{89.0, 1.0},                  {POS_INF_F32, POS_INF_F32},   FE_OVERFLOW},

        /* Very negative real (underflow) */
        {{-103.0, 0.0},                {0.0, 0.0},                   FE_UNDERFLOW},
        {{-120.0, 1.0},                {0.0, 0.0},                   FE_UNDERFLOW},

        /* Mixed signs at boundaries - underflow/overflow with rotation */
        {{-88.0f, POS_PI_F32},         {-6.0546019e-39f, 0.0f},      FE_UNDERFLOW},
        {{-88.0f, POS_PI_F32/2},       {0.0f, 6.0546019e-39f},       FE_UNDERFLOW},
        {{87.0f, POS_PI_F32},          {-6.0760302e+37f, 0.0f},      FE_INEXACT},
        {{87.0f, POS_PI_F32/2},        {0.0f, 6.0760302e+37f},       FE_INEXACT},
        {{87.0f, -POS_PI_F32/2},       {0.0f, -6.0760302e+37f},      FE_INEXACT},

        /* Test inputs of some random values */
        {{-1.0, 2.0},                  {-0.153092, 0.334512},        FE_INEXACT},
        {{-0.0076, 1827.76},           {0.8, -0.6},                  FE_INEXACT},
        {{1.0, 1.0},                   {1.4686939f, 2.2873552f},     FE_INEXACT},
        {{2.0, -3.0},                  {-7.3151102f, -1.0427437f},   FE_INEXACT},
        {{-2.0, 2.0},                  {-0.15324879f, -0.033689447f},FE_INEXACT},
        {{0.5, 0.5},                   {1.3803883f, 0.8509559f},     FE_INEXACT},

        /* Small values */
        {{0.001, 0.001},               {1.0010005f, 0.0010000005f},  FE_INEXACT},
        {{-0.001, -0.001},             {0.9990005f, -0.9990005e-3f}, FE_INEXACT},

        /* Subnormal inputs - very small denormalized values */
        /* For subnormal z, exp(z) ≈ 1 + z (Taylor series, since |z| << 1) */
        {{POS_SUBNORM_MIN_F32, 0.0},   {1.0f, 0.0f},                 FE_INEXACT},
        {{0.0, POS_SUBNORM_MIN_F32},   {1.0f, POS_SUBNORM_MIN_F32},  0},
        {{POS_SUBNORM_MAX_F32, 0.0},   {1.0f, 0.0f},                 FE_INEXACT},
        {{0.0, POS_SUBNORM_MAX_F32},   {1.0f, POS_SUBNORM_MAX_F32},  0},
        {{NEG_SUBNORM_MIN_F32, 0.0},   {1.0f, 0.0f},                 FE_INEXACT},
        {{0.0, NEG_SUBNORM_MIN_F32},   {1.0f, NEG_SUBNORM_MIN_F32},  0},
        {{POS_SUBNORM_MIN_F32, POS_SUBNORM_MIN_F32}, {1.0f, POS_SUBNORM_MIN_F32}, FE_INEXACT},
        {{POS_SUBNORM_MAX_F32, POS_SUBNORM_MAX_F32}, {1.0f, POS_SUBNORM_MAX_F32}, FE_INEXACT},

        /* Subnormal inputs - very small complex */
        {{1e-40f, 1e-40f},             {1.0f, 1e-40f},               FE_INEXACT},
        /* Large imaginary (tests sin/cos accuracy) */
        {{0.0f, 1000.0f},              {0.562379f, 0.826879f},       FE_INEXACT},
        /* Negative zero preservation */
        {{1.0f, -0.0f},                {2.7182817f, -0.0f},          FE_INEXACT},
        /* Mixed underflow with rotation */
        {{-103.0f, 3.14159f},          {-0.0f, 0.0f},   FE_UNDERFLOW|FE_INEXACT},

    // For z = -∞+yi, the result is +0cis(y)
    // For z = +∞+yi, the result is +∞cis(y)
};

/*
 * Test cases to check the conformance for the cexp() routine.
 * These test cases are not exhaustive.
 * These values as as per GLIBC output.
 */
static libm_test_complex_data_f64
test_cexp_conformance_data[] = {
        /* Test inputs of -/+0 */
        {{0.0, 0.0},                   {1.0, 0.0},                   0},
        {{-0.0, 0.0},                  {1.0, 0.0},                   0},
        {{0.0, -0.0},                  {1.0, -0.0},                  0},
        {{-0.0, -0.0},                 {1.0, -0.0},                  0},

        /* Test inputs of -/+inf, Signalling NAN and Quiet NAN*/
        {{POS_INF_F64, 0.0},           {POS_INF_F64, 0.0},           0},
        {{NEG_INF_F64, 0.0},           {0.0, 0.0},                   0},
        {{NEG_INF_F64, -0.0},          {0.0, -0.0},                  0},
        {{NEG_INF_F64, POS_INF_F64},   {0.0, 0.0},                   0},
        {{POS_INF_F64, POS_INF_F64},   {POS_INF_F64, POS_QNAN_F64},  FE_INVALID},
        {{POS_INF_F64, NEG_INF_F64},   {POS_INF_F64, POS_QNAN_F64},  FE_INVALID},
        {{NEG_INF_F64, POS_QNAN_F64},  {0.0, 0.0},                   0},
        {{POS_INF_F64, POS_QNAN_F64},  {POS_INF_F64, POS_QNAN_F64},  0},
        {{8.1, POS_INF_F64},           {POS_QNAN_F64, POS_QNAN_F64}, FE_INVALID},
        {{0.0, POS_INF_F64},           {POS_QNAN_F64, POS_QNAN_F64}, FE_INVALID},
        {{-0.0, POS_INF_F64},          {POS_QNAN_F64, POS_QNAN_F64}, FE_INVALID},
        {{1.0, POS_INF_F64},           {POS_QNAN_F64, POS_QNAN_F64}, FE_INVALID},
        {{-5.0, NEG_INF_F64},          {POS_QNAN_F64, POS_QNAN_F64}, FE_INVALID},
        {{-100.0, POS_INF_F64},        {POS_QNAN_F64, POS_QNAN_F64}, FE_INVALID},
        {{POS_QNAN_F64, 0.0},          {POS_QNAN_F64, 0.0},          0},
        {{POS_QNAN_F64, 3.1},          {POS_QNAN_F64, POS_QNAN_F64}, FE_INVALID},
        {{POS_QNAN_F64, POS_QNAN_F64}, {POS_QNAN_F64, POS_QNAN_F64}, 0},
        {{POS_QNAN_F64, POS_INF_F64},  {POS_QNAN_F64, POS_QNAN_F64}, FE_INVALID},
        {{POS_QNAN_F64, NEG_INF_F64},  {POS_QNAN_F64, POS_QNAN_F64}, FE_INVALID},
        {{8.1, POS_QNAN_F64},          {POS_QNAN_F64, POS_QNAN_F64}, FE_INVALID},

        /* -INF with finite non-zero imaginary (returns 0*cis(y) = ±0±0i) */
        {{NEG_INF_F64, POS_PI_F64},    {-0.0, 0.0},                  FE_INEXACT},
        {{NEG_INF_F64, POS_PI_F64/2},  {0.0, 0.0},                   FE_INEXACT},
        {{NEG_INF_F64, 1.0},           {0.0, 0.0},                   FE_INEXACT},
        {{NEG_INF_F64, -1.0},          {0.0, -0.0},                  FE_INEXACT},
        {{NEG_INF_F64, 3.0*POS_PI_F64/2.0},{-0.0, -0.0},              FE_INEXACT},

        /* +INF with finite non-zero imaginary (returns ∞*cis(y)) */
        {{POS_INF_F64, POS_PI_F64},    {NEG_INF_F64, POS_INF_F64},   FE_INEXACT},
        {{POS_INF_F64, POS_PI_F64/2},  {POS_INF_F64, POS_INF_F64},   FE_INEXACT},
        {{POS_INF_F64, 1.0},           {POS_INF_F64, POS_INF_F64},   FE_INEXACT},
        {{POS_INF_F64, -1.0},          {POS_INF_F64, NEG_INF_F64},   FE_INEXACT},
        {{POS_INF_F64, POS_PI_F64/4},  {POS_INF_F64, POS_INF_F64},   FE_INEXACT},
        {{POS_INF_F64, -POS_PI_F64/4}, {POS_INF_F64, NEG_INF_F64},   FE_INEXACT},
        {{POS_INF_F64, 3.0*POS_PI_F64/2.0},{NEG_INF_F64, NEG_INF_F64}, FE_INEXACT},

        /* Test inputs of some standard values */
        {{0.0, POS_PI_F64},            {-1.0, 0.0},                  FE_INEXACT}, // Euler's Formula
        {{POS_PI_F64, POS_PI_F64},     {-23.1, 0.0},                 FE_INEXACT},

        /* Real part only (imaginary = ±0) */
        {{1.0, 0.0},                   {2.718281828459045, 0.0},     FE_INEXACT},
        {{-1.0, 0.0},                  {0.36787944117144233, 0.0},   FE_INEXACT},
        {{2.0, -0.0},                  {7.38905609893065, -0.0},     FE_INEXACT},
        {{-1.0, -0.0},                 {0.36787944117144233, -0.0},  FE_INEXACT},
        {{POS_INF_F64, -0.0},          {POS_INF_F64, -0.0},          0},
        {{POS_QNAN_F64, -0.0},         {POS_QNAN_F64, -0.0},         0},
        {{-2.0, 0.0},                  {0.1353352832366127, 0.0},    FE_INEXACT},
        {{0.5, 0.0},                   {1.6487212707001282, 0.0},    FE_INEXACT},

        /* Imaginary part only (real = ±0) - tests cis function */
        {{0.0, 1.0},                   {0.5403023058681398, 0.8414709848078965}, FE_INEXACT},
        {{-0.0, 1.0},                  {0.5403023058681398, 0.8414709848078965}, FE_INEXACT},
        {{0.0, -1.0},                  {0.5403023058681398, -0.8414709848078965}, FE_INEXACT},
        {{0.0, POS_PI_F64/2},          {6.123233995736766e-17, 1.0}, FE_INEXACT},
        {{0.0, -POS_PI_F64/2},         {6.123233995736766e-17, -1.0},FE_INEXACT},
        {{0.0, 2.0*POS_PI_F64},        {1.0, -2.4492935982947064e-16}, FE_INEXACT},
        {{-0.0, 2.0*POS_PI_F64},       {1.0, -2.4492935982947064e-16}, FE_INEXACT},
        {{0.0, POS_PI_F64/6},          {0.8660254037844387, 0.49999999999999994}, FE_INEXACT},

        /* Large imaginary values - tests cos/sin with large angles */
        {{0.0, 100.0},                 {0.8623188722876839, -0.5063656411097588}, FE_INEXACT},
        {{0.0, 1000.0},                {0.5623790762907029, 0.8268795405320025},  FE_INEXACT},
        {{0.0, 1024.0},                {0.9873536182198476, -0.1585333800439961}, FE_INEXACT},
        {{1.0, 1024.0},                {2.6839053986702993, -0.4309384061777864}, FE_INEXACT},
        {{0.0, 1.0e6},                 {0.9367521275331447, -0.3499935021712929}, FE_INEXACT},
        {{0.0, 1.0e15},                {-0.5765716471159145, 0.8170608158219068}, FE_INEXACT},

        /* Large positive real (near overflow) */
        {{709.0, 0.0},                 {8.218407461554972e+307, 0.0}, FE_INEXACT},
        {{709.5, 0.0},                 {1.3937095806663797e+308, 0.0}, FE_INEXACT},
        {{710.0, 0.0},                 {POS_INF_F64, 0.0},           FE_OVERFLOW | FE_INEXACT},
        {{710.0, 1.0},                 {POS_INF_F64, POS_INF_F64},   FE_OVERFLOW | FE_INEXACT},
        {{750.0, POS_PI_F64},          {NEG_INF_F64, 0.0},           FE_OVERFLOW | FE_INEXACT},

        /* Very negative real (underflow) */
        {{-745.0, 0.0},                {0.0, 0.0},                   FE_UNDERFLOW | FE_INEXACT},
        {{-750.0, 1.0},                {0.0, 0.0},                   FE_UNDERFLOW | FE_INEXACT},
        {{-800.0, POS_PI_F64},         {-0.0, 0.0},                  FE_UNDERFLOW | FE_INEXACT},
        {{-1000.0, 0.0},               {0.0, 0.0},                   FE_UNDERFLOW | FE_INEXACT},

        /* Mixed signs at boundaries - underflow/overflow with rotation */
        {{-700.0, POS_PI_F64},         {-9.85967654375977e-305, 0.0}, FE_INEXACT},
        {{-700.0, POS_PI_F64/2},       {0.0, 9.85967654375977e-305}, FE_INEXACT},
        {{-700.0, POS_PI_F64/4},       {6.97184414439848e-305, 6.97184414439847e-305}, FE_INEXACT},
        {{700.0, POS_PI_F64},          {-1.014232054735e+304, 0.0},  FE_INEXACT},
        {{700.0, POS_PI_F64/2},        {0.0, 1.014232054735e+304},   FE_INEXACT},
        {{700.0, -POS_PI_F64/2},       {0.0, -1.014232054735e+304},  FE_INEXACT},
        {{700.0, POS_PI_F64/4},        {7.17170363599887e+303, 7.17170363599887e+303}, FE_INEXACT},

        /* Test inputs of some random values */
        {{-1.0, 2.0},                  {-0.153092, 0.334512},        FE_INEXACT},
        {{-0.0076, 1827.76},           {0.8, -0.6},                  FE_INEXACT},

        /* General complex values - various quadrants */
        {{1.0, 1.0},                   {1.4686939399158851, 2.2873552871788423}, FE_INEXACT},
        {{2.0, -3.0},                  {-7.315110094901103, 1.0427436562359045}, FE_INEXACT},
        {{-2.0, 2.0},                  {-0.15324878307273242, -0.03368944784169686}, FE_INEXACT},
        {{0.5, 0.5},                   {1.3803882853311363, 0.8509559436271786}, FE_INEXACT},
        {{-1.5, -2.5},                 {-0.06286777796922698, 0.08597397623051632}, FE_INEXACT},
        {{3.0, 4.0},                   {-13.128783081462158, -15.200784463067954}, FE_INEXACT},

        /* Small values */
        {{0.001, 0.001},               {1.0010005001667084, 0.0010000005000001666}, FE_INEXACT},
        {{-0.001, -0.001},             {0.9990005001666625, -0.0009990005001666625}, FE_INEXACT},

        /* Subnormal inputs - very small denormalized values */
        /* For subnormal z, exp(z) ≈ 1 + z (Taylor series, since |z| << 1) */
        {{POS_SUBNORM_MIN_F64, 0.0},   {1.0, 0.0},                   0},
        {{0.0, POS_SUBNORM_MIN_F64},   {1.0, POS_SUBNORM_MIN_F64},   0},
        {{POS_SUBNORM_MAX_F64, 0.0},   {1.0, 0.0},                   0},
        {{0.0, POS_SUBNORM_MAX_F64},   {1.0, POS_SUBNORM_MAX_F64},   0},
        {{NEG_SUBNORM_MIN_F64, 0.0},   {1.0, 0.0},                   0},
        {{0.0, NEG_SUBNORM_MIN_F64},   {1.0, NEG_SUBNORM_MIN_F64},   0},
        {{POS_SUBNORM_MIN_F64, POS_SUBNORM_MIN_F64}, {1.0, POS_SUBNORM_MIN_F64}, 0},
        {{POS_SUBNORM_MAX_F64, POS_SUBNORM_MAX_F64}, {1.0, POS_SUBNORM_MAX_F64}, 0},

        /* Subnormal inputs - very small complex */
        {{1e-310, 1e-310},             {1.0, 1e-310},                0},
        /* Large imaginary (tests sin/cos accuracy) */
        {{0.0, 1000.0},                {0.5623790762907029, 0.8268795405320025}, FE_INEXACT},
        /* Negative zero preservation */
        {{1.0, -0.0},                  {2.718281828459045, -0.0},    FE_INEXACT},
        /* Mixed underflow with rotation */
        {{-750.0, 3.141592653589793},  {-0.0, 0.0},       FE_UNDERFLOW |FE_INEXACT},

    // For z = -∞+yi, the result is +0cis(y)
    // For z = +∞+yi, the result is +∞cis(y)
};

#endif	/*__TEST_CEXP_DATA_H__*/