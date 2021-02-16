#include "libm_dynamic_load.h"

int test_nexttoward(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;
    long double input1 = 5.678;

    float (*funcf)(float, long double) = (float (*)(float, long double))dlsym(handle, "amd_nexttowardf");
    double (*func)(double, long double) = (double (*)(double, long double))dlsym(handle, "amd_nexttoward");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising nexttoward routines\n");
    outputf = funcf(inputf, input1);
    printf("amd_nexttowardf(%f, %Lf) = %f\n", inputf, input1, outputf);
    output = func(input, input1);
    printf("amd_nexttoward(%lf, %Lf) = %lf\n", input, input1, output);

    return 0;
}
