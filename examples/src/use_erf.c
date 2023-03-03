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

int use_erf()
{
    printf ("Using Scalar single precision erf()\n");
    float ipf=0.5, opf;
    int i;
    opf = amd_erff (ipf);
    printf ("Input: %f\tOutput: %f\n", ipf, opf);

    printf ("Using Scalar double precision erf()\n");
    double ipd=0.5, opd;
    opd = amd_erf (ipd);
    printf ("Input: %f\tOutput: %f\n", ipd, opd);

    printf("Using vrs4 (Single precision 4-element vector variant) of AMD erff()\n");
    __m128 result_erf_vrs4;
    __m128 input_vrs4;
    float  input_array_vrs4[4] = {34.65, 67.89, 91.0, 198.34};
    float  output_array_vrs4[4];
    input_vrs4 = _mm_loadu_ps(input_array_vrs4);
    result_erf_vrs4 = amd_vrs4_erff(input_vrs4);
    _mm_storeu_ps(output_array_vrs4, result_erf_vrs4 );
    printf("Input: {%f, %f, %f, %f}, Output = {%f, %f, %f, %f}\n",
        input_array_vrs4[0], input_array_vrs4[1], input_array_vrs4[2], input_array_vrs4[3],
        output_array_vrs4[0], output_array_vrs4[1], output_array_vrs4[2], output_array_vrs4[3]);

    printf ("Using vrs8 (Single precision 8-element vector variant) of AMD erff()\n");
    __m256 input_vrs8, result_erff_vrs8;
    float input_array_vrs8[8] = {1.2, 0.0, 2.3, 3.4, 5.6, 7.8, 8.9, 1.0};
    float output_array_vrs8[8];
    input_vrs8 = _mm256_loadu_ps(input_array_vrs8);
    result_erff_vrs8 = amd_vrs8_erff(input_vrs8);
    _mm256_storeu_ps(output_array_vrs8, result_erff_vrs8);
    printf("Input: {");
    for (i=0; i<8; i++) {
        printf("%f,",input_array_vrs8[i]);
    }
    printf("}, Output: {");
    for (i=0; i<8; i++) {
        printf("%f,", output_array_vrs8[i]);
    }
    printf("}\n");

    printf ("Using vrd2 (Double precision 2-element vector variant) of AMD erf()\n");
    __m128d input_vrd2, result_erf_vrd2;
    double input_array_vrd2[2] = {34.65, 67.89};
    double output_array_vrd2[2];
    input_vrd2 = _mm_loadu_pd(input_array_vrd2);
    result_erf_vrd2 = amd_vrd2_erf(input_vrd2);
    _mm_storeu_pd(output_array_vrd2, result_erf_vrd2);
    printf("Input: {%f, %f}, Output = {%f, %f}\n",
	   input_array_vrd2[0], input_array_vrd2[1], output_array_vrd2[0], output_array_vrd2[1]);

    printf ("Using vrd4 (Double precision 4-element vector variant) of AMD erf()\n");
    __m256d input_vrd4, result_erf_vrd4;
    double input_array_vrd4[4] = {34.65, 67.89, 91.0, 198.34};
    double output_array_vrd4[4];
    input_vrd4 = _mm256_loadu_pd(input_array_vrd4);
    result_erf_vrd4 = amd_vrd4_erf(input_vrd4);
    _mm256_storeu_pd(output_array_vrd4, result_erf_vrd4);
    printf("Input: {%f, %f, %f, %f}, Output = {%f, %f, %f, %f}\n",
        input_array_vrd4[0], input_array_vrd4[1], input_array_vrd4[2], input_array_vrd4[3],
        output_array_vrd4[0], output_array_vrd4[1], output_array_vrd4[2], output_array_vrd4[3]);

    return 0;
}
