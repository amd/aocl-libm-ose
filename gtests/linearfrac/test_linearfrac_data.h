/*
 * Copyright (C) 2024 Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __TEST_LINEARFRAC_DATA_H__
#define __TEST_LINEARFRAC_DATA_H__

#include <fenv.h>
#include <libm_tests.h>

/* Test cases to check for exceptions for the linearfrac() routine. These test cases are not exhaustive

    As linearfrac() is not a function specified in ISO-IEC, the special cases and exceptions have been
    populated based on specifications of mul and div operations.

*/

/* TODO: Add more test cases */
static libm_test_special_data_f64
test_linearfrac_conformance_data[] = {
    {0x400999999999999a, 0x0000000000000000, 0x4014666666666666, 0x4014666666666666, 0x4014666666666666, 0x0000000000000000, 0,0x7ff0000000000000 },
    {0x7fefffffffffffff, 0x3f747ae147ae147b, 0x3f747ae147ae147b, 0x3f747ae147ae147b, 0x4014666666666666, 0x4000cccccccccccd, 0, 0x7ff0000000000000},
    {0x10000000000000, 0x10000000000000, 0x10000000000000,  0x42dc122183e46000, 0x42dc122183e46000, 0x42dc122183e46000, 0, 0x10000000000000},
    {0x43DFFD0004000000, 0x3FF8000000000000, 0x4004000000000000, },
};

static libm_test_special_data_f32
test_linearfracf_conformance_data[] = {
};

#endif