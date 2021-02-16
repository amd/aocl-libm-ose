#include "libm_dynamic_load.h"

int test_rint(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    float (*funcf)(float) = (float (*)(float))dlsym(handle, "amd_rintf");
    double (*func)(double) = (double (*)(double))dlsym(handle, "amd_rint");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising rint routines\n");
    outputf = funcf(inputf);
    printf("amd_rintf(%f) = %f\n", inputf, outputf);
    output = func(input);
    printf("amd_rint(%lf) = %lf\n", input, output);

    return 0;
}
