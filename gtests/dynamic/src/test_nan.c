#include "libm_dynamic_load.h"

int test_nan(void* handle) {
    char* error;
    float (*lamd_nanf)(const char*);
    double (*lamd_nan)(const char*);
    /*scalar inputs*/
    const char* str_nanf = "0x0";
    const char* str_nan  = "0x0";
    double outputd;
    float outputf;

    /*scalar routines*/
    lamd_nanf = dlsym(handle, "amd_nanf");
    lamd_nan  = dlsym(handle, "amd_nan");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising nan routines\n");

    /*scalar*/
    outputf = (*lamd_nanf)(str_nanf);
    printf("amd_nanf(%s) = %f\n", str_nanf, outputf);
    outputd = (*lamd_nan)(str_nan);
    printf("amd_nan(%s) = %lf\n", str_nan, outputd);
    printf("\n");

    return 0;
}
