#include "libm_dynamic_load.h"

int test_lrint(void* handle) {
    const char* func_name = "lrint";
    /* update all the existing variants here */
    struct FuncData data {
        .s1f = (funcf)dlsym(handle, "amd_lrintf"),
        .s1d = (func)dlsym(handle, "amd_lrint"),
    };
    if (data.s1f == NULL || data.s1d == NULL) {
        printf ("Uninitialized variant in %s\n", func_name);
        exit(1);
    }
    test_func(handle, &data, func_name);
    return 0;
}
