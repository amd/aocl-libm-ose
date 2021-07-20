#include "libm_dynamic_load.h"

int test_acos(void* handle) {
    int ret = 0;
    struct FuncData data;
    const char* func_name = "acos";
    data.s1f = (funcf)dlsym(handle, "amd_acosf");
    data.s1d = (func)dlsym(handle, "amd_acos");
    data.v4s = (funcf_v4s)dlsym(handle, "amd_vrs4_acosf");

    if (data.s1f == NULL || data.s1d == NULL || data.v4s == NULL) {
        ret = 1;
    }
    if (ret == 1) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
