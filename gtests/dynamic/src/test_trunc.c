#include "libm_dynamic_load.h"

int test_trunc(void* handle) {
    char* error;

    funcf s1f = (funcf)dlsym(handle, "amd_truncf");
    func  s1d = (func)dlsym(handle, "amd_trunc");

    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising trunc routines\n");
    /*scalar*/
    outputf = s1f(inputf);
    printf("amd_truncf(%f) = %f\n", inputf, outputf);
    output = s1d(input);
    printf("amd_trunc(%lf) = %lf\n", input, output);

    return 0;
}
