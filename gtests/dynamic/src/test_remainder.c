#include "libm_dynamic_load.h"

int test_remainder(void* handle) {
    char* error;
    float (*lamd_remainderf)(float, float);
    double (*lamd_remainder)(double, double);
    /*scalar inputs*/
    float input1f = 3.145, input2f = 2.1, outputf;
    double input1d = 6.287, input2d = 4.5, outputd;

    /*scalar routines*/
    lamd_remainderf = dlsym(handle, "amd_remainderf");
    lamd_remainder  = dlsym(handle, "amd_remainder");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising remainder routines\n");

    /*scalar*/
    outputf = (*lamd_remainderf)(input1f, input2f);
    printf("amd_remainderf(%f, %f) = %f\n", input1f, input2f, outputf);
    outputd = (*lamd_remainder)(input1d, input2d);
    printf("amd_remainder(%lf, %lf) = %lf\n", input1d, input2d, outputd);
    printf("\n");

    return 0;
}
