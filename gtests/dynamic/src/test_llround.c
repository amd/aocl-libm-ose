#include "libm_dynamic_load.h"

int test_llround(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145;
    long long int output;
    double input = 6.287;

    long long int (*funcf)(float) = (long long int (*)(float))dlsym(handle, "amd_llroundf");
    long long int (*func)(double) = (long long int (*)(double))dlsym(handle, "amd_llround");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising llround routines\n");
    output = funcf(inputf);
    printf("amd_llroundf(%f) = %lld\n", inputf, output);
    output = func(input);
    printf("amd_llround(%lf) = %lld\n", input, output);

    return 0;
}
