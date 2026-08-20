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
 * Thin abstraction over the test framework. Tests use the AOCLSORT_* macros
 * only, so swapping the framework (e.g. gtest -> Catch2) touches this header
 * alone.
 */

#pragma once

#include "gtest.h"

#define AOCLSORT_TEST(suite, name)   TEST(suite, name)
#define AOCLSORT_TEST_F(fix, name)   TEST_F(fix, name)
#define AOCLSORT_TEST_P(fix, name)   TEST_P(fix, name)

#define AOCLSORT_EXPECT_TRUE(x)      EXPECT_TRUE(x)
#define AOCLSORT_EXPECT_FALSE(x)     EXPECT_FALSE(x)
#define AOCLSORT_EXPECT_EQ(a, b)     EXPECT_EQ(a, b)
#define AOCLSORT_EXPECT_NE(a, b)     EXPECT_NE(a, b)
#define AOCLSORT_EXPECT_LT(a, b)     EXPECT_LT(a, b)
#define AOCLSORT_EXPECT_LE(a, b)     EXPECT_LE(a, b)
#define AOCLSORT_EXPECT_GT(a, b)     EXPECT_GT(a, b)
#define AOCLSORT_EXPECT_GE(a, b)     EXPECT_GE(a, b)
#define AOCLSORT_ASSERT_TRUE(x)      ASSERT_TRUE(x)
#define AOCLSORT_ASSERT_EQ(a, b)     ASSERT_EQ(a, b)
