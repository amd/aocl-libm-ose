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

int test_modf(void* handle) {
    /*scalar inputs*/
    float inputf = 3.145f, outputf;
    double input = 6.287, output;

    #if defined(_WIN64) || defined(_WIN32)
        float (*func_f)(float, float*) = (float (*)(float, float*))GetProcAddress(handle, "amd_modff");
        double (*func_d)(double, double*) = (double (*)(double, double*))GetProcAddress(handle, "amd_modf");

        long int error = GetLastError();

        if (error != NULL) {
            printf("Error: %ld\n", error);
            return 1;
        }
    #else
        float (*func_f)(float, float*) = (float (*)(float, float*))dlsym(handle, "amd_modff");
        double (*func_d)(double, double*) = (double (*)(double, double*))dlsym(handle, "amd_modf");

        char* error = dlerror();

        if (error != NULL) {
            printf("Error: %s\n", error);
            return 1;
        }
    #endif

    printf("Exercising modf routines\n");
    outputf = func_f(inputf, &inputf);
    printf("amd_modff(%f) = %f\n", (double)inputf, (double)outputf);
    output = func_d(input, &input);
    printf("amd_modf(%lf) = %lf\n", input, output);

    return 0;
}
