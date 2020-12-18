#include "libm_dynamic_load.h"

int test_modf(void* handle) {
    char* error;
    float (*lamd_modff)(float, float*);
    double (*lamd_modf)(double, double*);
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;
    float *input1f = &inputf;
    double *input1d = &input;

    /*scalar routines*/
    lamd_modff = dlsym(handle, "amd_modff");
    lamd_modf  = dlsym(handle, "amd_modf");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising modf routines\n");

    /*scalar*/
    outputf = (*lamd_modff)(inputf, input1f);
    printf("amd_modff(%f, %f) = %f\n", inputf, input1f, outputf);
    output = (*lamd_modf)(input, input1d);
    printf("amd_modf(%lf, %lf) = %lf\n", input, input1d, output);
    printf("\n");

    return 0;
}
