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

int test_sincos(void* handle) {
    /*scalar inputs*/
    float inputf = 3.14f;
    double input = 6.28;

    #if defined(_WIN64) || defined(_WIN32)
        funcf_sincos     s1f = (funcf_sincos)GetProcAddress(handle, "amd_sincosf");
        func_sincos      s1d = (func_sincos)GetProcAddress(handle, "amd_sincos");

        long int error = GetLastError();

        if (error != NULL) {
            printf("Error: %ld\n", error);
            return 1;
        }
    #else
        funcf_sincos     s1f = (funcf_sincos)dlsym(handle, "amd_sincosf");
        func_sincos      s1d = (func_sincos)dlsym(handle, "amd_sincos");

        char* error = dlerror();

        if (error != NULL) {
            printf("Error: %s\n", error);
            return 1;
        }
    #endif

    printf("Exercising sincos routines\n");
    /*scalar*/
    s1f(inputf, &inputf, &inputf);
    printf("sincosf(%f)\n", (double)inputf);
    s1d(input, &input, &input);
    printf("sincos(%lf)\n", input);

    return 0;
}
