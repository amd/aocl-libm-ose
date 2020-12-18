#include "libm_dynamic_load.h"

int test_frexp(void* handle) {
    char* error;
    float (*lamd_frexpf)(float, int*);
    double (*lamd_frexp)(double, int*);
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;
    int n;

    /*scalar routines*/
    lamd_frexpf = dlsym(handle, "amd_frexpf");
    lamd_frexp  = dlsym(handle, "amd_frexp");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising frexp routines\n");

    /*scalar*/
    outputf = (*lamd_frexpf)(inputf, &n);
    printf("amd_frexpf(%f) = %f\n", inputf, outputf);
    printf("%f = %f * 2^%d\n", inputf, outputf, n);

    output = (*lamd_frexp)(input, &n);
    printf("amd_frexp(%lf) = %lf\n", input, output);
    printf("%lf = %lf * 2^%d\n", input, output, n);
    printf("\n");

    return 0;
}
