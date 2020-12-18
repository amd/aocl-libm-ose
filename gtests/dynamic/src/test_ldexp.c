#include "libm_dynamic_load.h"

/*function description*/
/*   lexpr(x,exp) = x * 2^exp  */

int test_ldexp(void* handle) {
    char* error;
    float (*lamd_ldexpf)(float, int);
    double (*lamd_ldexp)(double, int);
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;
    int exp = 5;

    /*scalar routines*/
    lamd_ldexpf = dlsym(handle, "amd_ldexpf");
    lamd_ldexp  = dlsym(handle, "amd_ldexp");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising ldexp routines\n");

    /*scalar*/
    outputf = (*lamd_ldexpf)(inputf, exp);
    printf("amd_ldexpf(%f, %d) = %f\n", inputf, exp, outputf);
    output = (*lamd_ldexp)(input, exp);
    printf("amd_ldexp(%lf, %d) = %lf\n", input, exp, output);
    printf("\n");

    return 0;
}
