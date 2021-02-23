#include "libm_dynamic_load.h"

int test_ldexp(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;
    int n = 2;

    funcf_int s1f = (funcf_int)dlsym(handle, "amd_ldexpf");
    func_int  s1d = (func_int)dlsym(handle, "amd_ldexp");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising ldexp routines\n");
    outputf = s1f(inputf, n);
    printf("amd_ldexpf(%f) = %f\n", inputf, outputf);
    output = s1d(input, n);
    printf("amd_ldexp(%lf) = %lf\n", input, output);

    return 0;
}
