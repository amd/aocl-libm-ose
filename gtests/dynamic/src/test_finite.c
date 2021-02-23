#include "libm_dynamic_load.h"

int test_finite(void* handle) {
    char* error;

    funcf s1f = (funcf)dlsym(handle, "amd_finitef");
    func  s1d = (func)dlsym(handle, "amd_finite");

    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising finite routines\n");
    /*scalar*/
    outputf = s1f(inputf);
    printf("amd_finitef(%f) = %f\n", inputf, outputf);
    output = s1d(input);
    printf("amd_finite(%lf) = %lf\n", input, output);

    return 0;
}
