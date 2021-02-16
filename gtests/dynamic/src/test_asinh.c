#include "libm_dynamic_load.h"

int test_asinh(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    float (*funcf)(float) = (float (*)(float))dlsym(handle, "amd_asinhf");
    double (*func)(double) = (double (*)(double))dlsym(handle, "amd_asinh");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising asinh routines\n");
    outputf = funcf(inputf);
    printf("amd_asinhf(%f) = %f\n", inputf, outputf);
    output = func(input);
    printf("amd_asinh(%lf) = %lf\n", input, output);

    return 0;
}
