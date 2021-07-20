#include "libm_dynamic_load.h"

int test_fabs(void* handle) {
    int ret = 0;
    struct FuncData data;
    const char* func_name = "fabs";
    data.s1f = (funcf)dlsym(handle, "amd_fabsf");
    data.s1d = (func)dlsym(handle, "amd_fabs");

    if (data.s1f == NULL || data.s1d == NULL) {
        ret = 1;
    }
    if (ret == 1) {
        printf ("Seg faulted in function %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
