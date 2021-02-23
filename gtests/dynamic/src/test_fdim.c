#include "libm_dynamic_load.h"

int test_fdim(void* handle) {
    char* error;

    funcf_2 s1f = (funcf_2)dlsym(handle, "amd_fdimf");
    func_2  s1d = (func_2) dlsym(handle, "amd_fdim");

    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising fdim routines\n");
    /*scalar*/
    outputf = s1f(inputf, inputf);
    printf("amd_fdimf(%f) = %f\n", inputf, outputf);
    output = s1d(input, input);
    printf("amd_fdim(%lf) = %lf\n", input, output);

    return 0;
}
