#include "libm_dynamic_load.h"

int test_hypot(void* handle) {
    char* error;

    funcf_2 s1f = (funcf_2)dlsym(handle, "amd_hypotf");
    func_2  s1d = (func_2) dlsym(handle, "amd_hypot");

    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising hypot routines\n");
    /*scalar*/
    outputf = s1f(inputf, inputf);
    printf("amd_hypotf(%f) = %f\n", inputf, outputf);
    output = s1d(input, input);
    printf("amd_hypot(%lf) = %lf\n", input, output);

    return 0;
}
