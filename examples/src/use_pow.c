/*
* Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
*
*/

#define AMD_LIBM_VEC_EXPERIMENTAL
#include <stdio.h>
#include "amdlibm.h"
#include "amdlibm_vec.h"
#include <immintrin.h>

int use_pow()
{
    printf ("Using Scalar single precision powf()\n");
    float ipf = 0.5, opf;
    int i;
    opf = amd_powf (ipf, ipf);
    printf("Input: %f,%f\tOutput: %f\n", ipf,ipf, opf);
    printf ("Using Scalar double precision pow()\n");
    double ipd = 0.45, opd;
    opd = amd_pow(ipd, ipd);
    printf("Input: %lf, %lf\tOutput: %f\n", ipd, ipd, opd);

    printf ("Using vrd2(Double precision vector) variant of AMD pow()\n");
    __m128d result_pow;
    __m128d input;
    double  input_array[2] = {34.65, 67.89};
    double  output_array[2];
    input = _mm_loadu_pd(input_array);

    result_pow = amd_vrd2_pow(input, input);
    _mm_storeu_pd(output_array, result_pow);
    printf("Input: {%lf, %lf}, Output = {%lf, %lf}\n",
    input_array[0], input_array[1],
    output_array[0], output_array[1]);

    printf("Using vrs4 (Single precision vector variant) of AMD pow()\n");
    __m128 result_pow_vrs4;
    __m128 input_vrs4;
    float  input_array_vrs4[4] = {34.65, 67.89, 91.0, 198.34};
    float  output_array_vrs4[4];
    input_vrs4 = _mm_loadu_ps(input_array_vrs4);
    result_pow_vrs4 = amd_vrs4_powf(input_vrs4, input_vrs4);
    _mm_storeu_ps(output_array_vrs4, result_pow_vrs4 );
    printf("Input: {%f, %f, %f, %f}, Output = {%f, %f, %f, %f}\n",
        input_array_vrs4[0], input_array_vrs4[1], input_array_vrs4[2], input_array_vrs4[3],
        output_array_vrs4[0], output_array_vrs4[1], output_array_vrs4[2], output_array_vrs4[3]);

    printf("\nUsing vrd4 (Double Precision vector 4 variant) of AMD pow()\n");
    __m256d input_vrd4, result_pow_vrd4;
    double input_array_vrd4[4] = {2.3, 4.5, 56.5, 43.4};
    double output_array_vrd4[4];
    input_vrd4 = _mm256_loadu_pd(input_array_vrd4);
    result_pow_vrd4 = amd_vrd4_pow(input_vrd4, input_vrd4);
    _mm256_storeu_pd(output_array_vrd4, result_pow_vrd4);
    printf("Input: {%lf, %lf, %lf, %lf}, Output = {%lf, %lf, %lf, %lf}\n",
            input_array_vrd4[0], input_array_vrd4[1], input_array_vrd4[2], input_array_vrd4[3],
            output_array_vrd4[0], output_array_vrd4[1], output_array_vrd4[2],output_array_vrd4[3]);

    printf ("\nUsing vrs8 (Single precision vector 8 element variant of AMD pow()\n");
    __m256 input_vrs8, result_pow_vrs8;
    float input_array_vrs8[8] = {1.2, 0.0, 2.3, 3.4, 5.6, 7.8, 8.9, 1.0};
    float output_array_vrs8[8];
    input_vrs8 = _mm256_loadu_ps(input_array_vrs8);
    result_pow_vrs8 = amd_vrs8_powf(input_vrs8, input_vrs8);
    _mm256_storeu_ps(output_array_vrs8, result_pow_vrs8);
    printf("Input: {");
    for (i=0; i<8; i++) {
        printf("%f,",input_array_vrs8[i]);
    }
    printf("}, Output: {");
    for (i=0; i<8; i++) {
        printf("%f,", output_array_vrs8[i]);
    }

    return 0;
}
