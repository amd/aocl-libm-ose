#include "libm_dynamic_load.h"

int test_scalbn(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;
    int n = 2;

    float (*funcf)(float, int) = (float (*)(float, int))dlsym(handle, "amd_scalbnf");
    double (*func)(double, int) = (double (*)(double, int))dlsym(handle, "amd_scalbn");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising scalbn routines\n");
    outputf = funcf(inputf, n);
    printf("amd_scalbnf(%f) = %f\n", inputf, outputf);
    output = func(input, n);
    printf("amd_scalbn(%lf) = %lf\n", input, output);

    return 0;
}
