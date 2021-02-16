#include "libm_dynamic_load.h"

int test_llrint(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145;
    long long int output;
    double input = 6.287;

    long long int (*funcf)(float) = (long long int (*)(float))dlsym(handle, "amd_llrintf");
    long long int (*func)(double) = (long long int (*)(double))dlsym(handle, "amd_llrint");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising llrint routines\n");
    output = funcf(inputf);
    printf("amd_llrintf(%f) = %lld\n", inputf, output);
    output = func(input);
    printf("amd_llrint(%lf) = %lld\n", input, output);

    return 0;
}
