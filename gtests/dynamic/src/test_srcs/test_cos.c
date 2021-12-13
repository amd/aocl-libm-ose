#include "libm_dynamic_load.h"

int test_cos(void* handle) {
    int ret = 0;
    struct FuncData data;
    const char* func_name = "cos";
    #if defined(_WIN64) || defined(_WIN32)
        data.s1f = (funcf)GetProcAddress(handle, "amd_cosf");
        data.s1d = (func)GetProcAddress(handle, "amd_cos");
        data.v2d = (func_v2d)GetProcAddress(handle, "amd_vrd2_cos");
        data.v4d = (func_v4d)GetProcAddress(handle, "amd_vrd4_cos");
        data.v4s = (funcf_v4s)GetProcAddress(handle, "amd_vrs4_cosf");
        data.v8s = (funcf_v8s)GetProcAddress(handle, "amd_vrs8_cosf");
        data.vas = (funcf_va)GetProcAddress(handle, "amd_vrsa_cosf");
        data.vad = (func_va)GetProcAddress(handle, "amd_vrda_cos");
    #else
        data.s1f = (funcf)dlsym(handle, "amd_cosf");
        data.s1d = (func)dlsym(handle, "amd_cos");
        data.v2d = (func_v2d)dlsym(handle, "amd_vrd2_cos");
        data.v4d = (func_v4d)dlsym(handle, "amd_vrd4_cos");
        data.v4s = (funcf_v4s)dlsym(handle, "amd_vrs4_cosf");
        data.v8s = (funcf_v8s)dlsym(handle, "amd_vrs8_cosf");
        data.vas = (funcf_va)dlsym(handle, "amd_vrsa_cosf");
        data.vad = (func_va)dlsym(handle, "amd_vrda_cos");
    #endif
    if (data.s1f == NULL || data.s1d == NULL || data.v2d == NULL ||
        data.v4d == NULL || data.v4s == NULL || data.v8s == NULL ||
        data.vas == NULL || data.vad == NULL) {
        ret = 1;
    }
    if (ret == 1) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
