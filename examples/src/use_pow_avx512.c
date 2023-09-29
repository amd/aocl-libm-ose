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
#include <immintrin.h>

void pow_vector_single_precision_16()
{
    #if defined (__AVX512__)
    printf("Using Vector single precision - 16 floats (vrs16) pow()\n");
    __m512 input, input2, result;
    float input_array_vrs16[16] = {0.2, 0.8, 0.3, 0.164, 0.006, 0.8, 0.1, -1.0,
                                   -0.0, 165.000012, 0.345, 10.5, 0.75, 0.98, 0.09, -1.0};
    float input_2_array_vrs16[16] = {1.2, 1.0, 1.3, -0.4, -0.006, 0.8, 0.1, -1.0,
                                   -0.0, 0.0, 0.5, 0.5, 0.75, 1.1, 0.0989, -1.0};
    float output_array_vrs16[16];

    input = _mm512_loadu_ps(input_array_vrs16);
    input2 = _mm512_loadu_ps(input_2_array_vrs16);
    result = amd_vrs16_powf(input, input2);
    _mm512_storeu_ps(output_array_vrs16, result);

    printf("Input: {%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f},\nInput2: {%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f},\nOutput: {%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f}\n",
            input_array_vrs16[0], input_array_vrs16[1], input_array_vrs16[2], input_array_vrs16[3],
            input_array_vrs16[4], input_array_vrs16[5], input_array_vrs16[6], input_array_vrs16[7],
            input_array_vrs16[8], input_array_vrs16[9], input_array_vrs16[10], input_array_vrs16[11],
            input_array_vrs16[12], input_array_vrs16[13], input_array_vrs16[14], input_array_vrs16[15],
            input_2_array_vrs16[0], input_2_array_vrs16[1], input_2_array_vrs16[2], input_2_array_vrs16[3],
            input_2_array_vrs16[4], input_2_array_vrs16[5], input_2_array_vrs16[6], input_2_array_vrs16[7],
            input_2_array_vrs16[8], input_2_array_vrs16[9], input_2_array_vrs16[10], input_2_array_vrs16[11],
            input_2_array_vrs16[12], input_2_array_vrs16[13], input_2_array_vrs16[14], input_2_array_vrs16[15],
            output_array_vrs16[0], output_array_vrs16[1], output_array_vrs16[2], output_array_vrs16[3],
            output_array_vrs16[4], output_array_vrs16[5], output_array_vrs16[6], output_array_vrs16[7],
            output_array_vrs16[8], output_array_vrs16[9], output_array_vrs16[10], output_array_vrs16[11],
            output_array_vrs16[12], output_array_vrs16[13], output_array_vrs16[14], output_array_vrs16[15]);
    printf("----------\n");
    #endif
}

void pow_vector_double_precision_8() {
    #if defined (__AVX512__)
    printf("Using Vector double precision - 8 doubles (vrd8) pow()\n");
    __m512d input, input2, result;
    double input_array_vrd8[8] = {-10.0, 0.1, 0.5, 0.5, 0.75, 0.98, 1.0989, -1.0};
    double input_2_array_vrd8[8] = {-0.0, 0.0, 1.5, -0.5, 0.75, 0.009, 0.0989, 0.01};
    double output_array_vrd8[8];

    input = _mm512_loadu_pd(input_array_vrd8);
    input2 = _mm512_loadu_pd(input_2_array_vrd8);
    result = amd_vrd8_pow(input, input2);
    _mm512_storeu_pd(output_array_vrd8, result);

    printf("Input: {%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf},\nInput2: {%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf},\nOutput: {%lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf}\n",
            input_array_vrd8[0], input_array_vrd8[1], input_array_vrd8[2], input_array_vrd8[3],
            input_array_vrd8[4], input_array_vrd8[5], input_array_vrd8[6], input_array_vrd8[7],
            input_2_array_vrd8[0], input_2_array_vrd8[1], input_2_array_vrd8[2], input_2_array_vrd8[3],
            input_2_array_vrd8[4], input_2_array_vrd8[5], input_2_array_vrd8[6], input_2_array_vrd8[7],
            output_array_vrd8[0], output_array_vrd8[1], output_array_vrd8[2],output_array_vrd8[3],
            output_array_vrd8[4], output_array_vrd8[5], output_array_vrd8[6],output_array_vrd8[7]);
    printf("----------\n");
    #endif
}

int use_pow_avx512()
{
    printf("\n\n***** pow_avx512() *****\n");
    pow_vector_single_precision_16();
    pow_vector_double_precision_8();
    return 0;
}
