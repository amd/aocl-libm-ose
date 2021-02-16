#include "libm_dynamic_load.h"

int test_ilogb(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    float (*funcf)(float) = (float (*)(float))dlsym(handle, "amd_ilogbf");
    double (*func)(double) = (double (*)(double))dlsym(handle, "amd_ilogb");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising ilogb routines\n");
    outputf = funcf(inputf);
    printf("amd_ilogbf(%f) = %f\n", inputf, outputf);
    output = func(input);
    printf("amd_ilogb(%lf) = %lf\n", input, output);

    return 0;
}
