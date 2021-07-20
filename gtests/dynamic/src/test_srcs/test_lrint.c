#include "libm_dynamic_load.h"

int test_lrint(void* handle) {
    char* error;

    funcf s1f = (funcf)dlsym(handle, "amd_lrintf");
    func  s1d = (func)dlsym(handle, "amd_lrint");

    /*scalar inputs*/
    float inputf = 3.145f, outputf;
    double input = 6.287, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising lrint routines\n");
    /*scalar*/
    outputf = s1f(inputf);
    printf("amd_lrintf(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input);
    printf("amd_lrint(%lf) = %lf\n", input, output);

    return 0;
}
