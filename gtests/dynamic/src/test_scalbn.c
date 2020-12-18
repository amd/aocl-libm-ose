#include "libm_dynamic_load.h"

int test_scalbn(void* handle) {
    char* error;
    float (*lamd_scalbnf)(float, int);
    double (*lamd_scalbn)(double, int);
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;
    int exp = 5;

    /*scalar routines*/
    lamd_scalbnf = dlsym(handle, "amd_scalbnf");
    lamd_scalbn  = dlsym(handle, "amd_scalbn");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising scalbn routines\n");

    /*scalar*/
    outputf = (*lamd_scalbnf)(inputf, exp);
    printf("amd_scalbnf(%f, %d) = %f\n", inputf, exp, outputf);
    output = (*lamd_scalbn)(input, exp);
    printf("amd_scalbn(%lf, %d) = %lf\n", input, exp, output);
    printf("\n");

    return 0;
}
