#include "libm_dynamic_load.h"

int test_nearbyint(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    float (*funcf)(float) = (float (*)(float))dlsym(handle, "amd_nearbyintf");
    double (*func)(double) = (double (*)(double))dlsym(handle, "amd_nearbyint");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising nearbyint routines\n");
    outputf = funcf(inputf);
    printf("amd_nearbyintf(%f) = %f\n", inputf, outputf);
    output = func(input);
    printf("amd_nearbyint(%lf) = %lf\n", input, output);

    return 0;
}
