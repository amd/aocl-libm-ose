#include "libm_dynamic_load.h"

int test_cosh(void* handle) {
    int ret = 0;
    struct FuncData data={0};
    const char* func_name = "cosh";

    data.s1f = (funcf)FUNC_LOAD(handle, "amd_coshf");
    data.s1d = (func)FUNC_LOAD(handle, "amd_cosh");
    data.v4s = (funcf_v4s)FUNC_LOAD(handle, "amd_vrs4_coshf");
    data.v8s = (funcf_v8s)FUNC_LOAD(handle, "amd_vrs8_coshf");

    if (data.s1f == NULL || data.s1d == NULL || data.v8s == NULL || data.v4s == NULL) {
        ret = 1;
    }
    if (ret == 1) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }

    test_func(handle, &data, func_name);
    return 0;
}
