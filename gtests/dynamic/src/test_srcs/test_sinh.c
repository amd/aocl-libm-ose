#include "libm_dynamic_load.h"

int test_sinh(void* handle) {
    char* error;

    /*scalar inputs*/
    float inputf = 3.145f, outputf;
    double input = 6.287, output;

    #if defined(_WIN64) || defined(_WIN32)
        funcf_2 s1f = (funcf_2)GetProcAddress(handle, "amd_sinhf");
        func_2  s1d = (func_2)GetProcAddress(handle, "amd_sinh");

        error = GetLastError();
    #else
        funcf_2 s1f = (funcf_2)dlsym(handle, "amd_sinhf");
        func_2  s1d = (func_2)dlsym(handle, "amd_sinh");

        error = dlerror();
    #endif

    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising sinh routines\n");
    /*scalar*/
    outputf = s1f(inputf, inputf);
    printf("amd_sinhf(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input, input);
    printf("amd_sinh(%lf) = %lf\n", input, output);

    return 0;
}
