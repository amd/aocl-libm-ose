/*
 * Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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

int use_asin()
{
    printf ("Uasing Scalar asingle precision asinf()\n");
    float ipf = 0.5, opf;
    int i;
    opf = amd_asinf (ipf);
    printf("Input: %f\tOutput: %f\n", ipf, opf);
    printf ("Uasing Scalar double precision asin()\n");
    double ipd = 0.45, opd;
    opd = amd_asin(ipd);
    printf("Input: %f\tOutput: %f\n", ipd, opd);

    printf ("Uasing vrd2(Double precision vector) variant of AMD asin()\n");
    __m128d result_asin;
    __m128d input;
    double  input_array[2] = {34.65, 67.89};
    double  output_array[2];
    input = _mm_loadu_pd(input_array);

    printf("Uasing vrs4 (Single precision vector variant) of AMD asin()\n");
    __m128 result_asin_vrs4;
    __m128 input_vrs4;
    float  input_array_vrs4[4] = {34.65, 67.89, 91.0, 198.34};
    float  output_array_vrs4[4];
    input_vrs4 = _mm_loadu_ps(input_array_vrs4);
    result_asin_vrs4 = amd_vrs4_asinf(input_vrs4);
    _mm_storeu_ps(output_array_vrs4, result_asin_vrs4 );
    printf("Input: {%f, %f, %f, %f}, Output = {%f, %f, %f, %f}\n",
        input_array_vrs4[0], input_array_vrs4[1], input_array_vrs4[2], input_array_vrs4[3],
        output_array_vrs4[0], output_array_vrs4[1], output_array_vrs4[2], output_array_vrs4[3]);
    return 0;
}
