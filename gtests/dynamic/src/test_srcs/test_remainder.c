#include "libm_dynamic_load.h"

int test_remainder(void* handle) {
    const char* func_name = "remainder";
    /* update all the existing variants here */
    struct FuncData data = {
        #if defined(_WIN64) || defined(_WIN32)
            .s1f_2 = (funcf_2)GetProcAddress(handle, "amd_remainderf"),
            .s1d_2 = (func_2)GetProcAddress(handle, "amd_remainder"),
        #else
            .s1f_2 = (funcf_2)dlsym(handle, "amd_remainderf"),
            .s1d_2 = (func_2)dlsym(handle, "amd_remainder"),
        #endif
    };
    if (data.s1f_2 == NULL || data.s1d_2 == NULL) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
