#include "libm_dynamic_load.h"

int test_atan(void* handle) {
    int ret = 0;
    struct FuncData data;
    const char* func_name = "atan";
    data.s1f = (funcf)dlsym(handle, "amd_atanf");
    data.s1d = (func)dlsym(handle, "amd_atan");

    if (data.s1f == NULL || data.s1d == NULL) {
        ret = 1;
    }
    if (ret == 1) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
