#include "libm_dynamic_load.h"

int test_atan2(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    float (*funcf)(float) = (float (*)(float))dlsym(handle, "amd_atan2f");
    double (*func)(double) = (double (*)(double))dlsym(handle, "amd_atan2");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising atan2 routines\n");
    outputf = funcf(inputf);
    printf("amd_atan2f(%f) = %f\n", inputf, outputf);
    output = func(input);
    printf("amd_atan2(%lf) = %lf\n", input, output);

    return 0;
}
