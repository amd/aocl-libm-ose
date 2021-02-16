#include "libm_dynamic_load.h"

int test_nextafter(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    float (*funcf)(float, float) = (float (*)(float, float))dlsym(handle, "amd_nextafterf");
    double (*func)(double, double) = (double (*)(double, double))dlsym(handle, "amd_nextafter");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising nextafter routines\n");
    outputf = funcf(inputf, inputf);
    printf("amd_nextafterf(%f, %f) = %f\n", inputf, inputf, outputf);
    output = func(input, input);
    printf("amd_nextafter(%lf, %lf) = %lf\n", input, input, output);

    return 0;
}
