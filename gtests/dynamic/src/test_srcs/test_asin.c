#include "libm_dynamic_load.h"

int test_asin(void* handle) {
    int ret = 0;
    struct FuncData data;
    const char* func_name = "asin";
    #if defined(_WIN64) || defined(_WIN32)
        data.s1f = (funcf)GetProcAddress(handle, "amd_asinf");
        data.s1d = (func)GetProcAddress(handle, "amd_asin");
        data.v4s = (funcf_v4s)GetProcAddress(handle, "amd_vrs4_asinf");
        data.v8s = (funcf_v8s)GetProcAddress(handle, "amd_vrs8_asinf");
    #else
        data.s1f = (funcf)dlsym(handle, "amd_asinf");
        data.s1d = (func)dlsym(handle, "amd_asin");
        data.v4s = (funcf_v4s)dlsym(handle, "amd_vrs4_asinf");
        data.v8s = (funcf_v8s)dlsym(handle, "amd_vrs8_asinf");
    #endif
    if (data.s1f == NULL || data.s1d == NULL || data.v4s == NULL) {
    if (data.s1f == NULL || data.s1d == NULL ||
        data.v8s == NULL || data.v4s == NULL) {
        ret = 1;
    }
    }
    if (ret == 1) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
