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


#include "libm_dynamic_load.h"
#include "test_functions.h"

float GenerateRangeFloat(float min, float max) {
    float range = (max - min);
    float div = (float)RAND_MAX/(float)range;
    return min + ((float)rand() / div);
}

double GenerateRangeDouble(double min, double max) {
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

/* used for getting full file path */
char *concatenate(const char *a, const char *b, const char *c) {
    char* temp = NULL;
    temp = (char*)malloc(strlen(a) + strlen(b) + strlen(c) + 1);
    if (temp == NULL) {
        printf("Failed to allocate in concatenate() function");
        return NULL;
    }
    return strcat(strcat(strcpy(temp, a), b), c);
}

/* check error */
int CheckError() {
    #if defined(_WIN64) || defined(_WIN32)
        long int error = GetLastError();
        if (error != NULL) {
            printf("Error: %ld\n", error);
            return 1;
        }
    #else
        char* error = dlerror();
        if (error != NULL) {
            printf("Error: %s\n", error);
            return 1;
        }
    #endif

    return 0;
}

int test_func(void* handle, struct FuncData * data, const char * func_name) {
    /* by default these wil be null */
    funcf s1f = data->s1f;
    func s1d = data->s1d;
    funcf_2 s1f_2 = data->s1f_2;
    func_2 s1d_2 = data->s1d_2;

    /* complex */
    funcf_cmplx s1f_cmplx = data->s1f_cmplx;
    func_cmplx s1d_cmplx = data->s1d_cmplx;
    funcf_cmplx_2 s1f_cmplx_2 = data->s1f_cmplx_2;
    func_cmplx_2 s1d_cmplx_2 = data->s1d_cmplx_2;

    /* vector double */
    func_v2d v2d = data->v2d;
    func_v2d_2 v2d_2 = data->v2d_2;
    func_v4d v4d = data->v4d;
    func_v4d_2 v4d_2 = data->v4d_2;

    /* vector float */
    funcf_v4s v4s = data->v4s;
    funcf_v4s_2 v4s_2 = data->v4s_2;
    funcf_v8s v8s = data->v8s;
    funcf_v8s_2 v8s_2 = data->v8s_2;

    /* vector array */
    funcf_va vas = data->vas;
    funcf_va_2 vas_2 = data->vas_2;
    func_va vad = data->vad;
    func_va_2 vad_2 = data->vad_2;

    #if defined(__AVX512__)
    /* vector avx512 */
    funcf_v16s v16s = data->v16s;
    func_v8d v8d = data->v8d;
    func_v8d_2 v8d_2 = data->v8d_2;
    funcf_v16s_2 v16s_2 = data->v16s_2;
    #endif

    printf("Exercising %s routines\n", func_name);
    /*scalar*/
    test_s1f(s1f, func_name);
    test_s1d(s1d, func_name);
    test_s1f_2(s1f_2, func_name);
    test_s1d_2(s1d_2, func_name);

    /*cmplx*/
    test_s1f_cmplx(s1f_cmplx, func_name);
    test_s1d_cmplx(s1d_cmplx, func_name);
    test_s1f_cmplx_2(s1f_cmplx_2, func_name);
    test_s1d_cmplx_2(s1d_cmplx_2, func_name);

    test_v2d(v2d, func_name);
    test_v2d_2(v2d_2, func_name);
    test_v4s(v4s, func_name);
    test_v4s_2(v4s_2, func_name);
    test_v4d(v4d, func_name);
    test_v4d_2(v4d_2, func_name);
    test_v8s(v8s, func_name);
    test_v8s_2(v8s_2, func_name);

    test_vas(vas, func_name);
    test_vas_2(vas_2, func_name);
    test_vad(vad, func_name);
    test_vad_2(vad_2, func_name);

    #if defined(__AVX512__)
    test_v8d(v8d, func_name);
    test_v8d_2(v8d_2, func_name);
    test_v16s(v16s, func_name);
    test_v16s_2(v16s_2, func_name);
    #endif

    return 0;
}

