/*
 * Copyright (C) 2008-2023 Advanced Micro Devices, Inc. All rights reserved.
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

/*
 * Test cases to check the conformance for the cexpf() routine.
 * These test cases are not exhaustive.
 * These values as as per GLIBC output.
 */

static libm_test_complex_data_f32
test_cexpf_conformance_data[] = {
    #if defined(_WIN64) || defined(_WIN32)
        /* Test inputs of -/+0 */
        {{0.0, 0.0},                   {1.0, 0.0},                   0},
        {{-0.0, 0.0},                  {1.0, 0.0},                   0},

        /* Test inputs of -/+inf, Signalling NAN and Quiet NAN*/
        {{POS_INF_F32, 0.0},           {POS_INF_F32, 0.0},           0},
        {{NEG_INF_F32, POS_INF_F32},   {0.0, 0.0},                   0},
        {{POS_INF_F32, POS_INF_F32},   {POS_INF_F32, POS_QNAN_F32},  0},
        {{NEG_INF_F32, POS_QNAN_F32},  {0.0, 0.0},                   0},
        {{POS_INF_F32, POS_QNAN_F32},  {POS_INF_F32, POS_QNAN_F32},  0},
        {{8.1, POS_INF_F32},           {POS_QNAN_F32, POS_QNAN_F32}, FE_INEXACT},
        {{POS_QNAN_F32, 0.0},          {POS_QNAN_F32, 0.0},          0},
        {{POS_QNAN_F32, 3.1},          {POS_QNAN_F32, POS_QNAN_F32}, FE_INVALID},
        {{POS_QNAN_F32, POS_QNAN_F32}, {POS_QNAN_F32, POS_QNAN_F32}, 0},
        {{8.1, POS_QNAN_F32},          {POS_QNAN_F32, POS_QNAN_F32}, FE_INEXACT},

        /* Test inputs of some standard values */
        {{0.0, POS_PI_F32},             {-1.0, 0.0},                   0}, // Euler's Formula
        {{POS_PI_F32, POS_PI_F32},      {-23.1, 0.0},                  0},

        /* Test inputs of some random values */
        {{-1.0, 2.0},                   {-0.153092, 0.334512},         0},
        {{-0.0076, 1827.76},            {0.8, -0.6},                   0},
    #else
        /* Test inputs of -/+0 */
        {{0.0 + 0.0 * I},                   {1.0 + 0.0 * I},                   0},
        {{-0.0 + 0.0 * I},                  {1.0 + 0.0 * I},                   0},

        /* Test inputs of -/+inf, Signalling NAN and Quiet NAN*/
        {{POS_INF_F32 + 0.0 * I},           {POS_INF_F32 + 0.0 * I},           0},
        {{NEG_INF_F32 + POS_INF_F32 * I},   {0.0 + 0.0 * I},                   0},
        {{POS_INF_F32 + POS_INF_F32 * I},   {POS_INF_F32 + POS_QNAN_F32 * I},  0},
        {{NEG_INF_F32 + POS_QNAN_F32 * I},  {0.0 + 0.0 * I},                   0},
        {{POS_INF_F32 + POS_QNAN_F32 * I},  {POS_INF_F32 + POS_QNAN_F32 * I},  0},
        {{8.1 + POS_INF_F32 * I},           {POS_QNAN_F32 + POS_QNAN_F32 * I}, FE_INEXACT},
        {{POS_QNAN_F32 + 0.0 * I},          {POS_QNAN_F32 + 0.0 * I},          0},
        {{POS_QNAN_F32 + 3.1 * I},          {POS_QNAN_F32 + POS_QNAN_F32 * I}, FE_INVALID},
        {{POS_QNAN_F32 + POS_QNAN_F32 * I}, {POS_QNAN_F32 + POS_QNAN_F32 * I}, 0},
        {{8.1 + POS_QNAN_F32 * I},          {POS_QNAN_F32 + POS_QNAN_F32 * I}, FE_INEXACT},

        /* Test inputs of some standard values */
        {{0.0 + POS_PI_F32 * I},             {-1.0 + 0.0 * I},                   0}, // Euler's Formula
        {{POS_PI_F32 + POS_PI_F32 * I},      {-23.1 + 0.0 * I},                  0},

        /* Test inputs of some random values */
        {{-1.0 + 2.0 * I},                   {-0.153092 + 0.334512 * I},         0},
        {{-0.0076 + 1827.76 * I},            {0.8 + -0.6 * I},                   0},
    #endif
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
    #if defined(_WIN64) || defined(_WIN32)
        /* Test inputs of -/+0 */
        {{0.0, 0.0},                   {1.0, 0.0},                   0},
        {{-0.0, 0.0},                  {1.0, 0.0},                   0},

        /* Test inputs of -/+inf, Signalling NAN and Quiet NAN*/
        {{POS_INF_F64, 0.0},           {POS_INF_F64, 0.0},           0},
        {{NEG_INF_F64, POS_INF_F64},   {0.0, 0.0},                   0},
        {{POS_INF_F64, POS_INF_F64},   {POS_INF_F64, POS_QNAN_F64},  0},
        {{NEG_INF_F64, POS_QNAN_F64},  {0.0, 0.0},                   0},
        {{POS_INF_F64, POS_QNAN_F64},  {POS_INF_F64, POS_QNAN_F64},  0},
        {{8.1, POS_INF_F64},           {POS_QNAN_F64, POS_QNAN_F64}, FE_INEXACT},
        {{POS_QNAN_F64, 0.0},          {POS_QNAN_F64, 0.0},          0},
        {{POS_QNAN_F64, 3.1},          {POS_QNAN_F64, POS_QNAN_F64}, FE_INVALID},
        {{POS_QNAN_F64, POS_QNAN_F64}, {POS_QNAN_F64, POS_QNAN_F64}, 0},
        {{8.1, POS_QNAN_F64},          {POS_QNAN_F64, POS_QNAN_F64}, FE_INEXACT},

        /* Test inputs of some standard values */
        {{0.0, POS_PI_F64},             {-1.0, 0.0},                   0}, // Euler's Formula
        {{POS_PI_F64, POS_PI_F64},      {-23.1, 0.0},                  0},

        /* Test inputs of some random values */
        {{-1.0, 2.0},                   {-0.153092, 0.334512},         0},
        {{-0.0076, 1827.76},            {0.8, -0.6},                   0},
    #else
        /* Test inputs of -/+0 */
        {{0.0 + 0.0 * I},                   {1.0 + 0.0 * I},                   0},
        {{-0.0 + 0.0 * I},                  {1.0 + 0.0 * I},                   0},

        /* Test inputs of -/+inf, Signalling NAN and Quiet NAN*/
        {{POS_INF_F64 + 0.0 * I},           {POS_INF_F64 + 0.0 * I},           0},
        {{NEG_INF_F64 + POS_INF_F64 * I},   {0.0 + 0.0 * I},                   0},
        {{POS_INF_F64 + POS_INF_F64 * I},   {POS_INF_F64 + POS_QNAN_F64 * I},  0},
        {{NEG_INF_F64 + POS_QNAN_F64 * I},  {0.0 + 0.0 * I},                   0},
        {{POS_INF_F64 + POS_QNAN_F64 * I},  {POS_INF_F64 + POS_QNAN_F64 * I},  0},
        {{8.1 + POS_INF_F64 * I},           {POS_QNAN_F64 + POS_QNAN_F64 * I}, FE_INEXACT},
        {{POS_QNAN_F64 + 0.0 * I},          {POS_QNAN_F64 + 0.0 * I},          0},
        {{POS_QNAN_F64 + 3.1 * I},          {POS_QNAN_F64 + POS_QNAN_F64 * I}, FE_INVALID},
        {{POS_QNAN_F64 + POS_QNAN_F64 * I}, {POS_QNAN_F64 + POS_QNAN_F64 * I}, 0},
        {{8.1 + POS_QNAN_F64 * I},          {POS_QNAN_F64 + POS_QNAN_F64 * I}, FE_INEXACT},

        /* Test inputs of some standard values */
        {{0.0 + POS_PI_F64 * I},             {-1.0 + 0.0 * I},                   0}, // Euler's Formula
        {{POS_PI_F64 + POS_PI_F64 * I},      {-23.1 + 0.0 * I},                  0},

        /* Test inputs of some random values */
        {{-1.0 + 2.0 * I},                   {-0.153092 + 0.334512 * I},         0},
        {{-0.0076 + 1827.76 * I},            {0.8 + -0.6 * I},                   0},
    #endif
    // For z = -∞+yi, the result is +0cis(y)
    // For z = +∞+yi, the result is +∞cis(y)
};

#endif	/*__TEST_CEXP_DATA_H__*/
