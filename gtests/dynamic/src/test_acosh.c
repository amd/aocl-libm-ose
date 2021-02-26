#include "libm_dynamic_load.h"

int test_acosh(void* handle) {
    char* error;

    funcf     s1f = (funcf)dlsym(handle, "amd_acoshf");
    func      s1d = (func)dlsym(handle, "amd_acosh");

    /*scalar inputs*/
    float inputf = 3.14f, outputf;
    double input = 6.28, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising acosh routines\n");
    /*scalar*/
    outputf = s1f(inputf);
    printf("amd_acoshf(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input);
    printf("amd_acosh(%lf) = %lf\n", input, output);

    return 0;
}
