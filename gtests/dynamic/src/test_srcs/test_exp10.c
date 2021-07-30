#include "libm_dynamic_load.h"

int test_exp10(void* handle) {
    const char* func_name = "exp10";
    /* update all the existing variants here */
    struct FuncData data {
        .s1f = (funcf)dlsym(handle, "amd_exp10f"),
        .s1d = (func)dlsym(handle, "amd_exp10"),
        .v2d = (func_v2d)dlsym(handle, "amd_vrd2_exp10"),
        .v4s = (funcf_v4s)dlsym(handle, "amd_vrs4_exp10f"),
        .vas = (funcf_va)dlsym(handle, "amd_vrsa_exp10f"),
        .vad = (func_va)dlsym(handle, "amd_vrda_exp10"),
    };

    if (data.s1f == NULL || data.s1d == NULL ||
        data.v2d == NULL || data.v4s == NULL ||
        data.vas == NULL || data.vad == NULL ) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
