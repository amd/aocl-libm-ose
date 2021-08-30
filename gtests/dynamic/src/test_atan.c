#include "libm_dynamic_load.h"

int test_atan(void* handle) {
    char* error;
    float (*lamd_atanf)(float);
    double (*lamd_atan)(double);
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    /*scalar routines*/
    lamd_atanf = dlsym(handle, "amd_atanf");
    lamd_atan  = dlsym(handle, "amd_atan");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising atan routines\n");

    /*scalar*/
    outputf = (*lamd_atanf)(inputf);
    printf("amd_atanf(%f) = %f\n", inputf, outputf);
    output = (*lamd_atan)(input);
    printf("amd_atan(%lf) = %lf\n", input, output);
    printf("\n");

    return 0;
}
