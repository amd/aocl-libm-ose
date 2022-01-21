#include "libm_dynamic_load.h"

int test_ceil(void* handle) {
    const char* func_name = "ceil";
    /* update all the existing variants here */
    struct FuncData data = {
        #if defined(_WIN64) || defined(_WIN32)
            .s1f = (funcf)GetProcAddress(handle, "amd_ceilf"),
            .s1d = (func)GetProcAddress(handle, "amd_ceil"),
        #else
            .s1f = (funcf)dlsym(handle, "amd_ceilf"),
            .s1d = (func)dlsym(handle, "amd_ceil"),
        #endif
    };
    if (data.s1f == NULL || data.s1d == NULL) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
