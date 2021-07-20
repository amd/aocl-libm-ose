#include "libm_dynamic_load.h"

int test_log10(void* handle) {
    const char* func_name = "log10";
    /* update all the existing variants here */
    struct FuncData data {
        .s1f = (funcf)dlsym(handle, "amd_log10f"),
        .s1d = (func)dlsym(handle, "amd_log10"),
        .v2d = (func_v2d)dlsym(handle, "amd_vrd2_log10"),
        .v4s = (funcf_v4s)dlsym(handle, "amd_vrs4_log10f"),
        .v8s = (funcf_v8s)dlsym(handle, "amd_vrs8_log10f"),
        .vas = (funcf_va)dlsym(handle, "amd_vrsa_log10f"),
        .vad = (func_va)dlsym(handle, "amd_vrda_log10"),
    };
    #if defined(__AVX512__)
    data.v16s = (funcf_v16s)dlsym(handle, "amd_vrs16_log10f");
    #endif

    if (data.s1f == NULL || data.s1d == NULL || data.v2d == NULL ||
        data.v8s == NULL || data.v4s == NULL ||
        data.vas == NULL || data.vad == NULL
        #if defined(__AVX512__)
        || data.v16s == NULL
        #endif
        ) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
