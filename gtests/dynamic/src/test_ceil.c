#include "libm_dynamic_load.h"

int test_ceil(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    float (*funcf)(float) = (float (*)(float))dlsym(handle, "amd_ceilf");
    double (*func)(double) = (double (*)(double))dlsym(handle, "amd_ceil");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising ceil routines\n");
    outputf = funcf(inputf);
    printf("amd_ceilf(%f) = %f\n", inputf, outputf);
    output = func(input);
    printf("amd_ceil(%lf) = %lf\n", input, output);

    return 0;
}
