#include "include.h"

int test_fabs(void* handle) {
    char* error;
    //int i;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    /*scalar routines*/
    *(void **) (&amd_fabsf) = dlsym(handle, "amd_fabsf");
    *(void **) (&amd_fabs)  = dlsym(handle, "amd_fabs");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising fabs routines\n");

    /*scalar*/
    outputf = (*amd_fabsf)(inputf);
    printf("amd_fabsf(%f) = %f\n", inputf, outputf);
    output = (*amd_fabs)(input);
    printf("amd_fabs(%lf) = %lf\n", input, output);
    printf("\n");

    return 0;
}
