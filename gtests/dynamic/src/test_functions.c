/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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
 

#include "test_functions.h"
#include <complex.h>
#include <libm/types.h>


/* scalar single precision */
int test_s1f(funcf s1f, const char* func_name) {
    float inputf=3.14f, outputf;
    if (s1f != NULL) {
        outputf = s1f(inputf);
        if (CheckError()) exit(1);
        printf("amd_%sf(%f) = %f\n", func_name, (double)inputf, (double)outputf);
    }
    return 0;
}

/* scalar double precision */
int test_s1d(func s1d, const char* func_name) {
    double input=3.14, output;
    if (s1d != NULL) {
        output = s1d(input);
        if (CheckError()) exit(1);
        printf("amd_%s(%lf) = %lf\n", func_name, input, output);
    }
    return 0;
}

/* scalar single precision with two inputs */
int test_s1f_2(funcf_2 s1f_2, const char* func_name) {
    float inputf=3.14f, outputf;
    if (s1f_2 != NULL) {
        outputf = s1f_2(inputf, inputf);
        if (CheckError()) exit(1);
        printf("amd_%sf(%f, %f) = %f\n", func_name, (double)inputf, (double)inputf, (double)outputf);
    }
    return 0;
}

/* scalar single precision with two inputs */
int test_s1d_2(func_2 s1d_2, const char* func_name) {
    double input = 3.14, output;
    if (s1d_2 != NULL) {
        output = s1d_2(input, input);
        if (CheckError()) exit(1);
        printf("amd_%s(%lf, %lf) = %lf\n", func_name, input, input, output);
    }
    return 0;
}

/* complex */
#if !defined(_WIN64) || !defined(_WIN32)
extern "C" float crealf(float _Complex);
extern "C" float cimagf(float _Complex);
extern "C" double creal(double _Complex);
extern "C" double cimag(double _Complex);
#endif

/* complex scalar */
int test_s1f_cmplx(funcf_cmplx s1f_cmplx, const char* func_name) {
	fc32_t inputf={3.14f}, outputf;
    if (s1f_cmplx != NULL) {
        outputf = s1f_cmplx(inputf);
        if (CheckError()) exit(1);
        printf("amd_%s(%f + i%f) %f + i%f\n",
            func_name, (double)crealf(inputf), (double)cimagf(inputf),
            (double)crealf(outputf), (double)cimagf(outputf));
    }
    return 0;
}

int test_s1d_cmplx(func_cmplx s1d_cmplx, const char* func_name) {
    fc64_t input={3.14}, output;
    if (s1d_cmplx != NULL) {
        output = s1d_cmplx(input);
        if (CheckError()) exit(1);
        printf("amd_%s(%lf + i%lf) %lf + i%lf\n",
            func_name, creal(input), cimag(input),
            creal(output), cimag(output));
    }
    return 0;
}

/* complex with two inputs */
int test_s1f_cmplx_2(funcf_cmplx_2 s1f_cmplx_2, const char* func_name) {
    fc32_t inputf={3.14f}, outputf;
    if (s1f_cmplx_2 != NULL) {
        outputf = s1f_cmplx_2(inputf, inputf);
        if (CheckError()) exit(1);
        printf("amd_%s(%f + i%f) %f + i%f\n",
            func_name, (double)crealf(inputf), (double)cimagf(inputf),
            (double)crealf(outputf), (double)cimagf(outputf));
    }
    return 0;
}

int test_s1d_cmplx_2(func_cmplx_2 s1d_cmplx_2, const char* func_name) {
    fc64_t input={3.14}, output;
    if (s1d_cmplx_2 != NULL) {
        output = s1d_cmplx_2(input, input);
        if (CheckError()) exit(1);
        printf("amd_%s(%lf + i%lf) %lf + i%lf\n",
            func_name, creal(input), cimag(input),
            creal(output), cimag(output));
    }
    return 0;
}

/* vector functions */
int test_v2d(func_v2d v2d, const char* func_name) {
    __m128d ip_vrd2, op_vrd2;
    double input_array_vrd2[2] = {0.0, 0.0}, output_array_vrd2[2] = {0.0, 0.0};
    ip_vrd2 = _mm_loadu_pd(input_array_vrd2);
    if (CheckError()) exit(1);
    if (v2d != NULL) {
        op_vrd2 = v2d(ip_vrd2);
        if (CheckError()) exit(1);
        _mm_storeu_pd(output_array_vrd2, op_vrd2);
        if (CheckError()) exit(1);
        printf("amd_vrd2_%s([%lf, %lf] = [%lf, %lf])\n",
                func_name,
                input_array_vrd2[0], input_array_vrd2[1],
                output_array_vrd2[0], output_array_vrd2[1]);
    }
    return 0;
}

int test_v2d_2(func_v2d_2 v2d, const char* func_name) {
    __m128d ip_vrd2, op_vrd2;
    double input_array_vrd2[2] = {0.0, 0.0}, output_array_vrd2[2] = {0.0, 0.0};
    ip_vrd2 = _mm_loadu_pd(input_array_vrd2);
    if (CheckError()) exit(1);
    if (v2d != NULL) {
        op_vrd2 = v2d(ip_vrd2, ip_vrd2);
        if (CheckError()) exit(1);
        _mm_storeu_pd(output_array_vrd2, op_vrd2);
        if (CheckError()) exit(1);
        printf("amd_vrd2_%s([%lf, %lf] = [%lf, %lf])\n", func_name,
                input_array_vrd2[0], input_array_vrd2[1],
                output_array_vrd2[0], output_array_vrd2[1]);
    }
    return 0;
}

int test_v4s(funcf_v4s v4s, const char* func_name) {
    __m128 ip_vrs4, op_vrs4;
    float input_array_vrs4[4] = {0.0f, 0.0f}, output_array_vrs4[4] = {0.0f, 0.0f};
    ip_vrs4 = _mm_loadu_ps(input_array_vrs4);
    if (CheckError()) exit(1);
    if (v4s != NULL) {
        op_vrs4 = v4s(ip_vrs4);
        if (CheckError()) exit(1);
        _mm_storeu_ps(output_array_vrs4, op_vrs4);
        if (CheckError()) exit(1);
        printf("amd_vrs4_%sf([%f, %f] = [%f, %f])\n", func_name,
                (double)input_array_vrs4[0], (double)input_array_vrs4[1],
                (double)output_array_vrs4[0], (double)output_array_vrs4[1]);
    }
    return 0;
}

int test_v4s_2(funcf_v4s_2 v4s_2, const char* func_name) {
    __m128 ip_vrs4, op_vrs4;
    float input_array_vrs4[4] = {0.0f, 0.0f}, output_array_vrs4[4] = {0.0f, 0.0f};
    ip_vrs4 = _mm_loadu_ps(input_array_vrs4);
    if (CheckError()) exit(1);
    if (v4s_2 != NULL) {
        op_vrs4 = v4s_2(ip_vrs4, ip_vrs4);
        if (CheckError()) exit(1);
        _mm_storeu_ps(output_array_vrs4, op_vrs4);
        if (CheckError()) exit(1);
        printf("amd_vrs4_%sf([%f, %f] = [%f, %f])\n", func_name,
                (double)input_array_vrs4[0], (double)input_array_vrs4[1],
                (double)output_array_vrs4[0], (double)output_array_vrs4[1]);
    }
    return 0;
}

int test_v4d(func_v4d v4d, const char * func_name) {
    __m256d ip_vrd4, op_vrd4;
    int i;
    double input_array_vrd4[4]={0.0,0.0,0.0,0.0}, output_array_vrd4[4]={0.0,0.0,0.0,0.0};
    ip_vrd4 = _mm256_loadu_pd(input_array_vrd4);
    if (CheckError()) exit(1);
    if (v4d != NULL) {
        op_vrd4 = v4d(ip_vrd4);
        if (CheckError()) exit(1);
        _mm256_storeu_pd(output_array_vrd4, op_vrd4);
        if (CheckError()) exit(1);
        printf("amd_vrs8_%sf\ninput:\n", func_name);
        for(i=0; i<4; i++)
            printf("%lf\t", input_array_vrd4[i]);
        printf("\nOutput:\n");
        for(i=0; i<4; i++)
            printf("%lf\t", output_array_vrd4[i]);
    }
    return 0;
}

int test_v4d_2(func_v4d_2 v4d_2, const char * func_name) {
    __m256d ip_vrd4, op_vrd4;
    double input_array_vrd4[4]={0.0,0.0,0.0,0.0}, output_array_vrd4[4]={0.0,0.0,0.0,0.0};
    int i;
    ip_vrd4 = _mm256_loadu_pd(input_array_vrd4);
    if (CheckError()) exit(1);
    if (v4d_2 != NULL) {
        op_vrd4 = v4d_2(ip_vrd4, ip_vrd4);
        if (CheckError()) exit(1);
        _mm256_storeu_pd(output_array_vrd4, op_vrd4);
        if (CheckError()) exit(1);
        printf("amd_vrs8_%sf\ninput:\n", func_name);
        for(i=0; i<4; i++)
            printf("%lf\t", input_array_vrd4[i]);
        printf("\nOutput:\n");
        for(i=0; i<4; i++)
            printf("%lf\t", output_array_vrd4[i]);
    }
    return 0;
}

int test_v8s(funcf_v8s v8s, const char * func_name) {
    __m256 ip_vrs8, op_vrs8;
    float input_array_vrs8[8] = {0.0f, 0.0f}, output_array_vrs8[8] = {0.0f, 0.0f};
    int i;
    ip_vrs8 = _mm256_loadu_ps(input_array_vrs8);
    if (CheckError()) exit(1);
    if (v8s != NULL) {
        op_vrs8 = v8s(ip_vrs8);
        if (CheckError()) exit(1);
        _mm256_storeu_ps(output_array_vrs8, op_vrs8);
        if (CheckError()) exit(1);
        printf("amd_vrs8_%sf\ninput:\n", func_name);
        for(i=0; i<8; i++)
            printf("%f\t",(double)input_array_vrs8[i]);
        printf("\nOutput:\n");
        for(i=0; i<8; i++)
            printf("%f\t",(double)output_array_vrs8[i]);
    }
    return 0;
}

int test_v8s_2(funcf_v8s_2 v8s_2, const char * func_name) {
    __m256 ip_vrs8, op_vrs8;
    float input_array_vrs8[8] = {0.0f, 0.0f};
    float output_array_vrs8[8] = {0.0f, 0.0f};
    int i;
    ip_vrs8 = _mm256_loadu_ps(input_array_vrs8);
    if (CheckError()) exit(1);
    if (v8s_2 != NULL) {
        op_vrs8 = v8s_2(ip_vrs8, ip_vrs8);
        if (CheckError()) exit(1);
        _mm256_storeu_ps(output_array_vrs8, op_vrs8);
        if (CheckError()) exit(1);
        printf("amd_vrs8_%sf\ninput:\n", func_name);
        for(i=0; i<8; i++)
            printf("%f\t",(double)input_array_vrs8[i]);
        printf("\nOutput:\n");
        for(i=0; i<8; i++)
            printf("%f\t",(double)output_array_vrs8[i]);
    }
    return 0;
}

#if defined(__AVX512__)
int test_v8d(func_v8d v8d, const char * func_name) {
    __m512d ip_vrd8, op_vrd8;
    double input_array_vrd8[8] = {0.0};
    double output_array_vrd8[8] = {0.0};
    int i;
    ip_vrd8 = _mm512_loadu_pd(input_array_vrd8);
    if (CheckError()) exit(1);
    if (v8d != NULL) {
        op_vrd8 = v8d(ip_vrd8);
        if (CheckError()) exit(1);
        _mm512_storeu_pd(output_array_vrd8, op_vrd8);
        if (CheckError()) exit(1);
        printf("amd_vrd8_%s\ninput:\n", func_name);
        for(i=0; i<8; i++)
            printf("%lf\t",(double)input_array_vrd8[i]);
        printf("\nOutput:\n");
        for(i=0; i<8; i++)
            printf("%lf\t",(double)output_array_vrd8[i]);
    }
    return 0;
}

int test_v8d_2(func_v8d_2 v8d_2, const char * func_name) {
    __m512d ip_vrd8, op_vrd8;
    double input_array_vrd8[8] = {0.0,0.0};
    double output_array_vrd8[8] = {0.0, 0.0};
    int i;
    ip_vrd8 = _mm512_loadu_pd(input_array_vrd8);
    if (CheckError()) exit(1);
    if (v8d_2 != NULL) {
        op_vrd8 = v8d_2(ip_vrd8, ip_vrd8);
        if (CheckError()) exit(1);
        _mm512_storeu_pd(output_array_vrd8, op_vrd8);
        if (CheckError()) exit(1);
        printf("amd_vrd8_%s\ninput:\n", func_name);
        for(i=0; i<8; i++)
            printf("%lf\t",(double)input_array_vrd8[i]);
        printf("\nOutput:\n");
        for(i=0; i<8; i++)
            printf("%lf\t",(double)output_array_vrd8[i]);
    }
    return 0;
}

int test_v16s(funcf_v16s v16s, const char * func_name) {
    __m512 ip_vrs16, op_vrs16;
    double input_array_vrs16[16] = {0.0f}, output_array_vrs16[16] = {0.0f};
    int i;
    ip_vrs16 = _mm512_loadu_ps(input_array_vrs16);
    if (CheckError()) exit(1);
    if (v16s != NULL) {
        op_vrs16 = v16s(ip_vrs16);
        if (CheckError()) exit(1);
        _mm512_storeu_ps(output_array_vrs16, op_vrs16);
        if (CheckError()) exit(1);
        printf("amd_vrs16_%sf\nInput:\n", func_name);
        for(i=0; i<16; i++)
            printf("%f\n", (double)input_array_vrs16[i]);
        printf("\nOutput\n");
        for (i=0; i<16; i++)
            printf("%f\n", (double)output_array_vrs16[i]);
    }
    return 0;
}

int test_v16s_2(funcf_v16s_2 v16s, const char * func_name) {
    __m512 ip_vrs16, op_vrs16;
    double input_array_vrs16[16]={0.0f}, output_array_vrs16[16] = {0.0f};
    int i;
    ip_vrs16 = _mm512_loadu_ps(input_array_vrs16);
    if (CheckError()) exit(1);
    if (v16s != NULL) {
        op_vrs16 = v16s(ip_vrs16, ip_vrs16);
        if (CheckError()) exit(1);
        _mm512_storeu_ps(output_array_vrs16, op_vrs16);
        if (CheckError()) exit(1);
        printf("amd_vrs16_%sf\nInput:\n", func_name);
        for(i=0; i<16; i++)
            printf("%f\n", (double)input_array_vrs16[i]);
        printf("\nOutput\n");
        for (i=0; i<16; i++)
            printf("%f\n", (double)output_array_vrs16[i]);
    }
    return 0;
}
#endif


/* array vector functions */
int test_vas(funcf_va vas, const char * func_name) {
    long unsigned int i=0, dim = 5, loopCount = 10;
    long unsigned int array_size = dim * loopCount;
    if (vas != NULL) {
        float *input_arrayf = (float *)malloc(sizeof(float) * array_size);
        float *output_arrayf = (float *)malloc(sizeof(float) * array_size);
        printf("amd_vrsa_%sf\nInput:\n", func_name);
        for (i = 0; i < array_size; i++)
            printf("%f\t", (double)input_arrayf[i]);
        for (i = 0; i < loopCount; i++)
            vas((int)dim, input_arrayf + i*dim, output_arrayf + i*dim);
        if (CheckError()) exit(1);
        printf("\nOutput:\n");
        for (i = 0; i < array_size; i++)
            printf("%f\t", (double)output_arrayf[i]);

        free(input_arrayf);
        free(output_arrayf);
    }
    return 0;
}

int test_vas_2(funcf_va_2 vas, const char * func_name) {
    long unsigned int i=0, dim = 5, loopCount = 10;
    long unsigned int array_size = dim * loopCount;
    if (vas != NULL) {
        float *input_arrayf = (float *)malloc(sizeof(float) * array_size);
        float *output_arrayf = (float *)malloc(sizeof(float) * array_size);
        printf("amd_vrsa_%sf\nInput:\n", func_name);
        for (i = 0; i < array_size; i++)
            printf("%f\t", (double)input_arrayf[i]);
        for (i = 0; i < loopCount; i++)
            vas((int)dim, input_arrayf + i*dim,
                     input_arrayf + i*dim,
                     output_arrayf + i*dim);
        if (CheckError()) exit(1);
        printf("\nOutput:\n");
        for (i = 0; i < array_size; i++)
            printf("%f\t", (double)output_arrayf[i]);

        free(input_arrayf);
        free(output_arrayf);
    }
    return 0;
}

int test_vad(func_va vad, const char* func_name) {
    long unsigned int i=0, dim = 5, loopCount = 10;
    long unsigned int array_size = dim * loopCount;
    if (vad != NULL) {
        double *input_arrayd = (double *)malloc(sizeof(double) * array_size);
        double *output_arrayd = (double *)malloc(sizeof(double) * array_size);
        printf("amd_vrda_%s\nInput:\n", func_name);
        for (i = 0; i < array_size; i++)
            printf("%lf\t", input_arrayd[i]);
        for (i = 0; i < loopCount; i++)
            vad((int)dim, input_arrayd + i*dim, output_arrayd + i*dim);
        if (CheckError()) exit(1);
        printf("\nOutput:\n");
        for (i = 0; i < array_size; i++)
            printf("%lf\t", output_arrayd[i]);
        free(input_arrayd);
        free(output_arrayd);
    }
    return 0;
}

int test_vad_2(func_va_2 vad, const char* func_name) {
    long unsigned int i=0, dim = 5, loopCount = 10;
    long unsigned int array_size = dim * loopCount;
    if (vad != NULL) {
        double *input_arrayd = (double *)malloc(sizeof(double) * array_size);
        double *output_arrayd = (double *)malloc(sizeof(double) * array_size);
        printf("amd_vrda_%s\nInput:\n", func_name);
        for (i = 0; i < array_size; i++)
            printf("%lf\t", input_arrayd[i]);
        for (i = 0; i < loopCount; i++)
            vad((int)dim, input_arrayd + i*dim, input_arrayd + i*dim, output_arrayd + i*dim);
        if (CheckError()) exit(1);
        printf("\nOutput:\n");
        for (i = 0; i < array_size; i++)
            printf("%lf\t", output_arrayd[i]);
        free(input_arrayd);
        free(output_arrayd);
    }
    return 0;
}


