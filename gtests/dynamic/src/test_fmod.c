#include "libm_dynamic_load.h"

int test_fmod(void* handle) {
    char* error;
    float (*lamd_fmodf)(float, float);
    double (*lamd_fmod)(double, double);
    /*scalar inputs*/
    float input1f = 3.145, input2f = 5.67, outputf;
    double input1d = 6.287, input2d = 7.89, outputd;

    /*scalar routines*/
    lamd_fmodf = dlsym(handle, "amd_fmodf");
    lamd_fmod  = dlsym(handle, "amd_fmod");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising fmod routines\n");

    /*scalar*/
    outputf = (*lamd_fmodf)(input1f, input2f);
    printf("amd_fmodf(%f, %f) = %f\n", input1f, input2f, outputf);
    outputd = (*lamd_fmod)(input1d, input2d);
    printf("amd_fmod(%lf, %lf) = %lf\n", input1d, input2d, outputd);
    printf("\n");

    return 0;
}
