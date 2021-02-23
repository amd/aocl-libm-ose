#include "libm_dynamic_load.h"

int test_sinh(void* handle) {
    char* error;

    funcf_2 s1f = (funcf_2)dlsym(handle, "amd_sinhf");
    func_2  s1d = (func_2) dlsym(handle, "amd_sinh");

    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising sinh routines\n");
    /*scalar*/
    outputf = s1f(inputf, inputf);
    printf("amd_sinhf(%f) = %f\n", inputf, outputf);
    output = s1d(input, input);
    printf("amd_sinh(%lf) = %lf\n", input, output);

    return 0;
}
