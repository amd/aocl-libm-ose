#include "libm_dynamic_load.h"

int test_fabs(void* handle) {
    char* error;
    float (*lamd_fabsf)(float);
    double (*lamd_fabs)(double);
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    /*scalar routines*/
    lamd_fabsf = dlsym(handle, "amd_fabsf");
    lamd_fabs  = dlsym(handle, "amd_fabs");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising fabs routines\n");

    /*scalar*/
    outputf = (*lamd_fabsf)(inputf);
    printf("amd_fabsf(%f) = %f\n", inputf, outputf);
    output = (*lamd_fabs)(input);
    printf("amd_fabs(%lf) = %lf\n", input, output);
    printf("\n");

    return 0;
}
