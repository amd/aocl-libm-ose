/* Copyright (C) 2020, Advanced Micro Devices. All rights reserved
 * Author: Pranoy Jayaraj <pranoy.jayaraj@amd.com>
 * An example code for exercising dynamic loading of the amdlibm
   library
 */

#include "libm_dynamic_load.h"

int main(int argc, char* argv[]) {
    void* handle;
    char* libm_path = NULL;

    if (argc <= 1) {
        printf("Error! Provide .so file path as argument\n");
        return 1;
    }

    libm_path = argv[1];
    handle = dlopen(libm_path, RTLD_LAZY);
    if (!handle) {
        printf("Error! %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    test_exp(handle);
    test_log(handle);
    test_pow(handle);

    test_sin(handle);
    test_cos(handle);
    test_tan(handle);

    test_fabs(handle);
    test_atan(handle);

    test_sinh(handle);

    test_cosh(handle);
    test_tanh(handle);

    test_atan2(handle);
    test_lround(handle);

    test_asinh(handle);
    test_floor(handle);

    dlclose(handle);
    return 0;
}
