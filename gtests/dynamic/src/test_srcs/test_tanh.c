#include "libm_dynamic_load.h"

int test_tanh(void* handle) {
    const char* func_name = "tanh";
    /* update all the existing variants here */
    struct FuncData data = {
        #if defined(_WIN64) || defined(_WIN32)
            .s1f = (funcf)GetProcAddress(handle, "amd_tanhf"),
            .s1d = (func)GetProcAddress(handle, "amd_tanh"),
            .v4s = (funcf_v4s)GetProcAddress(handle, "amd_vrs4_tanhf"),
            .v8s = (funcf_v8s)GetProcAddress(handle, "amd_vrs8_tanhf"),
        #else
            .s1f = (funcf)dlsym(handle, "amd_tanhf"),
            .s1d = (func)dlsym(handle, "amd_tanh"),
            .v4s = (funcf_v4s)dlsym(handle, "amd_vrs4_tanhf"),
            .v8s = (funcf_v8s)dlsym(handle, "amd_vrs8_tanhf"),
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
