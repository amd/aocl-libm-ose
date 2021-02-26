#include "libm_dynamic_load.h"

int test_fmin(void* handle) {
    char* error;

    funcf_2 s1f = (funcf_2)dlsym(handle, "amd_fminf");
    func_2  s1d = (func_2) dlsym(handle, "amd_fmin");

    /*scalar inputs*/
    float inputf = 3.145f, outputf;
    double input = 6.287, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising fmin routines\n");
    /*scalar*/
    outputf = s1f(inputf, inputf);
    printf("amd_fminf(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input, input);
    printf("amd_fmin(%lf) = %lf\n", input, output);

    return 0;
}
