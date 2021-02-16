#include "libm_dynamic_load.h"

int test_ldexp(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;
    int exp = 2;

    float (*funcf)(float, int) = (float (*)(float, int))dlsym(handle, "amd_ldexpf");
    double (*func)(double, int) = (double (*)(double, int))dlsym(handle, "amd_ldexp");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising ldexp routines\n");
    outputf = funcf(inputf, exp);
    printf("amd_ldexpf(%f) = %f\n", inputf, outputf);
    output = func(input, exp);
    printf("amd_ldexp(%lf) = %lf\n", input, output);

    return 0;
}
