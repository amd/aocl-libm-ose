#include "libm_dynamic_load.h"

int test_asinh(void* handle) {
    char* error;
    float (*lamd_asinhf)(float);
    double (*lamd_asinh)(double);
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    /*scalar routines*/
    lamd_asinhf = dlsym(handle, "amd_asinhf");
    lamd_asinh  = dlsym(handle, "amd_asinh");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising asinh routines\n");

    /*scalar*/
    outputf = (*lamd_asinhf)(inputf);
    printf("amd_asinhf(%f) = %f\n", inputf, outputf);
    output = (*lamd_asinh)(input);
    printf("amd_asinh(%lf) = %lf\n", input, output);
    printf("\n");

    return 0;
}
