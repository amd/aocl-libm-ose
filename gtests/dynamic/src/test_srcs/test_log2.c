#include "libm_dynamic_load.h"

int test_log2(void* handle) {
    const char* func_name = "log2";
    /* update all the existing variants here */
    struct FuncData data {
        .s1f = (funcf)dlsym(handle, "amd_log2f"),
        .s1d = (func)dlsym(handle, "amd_log2"),
        .v2d = (func_v2d)dlsym(handle, "amd_vrd2_log2"),
        .v4d = (func_v4d)dlsym(handle, "amd_vrd4_log2"),
        .v4s = (funcf_v4s)dlsym(handle, "amd_vrs4_log2f"),
        .v8s = (funcf_v8s)dlsym(handle, "amd_vrs8_log2f"),
        .vas = (funcf_va)dlsym(handle, "amd_vrsa_log2f"),
        .vad = (func_va)dlsym(handle, "amd_vrda_log2"),
    };
    #if defined(__AVX512__)
    data.v16s = (funcf_v16s)dlsym(handle, "amd_vrs16_log2f");
    data.v8d = (func_v8d)dlsym(handle, "amd_vrd8_log2");
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
