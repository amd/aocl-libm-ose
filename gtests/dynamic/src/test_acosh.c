#include "libm_dynamic_load.h"

int test_acosh(void* handle) {
    char* error;

    funcf_2 s1f = (funcf_2)dlsym(handle, "amd_acoshf");
    func_2  s1d = (func_2) dlsym(handle, "amd_acosh");

    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising acosh routines\n");
    /*scalar*/
    outputf = s1f(inputf, inputf);
    printf("amd_acoshf(%f) = %f\n", inputf, outputf);
    output = s1d(input, input);
    printf("amd_acosh(%lf) = %lf\n", input, output);

    return 0;
}
