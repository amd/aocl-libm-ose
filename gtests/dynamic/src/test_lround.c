#include "libm_dynamic_load.h"

int test_lround(void* handle) {
    char* error;

    funcf s1f = (funcf)dlsym(handle, "amd_lroundf");
    func  s1d = (func)dlsym(handle, "amd_lround");

    /*scalar inputs*/
    float inputf = 3.145f, outputf;
    double input = 6.287, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising lround routines\n");
    /*scalar*/
    outputf = s1f(inputf);
    printf("amd_lroundf(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input);
    printf("amd_lround(%lf) = %lf\n", input, output);

    return 0;
}
