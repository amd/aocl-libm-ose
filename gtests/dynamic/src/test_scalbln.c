#include "libm_dynamic_load.h"

int test_scalbln(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;
    int n = 2;

    float (*funcf)(float, int) = (float (*)(float, int))dlsym(handle, "amd_scalblnf");
    double (*func)(double, int) = (double (*)(double, int))dlsym(handle, "amd_scalbln");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising scalbln routines\n");
    outputf = funcf(inputf, n);
    printf("amd_scalblnf(%f) = %f\n", inputf, outputf);
    output = func(input, n);
    printf("amd_scalbln(%lf) = %lf\n", input, output);

    return 0;
}
