#include "libm_dynamic_load.h"

int test_llrint(void* handle) {
    char* error;
    long long int (*lamd_llrintf)(float);
    long long int (*lamd_llrint)(double);
    /*scalar inputs*/
    float inputf = 3.145;
    double input = 6.287;
    long long int output;

    /*scalar routines*/
    lamd_llrintf = dlsym(handle, "amd_llrintf");
    lamd_llrint  = dlsym(handle, "amd_llrint");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising llrint routines\n");

    /*scalar*/
    output = (*lamd_llrintf)(inputf);
    printf("amd_llrintf(%f) = %lld\n", inputf, output);
    output = (*lamd_llrint)(input);
    printf("amd_llrint(%lf) = %lld\n", input, output);
    printf("\n");

    return 0;
}
