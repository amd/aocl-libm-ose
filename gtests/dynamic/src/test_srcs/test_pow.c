#include "libm_dynamic_load.h"

int test_pow(void* handle) {
    const char* func_name = "pow";
    /* update all the existing variants here */
    struct FuncData data {
        .s1f_2 = (funcf_2)dlsym(handle, "amd_powf"),
        .s1d_2 = (func_2)dlsym(handle, "amd_pow"),
        .v2d_2 = (func_v2d_2)dlsym(handle, "amd_vrd2_pow"),
        .v4d_2 = (func_v4d_2)dlsym(handle, "amd_vrd4_pow"),
        .v4s_2 = (funcf_v4s_2)dlsym(handle, "amd_vrs4_powf"),
        .v8s_2 = (funcf_v8s_2)dlsym(handle, "amd_vrs8_powf"),
        .vas_2 = (funcf_va_2)dlsym(handle, "amd_vrsa_powf"),
    };
    #if defined(__AVX512__)
    data.v16s_2 = (funcf_v16s_2)dlsym(handle, "amd_vrs16_powf");
    data.v8d_2 = (func_v8d_2)dlsym(handle, "amd_vrd8_pow");
    #endif

    if (data.s1f_2 == NULL || data.s1d_2 == NULL || data.v2d_2 == NULL ||
        data.v4d_2 == NULL || data.v8s_2 == NULL || data.v4s_2 == NULL ||
        data.vas_2 == NULL
        #if defined(__AVX512__)
        || data.v16s_2 == NULL || data.v8d_2 == NULL
        #endif
        ) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
