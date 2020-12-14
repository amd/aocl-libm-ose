#include "libm_dynamic_load.h"

int test_finite(void* handle) {
    char* error;
    int (*lamd_finitef)(float);
    int (*lamd_finite)(double);
    /*scalar inputs*/
    float inputf = 3.145;
    double input = 6.287;
    int output;

    /*scalar routines*/
    lamd_finitef = dlsym(handle, "amd_finitef");
    lamd_finite  = dlsym(handle, "amd_finite");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising finite routines\n");

    /*scalar*/
    output = (*lamd_finitef)(inputf);
    printf("amd_finitef(%f) = %d\n", inputf, output);
    output = (*lamd_finite)(input);
    printf("amd_finite(%lf) = %d\n", input, output);
    printf("\n");

    return 0;
}
