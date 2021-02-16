#include "libm_dynamic_load.h"

int test_fdim(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    float (*funcf)(float, float) = (float (*)(float, float))dlsym(handle, "amd_fdimf");
    double (*func)(double, double) = (double (*)(double, double))dlsym(handle, "amd_fdim");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising fdim routines\n");
    outputf = funcf(inputf, inputf);
    printf("amd_fdimf(%f, %f) = %f\n", inputf, inputf, outputf);
    output = func(input, input);
    printf("amd_fdim(%lf, %lf) = %lf\n", input, input, output);

    return 0;
}
