#include "libm_dynamic_load.h"

int test_ceil(void* handle) {
    char* error;
    float (*lamd_ceilf)(float);
    double (*lamd_ceil)(double);
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    /*scalar routines*/
    lamd_ceilf = dlsym(handle, "amd_ceilf");
    lamd_ceil  = dlsym(handle, "amd_ceil");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising ceil routines\n");

    /*scalar*/
    outputf = (*lamd_ceilf)(inputf);
    printf("amd_ceilf(%f) = %f\n", inputf, outputf);
    output = (*lamd_ceil)(input);
    printf("amd_ceil(%lf) = %lf\n", input, output);
    printf("\n");

    return 0;
}
