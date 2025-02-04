/*
 * Copyright (C) 2025 Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __TEST_NEXTAFTER_DATA_H__
#define __TEST_NEXTAFTER_DATA_H__

#include <fenv.h>
#include <libm_tests.h>

/* Test cases to check for exceptions for the nextafter() routine. These test cases are not exhaustive */
/* TODO: Add more conformance cases */
static libm_test_special_data_f64
test_nextafter_conformance_data[] = {
    {0x7FF4001000000000,0x7FF4001000000000,0,0x7FF4001000000000,}, // nextafter(snan,snan)
    {0x7FF4001000000000,0x400199999999999A,0,0x7FF4001000000000,}, // nextafter(snan,2.2)
    {0x40560CCCCCCCCCCD,0x40560CCCCCCCCCCD,0,0x40560CCCCCCCCCCD,}, // nextafter(88.2,88.2) (x==y)
    {0x0000000000000000,0x0000000000000000,0,0x0000000000000000,}, // nextafter(0,0) (x==y)
    {0x0000000000000000,0x0000000000000000,FE_INEXACT | FE_UNDERFLOW,0x0000000000000001,}, // nextafter(0,1) (x==0 & y+ve)
    {0x7ff0000000000000,0x40560CCCCCCCCCCD,0,0x7ff0000000000000,}, // nextafter(+inf,88.2)
    {0x8000000000000000, 0x0000000000000000,FE_INEXACT | FE_UNDERFLOW,0x8000000000000001} // nextafter(0, -1) (x==-0 & y-ve)
};

/* Test cases to check for exceptions for the nextafter() routine. These test cases are not exhaustive */
/* TODO: Add more conformance cases */
static libm_test_special_data_f32
test_nextafterf_conformance_data[] = {
    {0x7fa00000,0x7fa00000,0,0x7fa00000,}, // nextafter(snan,snan)
    {0x7fa00000,0x400CCCCD,0,0x7fa00000,}, // nextafter(snan,2.2)
    {0x41266666,0x41266666,0,0x41266666,}, // nextafter(10.4,10.4) (x==y)
    {0x00000000,0x40ACCCCD,FE_INEXACT,0x00000001,}, // nextafter(0,5.4)
    {0x7f800000,0x40ACCCCD,0,0x7f800000}, // nextafter(+inf,5.4)
    {0x80000000, 0xC0800000,FE_INEXACT, 0x80000001}, // nextafter(-0,-4)
};

#endif	/*__TEST_POW_DATA_H__*/