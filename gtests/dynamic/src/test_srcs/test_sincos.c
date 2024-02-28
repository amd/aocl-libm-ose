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

#include "libm_dynamic_load.h"

int test_sincos(void* handle) {
    /*scalar inputs*/
    float inputf = 3.14f;
    double input = 6.28;
    v_f64x4_t vrd4_input = {6.28, 6.28, 6.28, 6.28};
    #if defined(__AVX512__)
        v_f64x8_t vrd8_input = {6.28, 6.28, 6.28, 6.28, 6.28, 6.28, 6.28, 6.28};
    #endif
 
    #if defined(_WIN64) || defined(_WIN32)
        funcf_sincos     s1f = (funcf_sincos)GetProcAddress(handle, "amd_sincosf");
        func_sincos      s1d = (func_sincos)GetProcAddress(handle, "amd_sincos");
        func_vrd4_sincos v4d = (func_vrd4_sincos)GetProcAddress(handle, "amd_vrd4_sincos");
        #if defined(__AVX512__)
   	    func_vrd8_sincos v8d = (func_vrd8_sincos)GetProcAddress(handle, "amd_vrd8_sincos");
        #endif
        long int error = GetLastError();

        if (error != NULL) {
            printf("Error: %ld\n", error);
            return 1;
        }
    #else
        funcf_sincos     s1f = (funcf_sincos)dlsym(handle, "amd_sincosf");
        func_sincos      s1d = (func_sincos)dlsym(handle, "amd_sincos");
        func_vrd4_sincos v4d = (func_vrd4_sincos)dlsym(handle, "amd_vrd4_sincos");
        #if defined(__AVX512__)
   	    func_vrd8_sincos v8d = (func_vrd8_sincos)dlsym(handle, "amd_vrd8_sincos");
        #endif

        char* error = dlerror();

        if (error != NULL) {
            printf("Error: %s\n", error);
            return 1;
        }
    #endif

    printf("Exercising sincos routines\n");
    /* scalar */
    s1f(inputf, &inputf, &inputf);
    printf("sincosf(%f)\n", (double)inputf);
    s1d(input, &input, &input);
    printf("sincos(%lf)\n", input);
    /* vector */
    v4d(vrd4_input, &vrd4_input, &vrd4_input);
    printf("vrd4_sincos(%lf,%lf,%lf,%lf)\n", vrd4_input[0], vrd4_input[1], vrd4_input[2], vrd4_input[3]);
    #if defined(__AVX512__)
        v8d(vrd8_input, &vrd8_input, &vrd8_input);
        printf("vrd8_sincos(%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf)\n", vrd8_input[0], vrd8_input[1], vrd8_input[2], vrd8_input[3], vrd8_input[4], vrd8_input[5], vrd8_input[6], vrd8_input[7]);
    #endif

    return 0;
}
