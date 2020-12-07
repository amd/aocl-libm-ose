#include "libm_dynamic_load.h"

int test_atan2(void* handle) {
    char* error;
    float (*lamd_atan2f)(float, float);
    double (*lamd_atan2)(double, double);
    /*scalar inputs*/
    float input1f = 3.145, input2f = 4.56, outputf;
    double input1d = 6.287, input2d = 5.67, outputd;

    /*scalar routines*/
    lamd_atan2f = dlsym(handle, "amd_atan2f");
    lamd_atan2  = dlsym(handle, "amd_atan2");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising atan2 routines\n");

    /*scalar*/
    outputf = (*lamd_atan2f)(input1f, input2f);
    printf("amd_atan2f(%f, %f) = %f\n", input1f, input2f, outputf);
    outputd = (*lamd_atan2)(input1d, input2d);
    printf("amd_atan2(%lf, %lf) = %lf\n", input1d, input2d, outputd);
    printf("\n");

    return 0;
}
