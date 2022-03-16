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

int test_sin(void* handle) {
    int ret = 0;
    struct FuncData data;
    const char* func_name = "sin";
    #if defined(_WIN64) || defined(_WIN32)
        data.s1f = (funcf)GetProcAddress(handle, "amd_sinf");
        data.s1d = (func)GetProcAddress(handle, "amd_sin");
        data.v2d = (func_v2d)GetProcAddress(handle, "amd_vrd2_sin");
        data.v4d = (func_v4d)GetProcAddress(handle, "amd_vrd4_sin");
        data.v4s = (funcf_v4s)GetProcAddress(handle, "amd_vrs4_sinf");
        data.v8s = (funcf_v8s)GetProcAddress(handle, "amd_vrs8_sinf");
        data.vas = (funcf_va)GetProcAddress(handle, "amd_vrsa_sinf");
        data.vad = (func_va)GetProcAddress(handle, "amd_vrda_sin");
    #else
        data.s1f = (funcf)dlsym(handle, "amd_sinf");
        data.s1d = (func)dlsym(handle, "amd_sin");
        data.v2d = (func_v2d)dlsym(handle, "amd_vrd2_sin");
        data.v4d = (func_v4d)dlsym(handle, "amd_vrd4_sin");
        data.v4s = (funcf_v4s)dlsym(handle, "amd_vrs4_sinf");
        data.v8s = (funcf_v8s)dlsym(handle, "amd_vrs8_sinf");
        data.vas = (funcf_va)dlsym(handle, "amd_vrsa_sinf");
        data.vad = (func_va)dlsym(handle, "amd_vrda_sin");
    #endif
    if (data.s1f == NULL || data.s1d == NULL || data.v2d == NULL ||
        data.v4d == NULL || data.v4s == NULL || data.v8s == NULL ||
        data.vas == NULL || data.vad == NULL) {
        ret = 1;
    }
    if (ret == 1) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
