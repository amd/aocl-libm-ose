/*
* Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
*
*/

#define AMD_LIBM_VEC_EXTERNAL_H
#define AMD_LIBM_VEC_EXPERIMENTAL
#include <stdio.h>
#include "amdlibm.h"
#include "amdlibm_vec.h"
#include <immintrin.h>

void use_exp()
{
    printf ("Using AMD expf()\n");
    float ipf = 0.5, opf;
    opf = amd_expf (ipf);
    printf("Input: %f\tOutput: %f\n", ipf, opf);
    printf ("Using AMD exp()\n");
    double ipd = 0.45, opd;
    opd = amd_exp(ipd);
    printf("Input: %f\tOutput: %f\n", ipd, opd);

    printf ("Using vrd2(Double precision vector) variant of AMD exp()\n");
    __m128d result_exp;
    __m128d input;
    double  input_array[2] = {34.65, 67.89};
    double  output_array[2];
    input = _mm_loadu_pd(input_array);

    printf("\nUsing amd_vrd2_exp ... \n");
    result_exp = amd_vrd2_exp(input);
    _mm_storeu_pd(output_array, result_exp);
    printf("Input: {%lf, %lf}, Output = {%lf, %lf}\n",
    input_array[0], input_array[1],
    output_array[0], output_array[1]);

    printf("Using vrs4 (Single precision vector variant) of AMD exp()\n");
    __m128 result_exp_vrs4;
    __m128 input_vrs4;
    float  input_array_vrs4[4] = {34.65, 67.89, 91.0, 198.34};
    float  output_array_vrs4[4];
    input_vrs4 = _mm_loadu_ps(input_array_vrs4);

    printf("\nUsing amd_vrs4_expf ... \n");
    result_exp_vrs4 = amd_vrs4_expf(input_vrs4);
    _mm_storeu_ps(output_array_vrs4, result_exp_vrs4 );
    printf("Input: {%f, %f, %f, %f}, Output = {%f, %f, %f, %f}\n",
        input_array_vrs4[0], input_array_vrs4[1], input_array_vrs4[2], input_array_vrs4[3],
        output_array_vrs4[0], output_array_vrs4[1], output_array_vrs4[2], output_array_vrs4[3]);
}
