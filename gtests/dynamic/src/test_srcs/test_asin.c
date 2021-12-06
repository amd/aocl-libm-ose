#include "libm_dynamic_load.h"

int test_asin(void* handle) {
    int ret = 0;
    struct FuncData data;
    const char* func_name = "asin";

    data.s1f = (funcf)FUNC_LOAD(handle, "amd_asinf");
    data.s1d = (func)FUNC_LOAD(handle, "amd_asin");
    data.v4s = (funcf_v4s)FUNC_LOAD(handle, "amd_vrs4_asinf");
    data.v8s = (funcf_v8s)FUNC_LOAD(handle, "amd_vrs8_asinf");

    #if defined(__AVX512__)
    data.v16s = (funcf_v16s)FUNC_LOAD(handle, "amd_vrs16_asinf");
    data.v8d = (func_v8d)FUNC_LOAD(handle, "amd_vrd8_asin");
    #endif

    if (data.s1f == NULL || data.s1d == NULL || data.v8s == NULL || data.v4s == NULL
        #if defined(__AVX512__)
        || data.v16s == NULL || data.v8d == NULL
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
