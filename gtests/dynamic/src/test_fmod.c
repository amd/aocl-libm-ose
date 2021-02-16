#include "libm_dynamic_load.h"

int test_fmod(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    float (*funcf)(float, float) = (float (*)(float, float))dlsym(handle, "amd_fmodf");
    double (*func)(double, double) = (double (*)(double, double))dlsym(handle, "amd_fmod");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising fmod routines\n");
    outputf = funcf(inputf, inputf);
    printf("amd_fmodf(%f, %f) = %f\n", inputf, inputf, outputf);
    output = func(input, input);
    printf("amd_fmod(%lf, %lf) = %lf\n", input, input, output);

    return 0;
}
