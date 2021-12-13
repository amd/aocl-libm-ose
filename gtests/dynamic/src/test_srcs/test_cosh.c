#include "libm_dynamic_load.h"

int test_cosh(void* handle) {
    const char* func_name = "cosh";
    /* update all the existing variants here */
    struct FuncData data = {
        #if defined(_WIN64) || defined(_WIN32)
            .s1f = (funcf)GetProcAddress(handle, "amd_coshf"),
            .s1d = (func)GetProcAddress(handle, "amd_cosh"),
            .v4s = (funcf_v4s)GetProcAddress(handle, "amd_vrs4_coshf"),
            .v8s = (funcf_v8s)GetProcAddress(handle, "amd_vrs8_coshf"),
        #else
            .s1f = (funcf)dlsym(handle, "amd_coshf"),
            .s1d = (func)dlsym(handle, "amd_cosh"),
            .v4s = (funcf_v4s)dlsym(handle, "amd_vrs4_coshf"),
            .v8s = (funcf_v8s)dlsym(handle, "amd_vrs8_coshf"),
        #endif
    };
    if (data.s1f == NULL || data.s1d == NULL ||
        data.v8s == NULL || data.v4s == NULL) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
