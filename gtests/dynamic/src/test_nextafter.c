#include "libm_dynamic_load.h"

int test_nextafter(void* handle) {
    char* error;
    float (*lamd_nextafterf)(float, float);
    double (*lamd_nextafter)(double, double);
    /*scalar inputs*/
    float input1f = 3.145, input2f = 4.5, outputf;
    double input1d = 6.287, input2d = 7.8, outputd;

    /*scalar routines*/
    lamd_nextafterf = dlsym(handle, "amd_nextafterf");
    lamd_nextafter  = dlsym(handle, "amd_nextafter");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising nextafter routines\n");

    /*scalar*/
    outputf = (*lamd_nextafterf)(input1f, input2f);
    printf("amd_nextafterf(%f, %f) = %f\n", input1f, input2f, outputf);
    outputd = (*lamd_nextafter)(input1d, input2d);
    printf("amd_nextafter(%lf, %lf) = %lf\n", input1d, input2d, outputd);
    printf("\n");

    return 0;
}
