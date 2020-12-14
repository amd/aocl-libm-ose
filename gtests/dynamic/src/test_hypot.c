#include "libm_dynamic_load.h"

int test_hypot(void* handle) {
    char* error;
    float (*lamd_hypotf)(float);
    double (*lamd_hypot)(double);
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    /*scalar routines*/
    lamd_hypotf = dlsym(handle, "amd_hypotf");
    lamd_hypot  = dlsym(handle, "amd_hypot");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising hypot routines\n");

    /*scalar*/
    outputf = (*lamd_hypotf)(inputf);
    printf("amd_hypotf(%f) = %f\n", inputf, outputf);
    output = (*lamd_hypot)(input);
    printf("amd_hypot(%lf) = %lf\n", input, output);
    printf("\n");

    return 0;
}
