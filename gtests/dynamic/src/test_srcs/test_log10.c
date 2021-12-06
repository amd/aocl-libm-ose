#include "libm_dynamic_load.h"

int test_log10(void* handle) {
    int ret = 0;
    struct FuncData data;
    const char* func_name = "log10";

    data.s1f = (funcf)FUNC_LOAD(handle, "amd_log10f");
    data.s1d = (func)FUNC_LOAD(handle, "amd_log10");
    data.v2d = (func_v2d)FUNC_LOAD(handle, "amd_vrd2_log10");
    data.v4s = (funcf_v4s)FUNC_LOAD(handle, "amd_vrs4_log10f");
    data.v8s = (funcf_v8s)FUNC_LOAD(handle, "amd_vrs8_log10f");

    #if defined(__AVX512__)
    data.v16s = (funcf_v16s)FUNC_LOAD(handle, "amd_vrs16_log10f");
    #endif

    if (data.s1f == NULL || data.s1d == NULL || data.v2d == NULL ||
        data.v8s == NULL || data.v4s == NULL
        #if defined(__AVX512__)
        || data.v16s == NULL
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
