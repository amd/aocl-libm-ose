#include "libm_dynamic_load.h"

int test_tanh(void* handle) {
    int ret = 0;
    struct FuncData data;
    const char* func_name = "tanh";

    data.s1f = (funcf)FUNC_LOAD(handle, "amd_tanhf");
    data.s1d = (func)FUNC_LOAD(handle, "amd_tanh");
    data.v4s = (funcf_v4s)FUNC_LOAD(handle, "amd_vrs4_tanhf");
    data.v8s = (funcf_v8s)FUNC_LOAD(handle, "amd_vrs8_tanhf");

    #if defined(__AVX512__)
    data.v16s = (funcf_v16s)FUNC_LOAD(handle, "amd_vrs16_tanhf");
    #endif

    if (data.s1f == NULL || data.s1d == NULL ||
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
