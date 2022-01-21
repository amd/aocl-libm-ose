#include "libm_dynamic_load.h"

int test_atan(void* handle) {
    int ret = 0;
    struct FuncData data={0};
    const char* func_name = "atan";

    data.s1f = (funcf)FUNC_LOAD(handle, "amd_atanf");
    data.s1d = (func)FUNC_LOAD(handle, "amd_atan");
    data.v2d = (func_v2d)FUNC_LOAD(handle, "amd_vrd2_atan");
    data.v4d = (func_v4d)FUNC_LOAD(handle, "amd_vrd4_atan");
    data.v4s = (funcf_v4s)FUNC_LOAD(handle, "amd_vrs4_atanf");
    data.v8s = (funcf_v8s)FUNC_LOAD(handle, "amd_vrs8_atanf");

    #if defined(__AVX512__)
    data.v16s = (funcf_v16s)FUNC_LOAD(handle, "amd_vrs16_atanf");
    data.v8d = (func_v8d)FUNC_LOAD(handle, "amd_vrd8_atan");
    #endif

    if (data.s1f == NULL || data.s1d == NULL || data.v8s == NULL || data.v4s == NULL ||
        data.v2d == NULL || data.v4d == NULL
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
