#include "libm_dynamic_load.h"

int test_lrint(void* handle) {
    char* error;
    long int (*lamd_lrintf)(float);
    long int (*lamd_lrint)(double);
    /*scalar inputs*/
    float inputf = 3.145;
    long int output;
    double input = 6.287;

    /*scalar routines*/
    lamd_lrintf = dlsym(handle, "amd_lrintf");
    lamd_lrint  = dlsym(handle, "amd_lrint");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising lrint routines\n");

    /*scalar*/
    output = (*lamd_lrintf)(inputf);
    printf("amd_lrintf(%f) = %ld\n", inputf, output);
    output = (*lamd_lrint)(input);
    printf("amd_lrint(%lf) = %ld\n", input, output);
    printf("\n");

    return 0;
}
