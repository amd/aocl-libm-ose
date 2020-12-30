#include "libm_dynamic_load.h"

int test_fmin(void* handle) {
    char* error;
    float  (*lamd_fminf)(float,  float);
    double (*lamd_fmin) (double, double);
    /*scalar inputs*/
    float  input1f  = 3.145, input2f =  1.0, outputf;
    double input1d  = 6.287, input2d = -1.0, outputd;

    /*scalar routines*/
    lamd_fminf = dlsym(handle, "amd_fminf");
    lamd_fmin  = dlsym(handle, "amd_fmin");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising fmin routines\n");

    /*scalar*/
    outputf = (*lamd_fminf)(input1f, input2f);
    printf("amd_fminf(%f, %f) = %f\n", input1f, input2f, outputf);
    outputd = (*lamd_fmin)(input1d, input2d);
    printf("amd_fmin(%lf, %lf) = %lf\n", input1d, input2d, outputd);
    printf("\n");

    return 0;
}
