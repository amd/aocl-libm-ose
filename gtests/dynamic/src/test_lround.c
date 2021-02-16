#include "libm_dynamic_load.h"

int test_lround(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145;
    long int output;
    double input = 6.287;

    long int (*funcf)(float) = (long int (*)(float))dlsym(handle, "amd_lroundf");
    long int (*func)(double) = (long int (*)(double))dlsym(handle, "amd_lround");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising lround routines\n");
    output = funcf(inputf);
    printf("amd_lroundf(%f) = %ld\n", inputf, output);
    output = func(input);
    printf("amd_lround(%lf) = %ld\n", input, output);

    return 0;
}
