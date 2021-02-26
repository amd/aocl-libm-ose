#include "libm_dynamic_load.h"

int test_log1p(void* handle) {
    char* error;

    funcf s1f = (funcf)dlsym(handle, "amd_log1pf");
    func  s1d = (func)dlsym(handle, "amd_log1p");

    /*scalar inputs*/
    float inputf = 3.145f, outputf;
    double input = 6.287, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising log1p routines\n");
    /*scalar*/
    outputf = s1f(inputf);
    printf("amd_log1pf(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input);
    printf("amd_log1p(%lf) = %lf\n", input, output);

    return 0;
}
