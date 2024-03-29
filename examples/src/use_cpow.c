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

#define AMD_LIBM_VEC_EXPERIMENTAL

#include <stdio.h>
#include "amdlibm.h"
#include "amdlibm_vec.h"
#include <complex.h>

/**********************************************
 *     Complex Scalar Variants
 * *******************************************/
void cpow_single_precision()
{
    printf ("Using Complex Scalar single precision cpowf()\n");
    #if (defined (_WIN64) || defined (_WIN32))
        fc32_t input1 = {0.5, 2.1}, input2 = {1.2, 0.1}, output;
    #else
        fc32_t input1 = 0.5 + 2.1*I, input2 = 1.2 + 0.1*I, output;
    #endif

    output = amd_cpowf (input1, input2);

    printf("Input: (%f +i %f), (%f +i %f)\tOutput: (%f +i %f)\n",
            crealf(input1), cimagf(input1),
            crealf(input2), cimagf(input2),
            crealf(output), cimagf(output));
    printf("----------\n");
}

void cpow_double_precision()
{
    printf ("Using Complex Scalar double precision cpow()\n");
    #if (defined (_WIN64) || defined (_WIN32))
        fc64_t input1 = {1.9, 0.08}, input2 = {0.8, 0.1}, output;
    #else
        fc64_t input1 = 1.9 + 0.08*I, input2 = 0.8 + 0.1*I, output;
    #endif

    output = amd_cpow (input1, input2);

    printf("Input: (%f +i %f), (%f +i %f)\tOutput: (%f +i %f)\n",
            creal(input1), cimag(input1),
            creal(input2), cimag(input2),
            creal(output), cimag(output));
    printf("----------\n");
}

int use_cpow()
{
    printf("\n\n***** cpow() *****\n");
    cpow_single_precision();
    cpow_double_precision();
    return 0;
}
