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

#ifndef __TEST_CLOG_DATA_H__
#define __TEST_CLOG_DATA_H__

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

static inline float clog_bits_to_float(uint32_t bits) {
    union { uint32_t u; float f; } conv;
    conv.u = bits;
    return conv.f;
}

static inline double clog_bits_to_double(uint64_t bits) {
    union { uint64_t u; double d; } conv;
    conv.u = bits;
    return conv.d;
}

#undef POS_INF_F32
#undef NEG_INF_F32
#undef POS_QNAN_F32
#undef NEG_QNAN_F32
#undef POS_PI_F32
#undef NEG_PI_F32

#define POS_INF_F32    (clog_bits_to_float(0x7F800000))
#define NEG_INF_F32    (clog_bits_to_float(0xFF800000))
#define POS_QNAN_F32   (clog_bits_to_float(0x7fc00000))
#define NEG_QNAN_F32   (clog_bits_to_float(0xffc00000))
#define POS_PI_F32     (clog_bits_to_float(0x40490fd8))
#define NEG_PI_F32     (clog_bits_to_float(0xc0490fd8))

#undef POS_INF_F64
#undef NEG_INF_F64
#undef POS_QNAN_F64
#undef NEG_QNAN_F64
#undef POS_PI_F64
#undef NEG_PI_F64

#define POS_INF_F64    (clog_bits_to_double(0x7ff0000000000000ULL))
#define NEG_INF_F64    (clog_bits_to_double(0xfff0000000000000ULL))
#define POS_QNAN_F64   (clog_bits_to_double(0x7ff87ff7fdedffffULL))
#define NEG_QNAN_F64   (clog_bits_to_double(0xfff8000000000000ULL))
#define POS_PI_F64     (clog_bits_to_double(0x400921FB54442D18ULL))
#define NEG_PI_F64     (clog_bits_to_double(0xc00921fb54442d18ULL))

/* Subnormal constants for F32 */
#define POS_SUBNORM_MIN_F32  (clog_bits_to_float(0x00000001))   /* ~1.4e-45 */
#define POS_SUBNORM_MAX_F32  (clog_bits_to_float(0x007FFFFF))   /* ~1.17e-38 */
#define NEG_SUBNORM_MIN_F32  (clog_bits_to_float(0x80000001))
#define NEG_SUBNORM_MAX_F32  (clog_bits_to_float(0x807FFFFF))

/* Subnormal constants for F64 */
#define POS_SUBNORM_MIN_F64  (clog_bits_to_double(0x0000000000000001ULL))
#define POS_SUBNORM_MAX_F64  (clog_bits_to_double(0x000FFFFFFFFFFFFFULL))
#define NEG_SUBNORM_MIN_F64  (clog_bits_to_double(0x8000000000000001ULL))
#define NEG_SUBNORM_MAX_F64  (clog_bits_to_double(0x800FFFFFFFFFFFFFULL))

/* PI/2, PI/4 derived (rounded) bit patterns – kept as macros for table use. */
#define HALF_PI_F32  (POS_PI_F32 * 0.5f)
#define HALF_PI_F64  (POS_PI_F64 * 0.5)
#define QTR_PI_F32   (POS_PI_F32 * 0.25f)
#define QTR_PI_F64   (POS_PI_F64 * 0.25)
#define THREE_QTR_PI_F32 (POS_PI_F32 * 0.75f)
#define THREE_QTR_PI_F64 (POS_PI_F64 * 0.75)

/*
 * Conformance table for clogf().
 *
 * Spec (cppreference / C99 7.3.7.1):
 *   (-0, +0)     -> (-inf, +pi)         FE_DIVBYZERO
 *   (+0, +0)     -> (-inf, +0)          FE_DIVBYZERO
 *   (x, +inf)    -> (+inf, +pi/2)       finite x
 *   (x, NaN)     -> (NaN, NaN)          finite x; FE_INVALID *may* be raised
 *   (-inf, y)    -> (+inf, +pi)         finite positive y
 *   (+inf, y)    -> (+inf, +0)          finite positive y
 *   (-inf, +inf) -> (+inf, +3*pi/4)
 *   (+inf, +inf) -> (+inf, +pi/4)
 *   (+/-inf, NaN)-> (+inf, NaN)
 *   (NaN, y)     -> (NaN, NaN)          finite y; FE_INVALID *may* be raised
 *   (NaN, +/-inf)-> (+inf, NaN)
 *   (NaN, NaN)   -> (NaN, NaN)
 */
static libm_test_complex_data_f32
test_clogf_conformance_data[] = {
    /* ===== IEEE / C99 special-value table ===== */

    /* (+/-0, +/-0) – branch-cut continuity and DIVBYZERO */
    {{ 0.0f,  0.0f},                  {NEG_INF_F32,  0.0f},          FE_DIVBYZERO},
    {{-0.0f,  0.0f},                  {NEG_INF_F32,  POS_PI_F32},    FE_DIVBYZERO},
    {{ 0.0f, -0.0f},                  {NEG_INF_F32, -0.0f},          FE_DIVBYZERO},
    {{-0.0f, -0.0f},                  {NEG_INF_F32,  NEG_PI_F32},    FE_DIVBYZERO},

    /* (x, +/-inf) finite x */
    {{ 1.0f,  POS_INF_F32},           {POS_INF_F32,  HALF_PI_F32},   0},
    {{-1.0f,  POS_INF_F32},           {POS_INF_F32,  HALF_PI_F32},   0},
    {{ 0.0f,  POS_INF_F32},           {POS_INF_F32,  HALF_PI_F32},   0},
    {{ 1.0f,  NEG_INF_F32},           {POS_INF_F32, -HALF_PI_F32},   0},

    /* (-inf, y) finite positive y */
    {{NEG_INF_F32,  1.0f},            {POS_INF_F32,  POS_PI_F32},    0},
    {{NEG_INF_F32, -1.0f},            {POS_INF_F32,  NEG_PI_F32},    0},

    /* (+inf, y) finite positive y */
    {{POS_INF_F32,  1.0f},            {POS_INF_F32,  0.0f},          0},
    {{POS_INF_F32, -1.0f},            {POS_INF_F32, -0.0f},          0},

    /* (+/-inf, +/-inf) */
    {{POS_INF_F32, POS_INF_F32},      {POS_INF_F32,  QTR_PI_F32},        0},
    {{NEG_INF_F32, POS_INF_F32},      {POS_INF_F32,  THREE_QTR_PI_F32},  0},
    {{POS_INF_F32, NEG_INF_F32},      {POS_INF_F32, -QTR_PI_F32},        0},
    {{NEG_INF_F32, NEG_INF_F32},      {POS_INF_F32, -THREE_QTR_PI_F32},  0},

    /* (+/-inf, NaN) -> (+inf, NaN) */
    {{POS_INF_F32, POS_QNAN_F32},     {POS_INF_F32,  POS_QNAN_F32},  0},
    {{NEG_INF_F32, POS_QNAN_F32},     {POS_INF_F32,  POS_QNAN_F32},  0},

    /* (NaN, finite y) -> (NaN, NaN) */
    {{POS_QNAN_F32, 1.0f},            {POS_QNAN_F32, POS_QNAN_F32},  0},
    {{POS_QNAN_F32, 0.0f},            {POS_QNAN_F32, POS_QNAN_F32},  0},

    /* (NaN, +/-inf) -> (+inf, NaN) */
    {{POS_QNAN_F32, POS_INF_F32},     {POS_INF_F32,  POS_QNAN_F32},  0},
    {{POS_QNAN_F32, NEG_INF_F32},     {POS_INF_F32,  POS_QNAN_F32},  0},

    /* (NaN, NaN) -> (NaN, NaN) */
    {{POS_QNAN_F32, POS_QNAN_F32},    {POS_QNAN_F32, POS_QNAN_F32},  0},

    /* (finite x, NaN) -> (NaN, NaN) */
    {{1.0f, POS_QNAN_F32},            {POS_QNAN_F32, POS_QNAN_F32},  0},
    {{0.0f, POS_QNAN_F32},            {POS_QNAN_F32, POS_QNAN_F32},  0},

    /* ===== Branch-cut continuity (sign of imag part on negative real axis) ===== */
    {{-1.0f,  0.0f},                  {0.0f,  POS_PI_F32},           FE_INEXACT},
    {{-1.0f, -0.0f},                  {0.0f,  NEG_PI_F32},           FE_INEXACT},
    {{-2.0f,  0.0f},                  {0.693147182f, POS_PI_F32},    FE_INEXACT},
    {{-2.0f, -0.0f},                  {0.693147182f, NEG_PI_F32},    FE_INEXACT},

    /* ===== Regular finite values ===== */
    {{ 1.0f,  0.0f},                  {0.0f,  0.0f},                 0},
    {{ 2.0f,  0.0f},                  {0.693147182f, 0.0f},          FE_INEXACT},
    {{ 0.5f,  0.0f},                  {-0.693147182f, 0.0f},         FE_INEXACT},
    {{ 0.5f,  0.25f},                 {-0.581575394f, 0.463647604f}, FE_INEXACT},
    {{-1.0f,  2.0f},                  {0.804718971f, 2.03444386f},   FE_INEXACT},
    {{ 3.0f,  4.0f},                  {1.60943794f,  0.927295208f},  FE_INEXACT},
    {{ 0.0f,  1.0f},                  {0.0f,  HALF_PI_F32},          0},
    {{ 0.0f, -1.0f},                  {0.0f, -HALF_PI_F32},          0},
    {{ 0.0f,  2.0f},                  {0.693147182f, HALF_PI_F32},   FE_INEXACT},

    /* ===== Near unit circle: stresses precision of Re(log z) = 1/2 log(|z|^2) =====*/

    {{ 1.0f,            3.0e-6f},     {4.5e-12f,             3.0e-6f},      FE_INEXACT},
    {{ 1.0f,            1.0e-4f},     {5.0e-9f,              1.0e-4f},      FE_INEXACT},
    {{ 1.0f,            1.0e-3f},     {5.0e-7f,              1.0e-3f},      FE_INEXACT},
    {{ 1.0f,            1.0e-2f},     {4.99975e-5f,          0.00999967f},  FE_INEXACT},
    /* On the unit circle (cos t, sin t) at small angles. */
    {{ 0.99999952f,     0.000999999931f}, {2.3162886e-8f,    1.0e-3f},      FE_INEXACT},
    {{ 0.6f,            0.8f},        {0.0f,                 0.927295208f}, FE_INEXACT},

    /* ===== Large / tiny magnitudes (cabsf-stable region) ===== */
    {{ 1.0e30f,         1.0e30f},     {69.4241257f,          QTR_PI_F32},   FE_INEXACT},
    {{ 1.0e-30f,        1.0e-30f},    {-68.7309799f,         QTR_PI_F32},   FE_INEXACT},
    {{ 1.0e20f,         0.0f},        {46.0517f,             0.0f},         FE_INEXACT},
    {{ 1.0e-20f,        0.0f},        {-46.0517f,            0.0f},         FE_INEXACT},

    /* ===== Subnormal inputs ===== */
    {{POS_SUBNORM_MIN_F32, 0.0f},     {-103.278931f,         0.0f},         FE_INEXACT},
    {{POS_SUBNORM_MAX_F32, 0.0f},     {-87.336548f,          0.0f},         FE_INEXACT},
    {{0.0f, POS_SUBNORM_MIN_F32},     {-103.278931f,         HALF_PI_F32},  FE_INEXACT},
};

/*
 * Conformance table for clog(). Same spec as clogf().
 */
static libm_test_complex_data_f64
test_clog_conformance_data[] = {
    /* ===== IEEE / C99 special-value table ===== */
    {{ 0.0,  0.0},                    {NEG_INF_F64,  0.0},           FE_DIVBYZERO},
    {{-0.0,  0.0},                    {NEG_INF_F64,  POS_PI_F64},    FE_DIVBYZERO},
    {{ 0.0, -0.0},                    {NEG_INF_F64, -0.0},           FE_DIVBYZERO},
    {{-0.0, -0.0},                    {NEG_INF_F64,  NEG_PI_F64},    FE_DIVBYZERO},

    {{ 1.0,  POS_INF_F64},            {POS_INF_F64,  HALF_PI_F64},   0},
    {{-1.0,  POS_INF_F64},            {POS_INF_F64,  HALF_PI_F64},   0},
    {{ 0.0,  POS_INF_F64},            {POS_INF_F64,  HALF_PI_F64},   0},
    {{ 1.0,  NEG_INF_F64},            {POS_INF_F64, -HALF_PI_F64},   0},

    {{NEG_INF_F64,  1.0},             {POS_INF_F64,  POS_PI_F64},    0},
    {{NEG_INF_F64, -1.0},             {POS_INF_F64,  NEG_PI_F64},    0},

    {{POS_INF_F64,  1.0},             {POS_INF_F64,  0.0},           0},
    {{POS_INF_F64, -1.0},             {POS_INF_F64, -0.0},           0},

    {{POS_INF_F64, POS_INF_F64},      {POS_INF_F64,  QTR_PI_F64},        0},
    {{NEG_INF_F64, POS_INF_F64},      {POS_INF_F64,  THREE_QTR_PI_F64},  0},
    {{POS_INF_F64, NEG_INF_F64},      {POS_INF_F64, -QTR_PI_F64},        0},
    {{NEG_INF_F64, NEG_INF_F64},      {POS_INF_F64, -THREE_QTR_PI_F64},  0},

    {{POS_INF_F64, POS_QNAN_F64},     {POS_INF_F64,  POS_QNAN_F64},  0},
    {{NEG_INF_F64, POS_QNAN_F64},     {POS_INF_F64,  POS_QNAN_F64},  0},

    {{POS_QNAN_F64, 1.0},             {POS_QNAN_F64, POS_QNAN_F64},  0},
    {{POS_QNAN_F64, 0.0},             {POS_QNAN_F64, POS_QNAN_F64},  0},

    {{POS_QNAN_F64, POS_INF_F64},     {POS_INF_F64,  POS_QNAN_F64},  0},
    {{POS_QNAN_F64, NEG_INF_F64},     {POS_INF_F64,  POS_QNAN_F64},  0},

    {{POS_QNAN_F64, POS_QNAN_F64},    {POS_QNAN_F64, POS_QNAN_F64},  0},

    {{1.0, POS_QNAN_F64},             {POS_QNAN_F64, POS_QNAN_F64},  0},
    {{0.0, POS_QNAN_F64},             {POS_QNAN_F64, POS_QNAN_F64},  0},

    /* Branch-cut */
    {{-1.0,  0.0},                    {0.0,  POS_PI_F64},            FE_INEXACT},
    {{-1.0, -0.0},                    {0.0,  NEG_PI_F64},            FE_INEXACT},
    {{-2.0,  0.0},                    {0.6931471805599453, POS_PI_F64}, FE_INEXACT},
    {{-2.0, -0.0},                    {0.6931471805599453, NEG_PI_F64}, FE_INEXACT},

    /* Regular finite */
    {{ 1.0,  0.0},                    {0.0,  0.0},                   0},
    {{ 2.0,  0.0},                    {0.6931471805599453,   0.0},   FE_INEXACT},
    {{ 0.5,  0.0},                    {-0.6931471805599453,  0.0},   FE_INEXACT},
    {{ 0.5,  0.25},                   {-0.5815754049028405, 0.4636476090008061},     FE_INEXACT},
    {{-1.0,  2.0},                    {0.8047189562170503,  2.0344439357957027},     FE_INEXACT},
    {{ 3.0,  4.0},                    {1.6094379124341003,  0.9272952180016122},     FE_INEXACT},
    {{ 0.0,  1.0},                    {0.0,  HALF_PI_F64},           0},
    {{ 0.0, -1.0},                    {0.0, -HALF_PI_F64},           0},
    {{ 0.0,  2.0},                    {0.6931471805599453, HALF_PI_F64},             FE_INEXACT},

    /* Near unit circle (same spirit as clogf table; double has more headroom
     * so the catastrophic-cancellation band shrinks but does not vanish).
     */
    {{ 1.0,  3.0e-6},                 {4.499955963390435e-12,  3.0e-6},              FE_INEXACT},
    {{ 1.0,  1.0e-8},                 {5.000000000000001e-17,  1.0e-8},              FE_INEXACT},
    {{ 1.0,  1.0e-4},                 {4.999999987500001e-9,   1.0e-4},              FE_INEXACT},
    /* On the unit circle. */
    {{ 0.6,  0.8},                    {0.0,                    0.9272952180016122},  FE_INEXACT},

    /* Large / tiny */
    {{ 1.0e150,  1.0e150},            {345.6741979,            QTR_PI_F64},          FE_INEXACT},
    {{ 1.0e-150, 1.0e-150},           {-345.0210708,           QTR_PI_F64},          FE_INEXACT},
    {{ 1.0e200,  0.0},                {460.5170185988091,      0.0},                 FE_INEXACT},
    {{ 1.0e-200, 0.0},                {-460.5170185988091,     0.0},                 FE_INEXACT},

    /* Subnormals */
    {{POS_SUBNORM_MIN_F64, 0.0},      {-744.4400719213812,     0.0},                 FE_INEXACT},
    {{POS_SUBNORM_MAX_F64, 0.0},      {-708.3964185322642,     0.0},                 FE_INEXACT},
    {{0.0, POS_SUBNORM_MIN_F64},      {-744.4400719213812,     HALF_PI_F64},         FE_INEXACT},
};

#endif /* __TEST_CLOG_DATA_H__ */
