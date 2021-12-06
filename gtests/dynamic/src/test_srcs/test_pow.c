#include "libm_dynamic_load.h"

int test_pow(void* handle) {
    int ret = 0;
    struct FuncData data;
    const char* func_name = "pow";

    data.s1f_2 = (funcf_2)FUNC_LOAD(handle, "amd_powf");
    data.s1d_2 = (func_2)FUNC_LOAD(handle, "amd_pow");
    data.v2d_2 = (func_v2d_2)FUNC_LOAD(handle, "amd_vrd2_pow");
    data.v4d_2 = (func_v4d_2)FUNC_LOAD(handle, "amd_vrd4_pow");
    data.v4s_2 = (funcf_v4s_2)FUNC_LOAD(handle, "amd_vrs4_powf");
    data.v8s_2 = (funcf_v8s_2)FUNC_LOAD(handle, "amd_vrs8_powf");
    data.vas_2 = (funcf_va_2)FUNC_LOAD(handle, "amd_vrsa_powf");

    #if defined(__AVX512__)
    data.v16s_2 = (funcf_v16s_2)FUNC_LOAD(handle, "amd_vrs16_powf");
    data.v8d_2 = (func_v8d_2)FUNC_LOAD(handle, "amd_vrd8_pow");
    #endif

    if (data.s1f_2 == NULL || data.s1d_2 == NULL || data.v2d_2 == NULL ||
        data.v4d_2 == NULL || data.v8s_2 == NULL || data.v4s_2 == NULL ||
        data.vas_2 == NULL
        #if defined(__AVX512__)
        || data.v16s_2 == NULL || data.v8d_2 == NULL
        #endif
        ) {
        ret = 1;
    }
    if (ret == 1) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }

    test_func(handle, &data, func_name);
    return 0;
}

