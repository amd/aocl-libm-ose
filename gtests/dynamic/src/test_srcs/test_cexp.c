#include "libm_dynamic_load.h"

int test_cexp(void* handle) {
    struct FuncData data;
    const char* func_name = "cexp";

    data.s1f_cmplx = (funcf_cmplx)FUNC_LOAD(handle, "amd_cexpf");
    data.s1d_cmplx = (func_cmplx)FUNC_LOAD(handle, "amd_cexp");

    if (data.s1f_cmplx == NULL || data.s1d_cmplx == NULL) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }

    test_func(handle, &data, func_name);
    return 0;
}
