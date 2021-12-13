#include "libm_dynamic_load.h"

int test_nan(void* handle) {
    char* error;
    const char *input = "0x0";
    float outputf;
    double output;

    #if defined(_WIN64) || defined(_WIN32)
        funcf_nan s1f = (funcf_nan)GetProcAddress(handle, "amd_nanf");
        func_nan  s1d = (func_nan)GetProcAddress(handle, "amd_nan");

        error = GetLastError();
    #else
        funcf_nan s1f = (funcf_nan)dlsym(handle, "amd_nanf");
        func_nan  s1d = (func_nan)dlsym(handle, "amd_nan");

        error = dlerror();
    #endif

    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising nan routines\n");
    /*scalar*/
    outputf = s1f(input);
    printf("amd_nanf(%s) = %f\n", input, (double)outputf);
    output = s1d(input);
    printf("amd_nan(%s) = %lf\n", input, output);

    return 0;
}
