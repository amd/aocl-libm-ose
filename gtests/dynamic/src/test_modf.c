#include "libm_dynamic_load.h"

int test_modf(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    float (*funcf)(float, float*) = (float (*)(float, float*))dlsym(handle, "amd_modff");
    double (*func)(double, double*) = (double (*)(double, double*))dlsym(handle, "amd_modf");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising modf routines\n");
    outputf = funcf(inputf, &inputf);
    printf("amd_modff(%f) = %f\n", inputf, outputf);
    output = func(input, &input);
    printf("amd_modf(%lf) = %lf\n", input, output);

    return 0;
}
