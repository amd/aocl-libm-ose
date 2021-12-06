#include "libm_dynamic_load.h"

int test_log(void* handle) {
    int ret = 0;
    struct FuncData data;
    const char* func_name = "log";

    data.s1f = (funcf)FUNC_LOAD(handle, "amd_logf");
    data.s1d = (func)FUNC_LOAD(handle, "amd_log");
    data.v2d = (func_v2d)FUNC_LOAD(handle, "amd_vrd2_log");
    data.v4d = (func_v4d)FUNC_LOAD(handle, "amd_vrd4_log");
    data.v4s = (funcf_v4s)FUNC_LOAD(handle, "amd_vrs4_logf");
    data.v8s = (funcf_v8s)FUNC_LOAD(handle, "amd_vrs8_logf");

    #if defined(__AVX512__)
    data.v16s = (funcf_v16s)FUNC_LOAD(handle, "amd_vrs16_logf");
    data.v8d = (func_v8d)FUNC_LOAD(handle, "amd_vrd8_log");
    #endif

    if (data.s1f == NULL || data.s1d == NULL || data.v2d == NULL ||
        data.v4d == NULL || data.v8s == NULL || data.v4s == NULL
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
