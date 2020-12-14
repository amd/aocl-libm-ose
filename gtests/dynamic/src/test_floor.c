#include "libm_dynamic_load.h"

int test_floor(void* handle) {
    char* error;
    float (*lamd_floorf)(float);
    double (*lamd_floor)(double);
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    /*scalar routines*/
    lamd_floorf = dlsym(handle, "amd_floorf");
    lamd_floor  = dlsym(handle, "amd_floor");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising floor routines\n");

    /*scalar*/
    outputf = (*lamd_floorf)(inputf);
    printf("amd_floorf(%f) = %f\n", inputf, outputf);
    output = (*lamd_floor)(input);
    printf("amd_floor(%lf) = %lf\n", input, output);
    printf("\n");

    return 0;
}
