#include "libm_glibc_compat.h"

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
    test_sin(handle);
    test_cos(handle);
    test_pow(handle);

    return 0;
}
