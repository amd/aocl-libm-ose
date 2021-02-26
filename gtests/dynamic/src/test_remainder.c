#include "libm_dynamic_load.h"

int test_remainder(void* handle) {
    char* error;

    funcf_2 s1f = (funcf_2)dlsym(handle, "amd_remainderf");
    func_2  s1d = (func_2) dlsym(handle, "amd_remainder");

    /*scalar inputs*/
    float inputf = 3.145f, outputf;
    double input = 6.287, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising remainder routines\n");
    /*scalar*/
    outputf = s1f(inputf, inputf);
    printf("amd_remainderf(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input, input);
    printf("amd_remainder(%lf) = %lf\n", input, output);

    return 0;
}
