#include "libm_dynamic_load.h"

int test_nexttoward(void* handle) {
    char* error;
    float (*lamd_nexttowardf)(float, long double);
    double (*lamd_nexttoward)(double, long double);
    /*scalar inputs*/
    float input1f = 3.145, outputf;
    long double input2 = 4.56;
    double input1d = 6.287, outputd;

    /*scalar routines*/
    lamd_nexttowardf = dlsym(handle, "amd_nexttowardf");
    lamd_nexttoward  = dlsym(handle, "amd_nexttoward");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising nexttoward routines\n");

    /*scalar*/
    outputf = (*lamd_nexttowardf)(input1f, input2);
    printf("amd_nexttowardf(%f, %ld) = %f\n", input1f, input2, outputf);
    outputd = (*lamd_nexttoward)(input1d, input2);
    printf("amd_nexttoward(%lf, %ld) = %lf\n", input1d, input2, outputd);
    printf("\n");

    return 0;
}
