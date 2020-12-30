#include "libm_dynamic_load.h"

int test_trunc(void* handle) {
    char* error;
    float (*lamd_truncf)(float);
    double (*lamd_trunc)(double);
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    /*scalar routines*/
    lamd_truncf = dlsym(handle, "amd_truncf");
    lamd_trunc  = dlsym(handle, "amd_trunc");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising trunc routines\n");

    /*scalar*/
    outputf = (*lamd_truncf)(inputf);
    printf("amd_truncf(%f) = %f\n", inputf, outputf);
    output = (*lamd_trunc)(input);
    printf("amd_trunc(%lf) = %lf\n", input, output);
    printf("\n");

    return 0;
}
