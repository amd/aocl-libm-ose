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
void hypot_single_precision()
{
    printf ("Using Scalar single precision hypotf()\n");
    float ipf1 = 0.01, ipf2 = -0.5, opf;
    opf = amd_hypotf (ipf1, ipf2);
    printf("Input: %f,%f\tOutput: %f\n", ipf1,ipf2, opf);
    printf("----------\n");
}

void hypot_double_precision()
{
    printf ("Using Scalar double precision hypot()\n");
    double ipd1 = 1.89, ipd2= 0.45, opd;
    opd = amd_hypot(ipd1, ipd2);
    printf("Input: %lf, %lf\tOutput: %f\n", ipd1, ipd2, opd);
    printf("----------\n");
}

/**********************************************
 *     Vector Variants
 * *******************************************/
void hypot_vector_single_precision_4()
{
    #if 0
    printf("Using Vector single precision - 4 floats (vrs4) hypot()\n");
    __m128 input, input2, result;
    float  input_array_vrs4[4] = {34.65, -7.89, 91.0, -198.34};
    float  input_2_array_vrs4[4] = {-3.65, -80.89, -1.0, -34.01};
    float  output_array_vrs4[4];

    input = _mm_loadu_ps(input_array_vrs4);
    input2 = _mm_loadu_ps(input_2_array_vrs4);
    result = amd_vrs4_hypotf(input, input2);
    _mm_storeu_ps(output_array_vrs4, result);

    printf("Input: {%f, %f, %f, %f}, Input2: {%f, %f, %f, %f}, Output: {%f, %f, %f, %f}\n",
            input_array_vrs4[0], input_array_vrs4[1], input_array_vrs4[2], input_array_vrs4[3],
            input_2_array_vrs4[0], input_2_array_vrs4[1], input_2_array_vrs4[2], input_2_array_vrs4[3],
            output_array_vrs4[0], output_array_vrs4[1], output_array_vrs4[2], output_array_vrs4[3]);
    printf("----------\n");
    #endif
}

void hypot_vector_single_precision_8()
{
    #if 0
    printf("Using Vector single precision - 8 floats (vrs8) hypot()\n");
    __m256 input, input2, result;
    float input_array_vrs8[8] = {1.2, 0.0, -2.3, 3.4, 5.6, -7.8, 8.9, -1.0};
    float input_2_array_vrs8[8] = {-2.123, -0.12, -0.0, 4.987, 6.342, 17.8, -98.9, -1.0};
    float output_array_vrs8[8];

    input = _mm256_loadu_ps(input_array_vrs8);
    input2 = _mm256_loadu_ps(input_2_array_vrs8);
    result = amd_vrs8_hypotf(input, input2);
    _mm256_storeu_ps(output_array_vrs8, result);

    printf("Input: {%f, %f, %f, %f, %f, %f, %f, %f}, Input2: {%f, %f, %f, %f, %f, %f, %f, %f}, Output: {%f, %f, %f, %f, %f, %f, %f, %f}\n",
            input_array_vrs8[0], input_array_vrs8[1], input_array_vrs8[2], input_array_vrs8[3],
            input_array_vrs8[4], input_array_vrs8[5], input_array_vrs8[6], input_array_vrs8[7],
            input_2_array_vrs8[0], input_2_array_vrs8[1], input_2_array_vrs8[2], input_2_array_vrs8[3],
            input_2_array_vrs8[4], input_2_array_vrs8[5], input_2_array_vrs8[6], input_2_array_vrs8[7],
            output_array_vrs8[0], output_array_vrs8[1], output_array_vrs8[2], output_array_vrs8[3],
            output_array_vrs8[4], output_array_vrs8[5], output_array_vrs8[6], output_array_vrs8[7]);
    printf("----------\n");
    #endif
}

void hypot_vector_double_precision_2()
{
    #if 0
    printf("Using Vector double precision - 2 doubles (vrd2) hypot()\n");
    __m128d input, input2, result;
    double  input_array_vrd2[2] = {0.0, 67.89};
    double  input_2_array_vrd2[2] = {-5.09, -0.0};
    double  output_array[2];

    input = _mm_loadu_pd(input_array_vrd2);
    input2 = _mm_loadu_pd(input_2_array_vrd2);
    result = amd_vrd2_hypot(input, input2);
    _mm_storeu_pd(output_array, result);

    printf("Input: {%lf, %lf}, Input2: {%lf, %lf}, Output: {%lf, %lf}\n",
            input_array_vrd2[0], input_array_vrd2[1],
            input_2_array_vrd2[0], input_2_array_vrd2[1],
            output_array[0], output_array[1]);
    printf("----------\n");
    #endif
}

void hypot_vector_double_precision_4()
{
    #if 0
    printf("Using Vector double precision - 4 doubles (vrd4) hypot()\n");
    __m256d input, input2, result;
    double input_array_vrd4[4] = {2.3, 0.0, -0.0, -43.4};
    double input_2_array_vrd4[4] = {-1.9, 0.0, 0.0, -983.4766};
    double output_array_vrd4[4];

    input = _mm256_loadu_pd(input_array_vrd4);
    input2 = _mm256_loadu_pd(input_2_array_vrd4);
    result = amd_vrd4_hypot(input, input2);
    _mm256_storeu_pd(output_array_vrd4, result);

    printf("Input: {%lf, %lf, %lf, %lf}, Input2: {%lf, %lf, %lf, %lf}, Output: {%lf, %lf, %lf, %lf}\n",
            input_array_vrd4[0], input_array_vrd4[1], input_array_vrd4[2], input_array_vrd4[3],
            input_2_array_vrd4[0], input_2_array_vrd4[1], input_2_array_vrd4[2], input_2_array_vrd4[3],
            output_array_vrd4[0], output_array_vrd4[1], output_array_vrd4[2],output_array_vrd4[3]);
    printf("----------\n");
    #endif
}

/**********************************************
 *     Vector Array Variants
 * *******************************************/
void hypot_single_precision_array()
{
    #if 0
    printf("Using Single Precision Vectory Array (vrsa) hypot()\n");
    int n=5;
    float input[10] = {1.0f, 3.0f, -5.0f, 0.0f, -9.0f, 11.0f, 13.0f, 15.0f, -17.0f, 19.0f};
    float input2[10] = {3.0f, -1.0f, -0.0f, -6.0f, 0.0f, 0.0f, 4.0f, -1.0f, 2.0f, -10.0f};
    float output[10] = {0};

    amd_vrsa_hypotf(n, input, input2, output);

    printf("Input: {");
    for(int i=0; i<10; ++i)
    {
        printf("%f, ", input[i]);
    }
    printf("}, Input2: {");
    for(int i=0; i<10; ++i)
    {
        printf("%f, ", input2[i]);
    }
    printf("}, Output: {");
    for(int i=0; i<10; ++i)
    {
        printf("%f, ", output[i]);
    }
    printf("}\n");
    printf("----------\n");
    #endif
}

void hypot_double_precision_array()
{
    #if 0
    printf("Using Double Precision Vectory Array (vrda) hypot()\n");
    int n=8;
    double input[10] = {-0.0, -0.0, 4.0, 2.0, 6.0, -12.0, 14.0, -1.0, 3.0, -5.0};
    double input2[10] = {0.0, -0.0, 3.0, -4.0, 5.0, 6.0, -7.0, 8.0, -9.0, -10.0};
    double output[10] = {0};

    amd_vrda_hypot(n, input, input2, output);

    printf("Input: {");
    for(int i=0; i<10; ++i)
    {
        printf("%f, ", input[i]);
    }
    printf("}, Input2: {");
    for(int i=0; i<10; ++i)
    {
        printf("%f, ", input2[i]);
    }
    printf("}, Output: {");
    for(int i=0; i<10; ++i)
    {
        printf("%f, ", output[i]);
    }
    printf("}\n");
    printf("----------\n");
    #endif
}

int use_hypot()
{
    printf("\n\n***** hypot() *****\n");
    hypot_single_precision();
    hypot_double_precision();
    hypot_vector_single_precision_4();
    hypot_vector_single_precision_8();
    hypot_vector_double_precision_2();
    hypot_vector_double_precision_4();
    hypot_single_precision_array();
    hypot_double_precision_array();
    return 0;
}
