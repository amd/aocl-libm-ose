#include "libm_dynamic_load.h"

int test_ilogb(void* handle) {
    char* error;

    funcf s1f = (funcf)dlsym(handle, "amd_ilogbf");
    func  s1d = (func)dlsym(handle, "amd_ilogb");

    /*scalar inputs*/
    float inputf = 3.145f, outputf;
    double input = 6.287, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising ilogb routines\n");
    /*scalar*/
    outputf = s1f(inputf);
    printf("amd_ilogbf(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input);
    printf("amd_ilogb(%lf) = %lf\n", (double)input, (double)output);

    return 0;
}
