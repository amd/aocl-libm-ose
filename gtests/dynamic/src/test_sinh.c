#include "libm_dynamic_load.h"

int test_sinh(void* handle) {
    char* error;
    float (*lamd_sinhf)(float);
    double (*lamd_sinh)(double);
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    /*scalar routines*/
    lamd_sinhf = dlsym(handle, "amd_sinhf");
    lamd_sinh  = dlsym(handle, "amd_sinh");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising sinh routines\n");

    /*scalar*/
    outputf = (*lamd_sinhf)(inputf);
    printf("amd_sinhf(%f) = %f\n", inputf, outputf);
    output = (*lamd_sinh)(input);
    printf("amd_sinh(%lf) = %lf\n", input, output);
    printf("\n");

    return 0;
}
