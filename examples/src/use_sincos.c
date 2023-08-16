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
void sincos_single_precision()
{
    printf ("Using Scalar single precision sincosf()\n");
    float input = 0.01;
    float sin;
    float cos;

    amd_sincosf(input, &sin, &cos);

    printf("Input: %f\tOutput: sin: %f\tcos: %f\n", input, sin, cos);
    printf("----------\n");
}

void sincos_double_precision()
{
    printf ("Using Scalar double precision sincos()\n");
    double input = 0.456;
    double sin;
    double cos;

    amd_sincos(input, &sin, &cos);

    printf("Input: %f\tOutput: sin: %f\tcos: %f\n", input, sin, cos);
    printf("----------\n");
}

/**********************************************
 *     Vector Variants
 * *******************************************/
void sincos_vector_single_precision_4()
{
    #if 0
    printf("Using Vector single precision - 4 floats (vrs4) sincos()\n");
    __m128 input;
    __m128 sin;
    __m128 cos;

    float input_array[4] = {3.65, -0.89, 0.0, -8.34};
    float sin_array[4];
    float cos_array[4];

    input = _mm_loadu_ps(input_array);

    amd_vrs4_sincosf(input, &sin, &cos);

    _mm_storeu_ps(sin_array, sin);
    _mm_storeu_ps(cos_array, cos);

    printf("Input: {%f, %f, %f, %f}, Output: sin: {%f, %f, %f, %f}, cos: {%f, %f, %f, %f}\n",
            input_array[0], input_array[1], input_array[2], input_array[3],
            sin_array[0], sin_array[1], sin_array[2], sin_array[3],
            cos_array[0], cos_array[1], cos_array[2], cos_array[3]);
    printf("----------\n");
    #endif
}

void sincos_vector_single_precision_8()
{
    #if 0
    printf("Using Vector single precision - 8 floats (vrs8) sincos()\n");
    __m256 input;
    __m256 sin;
    __m256 cos;

    float input_array[8] = {1.2, 0.0, -2.3, 3.4, 5.6, -7.8, 8.9, -1.0};
    float sin_array[8];
    float cos_array[8];

    input = _mm256_loadu_ps(input_array);

    amd_vrs8_sincosf(input, &sin, &cos);

    _mm256_storeu_ps(sin_array, sin);
    _mm256_storeu_ps(cos_array, cos);

    printf("Input: {%f, %f, %f, %f, %f, %f, %f, %f}, Input2: {%f, %f, %f, %f, %f, %f, %f, %f}, Output: {%f, %f, %f, %f, %f, %f, %f, %f}\n",
            input_array[0], input_array[1], input_array[2], input_array[3],
            input_array[4], input_array[5], input_array[6], input_array[7],
            sin_array[0], sin_array[1], sin_array[2], sin_array[3],
            sin_array[4], sin_array[5], sin_array[6], sin_array[7],
            cos_array[0], cos_array[1], cos_array[2], cos_array[3],
            cos_array[4], cos_array[5], cos_array[6], cos_array[7]);
    printf("----------\n");
    #endif
}

void sincos_vector_double_precision_2()
{
    #if 0
    printf("Using Vector double precision - 2 doubles (vrd2) sincos()\n");
    __m128d input;
    __m128d sin;
    __m128d cos;

    double input_array[2] = {-0.00, -18.34};
    double sin_array[2];
    double cos_array[2];

    input = _mm_loadu_pd(input_array);

    amd_vrd2_sincos(input, &sin, &cos);

    _mm_storeu_pd(sin_array, sin);
    _mm_storeu_pd(cos_array, cos);

    printf("Input: {%f, %f}, Output: sin: {%f, %f}, cos: {%f, %f}\n",
            input_array[0], input_array[1],
            sin_array[0], sin_array[1],
            cos_array[0], cos_array[1]);
    printf("----------\n");
    #endif
}

void sincos_vector_double_precision_4()
{
    printf("Using Vector double precision - 4 doubles (vrd4) sincos()\n");
    __m256d input;
    __m256d sin;
    __m256d cos;

    double input_array[4] = {34.65, -7.89, 91.0, -198.34};
    double sin_array[4];
    double cos_array[4];

    input = _mm256_loadu_pd(input_array);

    amd_vrd4_sincos(input, &sin, &cos);

    _mm256_storeu_pd(sin_array, sin);
    _mm256_storeu_pd(cos_array, cos);

    printf("Input: {%f, %f, %f, %f}, Output: sin: {%f, %f, %f, %f}, cos: {%f, %f, %f, %f}\n",
            input_array[0], input_array[1], input_array[2], input_array[3],
            sin_array[0], sin_array[1], sin_array[2], sin_array[3],
            cos_array[0], cos_array[1], cos_array[2], cos_array[3]);
    printf("----------\n");
}

/**********************************************
 *     Vector Array Variants
 * *******************************************/
void sincos_single_precision_array()
{
    #if 0
    printf("Using Single Precision Vectory Array (vrsa) sincos()\n");
    int n=5;
    float input[10] = {1.0f, 3.0f, -5.0f, 0.0f, -9.0f, 11.0f, 13.0f, 15.0f, -17.0f, 19.0f};
    float sin[10];
    float cos[10];

    amd_vrsa_sincosf(n, input, sin, cos);

    printf("Input: {");
    for(int i=0; i<10; ++i)
    {
        printf("%f, ", input[i]);
    }
    printf("}, Ouput: sin: {");
    for(int i=0; i<10; ++i)
    {
        printf("%f, ", sin[i]);
    }
    printf("}, cos: {");
    for(int i=0; i<10; ++i)
    {
        printf("%f, ", cos[i]);
    }
    printf("}\n");
    printf("----------\n");
    #endif
}

void sincos_double_precision_array()
{
    #if 0
    printf("Using Double Precision Vectory Array (vrda) sincos()\n");
    int n=8;
    double input[10] = {-0.0, -0.0, 4.0, 2.0, 6.0, -12.0, 14.0, -1.0, 3.0, -5.0};
    double sin[10];
    double cos[10];

    amd_vrda_sincos(n, input, sin, cos);

    printf("Input: {");
    for(int i=0; i<10; ++i)
    {
        printf("%f, ", input[i]);
    }
    printf("}, Ouput: sin: {");
    for(int i=0; i<10; ++i)
    {
        printf("%f, ", sin[i]);
    }
    printf("}, cos: {");
    for(int i=0; i<10; ++i)
    {
        printf("%f, ", cos[i]);
    }
    printf("}\n");
    printf("----------\n");
    #endif
}

int use_sincos()
{
    printf("\n\n***** sincos() *****\n");
    sincos_single_precision();
    sincos_double_precision();
    sincos_vector_single_precision_4();
    sincos_vector_single_precision_8();
    sincos_vector_double_precision_2();
    sincos_vector_double_precision_4();
    sincos_single_precision_array();
    sincos_double_precision_array();
    return 0;
}
