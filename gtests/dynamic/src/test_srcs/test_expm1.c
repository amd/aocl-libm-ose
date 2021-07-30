#include "libm_dynamic_load.h"

int test_expm1(void* handle) {
    const char* func_name = "expm1";
    /* update all the existing variants here */
    struct FuncData data {
        .s1f = (funcf)dlsym(handle, "amd_expm1f"),
        .s1d = (func)dlsym(handle, "amd_expm1"),
        .v4s = (funcf_v4s)dlsym(handle, "amd_vrs4_expm1f"),
        .vas = (funcf_va)dlsym(handle, "amd_vrsa_expm1f"),
        .vad = (func_va)dlsym(handle, "amd_vrda_expm1"),
    };

    if (data.s1f == NULL || data.s1d == NULL || data.v4s == NULL ||
        data.vas == NULL || data.vad == NULL ) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
