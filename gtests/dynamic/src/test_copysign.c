#include "libm_dynamic_load.h"

int test_copysign(void* handle) {
    char* error;
    float  (*lamd_copysignf)(float,  float);
    double (*lamd_copysign) (double, double);
    /*scalar inputs*/
    float  input1f = 3.145, input2f = -1.0, outputf;
    double input1d = 6.287, input2d = -1.0, outputd;

    /*scalar routines*/
    lamd_copysignf = dlsym(handle, "amd_copysignf");
    lamd_copysign  = dlsym(handle, "amd_copysign");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising copysign routines\n");

    /*scalar*/
    outputf = (*lamd_copysignf)(input1f, input2f);
    printf("amd_copysignf(%f, %f) = %f\n", input1f, input2f, outputf);
    outputd = (*lamd_copysign)(input1d, input2d);
    printf("amd_copysign(%lf, %lf) = %lf\n", input1d, input2d, outputd);
    printf("\n");

    return 0;
}
