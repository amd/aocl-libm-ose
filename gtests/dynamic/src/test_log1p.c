#include "libm_dynamic_load.h"

int test_log1p(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    float (*funcf)(float) = (float (*)(float))dlsym(handle, "amd_log1pf");
    double (*func)(double) = (double (*)(double))dlsym(handle, "amd_log1p");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising log1p routines\n");
    outputf = funcf(inputf);
    printf("amd_log1pf(%f) = %f\n", inputf, outputf);
    output = func(input);
    printf("amd_log1p(%lf) = %lf\n", input, output);

    return 0;
}
