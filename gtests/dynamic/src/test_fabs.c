#include "libm_dynamic_load.h"

int test_fabs(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    float (*funcf)(float) = (float (*)(float))dlsym(handle, "amd_fabsf");
    double (*func)(double) = (double (*)(double))dlsym(handle, "amd_fabs");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising fabs routines\n");
    outputf = funcf(inputf);
    printf("amd_fabsf(%f) = %f\n", inputf, outputf);
    output = func(input);
    printf("amd_fabs(%lf) = %lf\n", input, output);

    return 0;
}
