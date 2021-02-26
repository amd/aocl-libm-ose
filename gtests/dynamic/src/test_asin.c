#include "libm_dynamic_load.h"

int test_asin(void* handle) {
    char* error;

    funcf     s1f = (funcf)dlsym(handle, "amd_asinf");
    func      s1d = (func)dlsym(handle, "amd_asin");

    /*scalar inputs*/
    float inputf = 3.14f, outputf;
    double input = 6.28, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising asin routines\n");
    /*scalar*/
    outputf = s1f(inputf);
    printf("amd_asinf(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input);
    printf("amd_asin(%lf) = %lf\n", input, output);

    return 0;
}
