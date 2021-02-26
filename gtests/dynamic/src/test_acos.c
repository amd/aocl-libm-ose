#include "libm_dynamic_load.h"

int test_acos(void* handle) {
    char* error;

    funcf     s1f = (funcf)dlsym(handle, "amd_acosf");
    func      s1d = (func)dlsym(handle, "amd_acos");

    /*scalar inputs*/
    float inputf = 3.14f, outputf;
    double input = 6.28, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising acos routines\n");
    /*scalar*/
    outputf = s1f(inputf);
    printf("amd_acosf(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input);
    printf("amd_acos(%lf) = %lf\n", input, output);

    return 0;
}
