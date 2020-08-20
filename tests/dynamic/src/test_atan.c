#include "include.h"

int test_atan(void* handle) {
    char* error;
    //int i;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    /*scalar routines*/
    *(void **) (&amd_atanf) = dlsym(handle, "amd_atanf");
    *(void **) (&amd_atan)  = dlsym(handle, "amd_atan");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising atan routines\n");

    /*scalar*/
    outputf = (*amd_atanf)(inputf);
    printf("amd_atanf(%f) = %f\n", inputf, outputf);
    output = (*amd_atan)(input);
    printf("amd_atan(%lf) = %lf\n", input, output);
    printf("\n");

    return 0;
}
