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

/**********************************************
 *     Scalar Variants
 * *******************************************/
void log_single_precision()
{
    printf ("Using Scalar single precision logf()\n");
    float ipf1 = 0.01, opf;
    opf = amd_logf (ipf1);
    printf("Input: %f\tOutput: %f\n", ipf1, opf);
    printf("----------\n");
}

void log_double_precision()
{
    printf ("Using Scalar double precision log()\n");
    double ipd1 = 1.89, opd;
    opd = amd_log(ipd1);
    printf("Input: %lf\tOutput: %f\n", ipd1, opd);
    printf("----------\n");
}

/**********************************************
 *     Vector Variants
 * *******************************************/
void log_vector_single_precision_4()
{
    printf("Using Vector single precision - 4 floats (vrs4) log()\n");
    __m128 input, result;
    float  input_array_vrs4[4] = {34.65, 67.89, 91.0, 198.34};
    float  output_array_vrs4[4];

    input = _mm_loadu_ps(input_array_vrs4);
    result = amd_vrs4_logf(input);
    _mm_storeu_ps(output_array_vrs4, result);

    printf("Input: {%f, %f, %f, %f}, Output: {%f, %f, %f, %f}\n",
            input_array_vrs4[0], input_array_vrs4[1], input_array_vrs4[2], input_array_vrs4[3],
            output_array_vrs4[0], output_array_vrs4[1], output_array_vrs4[2], output_array_vrs4[3]);
    printf("----------\n");
}

void log_vector_single_precision_8()
{
    printf("Using Vector single precision - 8 floats (vrs8) log()\n");
    __m256 input, result;
    float input_array_vrs8[8] = {1.2, 0.1, 2.3, 3.4, 5.6, 7.8, 8.9, 1.0};
    float output_array_vrs8[8];

    input = _mm256_loadu_ps(input_array_vrs8);
    result = amd_vrs8_logf(input);
    _mm256_storeu_ps(output_array_vrs8, result);

    printf("Input: {%f, %f, %f, %f, %f, %f, %f, %f}, Output: {%f, %f, %f, %f, %f, %f, %f, %f}\n",
            input_array_vrs8[0], input_array_vrs8[1], input_array_vrs8[2], input_array_vrs8[3],
            input_array_vrs8[4], input_array_vrs8[5], input_array_vrs8[6], input_array_vrs8[7],
            output_array_vrs8[0], output_array_vrs8[1], output_array_vrs8[2], output_array_vrs8[3],
            output_array_vrs8[4], output_array_vrs8[5], output_array_vrs8[6], output_array_vrs8[7]);
    printf("----------\n");
}

void log_vector_double_precision_2()
{
    printf("Using Vector double precision - 2 doubles (vrd2) log()\n");
    __m128d input, result;
    double  input_array_vrd2[2] = {34.65, 67.89};
    double  output_array[2];

    input = _mm_loadu_pd(input_array_vrd2);
    result = amd_vrd2_log(input);
    _mm_storeu_pd(output_array, result);

    printf("Input: {%lf, %lf}, Output: {%lf, %lf}\n",
            input_array_vrd2[0], input_array_vrd2[1],
            output_array[0], output_array[1]);
    printf("----------\n");
}

void log_vector_double_precision_4()
{
    printf("Using Vector double precision - 4 doubles (vrd4) log()\n");
    __m256d input, result;
    double input_array_vrd4[4] = {2.3, 4.5, 56.5, 43.4};
    double output_array_vrd4[4];

    input = _mm256_loadu_pd(input_array_vrd4);
    result = amd_vrd4_log(input);
    _mm256_storeu_pd(output_array_vrd4, result);

    printf("Input: {%lf, %lf, %lf, %lf}, Output: {%lf, %lf, %lf, %lf}\n",
            input_array_vrd4[0], input_array_vrd4[1], input_array_vrd4[2], input_array_vrd4[3],
            output_array_vrd4[0], output_array_vrd4[1], output_array_vrd4[2],output_array_vrd4[3]);
    printf("----------\n");
}

/**********************************************
 *     Vector Array Variants
 * *******************************************/
void log_single_precision_array()
{
    printf("Using Single Precision Vectory Array (vrsa) log()\n");
    int n=5;
    float input[10] = {1.0f, 3.0f, 5.0f, 7.0f, 9.0f, 11.0f, 13.0f, 15.0f, 17.0f, 19.0f};
    float output[10] = {0};

    amd_vrsa_logf(n, input, output);

    printf("Input: {");
    for(int i=0; i<10; ++i)
    {
        printf("%f, ", input[i]);
    }
    printf("}, Output: {");
    for(int i=0; i<10; ++i)
    {
        printf("%f, ", output[i]);
    }
    printf("}\n");
    printf("----------\n");
}

void log_double_precision_array()
{
    printf("Using Double Precision Vectory Array (vrda) log()\n");
    int n=8;
    double input[10] = {10.0, 8.0, 4.0, 2.0, 6.0, 12.0, 14.0, 1.0, 3.0, 5.0};
    double output[10] = {0};

    amd_vrda_log(n, input, output);

    printf("Input: {");
    for(int i=0; i<10; ++i)
    {
        printf("%f, ", input[i]);
    }
    printf("}, Output: {");
    for(int i=0; i<10; ++i)
    {
        printf("%f, ", output[i]);
    }
    printf("}\n");
    printf("----------\n");
}

int use_log()
{
    printf("\n\n***** log() *****\n");
    log_single_precision();
    log_double_precision();
    log_vector_single_precision_4();
    log_vector_single_precision_8();
    log_vector_double_precision_2();
    log_vector_double_precision_4();
    log_single_precision_array();
    log_double_precision_array();
    return 0;
}
