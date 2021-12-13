#include "libm_dynamic_load.h"

int test_exp2(void* handle) {
    const char* func_name = "exp2";
    /* update all the existing variants here */
    struct FuncData data = {
        #if defined(_WIN64) || defined(_WIN32)
            .s1f = (funcf)GetProcAddress(handle, "amd_exp2f"),
            .s1d = (func)GetProcAddress(handle, "amd_exp2"),
            .v2d = (func_v2d)GetProcAddress(handle, "amd_vrd2_exp2"),
            .v4d = (func_v4d)GetProcAddress(handle, "amd_vrd4_exp2"),
            .v4s = (funcf_v4s)GetProcAddress(handle, "amd_vrs4_exp2f"),
            .v8s = (funcf_v8s)GetProcAddress(handle, "amd_vrs8_exp2f"),
            .vas = (funcf_va)GetProcAddress(handle, "amd_vrsa_exp2f"),
            .vad = (func_va)GetProcAddress(handle, "amd_vrda_exp2"),
        #else
            .s1f = (funcf)dlsym(handle, "amd_exp2f"),
            .s1d = (func)dlsym(handle, "amd_exp2"),
            .v2d = (func_v2d)dlsym(handle, "amd_vrd2_exp2"),
            .v4d = (func_v4d)dlsym(handle, "amd_vrd4_exp2"),
            .v4s = (funcf_v4s)dlsym(handle, "amd_vrs4_exp2f"),
            .v8s = (funcf_v8s)dlsym(handle, "amd_vrs8_exp2f"),
            .vas = (funcf_va)dlsym(handle, "amd_vrsa_exp2f"),
            .vad = (func_va)dlsym(handle, "amd_vrda_exp2"),
        #endif
    };
    #if defined(__AVX512__)
    data.v16s = (funcf_v16s)dlsym(handle, "amd_vrs16_exp2f");
    data.v8d = (func_v8d)dlsym(handle, "amd_vrd8_exp2");
    #endif

    if (data.s1f == NULL || data.s1d == NULL || data.v2d == NULL ||
        data.v4d == NULL || data.v8s == NULL || data.v4s == NULL ||
        data.vas == NULL || data.vad == NULL
        #if defined(__AVX512__)
        || data.v16s == NULL || data.v8d == NULL
        #endif
        ) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
