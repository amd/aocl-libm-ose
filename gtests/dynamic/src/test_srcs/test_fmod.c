#include "libm_dynamic_load.h"

int test_fmod(void* handle) {
    const char* func_name = "fmod";
    /* update all the existing variants here */
    struct FuncData data {
        .s1f_2 = (funcf_2)dlsym(handle, "amd_fmodf"),
        .s1d_2 = (func_2)dlsym(handle, "amd_fmod"),
    };
    if (data.s1f_2 == NULL || data.s1d_2 == NULL) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
