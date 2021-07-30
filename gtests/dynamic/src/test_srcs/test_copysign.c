#include "libm_dynamic_load.h"

int test_copysign(void* handle) {
    const char* func_name = "copysign";
    /* update all the existing variants here */
    struct FuncData data {
        .s1f_2 = (funcf_2)dlsym(handle, "amd_copysignf"),
        .s1d_2 = (func_2)dlsym(handle, "amd_copysign"),
    };
    if (data.s1f_2 == NULL || data.s1d_2 == NULL) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
