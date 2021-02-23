#include "libm_dynamic_load.h"

int test_fmax(void* handle) {
    char* error;

    funcf_2 s1f = (funcf_2)dlsym(handle, "amd_fmaxf");
    func_2  s1d = (func_2) dlsym(handle, "amd_fmax");

    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising fmax routines\n");
    /*scalar*/
    outputf = s1f(inputf, inputf);
    printf("amd_fmaxf(%f) = %f\n", inputf, outputf);
    output = s1d(input, input);
    printf("amd_fmax(%lf) = %lf\n", input, output);

    return 0;
}
