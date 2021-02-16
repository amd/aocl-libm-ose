#include "libm_dynamic_load.h"

int test_frexp(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;
    int exponent = 2;

    float (*funcf)(float, int*) = (float (*)(float, int*))dlsym(handle, "amd_frexpf");
    double (*func)(double, int*) = (double (*)(double, int*))dlsym(handle, "amd_frexp");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising frexp routines\n");
    outputf = funcf(inputf, &exponent);
    printf("amd_frexpf(%f) = %f\n", inputf, outputf);
    output = func(input, &exponent);
    printf("amd_frexp(%lf) = %lf\n", input, output);

    return 0;
}
