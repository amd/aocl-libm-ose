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

int test_erf(void* handle) {
    int ret = 0;
    struct FuncData data={0};
    const char* func_name = "erf";

    data.s1f = (funcf)FUNC_LOAD(handle, "amd_erff");
    data.s1d = (func)FUNC_LOAD(handle, "amd_erf");

    data.v2d = (func_v2d)FUNC_LOAD(handle, "amd_vrd2_erf");
    data.v4d = (func_v4d)FUNC_LOAD(handle, "amd_vrd4_erf");

    data.v4s = (funcf_v4s)FUNC_LOAD(handle, "amd_vrs4_erff");
    data.v8s = (funcf_v8s)FUNC_LOAD(handle, "amd_vrs8_erff");

    #if defined(__AVX512__)
    //data.v16s = (funcf_v16s)FUNC_LOAD(handle, "amd_vrs16_erff");
    //data.v8d = (func_v8d)FUNC_LOAD(handle, "amd_vrd8_erf");
    #endif

    if (data.s1f == NULL || data.v4s == NULL || data.v8s == NULL) {
        ret = 1;
    }
    if (ret == 1) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }

    test_func(handle, &data, func_name);
    return 0;
}
