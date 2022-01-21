#include "libm_dynamic_load.h"

int test_ldexp(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145f, outputf;
    double input = 6.287, output;
    int n = 2;

    #if defined(_WIN64) || defined(_WIN32)
        funcf_int s1f = (funcf_int)GetProcAddress(handle, "amd_ldexpf");
        func_int  s1d = (func_int)GetProcAddress(handle, "amd_ldexp");

        error = GetLastError();
    #else
        funcf_int s1f = (funcf_int)dlsym(handle, "amd_ldexpf");
        func_int  s1d = (func_int)dlsym(handle, "amd_ldexp");

        error = dlerror();
    #endif

    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising ldexp routines\n");
    outputf = s1f(inputf, n);
    printf("amd_ldexpf(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input, n);
    printf("amd_ldexp(%lf) = %lf\n", input, output);

    return 0;
}
