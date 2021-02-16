#include "libm_dynamic_load.h"

int test_finite(void* handle) {
    char* error;
    /*scalar inputs*/
    int output;
    float inputf = 3.14;
    double inputd = 6.565;

    int (*funcf)(float) = (int (*)(float))dlsym(handle, "amd_finitef");
    int (*func)(double) = (int (*)(double))dlsym(handle, "amd_finite");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising finite routines\n");
    output = funcf(inputf);
    printf("amd_finitef(%f) = %d\n", inputf, output);
    output = func(inputd);
    printf("amd_finite(%lf) = %d\n", inputd, output);

    return 0;
}
