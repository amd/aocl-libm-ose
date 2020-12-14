#include "libm_dynamic_load.h"

int test_lround(void* handle) {
    char* error;
    float (*lamd_lroundf)(float);
    double (*lamd_lround)(double);
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    /*scalar routines*/
    lamd_lroundf = dlsym(handle, "amd_lroundf");
    lamd_lround  = dlsym(handle, "amd_lround");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising lround routines\n");

    /*scalar*/
    outputf = (*lamd_lroundf)(inputf);
    printf("amd_lroundf(%f) = %f\n", inputf, outputf);
    output = (*lamd_lround)(input);
    printf("amd_lround(%lf) = %lf\n", input, output);
    printf("\n");

    return 0;
}
