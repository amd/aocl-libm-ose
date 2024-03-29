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

#include "libm_glibc_compat.h"

int test_cos(void* handle) {
    __m128d ip_vrd2, op_vrd2;
    __m128  ip_vrs4, op_vrs4;
    __m256d ip_vrd4, op_vrd4;
    __m256  ip_vrs8, op_vrs8;
    int i;

    double input_array_vrd2[2] = {1.2, 3.5};
    double output_array_vrd2[2];

    double input_array_vrd4[4] = {0.0, 1.1, 3.6, 2.8};
    double output_array_vrd4[4];

    float input_array_vrs4[4] = {3.5f, 1.2f, 3.4f, 0.5f};
    float output_array_vrs4[4];

    float input_array_vrs8[8] = {1.2f, 2.3f, 5.6f, 50.3f,
                                -50.45f, 45.3f, 23.4f, 4.5f};
    float output_array_vrs8[8];

    /*packed inputs*/
    ip_vrd2 = _mm_loadu_pd(input_array_vrd2);
    ip_vrs4 = _mm_loadu_ps(input_array_vrs4);
    ip_vrd4 = _mm256_loadu_pd(input_array_vrd4);
    ip_vrs8 = _mm256_loadu_ps(input_array_vrs8);

    func_v2d cos_v2d = (func_v2d)dlsym(handle, "_ZGVbN2v_cos");
    funcf_v4s cosf_v4s = (funcf_v4s)dlsym(handle, "_ZGVbN4v_cosf");
    func_v4d cos_v4d  = (func_v4d)dlsym(handle, "_ZGVdN4v_cos");
    funcf_v8s cosf_v8s = (funcf_v8s)dlsym(handle, "_ZGVdN8v_cosf");

    if (cos_v2d == NULL || cosf_v4s == NULL ||
        cos_v4d == NULL || cosf_v8s == NULL) {
        printf("Unable to find cos symbols\n");
        return 1;
    }

    /*vrd2*/
    op_vrd2 = (*cos_v2d)(ip_vrd2);
    _mm_storeu_pd(output_array_vrd2, op_vrd2);
    printf("cos_v2d([%lf, %lf] = [%lf, %lf])\n",
            input_array_vrd2[0], input_array_vrd2[1],
            output_array_vrd2[0], output_array_vrd2[1]);
    /*vrs4*/
    op_vrs4 = (*cosf_v4s)(ip_vrs4);
    _mm_storeu_ps(output_array_vrs4, op_vrs4);
    printf("cosf_v4s([%f, %f] = [%f, %f])\n",
            (double)input_array_vrs4[0], (double)input_array_vrs4[1],
            (double)output_array_vrs4[0], (double)output_array_vrs4[1]);
    /*vrd4*/
    op_vrd4 = (*cos_v4d)(ip_vrd4);
    _mm256_storeu_pd(output_array_vrd4, op_vrd4);
    printf("cos_v4d\nInput:\n");
    for(i=0;i<4;i++)
        printf("%lf\t", input_array_vrd4[i]);
    printf("\nOutput:\n");
    for(i=0;i<4;i++)
         printf("%lf\t", output_array_vrd4[i]);
    /*vrs8*/
    op_vrs8 = (*cosf_v8s)(ip_vrs8);
    _mm256_storeu_ps(output_array_vrs8, op_vrs8);
    printf("\ncosf_v8s\ninput:\n");
    for(i=0; i<8; i++)
        printf("%f\t", (double)input_array_vrs8[i]);
    printf("\nOutput:\n");
    for(i=0; i<8; i++)
        printf("%f\t", (double)output_array_vrs8[i]);

    return 0;
}

