#include "libm_dynamic_load.h"

int test_scalbln(void* handle) {
    char* error;
    float (*lamd_scalblnf)(float, long int);
    double (*lamd_scalbln)(double, long int);
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;
    long int n = 5L;

    /*scalar routines*/
    lamd_scalblnf = dlsym(handle, "amd_scalblnf");
    lamd_scalbln  = dlsym(handle, "amd_scalbln");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising scalbln routines\n");

    /*scalar*/
    outputf = (*lamd_scalblnf)(inputf, n);
    printf("amd_scalblnf(%f, %ld) = %f\n", inputf, n, outputf);
    output = (*lamd_scalbln)(input, n);
    printf("amd_scalbln(%lf, %ld) = %lf\n", input, n, output);
    printf("\n");

    return 0;
}
