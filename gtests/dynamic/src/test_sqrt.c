#include "libm_dynamic_load.h"

int test_sqrt(void* handle) {
    char* error;
    float (*lamd_sqrtf)(float);
    double (*lamd_sqrt)(double);
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    /*scalar routines*/
    lamd_sqrtf = dlsym(handle, "amd_sqrtf");
    lamd_sqrt  = dlsym(handle, "amd_sqrt");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising sqrt routines\n");

    /*scalar*/
    outputf = (*lamd_sqrtf)(inputf);
    printf("amd_sqrtf(%f) = %f\n", inputf, outputf);
    output = (*lamd_sqrt)(input);
    printf("amd_sqrt(%lf) = %lf\n", input, output);
    printf("\n");

    return 0;
}
