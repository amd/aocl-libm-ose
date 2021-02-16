#include "libm_dynamic_load.h"

int test_round(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    float (*funcf)(float) = (float (*)(float))dlsym(handle, "amd_roundf");
    double (*func)(double) = (double (*)(double))dlsym(handle, "amd_round");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising round routines\n");
    outputf = funcf(inputf);
    printf("amd_roundf(%f) = %f\n", inputf, outputf);
    output = func(input);
    printf("amd_round(%lf) = %lf\n", input, output);

    return 0;
}
