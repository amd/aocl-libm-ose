#include "libm_dynamic_load.h"

int test_fmax(void* handle) {
    char* error;
    float  (*lamd_fmaxf)(float,  float);
    double (*lamd_fmax) (double, double);
    /*scalar inputs*/
    float  input1f  = 3.145, input2f =  1.0, outputf;
    double input1d  = 6.287, input2d = -1.0, outputd;

    /*scalar routines*/
    lamd_fmaxf = dlsym(handle, "amd_fmaxf");
    lamd_fmax  = dlsym(handle, "amd_fmax");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising fmax routines\n");

    /*scalar*/
    outputf = (*lamd_fmaxf)(input1f, input2f);
    printf("amd_fmaxf(%f, %f) = %f\n", input1f, input2f, outputf);
    outputd = (*lamd_fmax)(input1d, input2d);
    printf("amd_fmax(%lf, %lf) = %lf\n", input1d, input2d, outputd);
    printf("\n");

    return 0;
}
