#include "libm_dynamic_load.h"

int test_nan(void* handle) {
    char* error;
    /*scalar inputs*/
    const char *nan = "0x0";
    float outputf;
    double output;

    float (*funcf)(const char*) = (float (*)(const char*))dlsym(handle, "amd_nanf");
    double (*func)(const char*) = (double (*)(const char*))dlsym(handle, "amd_nan");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising nan routines\n");
    outputf = funcf(nan);
    printf("amd_nanf(%s) = %f\n", nan, outputf);
    output = func(nan);
    printf("amd_nan(%s) = %lf\n", nan, output);

    return 0;
}
