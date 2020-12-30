#include "libm_dynamic_load.h"

int test_fdim(void* handle) {
    char* error;
    float  (*lamd_fdimf)(float,  float);
    double (*lamd_fdim) (double, double);
    /*scalar inputs*/
    float  input1f = 3.145, input2f = 2.1, outputf;
    double input1d = 6.287, input2d = 4.5, outputd;

    /*scalar routines*/
    lamd_fdimf = dlsym(handle, "amd_fdimf");
    lamd_fdim  = dlsym(handle, "amd_fdim");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising fdim routines\n");

    /*scalar*/
    outputf = (*lamd_fdimf)(input1f, input2f);
    printf("amd_fdimf(%f, %f) = %f\n", input1f, input2f, outputf);
    outputd = (*lamd_fdim)(input1d, input2d);
    printf("amd_fdim(%lf, %lf) = %lf\n", input1d, input2d, outputd);
    printf("\n");

    return 0;
}
